#!/bin/bash
# RadPro Build Environment Check Script
# Verifies that all required tools are installed and properly configured

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

ERRORS=0
WARNINGS=0

echo "=========================================="
echo "RadPro Build Environment Check"
echo "=========================================="
echo ""

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to check version
check_version() {
    local cmd=$1
    local min_ver=$2
    local current_ver=$3
    
    echo "  Current version: $current_ver"
    if [ -n "$min_ver" ]; then
        echo "  Minimum version: $min_ver"
    fi
}

# Check CMake
echo "[1/8]"
echo "Checking CMake..."
if command_exists cmake; then
    CMAKE_VERSION=$(cmake --version | head -n1 | awk '{print $3}')
    check_version "cmake" "3.19" "$CMAKE_VERSION"

    # Simple version check
    CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d. -f1)
    CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d. -f2)
    if [ "$CMAKE_MAJOR" -lt 3 ] || ([ "$CMAKE_MAJOR" -eq 3 ] && [ "$CMAKE_MINOR" -lt 19 ]); then
        echo -e "  ${RED}[ERROR] CMake version too old (need >= 3.19)${NC}"
        ERRORS=$((ERRORS + 1))
    else
        echo -e "  ${GREEN}[OK] CMake OK${NC}"
    fi
else
    echo -e "  ${RED}[ERROR] CMake not found${NC}"
    ERRORS=$((ERRORS + 1))
fi
echo ""

# Check Ninja
echo "[2/8]"
echo "Checking Ninja..."
if command_exists ninja; then
    NINJA_VERSION=$(ninja --version)
    check_version "ninja" "" "$NINJA_VERSION"
    echo -e "  ${GREEN}[OK] Ninja OK${NC}"
else
    echo -e "  ${YELLOW}[WARNING] Ninja not found (optional, can use make instead)${NC}"
    WARNINGS=$((WARNINGS + 1))
fi
echo ""

# Check ARM GCC
echo "[3/8]"
echo "Checking ARM GCC..."
if command_exists arm-none-eabi-gcc; then
    ARM_GCC_VERSION=$(arm-none-eabi-gcc --version | head -n1)
    check_version "arm-none-eabi-gcc" "" "$ARM_GCC_VERSION"
    echo -e "  ${GREEN}[OK] ARM GCC OK${NC}"
else
    echo -e "  ${RED}[ERROR] arm-none-eabi-gcc not found${NC}"
    echo "    Install from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm"
    ERRORS=$((ERRORS + 1))
fi
echo ""

# Check ARM objcopy
echo "[4/8]"
echo "Checking ARM objcopy..."
if command_exists arm-none-eabi-objcopy; then
    echo -e "  ${GREEN}[OK] ARM objcopy OK${NC}"
else
    echo -e "  ${RED}[ERROR] arm-none-eabi-objcopy not found${NC}"
    ERRORS=$((ERRORS + 1))
fi
echo ""

# Check ARM size
echo "[5/8]"
echo "Checking ARM size..."
if command_exists arm-none-eabi-size; then
    echo -e "  ${GREEN}[OK] ARM size OK${NC}"
else
    echo -e "  ${YELLOW}[WARNING] arm-none-eabi-size not found (optional)${NC}"
    WARNINGS=$((WARNINGS + 1))
fi
echo ""

# Check SDL2 (for simulator)
echo "[6/8]"
echo "Checking SDL2 (for simulator)..."
if pkg-config --exists sdl2 2>/dev/null; then
    SDL2_VERSION=$(pkg-config --modversion sdl2)
    check_version "SDL2" "" "$SDL2_VERSION"
    echo -e "  ${GREEN}[OK] SDL2 OK${NC}"
else
    echo -e "  ${YELLOW}[WARNING] SDL2 not found (only needed for simulator)${NC}"
    WARNINGS=$((WARNINGS + 1))
fi
echo ""

# Check Git
echo "[7/8]"
echo "Checking Git..."
if command_exists git; then
    GIT_VERSION=$(git --version | awk '{print $3}')
    check_version "git" "" "$GIT_VERSION"
    echo -e "  ${GREEN}[OK] Git OK${NC}"
else
    echo -e "  ${YELLOW}[WARNING] Git not found (optional)${NC}"
    WARNINGS=$((WARNINGS + 1))
fi
echo ""

# Check board definitions
echo "[8/8]"
echo "Checking board definitions..."
BOARD_DIR="platform.io/boards"
if [ -d "$BOARD_DIR" ]; then
    BOARD_COUNT=$(ls -1 "$BOARD_DIR"/*.json 2>/dev/null | wc -l)
    echo "  Found $BOARD_COUNT board definition(s)"
    echo -e "  ${GREEN}[OK] Board definitions OK${NC}"
else
    echo -e "  ${RED}[ERROR] Board directory not found: $BOARD_DIR${NC}"
    ERRORS=$((ERRORS + 1))
fi
echo ""

# Summary
echo "=========================================="
echo "Summary"
echo "=========================================="
if [ $ERRORS -eq 0 ]; then
    echo -e "${GREEN}[OK] All required tools are installed!${NC}"
    if [ $WARNINGS -gt 0 ]; then
        echo -e "${YELLOW}[WARNING] $WARNINGS optional tool(s) missing${NC}"
    fi
    echo ""
    echo "You can now build RadPro firmware:"
    echo "  ./cmake-build.sh"
    echo "  OR"
    echo "  cmake --preset fnirsi-gc01_ch32f103r8 && cmake --build --preset fnirsi-gc01_ch32f103r8"
    exit 0
else
    echo -e "${RED}[ERROR] $ERRORS required tool(s) missing!${NC}"
    if [ $WARNINGS -gt 0 ]; then
        echo -e "${YELLOW}[WARNING] $WARNINGS optional tool(s) missing${NC}"
    fi
    echo ""
    echo "Please install the missing tools before building."
    echo "See BUILD.md for installation instructions."
    exit 1
fi
