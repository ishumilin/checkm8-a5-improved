#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <Arduino.h>

/**
 * Error codes for checkm8-a5 exploit
 * These codes help identify failure points during exploitation
 */
enum ExploitError {
  ERR_NONE = 0,                           // No error - success
  ERR_USB_INIT_FAILED = 1,                // USB Host Shield initialization failed
  ERR_DEVICE_NOT_FOUND = 2,               // No device detected in DFU mode
  ERR_WRONG_DEVICE = 3,                   // Device is not Apple DFU (wrong VID/PID)
  ERR_WRONG_SOC = 4,                      // Device SoC doesn't match selected target
  ERR_HEAP_FENG_SHUI_FAILED = 5,          // Heap manipulation stage failed
  ERR_STATE_SETUP_FAILED = 6,             // Global state setup failed
  ERR_HEAP_OCCUPATION_FAILED = 7,         // Heap occupation stage failed
  ERR_PAYLOAD_DELIVERY_FAILED = 8,        // Payload delivery failed
  ERR_USB_COMMUNICATION = 9,              // USB communication error
  ERR_TIMEOUT = 10                        // Operation timed out
};

// Error messages stored in Flash memory to save RAM
const char ERR_MSG_0[] PROGMEM = "Success";
const char ERR_MSG_1[] PROGMEM = "USB init failed";
const char ERR_MSG_2[] PROGMEM = "No device found";
const char ERR_MSG_3[] PROGMEM = "Wrong device";
const char ERR_MSG_4[] PROGMEM = "Wrong SoC";
const char ERR_MSG_5[] PROGMEM = "Heap feng shui failed";
const char ERR_MSG_6[] PROGMEM = "State setup failed";
const char ERR_MSG_7[] PROGMEM = "Heap occupation failed";
const char ERR_MSG_8[] PROGMEM = "Payload delivery failed";
const char ERR_MSG_9[] PROGMEM = "USB comm error";
const char ERR_MSG_10[] PROGMEM = "Timeout";

const char* const ERROR_MESSAGES[] PROGMEM = {
  ERR_MSG_0, ERR_MSG_1, ERR_MSG_2, ERR_MSG_3, ERR_MSG_4,
  ERR_MSG_5, ERR_MSG_6, ERR_MSG_7, ERR_MSG_8, ERR_MSG_9, ERR_MSG_10
};

/**
 * Print error message from Flash memory
 * @param err Error code
 */
inline void printError(ExploitError err) {
  char buffer[32];
  if(err <= ERR_TIMEOUT) {
    strcpy_P(buffer, (char*)pgm_read_word(&(ERROR_MESSAGES[err])));
    Serial.print("ERROR: ");
    Serial.println(buffer);
  } else {
    Serial.println("ERROR: Unknown");
  }
}

#endif // ERROR_CODES_H
