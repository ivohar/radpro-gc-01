#!/bin/bash
# Bash script to vendor CMSIS headers from STMicroelectronics GitHub repositories
# This downloads the required CMSIS headers to remove PlatformIO dependency

set -e

VENDOR_DIR="vendor/cmsis"
TEMP_DIR="vendor/temp"

echo "========================================"
echo "  Vendoring CMSIS Headers"
echo "========================================"
echo ""

# Create vendor directory
echo "Creating vendor directory: $VENDOR_DIR"
mkdir -p "$VENDOR_DIR"

# Function to download and extract a repository
vendor_repo() {
    local repo=$1
    local tag=$2
    local source_path=$3
    local dest_name=$4
    
    echo ""
    echo "Processing $repo ($tag)..."
    
    # Clone the repository at the specific tag
    git clone --branch "$tag" --depth 1 "https://github.com/STMicroelectronics/$repo.git" "$TEMP_DIR"
    
    # Copy the Include directory
    mkdir -p "$VENDOR_DIR/$dest_name"
    cp -r "$TEMP_DIR/$source_path" "$VENDOR_DIR/$dest_name/"
    
    # Cleanup
    rm -rf "$TEMP_DIR"
    
    echo "✓ Completed $repo"
}

# CMSIS Core (ARM Cortex-M core definitions)
vendor_repo "cmsis-core" "v5.6.0_cm0" "CMSIS/Core/Include" "core"

# STM32F0 family
vendor_repo "cmsis_device_f0" "v2.3.7" "Include" "stm32f0"

# STM32F1 family (CH32, APM32, STM32F1)
vendor_repo "cmsis_device_f1" "v4.3.4" "Include" "stm32f1"

# STM32F2 family (CH32F2)
vendor_repo "cmsis_device_f2" "v2.2.6" "Include" "stm32f2"

# STM32F3 family (GD32F3)
vendor_repo "cmsis_device_f3" "v2.3.7" "Include" "stm32f3"

# STM32G0 family (Bosean FS600/FS1000)
vendor_repo "cmsis_device_g0" "v1.4.5" "Include" "stm32g0"

# STM32L4 family (Bosean FS5000)
vendor_repo "cmsis_device_l4" "v1.7.3" "Include" "stm32l4"

echo ""
echo "========================================"
echo "  CMSIS Headers Vendored Successfully!"
echo "========================================"
echo ""
echo "Next steps:"
echo "  1. Review the vendored headers in $VENDOR_DIR"
echo "  2. Update CMakeLists.txt to use vendored headers"
echo "  3. Remove PLATFORMIO_PACKAGES_DIR references"
echo ""

# Display directory structure
echo "Vendored directory structure:"
find "$VENDOR_DIR" -type d -maxdepth 2 | sort
echo ""
