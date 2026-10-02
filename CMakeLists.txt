cmake_minimum_required(VERSION 3.19)

# Project setup
project(radpro LANGUAGES C ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Build options
option(RADPRO_SIMULATOR "Build the SDL-based simulator target" OFF)
set(RADPRO_BOARD "fnirsi-gc01_ch32f103r8" CACHE STRING "Target board configuration for firmware builds")
set(RADPRO_LANGUAGE "en" CACHE STRING "Language for strings and font selection")

# Directory paths
set(RADPRO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/platform.io")
set(RADPRO_BOARD_DIR "${RADPRO_SOURCE_DIR}/boards")
set(RADPRO_SCRIPT_DIR "${RADPRO_SOURCE_DIR}/scripts")

# Supported board configurations
set(RADPRO_SUPPORTED_BOARDS
    fnirsi-gc01_ch32f103r8
    fnirsi-gc01_apm32f103rb
    fnirsi-gc03
    fs2011-stm32f051c8
    fs2011-gd32f150c8
    fs2011-gd32f103c8
    bosean-fs600
    bosean-fs1000
    bosean-fs5000
    bosean-fs5000_landscape
    gq-gmc800
    gq-gmc800_landscape
)

# Validate board selection
if(NOT RADPRO_SIMULATOR)
    if(NOT RADPRO_BOARD IN_LIST RADPRO_SUPPORTED_BOARDS)
        message(FATAL_ERROR "Unsupported RADPRO_BOARD: ${RADPRO_BOARD}\nSupported values: ${RADPRO_SUPPORTED_BOARDS}")
    endif()
endif()

# ============================================================================
# Helper Functions
# ============================================================================

# Map board name to JSON file name
function(radpro_board_json board out_json)
    if(board STREQUAL "fnirsi-gc01_ch32f103r8")
        set(${out_json} "ch32f103r8" PARENT_SCOPE)
    elseif(board STREQUAL "fnirsi-gc01_apm32f103rb")
        set(${out_json} "apm32f103rb" PARENT_SCOPE)
    elseif(board STREQUAL "fnirsi-gc03")
        set(${out_json} "ch32f203rc" PARENT_SCOPE)
    elseif(board STREQUAL "fs2011-stm32f051c8")
        set(${out_json} "stm32f051c8" PARENT_SCOPE)
    elseif(board STREQUAL "fs2011-gd32f150c8")
        set(${out_json} "gd32f150c8" PARENT_SCOPE)
    elseif(board STREQUAL "fs2011-gd32f103c8")
        set(${out_json} "gd32f103c8" PARENT_SCOPE)
    elseif(board STREQUAL "bosean-fs600" OR board STREQUAL "bosean-fs1000")
        set(${out_json} "stm32g070cb" PARENT_SCOPE)
    elseif(board STREQUAL "bosean-fs5000" OR board STREQUAL "bosean-fs5000_landscape")
        set(${out_json} "stm32l431rc" PARENT_SCOPE)
    elseif(board STREQUAL "gq-gmc800" OR board STREQUAL "gq-gmc800_landscape")
        set(${out_json} "gd32f303rc" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "No board JSON mapping for ${board}")
    endif()
endfunction()

# Extract value from JSON string (handles nested objects)
function(radpro_json_extract json key output)
    # Match the key anywhere in the JSON, capturing the value in quotes
    # Use .* to allow matching across newlines in CMake regex
    string(REGEX MATCH "\"${key}\"[ \t\r\n]*:[ \t\r\n]*\"([^\"]*)\"" _ "${json}")
    set(${output} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

# ============================================================================
# SDL Simulator Build
# ============================================================================

if(RADPRO_SIMULATOR)
    # Collect SDL simulator sources
    file(GLOB_RECURSE SDL_SOURCES CONFIGURE_DEPENDS
        "${RADPRO_SOURCE_DIR}/src/sdl/*.c"
        "${RADPRO_SOURCE_DIR}/lib/mcu-renderer/*.c"
        "${RADPRO_SOURCE_DIR}/lib/mcu-max/*.c"
    )

    # Create simulator executable
    add_executable(radpro-simulator ${SDL_SOURCES})

    target_include_directories(radpro-simulator PRIVATE
        "${RADPRO_SOURCE_DIR}/src"
        "${RADPRO_SOURCE_DIR}/lib/mcu-renderer"
        "${RADPRO_SOURCE_DIR}/lib/mcu-max"
    )

    target_compile_definitions(radpro-simulator PRIVATE
        SIMULATOR
        SIMULATOR_COMM
        GAME
        LANGUAGE="${RADPRO_LANGUAGE}"
        STRINGS="system/strings/${RADPRO_LANGUAGE}.h"
        MCURENDERER_SDL
    )

    target_compile_options(radpro-simulator PRIVATE -O3)

    # Find and link SDL2
    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(radpro-simulator PRIVATE
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2main>,SDL2::SDL2main,>
    )

    return()
endif()

# ============================================================================
# Firmware Build
# ============================================================================

# Read board configuration from JSON
radpro_board_json(${RADPRO_BOARD} RADPRO_BOARD_JSON)
set(RADPRO_BOARD_JSON_PATH "${RADPRO_BOARD_DIR}/${RADPRO_BOARD_JSON}.json")

if(NOT EXISTS "${RADPRO_BOARD_JSON_PATH}")
    message(FATAL_ERROR "Board configuration file not found: ${RADPRO_BOARD_JSON_PATH}")
endif()

file(READ "${RADPRO_BOARD_JSON_PATH}" RADPRO_BOARD_JSON_CONTENT)
radpro_json_extract("${RADPRO_BOARD_JSON_CONTENT}" cpu RADPRO_BOARD_CPU)
radpro_json_extract("${RADPRO_BOARD_JSON_CONTENT}" extra_flags RADPRO_BOARD_EXTRA_FLAGS)
radpro_json_extract("${RADPRO_BOARD_JSON_CONTENT}" f_cpu RADPRO_BOARD_F_CPU)

# Process extra flags from board JSON and extract defines
# The extra_flags string has format like: "-D STM32 -D STM32F1 -D FLASH_BASE_=0x08000000"
set(RADPRO_BOARD_JSON_DEFINES "")
if(RADPRO_BOARD_EXTRA_FLAGS)
    # Replace "-D " with "-D" to normalize
    string(REGEX REPLACE "-D " "-D" _normalized_flags "${RADPRO_BOARD_EXTRA_FLAGS}")
    # Split by spaces into a list
    string(REGEX REPLACE " +" ";" _flag_list "${_normalized_flags}")

    # Extract defines from each flag
    foreach(_flag ${_flag_list})
        if(_flag MATCHES "^-D(.+)")
            list(APPEND RADPRO_BOARD_JSON_DEFINES "${CMAKE_MATCH_1}")
        endif()
    endforeach()
endif()

# Base build flags (from platformio.ini [base] section)
set(RADPRO_BOARD_DEFINES
    QFP
    MCURENDERER_WITHOUT_BITMAP_SUPPORT
    ${RADPRO_BOARD_JSON_DEFINES}
)

set(RADPRO_BOARD_LINK_OPTIONS
    "-Wl,--undefined=main"
    "-Wl,--undefined=SystemInit"
    "-Wl,--undefined=_init"
    "-Wl,--defsym,__aeabi_fadd=qfp_fadd"
    "-Wl,--defsym,__aeabi_fsub=qfp_fsub"
    "-Wl,--defsym,__aeabi_fmul=qfp_fmul"
    "-Wl,--defsym,__aeabi_fdiv=qfp_fdiv"
    "-Wl,--defsym,__aeabi_f2iz=qfp_float2int"
    "-Wl,--defsym,__aeabi_f2uiz=qfp_float2uint"
    "-Wl,--defsym,__aeabi_i2f=qfp_int2float"
    "-Wl,--defsym,__aeabi_ui2f=qfp_uint2float"
    "-Wl,--defsym,__aeabi_fcmpeq=cmath_fcmpeq"
    "-Wl,--defsym,__aeabi_fcmplt=cmath_fcmplt"
    "-Wl,--defsym,__aeabi_fcmple=cmath_fcmple"
    "-Wl,--defsym,__aeabi_fcmpge=cmath_fcmpge"
    "-Wl,--defsym,__aeabi_fcmpgt=cmath_fcmpgt"
    "-Wl,--defsym,__aeabi_fcmpun=cmath_fcmpun"
    "-Wl,--defsym,logf=qfp_fln"
    "-Wl,--defsym,expf=qfp_fexp"
    "-Wl,--defsym,sqrtf=qfp_fsqrt"
)

# Board-specific configuration
set(RADPRO_BOARD_LINKER_SCRIPT "")
set(RADPRO_INCLUDE_USB FALSE)

if(RADPRO_BOARD STREQUAL "fnirsi-gc01_ch32f103r8")
    list(APPEND RADPRO_BOARD_DEFINES
        GC01
        FIRMWARE_BASE=0x08004000
        FIRMWARE_SIZE=0xb800
        START_POWERED
        USBD_ASM_DRIVER
        USB_PMASIZE=0x200
        TUBE_HV_PWM
        KEYBOARD_5_KEYS
        KEYBOARD_KEY_POWER_OK
        DISPLAY_320X240
        DISPLAY_COLOR
        BUZZER
        VIBRATOR
        PULSE_LED
        DISPLAY_EXTRA_COLOR_SCHEMES
    )
    set(RADPRO_INCLUDE_USB TRUE)
    set(RADPRO_BOARD_LINKER_SCRIPT "${RADPRO_SCRIPT_DIR}/gc01.ld")
elseif(RADPRO_BOARD STREQUAL "fnirsi-gc01_apm32f103rb")
    list(APPEND RADPRO_BOARD_DEFINES
        GC01
        FIRMWARE_BASE=0x08004000
        FIRMWARE_SIZE=0xe000
        START_POWERED
        USBD_ASM_DRIVER
        USB_PMASIZE=0x200
        TUBE_HV_PWM
        KEYBOARD_5_KEYS
        KEYBOARD_KEY_POWER_OK
        DISPLAY_320X240
        DISPLAY_COLOR
        BUZZER
        VIBRATOR
        PULSE_LED
        TUBE_DEFAULT_HVPROFILE_IN_FLASH
        GAME
    )
    set(RADPRO_INCLUDE_USB TRUE)
    set(RADPRO_BOARD_LINKER_SCRIPT "${RADPRO_SCRIPT_DIR}/gc01.ld")
elseif(RADPRO_BOARD STREQUAL "fnirsi-gc03")
    list(APPEND RADPRO_BOARD_DEFINES
        GC03
        FIRMWARE_BASE=0x08006000
        FIRMWARE_SIZE=0xe000
        USB_AUTOPOWER_ON
        USBD_ASM_DRIVER
        USB_PMASIZE=0x200
        TUBE_HV_PWM
        TUBE_DEFAULT_HVPROFILE_IN_FLASH
        EMFMETER
        KEYBOARD_5_KEYS
        KEYBOARD_KNOB
        DISPLAY_240X320
        DISPLAY_COLOR
        BUZZER
        VIBRATOR
        PULSE_LED
        ALERT_LED
        LED_MULTIPLEX
        GAME
    )
    set(RADPRO_INCLUDE_USB TRUE)
    set(RADPRO_BOARD_LINKER_SCRIPT "${RADPRO_SCRIPT_DIR}/gc03.ld")
elseif(RADPRO_BOARD STREQUAL "fs2011-stm32f051c8" OR RADPRO_BOARD STREQUAL "fs2011-gd32f150c8" OR RADPRO_BOARD STREQUAL "fs2011-gd32f103c8")
    list(APPEND RADPRO_BOARD_DEFINES
        FS2011
        FIRMWARE_BASE=0x08000000
        FIRMWARE_SIZE=0xa800
        FAST_SYSTEM_CLOCK
        BATTERY_REMOVABLE
        TUBE_HV_PWM
        KEYBOARD_5_KEYS
        DISPLAY_128X64
        DISPLAY_MONOCHROME
        BUZZER
        VIBRATOR
        DATA_MODE
        GAME
    )
elseif(RADPRO_BOARD STREQUAL "bosean-fs600")
    list(APPEND RADPRO_BOARD_DEFINES
        FS600
        FIRMWARE_BASE=0x08000000
        FIRMWARE_SIZE=0xa800
        BOOTLOADER
        FAST_SYSTEM_CLOCK
        BATTERY_REMOVABLE
        PWR_USB
        TUBE_HV_PWM
        KEYBOARD_5_KEYS
        KEYBOARD_KEY_POWER_OK
        DISPLAY_128X64
        DISPLAY_MONOCHROME
        BUZZER
        PULSE_LED
        GAME
    )
elseif(RADPRO_BOARD STREQUAL "bosean-fs1000")
    list(APPEND RADPRO_BOARD_DEFINES
        FS1000
        FIRMWARE_BASE=0x08000000
        FIRMWARE_SIZE=0xa800
        BOOTLOADER
        FAST_SYSTEM_CLOCK
        BATTERY_REMOVABLE
        PWR_USB
        TUBE_HV_PWM
        KEYBOARD_2_KEYS
        DISPLAY_128X64
        DISPLAY_MONOCHROME
        BUZZER
        PULSE_LED
        GAME
    )
elseif(RADPRO_BOARD STREQUAL "bosean-fs5000")
    list(APPEND RADPRO_BOARD_DEFINES
        FS5000
        FIRMWARE_BASE=0x08000000
        FIRMWARE_SIZE=0xd000
        USB_AUTOPOWER_ON
        BOOTLOADER
        STM32_SYNC_PERIPHERALS
        PWR_VCC
        PWR_USB
        KEYBOARD_3_KEYS
        DISPLAY_COLOR
        BUZZER
        VIBRATOR
        PULSE_LED
        ALERT_LED
        GAME
        DISPLAY_240X320
    )
elseif(RADPRO_BOARD STREQUAL "bosean-fs5000_landscape")
    list(APPEND RADPRO_BOARD_DEFINES
        FS5000
        FIRMWARE_BASE=0x08000000
        FIRMWARE_SIZE=0xd000
        USB_AUTOPOWER_ON
        BOOTLOADER
        STM32_SYNC_PERIPHERALS
        PWR_VCC
        PWR_USB
        KEYBOARD_3_KEYS
        DISPLAY_COLOR
        BUZZER
        VIBRATOR
        PULSE_LED
        ALERT_LED
        GAME
        DISPLAY_320X240
    )
elseif(RADPRO_BOARD STREQUAL "gq-gmc800")
    list(APPEND RADPRO_BOARD_DEFINES
        GMC800
        FIRMWARE_BASE=0x08000000
        FIRMWARE_SIZE=0xd000
        BOOTLOADER
        USB_AUTOPOWER_ON
        STM32_SYNC_PERIPHERALS
        PWR_USB
        TUBE_HV_PWM
        KEYBOARD_4_KEYS
        DISPLAY_COLOR
        DISPLAY_125PPI
        PULSE_LED_EN
        ALERT_LED_EN
        PULSESOUND_ENABLE
        VOICE
        VIBRATOR
        GAME
        DISPLAY_240X320
    )
elseif(RADPRO_BOARD STREQUAL "gq-gmc800_landscape")
    list(APPEND RADPRO_BOARD_DEFINES
        GMC800
        FIRMWARE_BASE=0x08000000
        FIRMWARE_SIZE=0xd000
        BOOTLOADER
        USB_AUTOPOWER_ON
        STM32_SYNC_PERIPHERALS
        PWR_USB
        TUBE_HV_PWM
        KEYBOARD_4_KEYS
        DISPLAY_COLOR
        DISPLAY_125PPI
        PULSE_LED_EN
        ALERT_LED_EN
        PULSESOUND_ENABLE
        VOICE
        VIBRATOR
        GAME
        DISPLAY_320X240
    )
endif()

# Set up CPU-specific compile options
set(RADPRO_BOARD_COMPILE_OPTIONS "")
if(RADPRO_BOARD_CPU)
    list(APPEND RADPRO_BOARD_COMPILE_OPTIONS
        "-mcpu=${RADPRO_BOARD_CPU}"
        "-mthumb"
        "-mfloat-abi=soft"
    )
endif()

# Add F_CPU definition
if(RADPRO_BOARD_F_CPU)
    list(APPEND RADPRO_BOARD_DEFINES F_CPU=${RADPRO_BOARD_F_CPU})
endif()

# ============================================================================
# Font Configuration
# ============================================================================

# Determine font definitions based on display type
set(RADPRO_FONT_DEFINITIONS "")
if(RADPRO_BOARD_DEFINES MATCHES "DISPLAY_COLOR" AND RADPRO_BOARD_DEFINES MATCHES "DISPLAY_320X240" AND NOT RADPRO_BOARD_DEFINES MATCHES "DISPLAY_125PPI")
    set(RADPRO_FONT_DEFINITIONS
        FONT_SYMBOLS="ui/fonts/font_symbols_color.h"
        FONT_LARGE="ui/fonts/font_large_color_115.h"
        FONT_SMALL="ui/fonts/font_small_${RADPRO_LANGUAGE}_color_21.h"
        FONT_MEDIUM="ui/fonts/font_medium_${RADPRO_LANGUAGE}_color_32.h"
    )
elseif(RADPRO_BOARD_DEFINES MATCHES "DISPLAY_COLOR" AND RADPRO_BOARD_DEFINES MATCHES "DISPLAY_240X320")
    set(RADPRO_FONT_DEFINITIONS
        FONT_SYMBOLS="ui/fonts/font_symbols_color.h"
        FONT_LARGE="ui/fonts/font_large_color_84.h"
        FONT_SMALL="ui/fonts/font_small_${RADPRO_LANGUAGE}_color_21.h"
        FONT_MEDIUM="ui/fonts/font_medium_${RADPRO_LANGUAGE}_color_32.h"
    )
elseif(RADPRO_BOARD_DEFINES MATCHES "DISPLAY_COLOR" AND RADPRO_BOARD_DEFINES MATCHES "DISPLAY_125PPI")
    set(RADPRO_FONT_DEFINITIONS
        FONT_SYMBOLS="ui/fonts/font_symbols_color.h"
        FONT_LARGE="ui/fonts/font_large_color_84.h"
        FONT_SMALL="ui/fonts/font_small_${RADPRO_LANGUAGE}_color_16.h"
        FONT_MEDIUM="ui/fonts/font_medium_${RADPRO_LANGUAGE}_color_24.h"
    )
elseif(RADPRO_BOARD_DEFINES MATCHES "DISPLAY_MONOCHROME")
    set(RADPRO_FONT_DEFINITIONS
        FONT_SYMBOLS="ui/fonts/font_symbols_monochrome.h"
        FONT_LARGE="ui/fonts/font_large_monochrome.h"
        FONT_SMALL="ui/fonts/font_small_${RADPRO_LANGUAGE}_monochrome.h"
        FONT_MEDIUM="ui/fonts/font_medium_${RADPRO_LANGUAGE}_monochrome.h"
    )
endif()

# ============================================================================
# Source Collection
# ============================================================================

# Collect all C sources
file(GLOB_RECURSE RADPRO_C_SOURCES CONFIGURE_DEPENDS
    "${RADPRO_SOURCE_DIR}/src/*.c"
)
list(FILTER RADPRO_C_SOURCES EXCLUDE REGEX ".*/src/sdl/.*")

# Collect library sources
file(GLOB_RECURSE RADPRO_LIB_SOURCES CONFIGURE_DEPENDS
    "${RADPRO_SOURCE_DIR}/lib/mcu-renderer/*.c"
    "${RADPRO_SOURCE_DIR}/lib/mcu-max/*.c"
    "${RADPRO_SOURCE_DIR}/lib/qfplib/*.c"
    "${RADPRO_SOURCE_DIR}/lib/stm32/*.c"
)

# Add USB library if needed
if(RADPRO_INCLUDE_USB)
    file(GLOB USB_C_SOURCES CONFIGURE_DEPENDS
        "${RADPRO_SOURCE_DIR}/lib/libusb_stm32/*.c"
    )
    list(APPEND RADPRO_LIB_SOURCES ${USB_C_SOURCES})
endif()

# Collect all assembly sources
file(GLOB_RECURSE RADPRO_ASM_SOURCES CONFIGURE_DEPENDS
    "${RADPRO_SOURCE_DIR}/src/*.[sS]"
)
list(FILTER RADPRO_ASM_SOURCES EXCLUDE REGEX ".*/src/sdl/.*")

file(GLOB_RECURSE RADPRO_LIB_ASM_SOURCES CONFIGURE_DEPENDS
    "${RADPRO_SOURCE_DIR}/lib/qfplib/*.[sS]"
)

# Add USB assembly if needed
if(RADPRO_INCLUDE_USB)
    file(GLOB USB_ASM_SOURCES CONFIGURE_DEPENDS
        "${RADPRO_SOURCE_DIR}/lib/libusb_stm32/*.[sS]"
    )
    list(APPEND RADPRO_LIB_ASM_SOURCES ${USB_ASM_SOURCES})
endif()

# ============================================================================
# Executable Target
# ============================================================================

add_executable(radpro
    ${RADPRO_C_SOURCES}
    ${RADPRO_LIB_SOURCES}
    ${RADPRO_ASM_SOURCES}
    ${RADPRO_LIB_ASM_SOURCES}
)

# ============================================================================
# CMSIS Headers (Vendored)
# ============================================================================

# Use vendored CMSIS headers instead of PlatformIO packages
set(CMSIS_VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor/cmsis")
set(CMSIS_INCLUDE_DIRS "")

# Determine which CMSIS device headers to use based on MCU family
if(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32F0")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32f0")
    set(CMSIS_FAMILY "STM32F0")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32F1")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32f1")
    set(CMSIS_FAMILY "STM32F1")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32F2")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32f2")
    set(CMSIS_FAMILY "STM32F2")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32F3")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32f3")
    set(CMSIS_FAMILY "STM32F3")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32F4")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32f4")
    set(CMSIS_FAMILY "STM32F4")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32F7")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32f7")
    set(CMSIS_FAMILY "STM32F7")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32G0")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32g0")
    set(CMSIS_FAMILY "STM32G0")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32L0")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32l0")
    set(CMSIS_FAMILY "STM32L0")
elseif(RADPRO_BOARD_JSON_DEFINES MATCHES "STM32L4")
    set(CMSIS_DEVICE_DIR "${CMSIS_VENDOR_DIR}/stm32l4")
    set(CMSIS_FAMILY "STM32L4")
else()
    message(FATAL_ERROR "Unknown STM32 family. Cannot determine CMSIS device headers to use.")
endif()

# Check if vendored CMSIS headers exist
set(CMSIS_CORE_DIR "${CMSIS_VENDOR_DIR}/core/Include")
set(CMSIS_DEVICE_INCLUDE_DIR "${CMSIS_DEVICE_DIR}/Include")

if(NOT EXISTS "${CMSIS_CORE_DIR}")
    message(FATAL_ERROR
        "Vendored CMSIS core headers not found at: ${CMSIS_CORE_DIR}\n"
        "Please run 'tools/vendor-cmsis.sh' or follow instructions in VENDOR_CMSIS_INSTRUCTIONS.md"
    )
endif()

if(NOT EXISTS "${CMSIS_DEVICE_INCLUDE_DIR}")
    message(FATAL_ERROR
        "Vendored CMSIS device headers for ${CMSIS_FAMILY} not found at: ${CMSIS_DEVICE_INCLUDE_DIR}\n"
        "Please run 'tools/vendor-cmsis.sh' or follow instructions in VENDOR_CMSIS_INSTRUCTIONS.md"
    )
endif()

# Add CMSIS include directories
list(APPEND CMSIS_INCLUDE_DIRS "${CMSIS_CORE_DIR}")
list(APPEND CMSIS_INCLUDE_DIRS "${CMSIS_DEVICE_INCLUDE_DIR}")

message(STATUS "Using vendored CMSIS headers:")
message(STATUS "  Core:   ${CMSIS_CORE_DIR}")
message(STATUS "  Device: ${CMSIS_DEVICE_INCLUDE_DIR} (${CMSIS_FAMILY})")

# Include directories
target_include_directories(radpro PRIVATE
    "${RADPRO_SOURCE_DIR}/src"
    "${RADPRO_SOURCE_DIR}/lib"
    "${RADPRO_SOURCE_DIR}/lib/mcu-renderer"
    "${RADPRO_SOURCE_DIR}/lib/mcu-max"
    "${RADPRO_SOURCE_DIR}/lib/qfplib"
    "${RADPRO_SOURCE_DIR}/lib/stm32"
    ${CMSIS_INCLUDE_DIRS}
)

# Add USB library include directory if USB is enabled
if(RADPRO_INCLUDE_USB)
    target_include_directories(radpro PRIVATE
        "${RADPRO_SOURCE_DIR}/lib/libusb_stm32"
    )
endif()

# Compile definitions
target_compile_definitions(radpro PRIVATE
    LANGUAGE="${RADPRO_LANGUAGE}"
    STRINGS="system/strings/${RADPRO_LANGUAGE}.h"
    ${RADPRO_BOARD_DEFINES}
    ${RADPRO_FONT_DEFINITIONS}
)

# Compile options
target_compile_options(radpro PRIVATE
    -Os
    -flto
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Wall
    ${RADPRO_BOARD_COMPILE_OPTIONS}
)

# Link options
target_link_options(radpro PRIVATE
    -nostdlib
    -flto
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/radpro.map
    ${RADPRO_BOARD_COMPILE_OPTIONS}
    ${RADPRO_BOARD_LINK_OPTIONS}
)

# Set linker script if specified
if(RADPRO_BOARD_LINKER_SCRIPT)
    if(NOT EXISTS "${RADPRO_BOARD_LINKER_SCRIPT}")
        message(FATAL_ERROR "Linker script not found: ${RADPRO_BOARD_LINKER_SCRIPT}")
    endif()
    target_link_options(radpro PRIVATE "-T${RADPRO_BOARD_LINKER_SCRIPT}")
    set_target_properties(radpro PROPERTIES LINK_DEPENDS "${RADPRO_BOARD_LINKER_SCRIPT}")
endif()

# ============================================================================
# Post-Build Steps
# ============================================================================

# Find required tools
find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy)
find_program(ARM_NONE_EABI_SIZE arm-none-eabi-size)
find_program(ARM_NONE_EABI_OBJDUMP arm-none-eabi-objdump)

if(NOT ARM_NONE_EABI_OBJCOPY)
    message(FATAL_ERROR "Required tool 'arm-none-eabi-objcopy' not found in PATH")
endif()

# Generate binary file
add_custom_command(TARGET radpro POST_BUILD
    COMMAND ${ARM_NONE_EABI_OBJCOPY} -O binary $<TARGET_FILE:radpro> ${CMAKE_BINARY_DIR}/${RADPRO_BOARD}.bin
    BYPRODUCTS ${CMAKE_BINARY_DIR}/${RADPRO_BOARD}.bin
    COMMENT "Generating ${RADPRO_BOARD}.bin"
)

# Generate hex file
add_custom_command(TARGET radpro POST_BUILD
    COMMAND ${ARM_NONE_EABI_OBJCOPY} -O ihex $<TARGET_FILE:radpro> ${CMAKE_BINARY_DIR}/${RADPRO_BOARD}.hex
    BYPRODUCTS ${CMAKE_BINARY_DIR}/${RADPRO_BOARD}.hex
    COMMENT "Generating ${RADPRO_BOARD}.hex"
)

# Display size information
if(ARM_NONE_EABI_SIZE)
    add_custom_command(TARGET radpro POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "========================================="
        COMMAND ${CMAKE_COMMAND} -E echo "Size information:"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================="
        COMMAND ${ARM_NONE_EABI_SIZE} $<TARGET_FILE:radpro>
        COMMENT "Size report"
    )
else()
    message(WARNING "arm-none-eabi-size not found: size report will be skipped")
endif()

# Display build success message
add_custom_command(TARGET radpro POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
    COMMAND ${CMAKE_COMMAND} -E echo "Build completed successfully!"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
    COMMAND ${CMAKE_COMMAND} -E echo "Output files in: ${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E echo "  ${RADPRO_BOARD} (ELF)"
    COMMAND ${CMAKE_COMMAND} -E echo "  ${RADPRO_BOARD}.bin (Binary)"
    COMMAND ${CMAKE_COMMAND} -E echo "  ${RADPRO_BOARD}.hex (Intel HEX)"
    COMMAND ${CMAKE_COMMAND} -E echo "  radpro.map (Linker map)"
    COMMAND ${CMAKE_COMMAND} -E echo ""
    COMMAND ${CMAKE_COMMAND} -E echo "Flash with:"
    COMMAND ${CMAKE_COMMAND} -E echo "  st-flash write ${RADPRO_BOARD}.bin 0x08004000"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================="
    COMMENT "Build summary"
)

# Set output name
set_target_properties(radpro PROPERTIES
    OUTPUT_NAME "${RADPRO_BOARD}"
)

# ============================================================================
# Build Summary
# ============================================================================

message("")
message("=========================================")
message("  RadPro Firmware Build Configuration")
message("=========================================")
message("  Board:           ${RADPRO_BOARD}")
message("  Language:        ${RADPRO_LANGUAGE}")
message("  CPU:             ${RADPRO_BOARD_CPU}")
message("  F_CPU:           ${RADPRO_BOARD_F_CPU}")
message("  Linker script:   ${RADPRO_BOARD_LINKER_SCRIPT}")
message("  USB enabled:     ${RADPRO_INCLUDE_USB}")
list(LENGTH RADPRO_C_SOURCES _c_count)
list(LENGTH RADPRO_LIB_SOURCES _lib_count)
math(EXPR _total_c "${_c_count} + ${_lib_count}")
message("  C sources:       ${_total_c} files")
message("  Output dir:      ${CMAKE_BINARY_DIR}")
message("=========================================")
message("")

