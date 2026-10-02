# Building RadPro with CMake

This document describes how to build the RadPro firmware using CMake instead of PlatformIO.

## Prerequisites

### For Firmware Builds

1. **ARM GNU Toolchain**
   - Download from: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
   - Add `arm-none-eabi-gcc` to your PATH
   - Required tools: `arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, `arm-none-eabi-size`

2. **CMake** (version 3.19 or later)
   - Download from: https://cmake.org/download/

3. **Ninja Build System** (recommended)
   - Download from: https://ninja-build.org/
   - Or use `make` as an alternative

### For Simulator Builds

1. **SDL2 Library**
   - Windows: Install via vcpkg or download from https://www.libsdl.org/
   - Linux: `sudo apt-get install libsdl2-dev`
   - macOS: `brew install sdl2`

2. **Standard C/C++ Compiler** (GCC, Clang, or MSVC)

## Quick Start

### Using CMake Presets (Recommended)

The project includes pre-configured CMake presets for common build configurations:

```bash
# List available presets
cmake --list-presets

# Configure for FNIRSI GC-01 CH32F103R8
cmake --preset fnirsi-gc01_ch32f103r8

# Build
cmake --build --preset fnirsi-gc01_ch32f103r8

# Or use ninja directly
ninja -C build/fnirsi-gc01_ch32f103r8
```

Available presets:
- `fnirsi-gc01_ch32f103r8` - FNIRSI GC-01 with CH32F103R8 MCU (English)
- `fnirsi-gc01_apm32f103rb` - FNIRSI GC-01 with APM32F103RB MCU (English)
- `fnirsi-gc03` - FNIRSI GC-03 (English)
- `simulator` - SDL-based desktop simulator

### Manual Configuration

If you prefer manual configuration:

```bash
# Configure for firmware build
mkdir build
cd build
cmake -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake \
  -DRADPRO_BOARD=fnirsi-gc01_ch32f103r8 \
  -DRADPRO_LANGUAGE=en \
  ..

# Build
ninja

# Or for simulator
mkdir build-sim
cd build-sim
cmake -G Ninja \
  -DRADPRO_SIMULATOR=ON \
  -DRADPRO_LANGUAGE=en \
  ..
ninja
```

## Build Options

### RADPRO_BOARD

Selects the target hardware board. Available options:

- `fnirsi-gc01_ch32f103r8` - FNIRSI GC-01 with CH32F103R8
- `fnirsi-gc01_apm32f103rb` - FNIRSI GC-01 with APM32F103RB  
- `fnirsi-gc03` - FNIRSI GC-03
- `fs2011-stm32f051c8` - FS2011 with STM32F051C8
- `fs2011-gd32f150c8` - FS2011 with GD32F150C8
- `fs2011-gd32f103c8` - FS2011 with GD32F103C8
- `bosean-fs600` - Bosean FS600
- `bosean-fs1000` - Bosean FS1000
- `bosean-fs5000` - Bosean FS5000
- `bosean-fs5000_landscape` - Bosean FS5000 (landscape)
- `gq-gmc800` - GQ GMC-800
- `gq-gmc800_landscape` - GQ GMC-800 (landscape)

### RADPRO_LANGUAGE

Selects the UI language and corresponding fonts. Examples: `en`, `de`, `fr`, `es`, `ja`, `zh_CN`, etc.

### RADPRO_SIMULATOR

Set to `ON` to build the SDL-based simulator instead of firmware.

## Output Files

After a successful firmware build, you will find:

- `build/<board>/<board>.elf` - ELF executable
- `build/<board>/<board>.bin` - Binary firmware image
- `build/<board>/<board>.hex` - Intel HEX firmware image
- `build/<board>/radpro.map` - Linker map file

## Flashing Firmware

The CMake build produces `.bin` and `.hex` files that can be flashed using your preferred tool:

```bash
# Example using st-flash (STM32)
st-flash write build/fnirsi-gc01_ch32f103r8/fnirsi-gc01_ch32f103r8.bin 0x08004000

# Example using OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
  -c "program build/fnirsi-gc01_ch32f103r8/fnirsi-gc01_ch32f103r8.bin 0x08004000 verify reset exit"
```

Note: The flash offset address varies by board (see linker scripts in `platform.io/scripts/`).

## Differences from PlatformIO

The CMake build system closely mirrors the PlatformIO configuration:

1. **Board Definitions**: Stored in `platform.io/boards/*.json` files
2. **Build Flags**: Defined per-board in `CMakeLists.txt`, matching `platformio.ini`
3. **Linker Scripts**: Same scripts used (`platform.io/scripts/*.ld`)
4. **Source Layout**: No changes to source code organization

## Troubleshooting

### ARM Toolchain Not Found

Ensure `arm-none-eabi-gcc` is in your PATH:
```bash
arm-none-eabi-gcc --version
```

### SDL2 Not Found (Simulator)

Make sure SDL2 is properly installed and CMake can find it. You may need to set `SDL2_DIR`:
```bash
cmake -DSDL2_DIR=/path/to/sdl2 ...
```

### Linker Errors

Check that you're using the correct board configuration and that the linker script exists.

## Additional Resources

- Project Documentation: `docs/`
- PlatformIO Config: `platform.io/platformio.ini`
- ARM Toolchain File: `cmake/arm-none-eabi.cmake`
