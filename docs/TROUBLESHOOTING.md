# Troubleshooting Guide

This guide helps you diagnose and fix common issues with the checkm8-a5 exploit.

## Table of Contents

1. [Hardware Issues](#hardware-issues)
2. [Software Issues](#software-issues)
3. [Exploit Stage Failures](#exploit-stage-failures)
4. [Device-Specific Issues](#device-specific-issues)
5. [Error Code Reference](#error-code-reference)
6. [Common Questions](#common-questions)

---

## Hardware Issues

### Arduino Not Detected by Computer

**Symptoms:**
- Arduino IDE shows "No board detected"
- COM port not appearing
- Upload fails immediately

**Solutions:**
1. **Check USB cable** - Try a different cable (must support data, not just charging)
2. **Try different USB port** - Some ports may not provide enough power
3. **Install/update drivers:**
   - Windows: Install CH340/FTDI drivers
   - Mac: Usually works out of the box
   - Linux: Add user to `dialout` group: `sudo usermod -a -G dialout $USER`
4. **Check Device Manager** (Windows) or `ls /dev/tty*` (Mac/Linux)
5. **Try different computer** to isolate hardware vs. software issue

### USB Host Shield Not Working

**Symptoms:**
- "USB initialization failed" error
- Shield power LED not lit
- No response when device connected

**Solutions:**
1. **Verify shield is properly seated:**
   - Remove and reseat shield
   - Check for bent pins
   - Ensure all pins make contact

2. **Test shield with example sketch:**
   ```
   File → Examples → USB Host Shield Library 2.0 → board_qc
   ```
   All tests should PASS

3. **Check power:**
   - Shield power LED should be lit
   - Measure 5V between VCC and GND pins
   - Try external power supply if USB power insufficient

4. **Verify SPI connections** (if using standalone module):
   - MOSI → Pin 11
   - MISO → Pin 12
   - SCK → Pin 13
   - SS → Pin 10

### Device Not Detected

**Symptoms:**
- "No device in DFU mode detected"
- "Non-Apple DFU device detected"
- Device connects but exploit doesn't start

**Solutions:**
1. **Verify DFU mode:**
   - Screen should be completely BLACK
   - No Apple logo, no iTunes logo
   - If you see anything, device is not in DFU mode

2. **Re-enter DFU mode:**
   - Power off device completely
   - Hold Home + Power for 10 seconds
   - Release Power, keep holding Home
   - Hold Home for 10 more seconds
   - Screen stays black = success

3. **Check USB cable:**
   - **CRITICAL:** No embedded USB hubs!
   - Avoid DCSD/Kong/Kanzi cables
   - Try different cable
   - Ensure cable supports data transfer

4. **Try different USB port on shield:**
   - Some shields have multiple ports
   - Try each port

5. **Check device battery:**
   - Charge to at least 50%
   - Low battery can cause detection issues

---

## Software Issues

### Compilation Errors

**Error: "Usb.h: No such file or directory"**

**Solution:**
1. Install USB Host Shield Library 2.0:
   ```
   Sketch → Include Library → Manage Libraries
   Search: "USB Host Shield Library 2.0"
   Install by Oleg Mazurov
   ```

2. Apply patch:
   ```bash
   cd ~/Arduino/libraries/USB_Host_Shield_2.0
   git checkout cd87628af4a693eeafe1bf04486cf86ba01d29b8
   git apply /path/to/usb_host_library.patch
   ```

**Error: "constants.h: No such file or directory"**

**Solution:**
- Ensure `constants.h` is in the same directory as the `.ino` file
- Check file name spelling (case-sensitive on Linux/Mac)

**Error: "chipid not defined"**

**Solution:**
- Uncomment ONE of the SoC definitions in the code:
  ```cpp
  #define A5_8942  // For iPhone 4S, iPad 2
  // #define A5_8945  // For iPad Mini, Apple TV 3
  // #define A5_8940  // For iPad 2 original
  ```

**Error: "redefinition of 'USB_REQUEST_GET_DESCRIPTOR'" or similar**

**Problem:**
The USB Host Shield Library defines certain constants as preprocessor macros:
```c
#define USB_REQUEST_GET_DESCRIPTOR  6
#define USB_DESCRIPTOR_STRING       0x03
```

If you're using the modular code (`src/` directory), there may be naming conflicts.

**Solution:**
The improved fork uses renamed constants to avoid conflicts:
- `USB_REQUEST_GET_DESCRIPTOR` → `USB_REQ_GET_DESCRIPTOR`
- `USB_DESCRIPTOR_STRING` → `USB_DESC_STRING`

These are already fixed in the current version. If you see this error:
1. Ensure you're using the latest version of the code
2. Check that `src/config/constants_named.h` uses the renamed constants
3. Verify `src/exploit/heap_feng_shui.h` references the new names

**Error: "data section exceeds available space in board"**

**Problem:**
Arduino Uno has only 2KB of RAM. The code with all modules may exceed this limit.

**Solution:**
The improved fork is memory-optimized using PROGMEM:
- Error messages stored in Flash instead of RAM
- Stage names stored in Flash instead of RAM
- Heavy modules removed for Uno compatibility

If you still see this error:
1. Verify you're compiling the main `checkm8-a5.ino` (not a test file)
2. Check that heavy modules are not included (config_menu.h, soc_detector.h, optimizations.h should not exist)
3. For Arduino Mega users: You can add back removed modules for extra features

See [MEMORY_OPTIMIZATION.md](../MEMORY_OPTIMIZATION.md) for details.

### Upload Errors

**Error: "avrdude: stk500_recv(): programmer is not responding"**

**Solutions:**
1. Select correct board: `Tools → Board → Arduino Uno`
2. Select correct port: `Tools → Port → COM# or /dev/tty*`
3. Close Serial Monitor before uploading
4. Press reset button on Arduino, then immediately upload
5. Try different USB cable

**Error: "Sketch too big"**

**Solution:**
- Use Arduino Mega 2560 (more memory)
- Or optimize code by removing debug Serial.print statements

### Serial Monitor Issues

**No output in Serial Monitor**

**Solutions:**
1. Set baud rate to **115200**
2. Verify correct COM port selected
3. Press Arduino reset button
4. Check USB cable connection
5. Try different terminal program (PuTTY, screen, minicom)

**Garbled output**

**Solution:**
- Set baud rate to 115200
- Check for loose connections
- Verify Arduino is not resetting repeatedly

---

## Exploit Stage Failures

### Stage 1: Heap Feng Shui Fails

**Symptoms:**
- Exploit stops at "Performing 620 heap spray iterations"
- Progress counter doesn't increase
- Timeout or USB errors

**Solutions:**
1. **Check USB connection:**
   - Reseat cable
   - Try different cable
   - Ensure no USB hubs

2. **Verify device is in DFU mode:**
   - Screen must be completely black
   - Re-enter DFU mode if unsure

3. **Check SoC selection:**
   - Ensure correct `#define` for your device
   - iPhone 4S = A5_8942
   - iPad Mini = A5_8945
   - iPad 2 original = A5_8940

4. **Power cycle everything:**
   - Disconnect device
   - Unplug Arduino
   - Wait 10 seconds
   - Reconnect and try again

### Stage 2: Set Global State Fails

**Symptoms:**
- "ERROR: Sending failed!"
- "Global state setup failed"
- USB communication errors

**Solutions:**
1. **Check serial output for error codes:**
   - Look for specific USB error codes (0x0X)
   - Cross-reference with USB Host Shield documentation

2. **Verify USB Host Shield patch applied:**
   ```bash
   cd ~/Arduino/libraries/USB_Host_Shield_2.0
   git diff
   ```
   Should show modifications to `Usb.cpp` and `UsbCore.h`

3. **Try slower timing:**
   - Add delays between operations
   - Modify `USB_XFER_TIMEOUT` in patch

4. **Check power supply:**
   - Use external power for Arduino
   - Ensure stable 5V supply

### Stage 3: Heap Occupation Fails

**Symptoms:**
- Payload delivery fails
- Device doesn't enter pwned DFU
- Exploit completes but device unresponsive

**Solutions:**
1. **Verify payload for correct SoC:**
   - Check `constants.h` has correct payload
   - Ensure `#define` matches device

2. **Check device firmware version:**
   - Very old or very new firmware may behave differently
   - Try updating/downgrading device firmware

3. **Try multiple times:**
   - Exploit may need 2-3 attempts
   - Reset Arduino and device between attempts

4. **Check for hardware issues:**
   - Loose connections
   - Insufficient power
   - Damaged USB port on device

---

## Device-Specific Issues

### iPhone 4S

**Issue: Device keeps exiting DFU mode**

**Solution:**
- Ensure battery is charged (>50%)
- Hold Home button firmly during DFU entry
- Try entering DFU mode while connected to Arduino

**Issue: Exploit works but device won't boot**

**Solution:**
- This is expected - device is demoted
- Use tools like irecovery to interact
- To restore: Use iTunes/Finder to restore firmware

### iPad 2

**Issue: Different iPad 2 models behave differently**

**Solution:**
- Identify exact model:
  - Settings → General → About → Model Number
  - A1395 = WiFi (A5_8940)
  - A1396 = GSM (A5_8942)
  - A1397 = CDMA (A5_8942)
  - A1416 = WiFi Rev A (A5_8945)

**Issue: Exploit fails on Rev A models**

**Solution:**
- Use `#define A5_8945` instead of A5_8940
- Rev A models have different SoC

### iPad Mini (1st gen)

**Issue: Device not detected**

**Solution:**
- iPad Mini uses Lightning connector
- Ensure cable supports data transfer
- Some cheap Lightning cables don't work

### Apple TV 3

**Issue: Can't enter DFU mode**

**Solution:**
- Must use Micro-USB port (not HDMI)
- Hold Menu + Play/Pause while plugging in power
- Keep holding for full 10 seconds

---

## Error Code Reference

### Error Codes from checkm8-a5

| Code | Name | Meaning | Solution |
|------|------|---------|----------|
| 1 | ERR_USB_INIT_FAILED | USB Host Shield initialization failed | Check shield connection, verify power |
| 2 | ERR_DEVICE_NOT_FOUND | No device in DFU mode | Enter DFU mode, check cable |
| 3 | ERR_WRONG_DEVICE | Device is not Apple DFU | Verify device, check VID:PID |
| 4 | ERR_WRONG_SOC | SoC doesn't match selected target | Change #define to match device |
| 5 | ERR_HEAP_FENG_SHUI_FAILED | Heap manipulation failed | Check USB connection, try again |
| 6 | ERR_STATE_SETUP_FAILED | Global state setup failed | Verify patch applied, check power |
| 7 | ERR_HEAP_OCCUPATION_FAILED | Heap occupation failed | Try multiple times, check payload |
| 8 | ERR_PAYLOAD_DELIVERY_FAILED | Payload delivery failed | Check USB connection, verify SoC |
| 9 | ERR_USB_COMMUNICATION | USB communication error | Check cable, reseat connections |
| 10 | ERR_TIMEOUT | Operation timed out | Device not responding, re-enter DFU |

### USB Host Shield Error Codes

| Code | Meaning |
|------|---------|
| 0x00 | Success |
| 0x01 | Busy |
| 0x02 | Bad request |
| 0x03 | Undefined |
| 0x04 | NAK (Not Acknowledged) |
| 0x05 | Stall |
| 0x06 | Toggle error |
| 0x07 | Wrong PID |
| 0x08 | Bad byte count |
| 0x09 | PID error |
| 0x0A | Packet error |
| 0x0B | CRC error |
| 0x0C | Timeout |

---

## Common Questions

### Q: How many times should I try before giving up?

**A:** Try 3-5 times. If it consistently fails at the same stage, there's likely a hardware or configuration issue.

### Q: Can I use this on newer devices?

**A:** No. This exploit only works on A5/A5X SoCs (iPhone 4S, iPad 2, iPad Mini 1, Apple TV 3). Newer devices are not vulnerable.

### Q: Will this brick my device?

**A:** No, but it will demote your device. You can restore via iTunes/Finder, but the demotion is permanent at the bootrom level.

### Q: Why does my device use development KBAG after exploit?

**A:** The exploit demotes the device, which changes how it decrypts firmware. This is intentional and documented behavior.

### Q: Can I undo the demotion?

**A:** No. Bootrom modifications are permanent. However, you can restore the device to normal operation via iTunes/Finder.

### Q: Why can't I use cables with USB hubs?

**A:** The exploit requires precise USB timing and control. USB hubs introduce latency and interfere with the low-level USB manipulation needed for the exploit.

### Q: My exploit completes but device doesn't respond

**A:** This is normal. The device is now in pwned DFU mode. Use tools like `irecovery` or `ipwndfu` to interact with it.

### Q: Can I use this for jailbreaking?

**A:** This exploit provides low-level access but is not a complete jailbreak. You'll need additional tools and payloads for a full jailbreak.

### Q: Why does the exploit take so long?

**A:** The heap feng shui stage performs 620 iterations to create the correct heap layout. This is necessary for reliable exploitation and takes 2-3 minutes.

---

## Still Having Issues?

If you've tried everything in this guide and still have problems:

1. **Double-check hardware connections** - Most issues are hardware-related
2. **Verify software versions:**
   - Arduino IDE version
   - USB Host Shield Library version
   - Patch applied correctly

3. **Test with known-good hardware:**
   - Try different Arduino board
   - Try different USB Host Shield
   - Try different USB cable
   - Try different target device

4. **Review serial output carefully:**
   - Note exact error messages
   - Note which stage fails
   - Look for patterns in failures

5. **Check original resources:**
   - [Original checkm8 paper](https://github.com/axi0mX/ipwndfu)
   - [USB Host Shield Library docs](https://github.com/felis/USB_Host_Shield_2.0)
   - [Arduino forums](https://forum.arduino.cc/)

6. **Document your setup:**
   - Arduino model
   - Shield model
   - Device model and iOS version
   - Cable type
   - Error messages
   - What you've tried

This information will help if you need to seek additional support.
