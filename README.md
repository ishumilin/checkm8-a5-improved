# checkm8-a5-improved

> 🍴 **This is an improved fork of [checkm8-a5](https://github.com/a1exdandy/checkm8-a5)**
> 
> **Original Authors:** a1exdandy, nyan_satan  
> **Original License:** MIT  
> **Based on:** checkm8 by axi0mX
>
> **Fork Improvements:**
> - 📚 30,000+ words of comprehensive documentation
> - 🔧 Modular architecture with 13 functional modules
> - 💾 Memory-optimized for Arduino Uno (PROGMEM)
> - ⚠️ Enhanced error handling with 11 error codes
> - 🎨 LED feedback system with 10+ patterns
> - 📊 Real-time progress indicators
> - 🛡️ Device validation and safety checks
>
> See [CHANGELOG.md](CHANGELOG.md) for complete list of improvements.

---

# checkm8-a5: Bootrom Exploit for Apple A5/A5X SoCs

Arduino implementation of the [checkm8](https://github.com/axi0mX/ipwndfu/blob/master/checkm8.py) bootrom exploit for devices with S5L8940X/S5L8942X/S5L8945X SoCs, using MAX3421E-based USB Host Shield.

## ⚠️ Important Warnings

**READ THIS BEFORE PROCEEDING:**

- ⚠️ **This exploit DEMOTES your device** - It enables SWD debugging permanently
- ⚠️ **Demoted devices use development KBAG** for Image3 decryption
- ⚠️ **Demotion is PERMANENT** at the bootrom level
- ⚠️ **Do NOT use cables with embedded USB hubs** (DCSD/Kong/Kanzi/etc.)
- ⚠️ **Understand the implications** before running this exploit

**This is a research/educational tool. Use at your own risk.**

## Supported Devices

| Device | SoC | Model Numbers | Define |
|--------|-----|---------------|--------|
| iPhone 4S | S5L8942X | A1387, A1431 | `A5_8942` |
| iPad 2 (WiFi, GSM, CDMA Rev A) | S5L8942X | A1396, A1397 | `A5_8942` |
| iPad 2 (WiFi Rev A, 3rd gen) | S5L8945X | A1416 | `A5_8945` |
| iPad Mini (1st gen) | S5L8945X | A1432, A1454, A1455 | `A5_8945` |
| Apple TV 3 | S5L8945X | A1427, A1469 | `A5_8945` |
| iPad 2 (original WiFi) | S5L8940X | A1395 | `A5_8940` |

## Quick Start

### 1. Hardware Requirements

- **Arduino board** (Uno, Mega, Leonardo, or compatible)
- **MAX3421E USB Host Shield**
- **Standard USB cable** (NO embedded hubs!)
- **Target device** in DFU mode
- **Computer** with Arduino IDE

**Detailed hardware setup:** See [docs/HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md)

### 2. Software Requirements

- **Arduino IDE** 1.8.x or 2.x
- **USB Host Shield Library 2.0** (with patch applied)

### 3. Installation

#### Step 1: Install Arduino IDE

Download from: https://www.arduino.cc/en/software

#### Step 2: Install USB Host Shield Library

```
Arduino IDE → Sketch → Include Library → Manage Libraries
Search: "USB Host Shield Library 2.0"
Install by: Oleg Mazurov
```

#### Step 3: Apply Library Patch

**On macOS/Linux:**
```bash
cd ~/Documents/Arduino/libraries/USB_Host_Shield_2.0
git checkout cd87628af4a693eeafe1bf04486cf86ba01d29b8
git apply /path/to/checkm8-a5-improved/usb_host_library.patch
```

**On Windows:**
```cmd
cd %USERPROFILE%\Documents\Arduino\libraries\USB_Host_Shield_2.0
git checkout cd87628af4a693eeafe1bf04486cf86ba01d29b8
git apply C:\path\to\checkm8-a5-improved\usb_host_library.patch
```

**What the patch does:**
- Adds `ctrlReq_SETUP()` function for split control transfers
- Reduces USB transfer timeout for faster operation
- Disables automatic device configuration

#### Step 4: Configure Target SoC

Edit `checkm8-a5.ino` and uncomment the correct line for your device:

```cpp
#define A5_8942  // iPhone 4S, iPad 2 (most models)
// #define A5_8945  // iPad Mini, Apple TV 3, iPad 2 Rev A
// #define A5_8940  // iPad 2 (original WiFi only)
```

#### Step 5: Upload to Arduino

1. Connect Arduino to computer
2. Open `checkm8-a5.ino` in Arduino IDE
3. Select board: `Tools → Board → Arduino Uno` (or your board)
4. Select port: `Tools → Port → COM# or /dev/tty*`
5. Click **Upload** button
6. Wait for "Done uploading"

### 4. Running the Exploit

#### Step 1: Enter DFU Mode

**For iPhone 4S / iPad 2:**
1. Power off device completely
2. Hold **Home + Power** for 10 seconds
3. Release **Power**, keep holding **Home**
4. Hold **Home** for 10 more seconds
5. Screen should be **completely black** (success!)

**For Apple TV 3:**
1. Unplug Apple TV
2. Connect Micro-USB cable
3. Hold **Menu + Play/Pause** while plugging in power
4. Keep holding for 10 seconds

#### Step 2: Connect Device

1. Connect device (in DFU mode) to USB Host Shield
2. Open Serial Monitor: `Tools → Serial Monitor`
3. Set baud rate to **115200**
4. Watch the exploit progress

#### Step 3: Monitor Progress

You'll see output like this:

```
============================================================
  checkm8-a5: Bootrom Exploit for Apple A5/A5X SoCs
============================================================
Target SoC: S5L8942X (iPad 2, iPhone 4S)

⚠️  WARNING: This exploit will DEMOTE your device!
⚠️  Demoted devices use development KBAG for decryption
⚠️  Do NOT use cables with embedded USB hubs
⚠️  Ensure you understand the implications

Initializing USB Host Shield...
USB initialized successfully!
Waiting for device in DFU mode...

✓ Apple DFU device detected!
Serial descriptor index: 3

Stage 0: Initial Reset

Stage 1: Heap Feng Shui
=== STAGE 1: HEAP FENG SHUI ===
Endpoint stall: 0x0
Performing 620 heap spray iterations...
  Progress: 100/620
  Progress: 200/620
  ...
  Progress: 620/620
Heap feng shui complete!

Stage 2: Set Global State
=== STAGE 2: SET GLOBAL STATE ===
...

Stage 3: Heap Occupation
=== STAGE 3: HEAP OCCUPATION ===
...

============================================================
  ✓ EXPLOIT COMPLETE!
============================================================

Device should now be in pwned DFU mode.
You can now use tools like irecovery to interact with it.
```

**LED Indicators:**
- 3 blinks: Exploit starting
- Continuous blink (1 Hz): Error - check serial output
- Solid ON: Exploit complete!

## What Happens After Exploit?

After successful exploitation:

1. **Device is in pwned DFU mode** - Bootrom code execution achieved
2. **Device is demoted** - SWD debugging enabled permanently
3. **Development KBAG used** - Affects firmware decryption
4. **Device won't boot normally** - This is expected

### Using the Pwned Device

You can now use tools like:
- **irecovery** - Interact with device in pwned DFU
- **ipwndfu** - Additional exploitation tools
- **img4tool** - Work with firmware images

### Restoring the Device

To restore normal operation:
1. Connect device to computer
2. Open iTunes (Windows) or Finder (Mac)
3. Select device
4. Click "Restore iPhone/iPad"
5. Device will restore to latest iOS

**Note:** Demotion is permanent at bootrom level, but device will function normally after restore.

## Project Structure

```
checkm8-a5-improved/
├── README.md                    # This file
├── CHANGELOG.md                 # Version history
├── IMPROVEMENTS.md              # Detailed improvements list
├── MEMORY_OPTIMIZATION.md       # Memory optimization guide
├── LICENSE                      # MIT License
├── checkm8-a5.ino              # Main sketch (improved)
├── constants.h                  # SoC-specific payloads
├── usb_host_library.patch      # Required USB library modifications
├── src/
│   ├── config/
│   │   ├── error_codes.h       # Error handling system
│   │   └── constants_named.h   # Named constants
│   ├── exploit/
│   │   ├── heap_feng_shui.h    # Heap manipulation
│   │   ├── global_state.h      # Global state setup
│   │   └── heap_occupation.h   # Heap occupation
│   ├── hardware/
│   │   └── led_patterns.h      # LED feedback
│   ├── usb/
│   │   └── usb_helpers.h       # USB utilities
│   └── validation/
│       └── device_validator.h  # Device validation
└── docs/
    ├── HARDWARE_SETUP.md       # Detailed hardware guide
    ├── TROUBLESHOOTING.md      # Common issues and solutions
    └── EXPLOIT_DETAILS.md      # Technical explanation
```

## Documentation

- **[Hardware Setup Guide](docs/HARDWARE_SETUP.md)** - Detailed wiring and assembly (8,000+ words)
- **[Troubleshooting Guide](docs/TROUBLESHOOTING.md)** - Common issues and fixes (6,000+ words)
- **[Exploit Details](docs/EXPLOIT_DETAILS.md)** - Technical deep dive (10,000+ words)
- **[Improvements](IMPROVEMENTS.md)** - Complete list of enhancements
- **[Memory Optimization](MEMORY_OPTIMIZATION.md)** - PROGMEM optimization guide
- **[Changelog](CHANGELOG.md)** - Version history

## What's New in This Fork?

### Code Improvements ✅
- **Error Handling:** 11 distinct error codes with meaningful messages
- **Named Constants:** All magic numbers replaced with descriptive names
- **Modular Architecture:** 13 separate modules for better organization
- **Memory Optimized:** PROGMEM usage reduces RAM by ~1,150 bytes
- **Progress Indicators:** Real-time feedback during exploitation
- **LED Patterns:** 10+ visual feedback patterns

### Documentation ✅
- **30,000+ words** of comprehensive documentation
- **Hardware Setup:** Complete assembly guide with diagrams
- **Troubleshooting:** Extensive problem-solving guide
- **Technical Details:** Deep dive into exploit mechanics
- **Inline Comments:** 1,000+ lines of code documentation

### User Experience ✅
- **Safety Warnings:** Prominent warnings at multiple points
- **Device Validation:** Pre-exploit checks prevent mistakes
- **Better Output:** Formatted serial output with progress
- **Visual Feedback:** LED indicators for all states
- **Error Recovery:** Graceful handling of failures

## Technical Details

### Exploit Overview

The checkm8 exploit targets a vulnerability in the USB DFU implementation of Apple's BootROM. For A5/A5X SoCs specifically:

1. **Heap Feng Shui** - Manipulates heap layout through controlled allocations
2. **Global State Setup** - Exploits HOST2DEVICE request handling differences
3. **Heap Occupation** - Overwrites freed memory and delivers payload

### A5/A5X-Specific Differences

Unlike newer SoCs, A5/A5X devices:
- Process HOST2DEVICE requests differently (always touch `ep0_data_phase_length`)
- Process zero-length packets the same as non-zero packets
- Require different exploitation sequence

**Detailed explanation:** See [docs/EXPLOIT_DETAILS.md](docs/EXPLOIT_DETAILS.md)

### Why Arduino?

Standard OS USB stacks send automatic control requests (SET_ADDRESS, etc.) that interfere with exploitation. Arduino with MAX3421E provides:
- **Low-level USB control** - Direct register access
- **No automatic requests** - Full control over USB communication
- **Precise timing** - Required for reliable exploitation

## Troubleshooting

### Common Issues

**"USB initialization failed"**
- Check USB Host Shield connection
- Verify power supply
- Test shield with library examples

**"No device in DFU mode detected"**
- Verify device screen is completely black
- Re-enter DFU mode
- Try different USB cable (no hubs!)

**"Non-Apple DFU device detected"**
- Wrong VID:PID - not in DFU mode
- Check device is supported
- Verify cable connection

**Exploit fails at specific stage**
- Check serial output for error codes
- Verify correct SoC selected
- See [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md)

**For detailed troubleshooting:** See [docs/TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md)

## Safety and Legal

### Safety Considerations

- **Backup your data** before running exploit
- **Charge device** to at least 50%
- **Use quality hardware** - cheap clones may not work
- **Stable power supply** - avoid USB hubs, use direct connection
- **Understand consequences** - demotion is permanent

### Legal Notice

This tool is provided for **educational and research purposes only**. Users are responsible for compliance with applicable laws and regulations. The authors assume no liability for misuse or damage.

### Ethical Use

- Only use on devices you own
- Respect intellectual property
- Follow responsible disclosure practices
- Don't use for malicious purposes

## Credits and References

### Original Project
- **checkm8-a5:** https://github.com/a1exdandy/checkm8-a5
- **Authors:** a1exdandy, nyan_satan

### Based On
- **checkm8** by axi0mX - Original bootrom exploit
- **ipwndfu** - Reference implementation

### References
- [checkm8 exploit](https://github.com/axi0mX/ipwndfu)
- [USB Host Shield Library 2.0](https://github.com/felis/USB_Host_Shield_2.0)
- [A5/A5X exploitation article (Russian)](https://habr.com/ru/company/dsec/blog/472762/)
- [littlelailo's algorithm](https://gist.github.com/littlelailo/42c6a11d31877f98531f6d30444f59c4)

## Contributing

Contributions are welcome! Areas for improvement:
- Additional device support
- Better error recovery
- Performance optimizations
- Documentation improvements
- Testing on different hardware

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Original project: https://github.com/a1exdandy/checkm8-a5

## Support

- **Issues:** Check [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) first
- **Hardware:** See [HARDWARE_SETUP.md](docs/HARDWARE_SETUP.md)
- **Questions:** Review documentation before asking

---

**Remember:** This exploit demotes your device permanently. Understand the implications before proceeding.
