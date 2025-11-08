#ifndef LED_PATTERNS_H
#define LED_PATTERNS_H

#include <Arduino.h>
#include "../config/constants_named.h"

/**
 * LED Pattern Controller
 * 
 * Provides visual feedback through LED patterns for different
 * exploit stages and error conditions.
 */

class LEDPatterns {
private:
  uint8_t pin;
  
  /**
   * Blink LED a specific number of times
   * @param times Number of blinks
   * @param onTime Time LED is on (ms)
   * @param offTime Time LED is off (ms)
   */
  void blink(int times, int onTime, int offTime) {
    for(int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(onTime);
      digitalWrite(pin, LOW);
      if(i < times - 1) {  // Don't delay after last blink
        delay(offTime);
      }
    }
  }
  
public:
  /**
   * Constructor
   * @param ledPin Pin number for status LED
   */
  LEDPatterns(uint8_t ledPin = LED_STATUS_PIN) : pin(ledPin) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  
  /**
   * Initialize - quick double blink to show system ready
   */
  void initialize() {
    blink(2, 100, 100);
    delay(500);
  }
  
  /**
   * Exploit starting - 3 slow blinks
   */
  void exploitStarting() {
    blink(LED_BLINKS_START, LED_BLINK_DELAY, LED_BLINK_DELAY);
  }
  
  /**
   * Stage progress - single quick blink
   */
  void stageProgress() {
    blink(1, 100, 0);
  }
  
  /**
   * Stage complete - double quick blink
   */
  void stageComplete() {
    blink(2, 150, 150);
    delay(300);
  }
  
  /**
   * Heap feng shui progress indicator
   * @param current Current iteration
   * @param total Total iterations
   */
  void heapProgress(int current, int total) {
    // Blink every 10% progress
    int percentage = (current * 100) / total;
    static int lastPercentage = 0;
    
    if(percentage >= lastPercentage + 10) {
      blink(1, 50, 0);
      lastPercentage = percentage;
    }
  }
  
  /**
   * Exploit complete - solid on
   */
  void exploitComplete() {
    digitalWrite(pin, HIGH);
  }
  
  /**
   * Error pattern - rapid blinking
   * @param errorCode Error code to indicate
   */
  void error(int errorCode) {
    // Rapid blinks equal to error code
    for(int i = 0; i < 3; i++) {
      blink(errorCode, 200, 200);
      delay(1000);
    }
  }
  
  /**
   * Fatal error - continuous slow blink
   * Call this in a loop for persistent error indication
   */
  void fatalError() {
    blink(1, 1000, 1000);
  }
  
  /**
   * Waiting for device - slow pulse effect
   */
  void waitingForDevice() {
    // Fade in
    for(int i = 0; i < 255; i += 5) {
      analogWrite(pin, i);
      delay(10);
    }
    // Fade out
    for(int i = 255; i >= 0; i -= 5) {
      analogWrite(pin, i);
      delay(10);
    }
  }
  
  /**
   * Device detected - quick triple blink
   */
  void deviceDetected() {
    blink(3, 100, 100);
    delay(500);
  }
  
  /**
   * USB communication - very quick blink
   */
  void usbActivity() {
    blink(1, 50, 0);
  }
  
  /**
   * Turn LED off
   */
  void off() {
    digitalWrite(pin, LOW);
  }
  
  /**
   * Turn LED on
   */
  void on() {
    digitalWrite(pin, HIGH);
  }
  
  /**
   * Custom pattern - specify your own timing
   * @param pattern Array of on/off times in ms (alternating)
   * @param length Length of pattern array
   */
  void customPattern(int* pattern, int length) {
    for(int i = 0; i < length; i++) {
      digitalWrite(pin, i % 2 == 0 ? HIGH : LOW);
      delay(pattern[i]);
    }
    digitalWrite(pin, LOW);
  }
};

#endif // LED_PATTERNS_H
