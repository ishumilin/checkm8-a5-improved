#ifndef CONSTANTS_NAMED_H
#define CONSTANTS_NAMED_H

#include <Arduino.h>

// ============================================================================
// USB DEVICE IDENTIFICATION
// ============================================================================

/** Apple Inc. USB Vendor ID */
const uint16_t APPLE_VENDOR_ID = 0x05AC;

/** Apple DFU Mode Product ID */
const uint16_t APPLE_DFU_PRODUCT_ID = 0x1227;

// ============================================================================
// HEAP FENG SHUI PARAMETERS
// ============================================================================

/**
 * Number of heap spray iterations required for reliable heap layout
 * This value was determined empirically to create the necessary heap
 * fragmentation pattern for the exploit to work reliably
 */
const int HEAP_SPRAY_ITERATIONS = 620;

/** Size of each heap spray allocation (128 bytes) */
const uint8_t HEAP_SPRAY_SIZE = 0x80;

/** Size for leak detection (129 bytes) */
const uint8_t HEAP_LEAK_SIZE = 0x81;

// ============================================================================
// USB CONTROL REQUEST PARAMETERS
// ============================================================================

/** DFU request type for HOST2DEVICE */
const uint8_t DFU_REQUEST_TYPE_H2D = 0x21;

/** DFU request type for DEVICE2HOST */
const uint8_t DFU_REQUEST_TYPE_D2H = 0xA1;

/** DFU request: DNLOAD (download data to device) */
const uint8_t DFU_REQUEST_DNLOAD = 1;

/** DFU request: GETSTATUS (get device status) */
const uint8_t DFU_REQUEST_GETSTATUS = 3;

/** Standard USB request: GET_DESCRIPTOR (renamed to avoid USB library macro conflict) */
const uint8_t USB_REQ_GET_DESCRIPTOR = 6;

/** USB descriptor type: STRING (renamed to avoid USB library macro conflict) */
const uint8_t USB_DESC_STRING = 3;

/** Language ID for string descriptors (0x040a = US English) */
const uint16_t USB_LANGID_EN_US = 0x040a;

// ============================================================================
// BUFFER SIZES
// ============================================================================

/** Standard USB control transfer packet size */
const uint8_t USB_PACKET_SIZE = 0x40;

/** Size of status response (6 bytes) */
const uint8_t DFU_STATUS_SIZE = 6;

// ============================================================================
// LED CONTROL
// ============================================================================

/** Pin number for status LED (Arduino digital pin 6) */
const uint8_t LED_STATUS_PIN = 6;

/** LED blink delay for status indication (milliseconds) */
const int LED_BLINK_DELAY = 500;

/** Number of LED blinks to indicate exploit start */
const int LED_BLINKS_START = 3;

// ============================================================================
// TIMING PARAMETERS
// ============================================================================

/** Delay for USB initialization (milliseconds) */
const int USB_INIT_DELAY = 200;

/** Delay when wrong device detected (milliseconds) */
const int WRONG_DEVICE_DELAY = 5000;

// ============================================================================
// EXPLOIT STAGE IDENTIFIERS
// ============================================================================

/** Stage names for better logging (stored in Flash memory to save RAM) */
const char STAGE_NAME_0[] PROGMEM = "Initial Reset";
const char STAGE_NAME_1[] PROGMEM = "Heap Feng Shui";
const char STAGE_NAME_2[] PROGMEM = "Set Global State";
const char STAGE_NAME_3[] PROGMEM = "Heap Occupation";
const char STAGE_NAME_4[] PROGMEM = "Complete";

const char* const STAGE_NAMES[] PROGMEM = {
  STAGE_NAME_0,
  STAGE_NAME_1,
  STAGE_NAME_2,
  STAGE_NAME_3,
  STAGE_NAME_4
};

#endif // CONSTANTS_NAMED_H
