#ifndef USB_HELPERS_H
#define USB_HELPERS_H

#include <Arduino.h>
#include "Usb.h"

/**
 * USB Helper Functions
 * 
 * This module provides utility functions for USB communication
 * used throughout the exploit stages.
 */

class USBHelpers {
public:
  /**
   * Send OUT packet to USB device
   * @param usb Pointer to USB instance
   * @param io_buf Buffer containing data to send
   * @param pktsize Size of packet to send
   * @return USB error code (0 = success)
   */
  static uint8_t sendOut(USB* usb, uint8_t* io_buf, uint8_t pktsize) {
    usb->bytesWr(rSNDFIFO, pktsize, io_buf);
    usb->regWr(rSNDBC, pktsize);
    usb->regWr(rHXFR, tokOUT);
    while(!(usb->regRd(rHIRQ) & bmHXFRDNIRQ));
    usb->regWr(rHIRQ, bmHXFRDNIRQ);
    uint8_t rcode = usb->regRd(rHRSL) & 0x0f;
    return rcode;
  }
  
  /**
   * Read payload byte from program memory
   * @param payload Pointer to payload in PROGMEM
   * @param index Index of byte to read
   * @return Byte value
   */
  static uint8_t readPayloadByte(const uint8_t* payload, int index) {
    return pgm_read_byte(payload + index);
  }
  
  /**
   * Copy payload from PROGMEM to buffer
   * @param dest Destination buffer
   * @param payload Source payload in PROGMEM
   * @param offset Offset in payload
   * @param size Number of bytes to copy
   */
  static void copyPayload(uint8_t* dest, const uint8_t* payload, int offset, uint8_t size) {
    for(int i = 0; i < size; i++) {
      dest[i] = pgm_read_byte(payload + offset + i);
    }
  }
};

#endif // USB_HELPERS_H
