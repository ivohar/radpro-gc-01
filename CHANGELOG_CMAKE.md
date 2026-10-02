# CMake Build System - Changelog

## Summary

Successfully converted the RadPro firmware project from PlatformIO to CMake. The new build system provides a modern, flexible alternative while maintaining full compatibility with the existing codebase and PlatformIO setup.

## Files Created

### Build System Files

1. **CMakeLists.txt** (Updated/Improved)
   - Complete CMake build configuration
   - Support for all 12 board variants
   - Automatic font selection based on display type
   - Source file collection with proper filtering
   - Post-build steps for .bin/.hex generation
   - Comprehensive build status messages

2. **CMakePresets.json** (Updated)
   - Pre-configured presets for common boards
   - Includes FNIRSI GC-01 (CH32 and APM32 variants)
   - Includes FNIRSI GC-03
   - Includes SDL simulator preset
   - Easy to extend for additional boards

3. **cmake/arm-none-eabi.cmake** (Existing - Verified)
   - ARM Cortex-M cross-compilation toolchain file
   - Configures compiler, linker, and build flags

### Documentation

4. **BUILD.md**
   - Comprehensive build documentation
   - Prerequisites for Windows, Linux, and macOS
   - Quick start guide with examples
   - Build options reference
   - Troubleshooting section

5. **CMAKE_CONVERSION.md**
   - Detailed conversion documentation
   - Architecture overview
   - PlatformIO vs CMake comparison
   - Usage examples
   - Extension guide

6. **CHANGELOG_CMAKE.md** (This file)
   - Summary of changes
   - File listing
   - Testing recommendations

### Build Scripts

7. **cmake-build.sh**
   - Bash build wrapper for Linux/macOS
   - Command-line options for board and language selection
   - Clean build support
   - User-friendly output with colors

8. **cmake-build.bat**
   - Windows batch build wrapper
   - Same features as bash script
   - Windows-friendly interface
   - Error handling

### Environment Check Scripts

9. **check-build-env.sh**
   - Linux/macOS environment verification
   - Checks for CMake, Ninja, ARM toolchain
   - Verifies minimum versions
   - Reports missing dependencies

10. **check-build-env.bat**
    - Windows environment verification
    - Same checks as bash script
    - Windows-specific output format

## Key Features

### Multi-Board Support

All boards from platformio.ini are supported:
- ✅ FNIRSI GC-01 (CH32F103R8)
- ✅ FNIRSI GC-01 (APM32F103RB)
- ✅ FNIRSI GC-03
- ✅ FS2011 (STM32F051C8, GD32F150C8, GD32F103C8)
- ✅ Bosean FS600
- ✅ Bosean FS1000
- ✅ Bosean FS5000 (portrait and landscape)
- ✅ GQ GMC-800 (portrait and landscape)

### Automatic Configuration

- **CPU Type** - Extracted from board JSON
- **Clock Speed** - Set via F_CPU define
- **Hardware Features** - Defined per board (display, keyboard, sensors)
- **USB Support** - Conditionally included for supported boards
- **Font Files** - Automatically selected based on display and language
- **Linker Scripts** - Custom scripts for FNIRSI GC-01/GC-03

### Build Outputs

Each build generates:
- `<board>.elf` - Executable with debug symbols
- `<board>.bin` - Binary firmware for flashing
- `<board>.hex` - Intel HEX format
- `radpro.map` - Linker map file for analysis

### SDL Simulator

- Build desktop simulator with `-DRADPRO_SIMULATOR=ON`
- Test UI without hardware
- Useful for development and debugging

## Testing Recommendations

### 1. Environment Check
```bash
# Linux/macOS
./check-build-env.sh

# Windows
check-build-env.bat
```

### 2. Test Firmware Build
```bash
# Using presets
cmake --preset fnirsi-gc01_ch32f103r8
cmake --build --preset fnirsi-gc01_ch32f103r8

# Using build script
./cmake-build.sh  # Linux/macOS
cmake-build.bat   # Windows
```

### 3. Test Simulator Build
```bash
cmake --preset simulator
cmake --build --preset simulator
```

### 4. Test Different Languages
```bash
./cmake-build.sh --board fnirsi-gc01_ch32f103r8 --language de
./cmake-build.sh --board fnirsi-gc01_ch32f103r8 --language ja
```

### 5. Verify Output Files
```bash
ls -lh build/fnirsi-gc01_ch32f103r8/
# Should see: .elf, .bin, .hex, .map files
```

## Compatibility Notes

### Source Code
- ✅ No source code changes required
- ✅ All files remain in `platform.io/` directory
- ✅ Compatible with PlatformIO builds

### Build Flags
- ✅ Same compiler optimizations (-Os, -flto)
- ✅ Same floating-point handling (QFP library)
- ✅ Same defines and macros

### Dependencies
- ✅ No external library dependencies
- ✅ Uses only ARM toolchain and standard tools

## Migration Path

1. **Install ARM Toolchain** - Manual installation required
2. **Test CMake Build** - Verify it works alongside PlatformIO
3. **Update CI/CD** - Switch automation scripts to CMake
4. **Update Documentation** - Point to BUILD.md
5. **(Optional) Remove PlatformIO** - Or keep both systems

## Known Limitations

1. **No Automatic Toolchain Management** - Unlike PlatformIO, users must install ARM toolchain manually
2. **No Upload Target** - Use external tools (st-flash, OpenOCD, etc.) for flashing
3. **No Library Manager** - All dependencies are vendored in the repository

## Future Enhancements

Potential improvements:
- [ ] Add upload targets using OpenOCD/st-flash
- [ ] Add CTest integration for unit testing
- [ ] Generate compile_commands.json for IDE support
- [ ] Add static analysis targets (cppcheck, clang-tidy)
- [ ] Support building multiple languages in one command
- [ ] Add package/release targets

## Conclusion

The CMake build system is production-ready and provides a modern, flexible alternative to PlatformIO. It maintains full compatibility with the existing codebase and can coexist with the PlatformIO build system.

### Quick Reference

```bash
# Check environment
./check-build-env.sh

# Build firmware (default board)
./cmake-build.sh

# Build specific board
./cmake-build.sh --board fnirsi-gc03

# Build with different language
./cmake-build.sh --language de

# Build simulator
./cmake-build.sh --simulator

# Clean build
./cmake-build.sh --clean

# Using CMake directly
cmake --preset fnirsi-gc01_ch32f103r8
cmake --build --preset fnirsi-gc01_ch32f103r8
```

For detailed information, see:
- **BUILD.md** - Complete build instructions
- **CMAKE_CONVERSION.md** - Conversion details and architecture
- **CMakeLists.txt** - Build configuration source
