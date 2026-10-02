# Vendoring CMSIS Headers - Instructions

This document explains how to vendor the CMSIS headers from STMicroelectronics' official GitHub repositories to remove the PlatformIO dependency.

## Required CMSIS Packages

Based on the board configurations in this project, you need to download the following CMSIS packages:

### 1. CMSIS Core (ARM Cortex-M)
- **Repository**: https://github.com/STMicroelectronics/cmsis-core
- **Tag/Version**: v5.6.0_cm0
- **What to copy**: `CMSIS/Core/Include/` directory
- **Destination**: `vendor/cmsis/core/Include/`

### 2. STM32F0 Family
- **Repository**: https://github.com/STMicroelectronics/cmsis_device_f0
- **Tag/Version**: v2.3.7
- **What to copy**: `Include/` directory
- **Destination**: `vendor/cmsis/stm32f0/Include/`

### 3. STM32F1 Family (CH32, APM32, STM32F1)
- **Repository**: https://github.com/STMicroelectronics/cmsis_device_f1
- **Tag/Version**: v4.3.4
- **What to copy**: `Include/` directory
- **Destination**: `vendor/cmsis/stm32f1/Include/`

### 4. STM32F2 Family (CH32F2)
- **Repository**: https://github.com/STMicroelectronics/cmsis_device_f2
- **Tag/Version**: v2.2.6
- **What to copy**: `Include/` directory
- **Destination**: `vendor/cmsis/stm32f2/Include/`

### 5. STM32G0 Family (Bosean FS600/FS1000)
- **Repository**: https://github.com/STMicroelectronics/cmsis_device_g0
- **Tag/Version**: v1.4.5
- **What to copy**: `Include/` directory
- **Destination**: `vendor/cmsis/stm32g0/Include/`

### 6. STM32L4 Family (Bosean FS5000)
- **Repository**: https://github.com/STMicroelectronics/cmsis_device_l4
- **Tag/Version**: v1.7.3
- **What to copy**: `Include/` directory
- **Destination**: `vendor/cmsis/stm32l4/Include/`

### 7. GD32F3 Family (GQ-GMC800)
- **Repository**: https://github.com/STMicroelectronics/cmsis_device_f3
- **Tag/Version**: v2.3.7
- **What to copy**: `Include/` directory
- **Destination**: `vendor/cmsis/stm32f3/Include/`

## Manual Download Steps

For each repository above:

1. Navigate to the repository URL in your browser
2. Click on "Tags" or "Releases"
3. Find the specified tag/version
4. Download the ZIP file (or use `git clone --branch <tag> --depth 1 <repo_url>`)
5. Extract the ZIP file
6. Copy the `Include/` directory (or `CMSIS/Core/Include/` for cmsis-core) to the destination folder
7. Delete the downloaded ZIP and extracted folder

## Automated Download (Using Git)

Alternatively, you can use git to download each repository:

```bash
# Create vendor directory
mkdir -p vendor/cmsis

# CMSIS Core
git clone --branch v5.6.0_cm0 --depth 1 https://github.com/STMicroelectronics/cmsis-core.git vendor/temp
cp -r vendor/temp/CMSIS/Core/Include vendor/cmsis/core/
rm -rf vendor/temp

# STM32F0
git clone --branch v2.3.7 --depth 1 https://github.com/STMicroelectronics/cmsis_device_f0.git vendor/temp
cp -r vendor/temp/Include vendor/cmsis/stm32f0/
rm -rf vendor/temp

# STM32F1
git clone --branch v4.3.4 --depth 1 https://github.com/STMicroelectronics/cmsis_device_f1.git vendor/temp
cp -r vendor/temp/Include vendor/cmsis/stm32f1/
rm -rf vendor/temp

# STM32F2
git clone --branch v2.2.6 --depth 1 https://github.com/STMicroelectronics/cmsis_device_f2.git vendor/temp
cp -r vendor/temp/Include vendor/cmsis/stm32f2/
rm -rf vendor/temp

# STM32F3
git clone --branch v2.3.7 --depth 1 https://github.com/STMicroelectronics/cmsis_device_f3.git vendor/temp
cp -r vendor/temp/Include vendor/cmsis/stm32f3/
rm -rf vendor/temp

# STM32G0
git clone --branch v1.4.5 --depth 1 https://github.com/STMicroelectronics/cmsis_device_g0.git vendor/temp
cp -r vendor/temp/Include vendor/cmsis/stm32g0/
rm -rf vendor/temp

# STM32L4
git clone --branch v1.7.3 --depth 1 https://github.com/STMicroelectronics/cmsis_device_l4.git vendor/temp
cp -r vendor/temp/Include vendor/cmsis/stm32l4/
rm -rf vendor/temp
```

## Expected Directory Structure

After vendoring, you should have:

```
vendor/cmsis/
├── core/
│   └── Include/
│       ├── cmsis_compiler.h
│       ├── cmsis_gcc.h
│       ├── cmsis_version.h
│       ├── core_cm0.h
│       ├── core_cm0plus.h
│       ├── core_cm3.h
│       ├── core_cm4.h
│       └── ...
├── stm32f0/
│   └── Include/
│       ├── stm32f0xx.h
│       ├── system_stm32f0xx.h
│       └── ...
├── stm32f1/
│   └── Include/
│       ├── stm32f1xx.h
│       ├── system_stm32f1xx.h
│       └── ...
└── ... (other families)
```

## Next Step

After vendoring the headers, update `CMakeLists.txt` to use the vendored headers instead of PlatformIO packages.
