#!/bin/bash
# GMpro87dev Firmware Build Script
# Run this script in the project directory: bash compile.sh

echo "=========================================="
echo "  GMpro87dev Firmware Build Script"
echo "=========================================="
echo ""

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    echo "Error: PlatformIO not found!"
    echo "Please install PlatformIO: https://docs.platformio.org/page/installation.html"
    exit 1
fi

# Clean previous builds
echo "Cleaning previous builds..."
rm -rf .pio/build 2>/dev/null

# Install dependencies
echo "Installing dependencies..."
pio pkg install

# Build firmware
echo ""
echo "Building firmware..."
pio run

# Check if build was successful
if [ -f ".pio/build/esp12e/firmware.bin" ]; then
    echo ""
    echo "=========================================="
    echo "  Build successful!"
    echo "=========================================="
    echo ""
    echo "Firmware location: .pio/build/esp12e/firmware.bin"
    
    # Copy to main directory with new name
    cp .pio/build/esp12e/firmware.bin /WifiX-Version-1.5/GMpro87dev.bin
    echo "Copied to: GMpro87dev.bin"
    echo ""
    echo "To flash to ESP8266:"
    echo "  pio run --target upload"
    echo "  OR use esptool:"
    echo "  esptool.py --port /dev/ttyUSB0 write_flash 0x00000 GMpro87dev.bin"
else
    echo ""
    echo "=========================================="
    echo "  Build FAILED!"
    echo "=========================================="
    echo "Please check the errors above."
    exit 1
fi
