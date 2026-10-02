#!/bin/bash
# RadPro CMake Build Script
# This script simplifies building RadPro firmware with CMake

set -e

# Default values
BOARD="fnirsi-gc01_ch32f103r8"
LANGUAGE="en"
BUILD_TYPE="Release"
CLEAN=false
SIMULATOR=false

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Print usage
usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -b, --board BOARD       Target board (default: fnirsi-gc01_ch32f103r8)"
    echo "  -l, --language LANG     UI language (default: en)"
    echo "  -s, --simulator         Build SDL simulator instead of firmware"
    echo "  -c, --clean             Clean build directory before building"
    echo "  -h, --help              Show this help message"
    echo ""
    echo "Supported boards:"
    echo "  fnirsi-gc01_ch32f103r8, fnirsi-gc01_apm32f103rb, fnirsi-gc03"
    echo "  fs2011-stm32f051c8, fs2011-gd32f150c8, fs2011-gd32f103c8"
    echo "  bosean-fs600, bosean-fs1000, bosean-fs5000, bosean-fs5000_landscape"
    echo "  gq-gmc800, gq-gmc800_landscape"
    echo ""
    echo "Examples:"
    echo "  $0                                    # Build default board (FNIRSI GC-01 CH32F103R8)"
    echo "  $0 --board fnirsi-gc03 --language de # Build FNIRSI GC-03 with German UI"
    echo "  $0 --simulator                        # Build SDL simulator"
    echo "  $0 --clean --board fnirsi-gc01_ch32f103r8  # Clean build"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -b|--board)
            BOARD="$2"
            shift 2
            ;;
        -l|--language)
            LANGUAGE="$2"
            shift 2
            ;;
        -s|--simulator)
            SIMULATOR=true
            shift
            ;;
        -c|--clean)
            CLEAN=true
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            usage
            exit 1
            ;;
    esac
done

# Set build directory
if [ "$SIMULATOR" = true ]; then
    BUILD_DIR="build/simulator"
    BUILD_NAME="simulator"
else
    BUILD_DIR="build/${BOARD}"
    BUILD_NAME="${BOARD}"
fi

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}RadPro CMake Build${NC}"
echo -e "${GREEN}========================================${NC}"
if [ "$SIMULATOR" = true ]; then
    echo -e "${YELLOW}Target:${NC} SDL Simulator"
else
    echo -e "${YELLOW}Board:${NC} ${BOARD}"
fi
echo -e "${YELLOW}Language:${NC} ${LANGUAGE}"
echo -e "${YELLOW}Build Dir:${NC} ${BUILD_DIR}"
echo -e "${GREEN}========================================${NC}"
echo ""

# Clean if requested
if [ "$CLEAN" = true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf "${BUILD_DIR}"
fi

# Create build directory
mkdir -p "${BUILD_DIR}"

# Configure
echo -e "${GREEN}Configuring...${NC}"
if [ "$SIMULATOR" = true ]; then
    cmake -G Ninja -B "${BUILD_DIR}" \
        -DRADPRO_SIMULATOR=ON \
        -DRADPRO_LANGUAGE="${LANGUAGE}" \
        -DCMAKE_BUILD_TYPE=Debug
else
    cmake -G Ninja -B "${BUILD_DIR}" \
        -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake \
        -DRADPRO_BOARD="${BOARD}" \
        -DRADPRO_LANGUAGE="${LANGUAGE}" \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
fi

# Build
echo ""
echo -e "${GREEN}Building...${NC}"
cmake --build "${BUILD_DIR}" --verbose

# Success message
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Build Successful!${NC}"
echo -e "${GREEN}========================================${NC}"

if [ "$SIMULATOR" = true ]; then
    echo -e "${YELLOW}Executable:${NC} ${BUILD_DIR}/radpro-simulator"
else
    echo -e "${YELLOW}Firmware files:${NC}"
    echo "  ${BUILD_DIR}/${BUILD_NAME}.elf"
    echo "  ${BUILD_DIR}/${BUILD_NAME}.bin"
    echo "  ${BUILD_DIR}/${BUILD_NAME}.hex"
    echo "  ${BUILD_DIR}/radpro.map"
fi
echo -e "${GREEN}========================================${NC}"
