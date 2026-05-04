
## Introduction

**Rad Pro** is a custom firmware designed to enhance the capabilities of supported Geiger counters. It provides advanced measurement tools, data logging, alerting, and visualization features beyond stock firmware.

This fork more or less tracks the original [Gissio/radpro](https://github.com/Gissio/radpro) repo, but is oriented to customizations for older FNIRSI GC-01 HW, specifically the versions with ch32f103r8 and not-so-high quality display. Currently, it addresses the display deficiencies by slightly changing the color space and adopting different font sizes. It also adds some GC-01-related bug-fixes and changes to other parts of the code. 

For more info about installation, etc, please visit the original repo [here](https://github.com/Gissio/radpro)

## CMake Build

This repository has been migrated to a CMake-oriented firmware build. The firmware sources remain under `platform.io/` and are now built directly from the top-level `CMakeLists.txt`.

### Recommended build

```powershell
cmake -S . -B build -G Ninja -DRADPRO_BOARD=fnirsi-gc01_ch32f103r8 -DRADPRO_LANGUAGE=en -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake
cmake --build build
cmake --build build --target radpro
```

If Ninja is not installed, install it or use another cross-build generator that supports the ARM toolchain.

If your CMake supports presets, the included `CMakePresets.json` provides a ready configuration for `fnirsi-gc01_ch32f103r8`:

```powershell
cmake --preset fnirsi-gc01_ch32f103r8
cmake --build --preset fnirsi-gc01_ch32f103r8
```

The firmware binary is generated as `build/radpro.bin`.

