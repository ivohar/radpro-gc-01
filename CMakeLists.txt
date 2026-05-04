cmake_minimum_required(VERSION 3.16)
project(radpro LANGUAGES C ASM)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(RADPRO_SIMULATOR "Build the SDL-based simulator target" OFF)
set(RADPRO_BOARD "fnirsi-gc01_ch32f103r8" CACHE STRING "Target board configuration for firmware builds")
set(RADPRO_LANGUAGE "en" CACHE STRING "Language for strings and font selection")

set(RADPRO_SOURCE_DIR "${CMAKE_SOURCE_DIR}/platform.io")
set(RADPRO_BOARD_DIR "${RADPRO_SOURCE_DIR}/boards")
set(RADPRO_SCRIPT_DIR "${RADPRO_SOURCE_DIR}/scripts")

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

if(NOT RADPRO_SIMULATOR)
    if(NOT RADPRO_BOARD IN_LIST RADPRO_SUPPORTED_BOARDS)
        message(FATAL_ERROR "Unsupported RADPRO_BOARD: ${RADPRO_BOARD}\nSupported values: ${RADPRO_SUPPORTED_BOARDS}")
    endif()
endif()

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

function(radpro_json_extract json key output)
    string(REGEX MATCH "\"${key}\"[[:space:]]*:[[:space:]]*\"([^\"]*)\"" _ "${json}")
    set(${output} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

if(RADPRO_SIMULATOR)
    add_compile_definitions(SIMULATOR)
    add_compile_definitions(SIMULATOR_COMM)
    add_compile_definitions(GAME)
    add_compile_definitions(LANGUAGE="${RADPRO_LANGUAGE}")
    add_compile_definitions(STRINGS="system/strings/${RADPRO_LANGUAGE}.h")
    add_definitions(-DMCURENDERER_SDL)

    file(GLOB_RECURSE SDL_SOURCES CONFIGURE_DEPENDS
        "${RADPRO_SOURCE_DIR}/src/sdl/*.c"
        "${RADPRO_SOURCE_DIR}/lib/mcu-renderer/*.c"
        "${RADPRO_SOURCE_DIR}/lib/mcu-max/*.c"
    )

    add_executable(radpro-simulator ${SDL_SOURCES})
    target_include_directories(radpro-simulator PRIVATE
        "${RADPRO_SOURCE_DIR}/src"
        "${RADPRO_SOURCE_DIR}/lib/mcu-renderer"
        "${RADPRO_SOURCE_DIR}/lib/mcu-max"
    )

    find_package(SDL2 CONFIG REQUIRED)
    target_link_libraries(radpro-simulator PRIVATE SDL2::SDL2main SDL2::SDL2)
    target_compile_options(radpro-simulator PRIVATE -O3)
    return()
endif()

radpro_board_json(${RADPRO_BOARD} RADPRO_BOARD_JSON)
file(READ "${RADPRO_BOARD_DIR}/${RADPRO_BOARD_JSON}.json" RADPRO_BOARD_JSON_CONTENT)
radpro_json_extract("${RADPRO_BOARD_JSON_CONTENT}" cpu RADPRO_BOARD_CPU)
radpro_json_extract("${RADPRO_BOARD_JSON_CONTENT}" extra_flags RADPRO_BOARD_EXTRA_FLAGS)
radpro_json_extract("${RADPRO_BOARD_JSON_CONTENT}" f_cpu RADPRO_BOARD_F_CPU)

string(REGEX REPLACE "-D[ ]+" "-D" RADPRO_BOARD_EXTRA_FLAGS "${RADPRO_BOARD_EXTRA_FLAGS}")
separate_arguments(RADPRO_BOARD_EXTRA_FLAGS UNIX_COMMAND "${RADPRO_BOARD_EXTRA_FLAGS}")

set(RADPRO_BOARD_DEFINES
    QFP
    MCURENDERER_WITHOUT_BITMAP_SUPPORT
)
set(RADPRO_BOARD_LINK_OPTIONS
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

set(RADPRO_BOARD_LINKER_SCRIPT "")
set(RADPRO_INCLUDE_USB FALSE)

if(RADPRO_BOARD STREQUAL "fnirsi-gc01_ch32f103r8")
    list(APPEND RADPRO_BOARD_DEFINES
        GC01
        FIRMWARE_BASE_=0x08004000
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
        FIRMWARE_BASE_=0x08004000
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
        TUBE_DEFAULT_HVPROFILE_IN_FLASH
        GAME
    )
    set(RADPRO_INCLUDE_USB TRUE)
    set(RADPRO_BOARD_LINKER_SCRIPT "${RADPRO_SCRIPT_DIR}/gc01.ld")
elseif(RADPRO_BOARD STREQUAL "fnirsi-gc03")
    list(APPEND RADPRO_BOARD_DEFINES
        GC03
        FIRMWARE_BASE_=0x08006000
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
        FIRMWARE_BASE_=0x08000000
        FAST_SYSTEM_CLOCK
        BATTERY_REMOVABLE
        DATA_MODE
        TUBE_HV_PWM
        KEYBOARD_5_KEYS
        DISPLAY_128X64
        DISPLAY_MONOCHROME
        BUZZER
        VIBRATOR
        GAME
    )
elseif(RADPRO_BOARD STREQUAL "bosean-fs600")
    list(APPEND RADPRO_BOARD_DEFINES
        FIRMWARE_BASE_=0x08000000
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
        GAME
    )
elseif(RADPRO_BOARD STREQUAL "bosean-fs1000")
    list(APPEND RADPRO_BOARD_DEFINES
        FIRMWARE_BASE_=0x08000000
        BOOTLOADER
        FAST_SYSTEM_CLOCK
        BATTERY_REMOVABLE
        PWR_USB
        TUBE_HV_PWM
        KEYBOARD_2_KEYS
        DISPLAY_128X64
        DISPLAY_MONOCHROME
        BUZZER
        GAME
    )
elseif(RADPRO_BOARD STREQUAL "bosean-fs5000")
    list(APPEND RADPRO_BOARD_DEFINES
        FS5000
        FIRMWARE_BASE_=0x08000000
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
        FIRMWARE_BASE_=0x08000000
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
        FIRMWARE_BASE_=0x08000000
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
        FIRMWARE_BASE_=0x08000000
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

if(RADPRO_BOARD STREQUAL "fnirsi-gc01_ch32f103r8" OR RADPRO_BOARD STREQUAL "fnirsi-gc01_apm32f103rb" OR RADPRO_BOARD STREQUAL "fnirsi-gc03")
    set(RADPRO_INCLUDE_USB TRUE)
endif()

set(RADPRO_BOARD_COMPILE_OPTIONS "")
if(RADPRO_BOARD_CPU)
    list(APPEND RADPRO_BOARD_COMPILE_OPTIONS "-mcpu=${RADPRO_BOARD_CPU}" "-mthumb" "-mfloat-abi=soft")
endif()

if(RADPRO_BOARD_F_CPU)
    list(APPEND RADPRO_BOARD_DEFINES F_CPU=${RADPRO_BOARD_F_CPU})
endif()

set(RADPRO_FONT_DEFINITIONS "")
if(RADPRO_BOARD_DEFINES)
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
endif()

file(GLOB_RECURSE RADPRO_C_SOURCES CONFIGURE_DEPENDS
    "${RADPRO_SOURCE_DIR}/src/*.c"
    "${RADPRO_SOURCE_DIR}/lib/**/*.c"
)
list(FILTER RADPRO_C_SOURCES EXCLUDE REGEX ".*/src/sdl/.*")
if(NOT RADPRO_INCLUDE_USB)
    list(FILTER RADPRO_C_SOURCES EXCLUDE REGEX ".*/lib/libusb_stm32/.*")
endif()

file(GLOB_RECURSE RADPRO_ASM_SOURCES CONFIGURE_DEPENDS
    "${RADPRO_SOURCE_DIR}/src/*.[sS]"
    "${RADPRO_SOURCE_DIR}/lib/*.[sS]"
)
list(FILTER RADPRO_ASM_SOURCES EXCLUDE REGEX ".*/src/sdl/.*")
if(NOT RADPRO_INCLUDE_USB)
    list(FILTER RADPRO_ASM_SOURCES EXCLUDE REGEX ".*/lib/libusb_stm32/.*")
endif()

add_executable(radpro ${RADPRO_C_SOURCES} ${RADPRO_ASM_SOURCES})

target_include_directories(radpro PRIVATE
    "${RADPRO_SOURCE_DIR}/src"
    "${RADPRO_SOURCE_DIR}/lib"
    "${RADPRO_SOURCE_DIR}/lib/mcu-renderer"
    "${RADPRO_SOURCE_DIR}/lib/mcu-max"
    "${RADPRO_SOURCE_DIR}/lib/qfplib"
    "${RADPRO_SOURCE_DIR}/lib/stm32"
)

target_compile_definitions(radpro PRIVATE
    LANGUAGE="${RADPRO_LANGUAGE}"
    STRINGS="system/strings/${RADPRO_LANGUAGE}.h"
    ${RADPRO_BOARD_DEFINES}
    ${RADPRO_FONT_DEFINITIONS}
)

target_compile_options(radpro PRIVATE
    -Os
    -flto
    -ffunction-sections
    -fdata-sections
    -fno-common
    ${RADPRO_BOARD_COMPILE_OPTIONS}
    ${RADPRO_BOARD_EXTRA_FLAGS}
)

target_link_options(radpro PRIVATE
    -nostdlib
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_BINARY_DIR}/radpro.map
    ${RADPRO_BOARD_LINK_OPTIONS}
)

if(RADPRO_BOARD_LINKER_SCRIPT)
    target_link_options(radpro PRIVATE "-T${RADPRO_BOARD_LINKER_SCRIPT}")
endif()

if(RADPRO_INCLUDE_USB)
    file(GLOB USB_C_SOURCES CONFIGURE_DEPENDS "${RADPRO_SOURCE_DIR}/lib/libusb_stm32/*.c")
    file(GLOB USB_ASM_SOURCES CONFIGURE_DEPENDS "${RADPRO_SOURCE_DIR}/lib/libusb_stm32/*.[sS]")
    target_sources(radpro PRIVATE ${USB_C_SOURCES} ${USB_ASM_SOURCES})
endif()

find_program(ARM_NONE_EABI_OBJCOPY arm-none-eabi-objcopy objcopy)
find_program(ARM_NONE_EABI_SIZE arm-none-eabi-size size)

if(NOT ARM_NONE_EABI_OBJCOPY)
    message(FATAL_ERROR "Required tool 'arm-none-eabi-objcopy' not found in PATH")
endif()

if(ARM_NONE_EABI_SIZE)
    add_custom_command(TARGET radpro POST_BUILD
        COMMAND ${ARM_NONE_EABI_OBJCOPY} -O binary $<TARGET_FILE:radpro> ${CMAKE_BINARY_DIR}/radpro.bin
        COMMAND ${ARM_NONE_EABI_SIZE} $<TARGET_FILE:radpro>
        COMMENT "Generating radpro.bin and size report"
    )
else()
    add_custom_command(TARGET radpro POST_BUILD
        COMMAND ${ARM_NONE_EABI_OBJCOPY} -O binary $<TARGET_FILE:radpro> ${CMAKE_BINARY_DIR}/radpro.bin
        COMMENT "Generating radpro.bin"
    )
    message(WARNING "arm-none-eabi-size not found: size report will be skipped")
endif()

set_target_properties(radpro PROPERTIES
    OUTPUT_NAME "radpro"
)

