# Hardware Setup Guide

This guide will help you set up the hardware required to run the checkm8-a5 exploit.

## Required Hardware

### 1. Arduino Board
**Supported boards:**
- Arduino Uno (recommended for beginners)
- Arduino Mega 2560
- Arduino Leonardo
- Any Arduino-compatible board with SPI support

**Recommended:** Arduino Uno R3 or compatible clone

### 2. MAX3421E USB Host Shield
**Required specifications:**
- MAX3421E chip (USB Host Controller)
- SPI interface
- 3.3V/5V compatible

**Where to buy:**
- SparkFun USB Host Shield (DEV-09947)
- Arduino USB Host Shield
- Compatible clones from various manufacturers

**Important:** Ensure the shield is compatible with your Arduino board's form factor.

### 3. USB Cable
**Requirements:**
- Standard USB-A to Lightning/30-pin cable (depending on your device)
- **MUST be a direct cable** - NO USB hubs embedded
- Good quality cable with proper data lines

**⚠️ DO NOT USE:**
- DCSD cables
- Kong cables
- Kanzi cables
- Any cable with embedded USB hub
- Charge-only cables

### 4. Target Device
**Supported devices:**
- **S5L8942X:** iPad 2 (WiFi, GSM, CDMA Rev A), iPhone 4S
- **S5L8945X:** iPad 2 (WiFi Rev A), iPad Mini (1st gen), Apple TV 3
- **S5L8940X:** iPad 2 (original)

**Device must be in DFU mode** (see "Entering DFU Mode" section below)

### 5. Computer
**Requirements:**
- USB port for Arduino
- Arduino IDE installed (version 1.8.x or 2.x)
- Serial monitor capability

## Wiring Diagram

### Arduino Uno + MAX3421E Shield

The MAX3421E USB Host Shield typically stacks directly onto the Arduino:

```
Arduino Uno
    |
    | (Stack directly)
    |
MAX3421E USB Host Shield
    |
    | (USB-A port)
    |
USB Cable → Target Device (in DFU mode)
```

### Pin Connections (if not using shield)

If you're using a standalone MAX3421E module:

| MAX3421E Pin | Arduino Uno Pin | Function |
|--------------|-----------------|----------|
| VCC          | 5V              | Power    |
| GND          | GND             | Ground   |
| MOSI         | 11              | SPI MOSI |
| MISO         | 12              | SPI MISO |
| SCK          | 13              | SPI SCK  |
| SS/CS        | 10              | SPI CS   |
| INT          | 9               | Interrupt|
| RST          | 8               | Reset    |

### LED Connection (Optional but Recommended)

Connect an LED to pin 6 for status indication:

```
Arduino Pin 6 → [220Ω Resistor] → LED Anode (+)
LED Cathode (-) → GND
```

**LED Status Indicators:**
- 3 blinks at start: Exploit initialization
- Continuous blink (1 Hz): USB initialization failed
- Solid ON: Exploit complete

## Assembly Instructions

### Step 1: Prepare Arduino
1. Ensure Arduino is unplugged
2. Remove any existing shields
3. Verify Arduino is functional (upload blink sketch first)

### Step 2: Install USB Host Shield
1. Align shield pins with Arduino headers
2. Gently press shield onto Arduino
3. Ensure all pins are properly seated
4. Check for any bent pins

### Step 3: Connect LED (Optional)
1. Insert 220Ω resistor into breadboard
2. Connect resistor to Arduino pin 6
3. Connect LED anode to resistor
4. Connect LED cathode to GND

### Step 4: Power Check
1. Connect Arduino to computer via USB
2. Verify power LED on Arduino lights up
3. Verify power LED on USB Host Shield lights up (if present)
4. Check for any unusual heat or smell

### Step 5: Upload Code
1. Open Arduino IDE
2. Load checkm8-a5-improved.ino
3. Select correct board (Tools → Board)
4. Select correct port (Tools → Port)
5. Click Upload
6. Wait for "Done uploading" message

### Step 6: Verify Upload
1. Open Serial Monitor (Tools → Serial Monitor)
2. Set baud rate to 115200
3. Press Arduino reset button
4. Verify you see the checkm8-a5 banner

## Entering DFU Mode

### For iPhone 4S / iPad 2:

1. **Connect device to Arduino** (via USB Host Shield)
2. **Power off device completely**
3. **Hold Home + Power** for 10 seconds
4. **Release Power** but keep holding Home
5. **Continue holding Home** for another 10 seconds
6. **Screen should remain black** (if you see iTunes logo, try again)

**Success indicators:**
- Screen is completely black
- Device is recognized by Arduino
- Serial monitor shows "Apple DFU device detected"

### For Apple TV 3:

1. **Unplug Apple TV**
2. **Connect Micro-USB cable** to Apple TV
3. **Connect other end** to USB Host Shield
4. **Hold Menu + Play/Pause** while plugging in power
5. **Keep holding** for 10 seconds
6. **Release buttons**

## Troubleshooting Hardware Issues

### Arduino not recognized by computer
- Try different USB cable
- Try different USB port
- Install/update Arduino drivers
- Check Device Manager (Windows) or System Information (Mac)

### USB Host Shield not working
- Verify shield is properly seated
- Check for bent pins
- Verify power LED on shield
- Try shield on different Arduino
- Test with USB Host Shield library examples

### Device not detected
- Verify device is in DFU mode (black screen)
- Try different USB cable (no hubs!)
- Check cable data lines (not charge-only)
- Try different USB port on shield
- Verify device battery is charged (>50%)

### Exploit fails at specific stage
- Check serial monitor output for error codes
- Verify correct SoC selected in code
- Try different USB cable
- Ensure stable power supply
- Check for loose connections

### LED not working
- Verify LED polarity (long leg = anode/+)
- Check resistor value (220Ω recommended)
- Test LED with multimeter
- Verify pin 6 connection

## Power Considerations

### Power Requirements
- Arduino: ~50mA (via USB)
- MAX3421E: ~50-100mA
- Target device: ~100-500mA (in DFU mode)

**Total: ~200-650mA**

### Power Sources

**Option 1: USB Power (Recommended)**
- Connect Arduino to computer USB port
- Most computers provide 500mA per port
- Sufficient for most scenarios

**Option 2: External Power**
- Use 7-12V DC adapter on Arduino barrel jack
- Provides more stable power
- Recommended for Arduino Mega or multiple devices

**⚠️ Warning:** Do not power Arduino from both USB and external power simultaneously unless using a board with automatic switching.

## Safety Precautions

1. **Always disconnect power** before making wiring changes
2. **Check polarity** of all connections
3. **Use appropriate resistors** for LEDs
4. **Avoid short circuits** - double-check wiring
5. **Work on non-conductive surface** (wood, plastic)
6. **Keep liquids away** from electronics
7. **Don't force connections** - pins should slide in smoothly

## Testing Your Setup

### Basic Hardware Test

Before running the exploit, test your hardware:

1. **Upload USB Host Shield test sketch:**
   ```
   File → Examples → USB Host Shield Library 2.0 → board_qc
   ```

2. **Open Serial Monitor** (115200 baud)

3. **Verify output shows:**
   - "Circuits At Home 2011"
   - "USB Host Shield Quality Control Routine"
   - Oscillation test results

4. **All tests should PASS**

### Device Detection Test

1. **Upload checkm8-a5-improved.ino**
2. **Put device in DFU mode**
3. **Connect device to shield**
4. **Check Serial Monitor** for:
   - "Apple DFU device detected"
   - Device VID:PID (05AC:1227)

If you see these messages, your hardware is working correctly!

## Next Steps

Once your hardware is set up and tested:

1. Read [README.md](../README.md) for exploit overview
2. Review [EXPLOIT_DETAILS.md](EXPLOIT_DETAILS.md) for technical details
3. Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md) if you encounter issues
4. Understand the warnings about device demotion
5. Run the exploit!

## Additional Resources

- **Arduino IDE:** https://www.arduino.cc/en/software
- **USB Host Shield Library:** https://github.com/felis/USB_Host_Shield_2.0
- **MAX3421E Datasheet:** https://datasheets.maximintegrated.com/en/ds/MAX3421E.pdf
- **Arduino Uno Pinout:** https://docs.arduino.cc/hardware/uno-rev3

## Support

If you encounter hardware issues not covered here:

1. Check [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. Verify all connections match this guide
3. Test with USB Host Shield library examples
4. Check for hardware defects (damaged pins, burnt components)
5. Try different Arduino board or shield if available
