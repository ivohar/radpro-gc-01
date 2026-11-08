set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Optional toolchain location (directory containing arm-none-eabi-gcc), for when
# the toolchain is not on PATH:  -DARM_TOOLCHAIN_DIR=<dir>  or env ARM_TOOLCHAIN_DIR
if(NOT ARM_TOOLCHAIN_DIR AND DEFINED ENV{ARM_TOOLCHAIN_DIR})
    set(ARM_TOOLCHAIN_DIR "$ENV{ARM_TOOLCHAIN_DIR}")
endif()
if(ARM_TOOLCHAIN_DIR)
    file(TO_CMAKE_PATH "${ARM_TOOLCHAIN_DIR}" ARM_TOOLCHAIN_DIR)
    set(_arm_prefix "${ARM_TOOLCHAIN_DIR}/arm-none-eabi-")
else()
    set(_arm_prefix "arm-none-eabi-")
endif()
if(CMAKE_HOST_WIN32 AND ARM_TOOLCHAIN_DIR)
    set(_arm_suffix ".exe")
else()
    set(_arm_suffix "")
endif()
# Make the value visible inside try_compile() projects as well
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES ARM_TOOLCHAIN_DIR)

# Cross compiler toolchain binaries for ARM Cortex-M builds.
set(CMAKE_C_COMPILER "${_arm_prefix}gcc${_arm_suffix}")
set(CMAKE_ASM_COMPILER "${_arm_prefix}gcc${_arm_suffix}")
set(CMAKE_OBJCOPY "${_arm_prefix}objcopy${_arm_suffix}")
set(CMAKE_SIZE "${_arm_prefix}size${_arm_suffix}")

# Bare-metal: there is no startup code or linker script during CMake's compiler
# checks, so test-compile a static library instead of linking an executable.
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Never look for programs in the target environment, only for libraries/headers
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_C_FLAGS_INIT "-Os -ffunction-sections -fdata-sections -fno-common")
set(CMAKE_ASM_FLAGS_INIT "-Os -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostdlib -Wl,--gc-sections")
