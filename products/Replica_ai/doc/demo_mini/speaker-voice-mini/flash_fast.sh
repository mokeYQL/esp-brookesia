#!/bin/bash

# Quick flash script for speaker-voice-mini
# Only flashes the main application binary by default for faster development
# Usage: ./flash_fast.sh [--all] [--port PORT] [--baud BAUD]

# Default values
PORT="/dev/ttyACM0"
BAUD="921600"
CHIP="esp32s3"
FLASH_MODE="dio"
FLASH_FREQ="80m"
FLASH_SIZE="16MB"
BUILD_DIR="./build"
MAIN_BIN="speaker_audio_mini.bin"
ALL=false

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --all)
            ALL=true
            shift
            ;;
        --port)
            PORT="$2"
            shift 2
            ;;
        --baud)
            BAUD="$2"
            shift 2
            ;;
        --main-bin)
            MAIN_BIN="$2"
            shift 2
            ;;
        *)
            echo "Unknown parameter: $1"
            echo "Usage: $0 [--all] [--port PORT] [--baud BAUD] [--main-bin MAIN_BIN]"
            exit 1
            ;;
    esac
done

# Check if esptool is available
esptool_path="$(which esptool.py)"
if [ -z "$esptool_path" ]; then
    # Try to find esptool in ESP-IDF path
    esptool_path="$HOME/.espressif/python_env/idf5.5_py3.10_env/bin/python $HOME/esp/v5.5.1/esp-idf/components/esptool_py/esptool/esptool.py"
    if [ ! -f "$(echo $esptool_path | awk '{print $2}')" ]; then
        echo "Error: esptool.py not found. Please make sure ESP-IDF is properly setup."
        exit 1
    fi
fi

# Base esptool command
esptool_cmd="$esptool_path -p $PORT -b $BAUD --before default_reset --after hard_reset --chip $CHIP write_flash --flash_mode $FLASH_MODE --flash_freq $FLASH_FREQ --flash_size $FLASH_SIZE"

# Build flash command based on options
if [ "$ALL" = true ]; then
    # Full flash (all files)
    flash_cmd="$esptool_cmd 0x0 $BUILD_DIR/bootloader/bootloader.bin 0xb0000 $BUILD_DIR/$MAIN_BIN 0x8000 $BUILD_DIR/partition_table/partition-table.bin 0xd000 $BUILD_DIR/ota_data_initial.bin 0x10000 $BUILD_DIR/srmodels/srmodels.bin 0xc4e000 $BUILD_DIR/mmap_build/boot/anim_boot/anim_boot.bin 0x92f000 $BUILD_DIR/mmap_build/emotion/anim_emotion/anim_emotion.bin 0xb0a000 $BUILD_DIR/mmap_build/icon/anim_icon/anim_icon.bin 0x880000 $BUILD_DIR/spiffs_data.bin"
else
    # Quick flash (only main application)
    flash_cmd="$esptool_cmd 0xb0000 $BUILD_DIR/$MAIN_BIN"
fi

# Check if required files exist
if [ "$ALL" = true ]; then
    required_files=("$BUILD_DIR/bootloader/bootloader.bin" "$BUILD_DIR/$MAIN_BIN" "$BUILD_DIR/partition_table/partition-table.bin" "$BUILD_DIR/ota_data_initial.bin" "$BUILD_DIR/srmodels/srmodels.bin")
else
    required_files=("$BUILD_DIR/$MAIN_BIN")
fi

for file in "${required_files[@]}"; do
    if [ ! -f "$file" ]; then
        echo "Error: Required file $file not found. Please build the project first."
        exit 1
    fi
done

# Execute flash command
echo "Executing flash command..."
echo "$flash_cmd"
$flash_cmd

if [ $? -eq 0 ]; then
    echo "\nFlash completed successfully!"
    if [ "$ALL" = false ]; then
        echo "Note: Only main application was flashed. Use --all to flash all files."
    fi
else
    echo "\nFlash failed!"
    exit 1
fi