# RadPro CMake Quick Start

## TL;DR - Get Building in 60 Seconds

### 1. Install Prerequisites

**Windows:**
```powershell
# Install ARM toolchain from:
# https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm
# Add to PATH, then restart terminal

# Install CMake and Ninja (via Chocolatey)
choco install cmake ninja
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install cmake ninja-build gcc-arm-none-eabi
```

**macOS:**
```bash
brew install cmake ninja arm-none-eabi-gcc
```

### 2. Verify Environment

```bash
./check-build-env.sh    # Linux/macOS
check-build-env.bat     # Windows
```

### 3. Build

```bash
./cmake-build.sh        # Linux/macOS - builds default board
cmake-build.bat         # Windows - builds default board
```

### 4. Flash

```bash
# Find your firmware in: build/fnirsi-gc01_ch32f103r8/
# Flash with your preferred tool (st-flash, OpenOCD, etc.)
```

## Common Commands

### Build with CMake Presets

```bash
# List available presets
cmake --list-presets

# Configure
cmake --preset fnirsi-gc01_ch32f103r8

# Build
cmake --build --preset fnirsi-gc01_ch32f103r8

# Output in: build/fnirsi-gc01_ch32f103r8/
```

### Build with Wrapper Script

```bash
# Default board (FNIRSI GC-01 CH32)
./cmake-build.sh

# Specific board
./cmake-build.sh --board fnirsi-gc03

# Different language
./cmake-build.sh --language de

# Clean build
./cmake-build.sh --clean

# Simulator
./cmake-build.sh --simulator

# Help
./cmake-build.sh --help
```

### Manual CMake

```bash
mkdir build && cd build
cmake -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-none-eabi.cmake \
  -DRADPRO_BOARD=fnirsi-gc01_ch32f103r8 \
  -DRADPRO_LANGUAGE=en \
  ..
ninja
```

## Supported Boards

Use with `-DRADPRO_BOARD=<name>` or `--board <name>`:

- `fnirsi-gc01_ch32f103r8` - FNIRSI GC-01 (CH32, default)
- `fnirsi-gc01_apm32f103rb` - FNIRSI GC-01 (APM32)
- `fnirsi-gc03` - FNIRSI GC-03
- `fs2011-stm32f051c8` - FS2011 (STM32)
- `fs2011-gd32f150c8` - FS2011 (GD32F150)
- `fs2011-gd32f103c8` - FS2011 (GD32F103)
- `bosean-fs600` - Bosean FS600
- `bosean-fs1000` - Bosean FS1000
- `bosean-fs5000` - Bosean FS5000
- `bosean-fs5000_landscape` - Bosean FS5000 (landscape)
- `gq-gmc800` - GQ GMC-800
- `gq-gmc800_landscape` - GQ GMC-800 (landscape)

## Supported Languages

Use with `-DRADPRO_LANGUAGE=<code>` or `--language <code>`:

`en`, `de`, `fr`, `es`, `it`, `pt`, `nl`, `ru`, `pl`, `cs`, `sk`, `hu`, `ro`, `bg`, `hr`, `sl`, `sr`, `uk`, `el`, `tr`, `da`, `fi`, `no`, `sv`, `id`, `vi`, `ja`, `ko`, `zh_CN`

## Output Files

After building, find in `build/<board>/`:

- `<board>.elf` - Executable with debug symbols
- `<board>.bin` - Binary for flashing
- `<board>.hex` - Intel HEX format
- `radpro.map` - Linker map file

## Flashing Examples

```bash
# st-flash (STM32)
st-flash write build/fnirsi-gc01_ch32f103r8/fnirsi-gc01_ch32f103r8.bin 0x08004000

# OpenOCD
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
  -c "program build/fnirsi-gc01_ch32f103r8/fnirsi-gc01_ch32f103r8.bin 0x08004000 verify reset exit"

# WCH-Link (for CH32)
wlink flash build/fnirsi-gc01_ch32f103r8/fnirsi-gc01_ch32f103r8.bin
```

## Troubleshooting

### ARM toolchain not found
```bash
# Verify installation
arm-none-eabi-gcc --version

# Add to PATH if needed (adjust path)
export PATH="/usr/local/gcc-arm-none-eabi/bin:$PATH"  # Linux/macOS
set PATH=C:\Program Files\ARM\bin;%PATH%              # Windows
```

### CMake version too old
```bash
# Need CMake >= 3.19
cmake --version

# Update CMake from: https://cmake.org/download/
```

### Ninja not found
```bash
# Optional - can use Make instead
cmake -G "Unix Makefiles" ...  # Linux/macOS
cmake -G "NMake Makefiles" ... # Windows with MSVC
```

### Build fails - undefined references
- Make sure you're using the ARM toolchain, not native GCC
- Verify toolchain file is specified: `-DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake`

## IDE Integration

### VS Code

1. Install "CMake Tools" extension
2. Select kit: "GCC for arm-none-eabi"
3. Select preset: "fnirsi-gc01_ch32f103r8"
4. Build with F7 or Ctrl+Shift+B

### CLion

1. File → Settings → Build → CMake
2. Add preset: "fnirsi-gc01_ch32f103r8"
3. Build → Build Project (Ctrl+F9)

### Visual Studio

1. File → Open → CMake
2. Select preset from dropdown
3. Build → Build All

## More Information

- **Full Build Guide:** [BUILD.md](BUILD.md)
- **Conversion Details:** [CMAKE_CONVERSION.md](CMAKE_CONVERSION.md)
- **Changes Log:** [CHANGELOG_CMAKE.md](CHANGELOG_CMAKE.md)
- **CMake Configuration:** [CMakeLists.txt](CMakeLists.txt)

## Need Help?

1. Run environment check: `./check-build-env.sh`
2. Check BUILD.md troubleshooting section
3. Verify ARM toolchain is in PATH
4. Make sure you're in the project root directory
5. Try a clean build: `./cmake-build.sh --clean`
