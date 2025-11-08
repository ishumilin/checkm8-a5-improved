#ifndef DEVICE_VALIDATOR_H
#define DEVICE_VALIDATOR_H

#include <Arduino.h>
#include "Usb.h"
#include "../config/constants_named.h"
#include "../config/error_codes.h"

/**
 * Device Validation Module
 * 
 * Validates device compatibility and state before exploitation
 */

class DeviceValidator {
private:
  USB* usb;
  
public:
  /**
   * Constructor
   * @param usbInstance Pointer to USB instance
   */
  DeviceValidator(USB* usbInstance) : usb(usbInstance) {}
  
  /**
   * Validate device is in DFU mode
   * @param desc Device descriptor
   * @return Error code (ERR_NONE if valid)
   */
  ExploitError validateDFUMode(USB_DEVICE_DESCRIPTOR* desc) {
    if(desc->idVendor != APPLE_VENDOR_ID) {
      Serial.print("Invalid vendor ID: 0x");
      Serial.println(desc->idVendor, HEX);
      return ERR_WRONG_DEVICE;
    }
    
    if(desc->idProduct != APPLE_DFU_PRODUCT_ID) {
      Serial.print("Invalid product ID: 0x");
      Serial.println(desc->idProduct, HEX);
      Serial.println("Device is not in DFU mode");
      return ERR_WRONG_DEVICE;
    }
    
    return ERR_NONE;
  }
  
  /**
   * Print device information
   * @param desc Device descriptor
   */
  void printDeviceInfo(USB_DEVICE_DESCRIPTOR* desc) {
    Serial.println("\n--- Device Information ---");
    Serial.print("Vendor ID:  0x");
    Serial.println(desc->idVendor, HEX);
    Serial.print("Product ID: 0x");
    Serial.println(desc->idProduct, HEX);
    Serial.print("USB Version: ");
    Serial.println(desc->bcdUSB, HEX);
    Serial.print("Device Class: 0x");
    Serial.println(desc->bDeviceClass, HEX);
    Serial.print("Serial Index: ");
    Serial.println(desc->iSerialNumber);
    Serial.println("-------------------------\n");
  }
  
  /**
   * Verify USB Host Shield is working
   * @return Error code (ERR_NONE if working)
   */
  ExploitError verifyUSBHost() {
    // Try to read a register
    uint8_t revision = usb->regRd(rREVISION);
    
    if(revision == 0x00 || revision == 0xFF) {
      Serial.println("USB Host Shield not responding");
      Serial.println("Check connections and power");
      return ERR_USB_INIT_FAILED;
    }
    
    Serial.print("USB Host Shield revision: 0x");
    Serial.println(revision, HEX);
    return ERR_NONE;
  }
  
  /**
   * Check if device battery is sufficient
   * Note: This is a basic check, actual battery level not available in DFU
   * @return true if device seems powered adequately
   */
  bool checkDevicePower() {
    // In DFU mode, we can't directly check battery
    // But we can verify device is responding to USB
    uint8_t state = usb->getUsbTaskState();
    return (state == USB_STATE_RUNNING);
  }
  
  /**
   * Validate SoC selection matches device
   * Note: In DFU mode, we can't directly read CPID
   * This is a placeholder for future enhancement
   * @return true if validation passes
   */
  bool validateSoCSelection() {
    // TODO: Implement CPID reading if possible
    // For now, we rely on user selecting correct SoC
    Serial.println("Note: Ensure correct SoC selected in code");
    Serial.println("Wrong SoC selection will cause exploit to fail");
    return true;
  }
  
  /**
   * Perform comprehensive pre-exploit validation
   * @param desc Device descriptor
   * @return Error code (ERR_NONE if all checks pass)
   */
  ExploitError performPreExploitChecks(USB_DEVICE_DESCRIPTOR* desc) {
    Serial.println("\n=== PRE-EXPLOIT VALIDATION ===");
    
    // Check USB Host Shield
    ExploitError err = verifyUSBHost();
    if(err != ERR_NONE) return err;
    Serial.println("✓ USB Host Shield OK");
    
    // Validate DFU mode
    err = validateDFUMode(desc);
    if(err != ERR_NONE) return err;
    Serial.println("✓ Device in DFU mode");
    
    // Check device power
    if(!checkDevicePower()) {
      Serial.println("⚠ Device power may be insufficient");
      Serial.println("  Ensure device battery >50%");
    } else {
      Serial.println("✓ Device power OK");
    }
    
    // Validate SoC selection
    if(!validateSoCSelection()) {
      Serial.println("⚠ Could not validate SoC selection");
    } else {
      Serial.println("✓ SoC selection noted");
    }
    
    Serial.println("=== VALIDATION COMPLETE ===\n");
    return ERR_NONE;
  }
};

#endif // DEVICE_VALIDATOR_H
