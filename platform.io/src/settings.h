/*
 * Rad Pro
 * Settings
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#if !defined(SETTINGS_H)
#define SETTINGS_H

#include <stdint.h>

#include "menu.h"
#include "view.h"

typedef struct
{
    uint32_t time;
    uint32_t pulseCount;
} Dose;

enum
{
    PULSE_SOUND_OFF,
    PULSE_SOUND_OFF_CLICKS,
    PULSE_SOUND_OFF_CHIRPS,
    PULSE_SOUND_OFF_BEEPS,
    PULSE_SOUND_NUM,
    PULSE_SOUND_ON = PULSE_SOUND_NUM,
    PULSE_SOUND_ON_CLICKS,
    PULSE_SOUND_ON_CHIRPS,
    PULSE_SOUND_ON_BEEPS,

    PULSE_SOUND_ON_MASK = PULSE_SOUND_ON,
    PULSE_SOUND_TYPE_MASK = PULSE_SOUND_OFF_BEEPS,
};

#if defined(VIBRATION)
enum
{
    PULSE_VIBRATION_OFF,
    PULSE_VIBRATION_ON,

    PULSE_VIBRATION_NUM,
};
#endif

#if defined(PULSE_LED)
enum
{
    PULSE_LED_OFF,
    PULSE_LED_ON,

    PULSE_LED_NUM,
};
#endif

enum
{
    PULSE_DISPLAY_FLASH_OFF,
    PULSE_DISPLAY_FLASH_ON,

    PULSE_DISPLAY_FLASH_NUM,
};

enum
{
    UNITS_SIEVERTS,
    UNITS_REM,
    UNITS_CPM,
    UNITS_CPS,

    UNITS_NUM,
};

enum
{
    AVERAGING_UNLIMITED,
    AVERAGING_24H,
    AVERAGING_12H,
    AVERAGING_6H,
    AVERAGING_3H,
    AVERAGING_1H,
    AVERAGING_30M,
    AVERAGING_10M,
    AVERAGING_5M,
    AVERAGING_1M,
    AVERAGING_30S,
    AVERAGING_10S,
    AVERAGING_5S,
    AVERAGING_1S,
    AVERAGING_TIME_NUM,

    AVERAGING_50CONFIDENCE = AVERAGING_TIME_NUM,
    AVERAGING_20CONFIDENCE,
    AVERAGING_10CONFIDENCE,
    AVERAGING_5CONFIDENCE,
    AVERAGING_2CONFIDENCE,
    AVERAGING_1CONFIDENCE,

    AVERAGING_NUM,
};

enum
{
    INSTANTANEOUSAVERAGING_ADAPTIVEFAST,
    INSTANTANEOUSAVERAGING_ADAPTIVEPRECISION,
    INSTANTANEOUSAVERAGING_60SECONDS,
    INSTANTANEOUSAVERAGING_30SECONDS,
    INSTANTANEOUSAVERAGING_10SECONDS,

    INSTANTANEOUSAVERAGING_NUM,
};

enum
{
    RATEALARM_OFF,
    RATEALARM_0_5,
    RATEALARM_1,
    RATEALARM_2,
    RATEALARM_5,
    RATEALARM_10,
    RATEALARM_20,
    RATEALARM_50,
    RATEALARM_100,
    RATEALARM_200,
    RATEALARM_500,
    RATEALARM_1000,
    RATEALARM_2000,
    RATEALARM_5000,
    RATEALARM_10000,
    RATEALARM_20000,

    RATEALARM_NUM,
};

enum
{
    DOSEALARM_OFF,
    DOSEALARM_2,
    DOSEALARM_5,
    DOSEALARM_10,
    DOSEALARM_20,
    DOSEALARM_50,
    DOSEALARM_100,
    DOSEALARM_200,
    DOSEALARM_500,
    DOSEALARM_1000,
    DOSEALARM_2000,
    DOSEALARM_5000,
    DOSEALARM_10000,
    DOSEALARM_20000,
    DOSEALARM_50000,
    DOSEALARM_100000,

    DOSEALARM_NUM,
};

enum
{
    ALARMSIGNALING_SOUND,
#if defined(VIBRATION)
    ALARMSIGNALING_VIBRATION,
#endif
#if defined(ALERT_LED) || defined(PULSE_LED)
    ALARMSIGNALING_ALERT_LED,
#endif
    ALARMSIGNALING_DISPLAY_FLASH,

    ALARMSIGNALING_NUM,
};

enum
{
    TUBE_SENSITIVITY_J305,
    TUBE_SENSITIVITY_J321,
    TUBE_SENSITIVITY_J613,
    TUBE_SENSITIVITY_J614,
    TUBE_SENSITIVITY_M4011,
    TUBE_SENSITIVITY_SBM20,

    TUBE_SENSITIVITY_PRESETS_NUM,
};

#define TUBE_SENSITIVITY_VALUE_MIN 1.0F
#define TUBE_SENSITIVITY_VALUE_MAX 10000.01F
#define TUBE_SENSITIVITY_VALUE_LOG2_MAX_MIN 13.28771237F
#define TUBE_SENSITIVITY_VALUE_NUM 241
#define TUBE_SENSITIVITY_NUM (TUBE_SENSITIVITY_PRESETS_NUM + TUBE_SENSITIVITY_VALUE_NUM)

#define TUBE_DEADTIMECOMPENSATION_MIN 0.000005F
#define TUBE_DEADTIMECOMPENSATION_MAX 0.000500F
#define TUBE_DEADTIMECOMPENSATION_LOG2_MAX_MIN 6.643856189F
#define TUBE_DEADTIMECOMPENSATION_NUM 121

#if defined(SIMULATOR)
#define TUBE_SENSITIVITY_DEFAULT TUBE_SENSITIVITY_M4011
#define TUBE_HVPROFILE_FACTORYDEFAULT_FREQUENCY 40000
#define TUBE_HVPROFILE_FACTORYDEFAULT_DUTYCYCLE 0.5F
#define TUBE_HVPROFILE_ACCURACY_FREQUENCY 10000
#define TUBE_HVPROFILE_ACCURACY_DUTYCYCLE 0.1F
#define TUBE_HVPROFILE_ENERGYSAVING_FREQUENCY 2500
#define TUBE_HVPROFILE_ENERGYSAVING_DUTYCYCLE 0.03F
#elif defined(FS2011) || defined(FS600) || defined(FS1000)
#define TUBE_SENSITIVITY_DEFAULT TUBE_SENSITIVITY_J614
#define TUBE_HVPROFILE_FACTORYDEFAULT_FREQUENCY 40000
#define TUBE_HVPROFILE_FACTORYDEFAULT_DUTYCYCLE 0.5F
#define TUBE_HVPROFILE_ACCURACY_FREQUENCY 2500
#define TUBE_HVPROFILE_ACCURACY_DUTYCYCLE 0.065F
#define TUBE_HVPROFILE_ENERGYSAVING_FREQUENCY 2500
#define TUBE_HVPROFILE_ENERGYSAVING_DUTYCYCLE 0.03F
#elif defined(FS5000)
#define TUBE_SENSITIVITY_DEFAULT TUBE_SENSITIVITY_J321
#elif defined(GC01)
#if defined(CH32)
#define TUBE_SENSITIVITY_DEFAULT TUBE_SENSITIVITY_J321
#define TUBE_HVPROFILE_FACTORYDEFAULT_FREQUENCY 9207.16F
#define TUBE_HVPROFILE_FACTORYDEFAULT_DUTYCYCLE 0.75F
#elif defined(APM32)
#define TUBE_SENSITIVITY_DEFAULT TUBE_SENSITIVITY_J613
#define TUBE_HVPROFILE_FACTORYDEFAULT_FREQUENCY 47058.82F
#define TUBE_HVPROFILE_FACTORYDEFAULT_DUTYCYCLE 0.5F
#endif
#define TUBE_HVPROFILE_ENERGYSAVING_FREQUENCY 5000
#define TUBE_HVPROFILE_ENERGYSAVING_DUTYCYCLE 0.015F
#endif

enum
{
    TUBE_HVPROFILE_FACTORYDEFAULT,
#if defined(TUBE_HVPROFILE_ACCURACY_FREQUENCY)
    TUBE_HVPROFILE_ACCURACY,
#endif
#if defined(TUBE_HVPROFILE_ENERGYSAVING_FREQUENCY)
    TUBE_HVPROFILE_ENERGYSAVING,
#endif
    TUBE_HVPROFILE_CUSTOM,

    TUBE_HVPROFILE_NUM,
};

enum
{
    TUBE_HVFREQUENCY_1_25,
    TUBE_HVFREQUENCY_2_5,
    TUBE_HVFREQUENCY_5,
    TUBE_HVFREQUENCY_10,
    TUBE_HVFREQUENCY_20,
    TUBE_HVFREQUENCY_40,

    TUBE_HVFREQUENCY_NUM,
};

#define TUBE_HVDUTYCYCLE_MIN 0.0025F
#define TUBE_HVDUTYCYCLE_MAX 0.9F
#define TUBE_HVDUTYCYCLE_STEP 0.0025F
#define TUBE_HVDUTYCYCLE_NUM ((uint32_t)((TUBE_HVDUTYCYCLE_MAX -   \
                                          TUBE_HVDUTYCYCLE_MIN +   \
                                          TUBE_HVDUTYCYCLE_STEP) / \
                                         TUBE_HVDUTYCYCLE_STEP))

enum
{
    DATALOG_INTERVAL_OFF,
    DATALOG_INTERVAL_60M,
    DATALOG_INTERVAL_10M,
    DATALOG_INTERVAL_1M,
    DATALOG_INTERVAL_10S,
    DATALOG_INTERVAL_1S,

    DATALOG_INTERVAL_NUM,
};

#if defined(DISPLAY_MONOCHROME)
#define DISPLAY_CONTRAST_DEFAULT 4
#define DISPLAY_CONTRAST_NUM 8
#elif defined(DISPLAY_COLOR)
enum
{
    DISPLAY_THEME_DAY,
    DISPLAY_THEME_DUSK,
    DISPLAY_THEME_NIGHT,

    DISPLAY_THEME_NUM,
};
#endif

enum
{
#if defined(DISPLAY_MONOCHROME)
    DISPLAY_SLEEP_ALWAYS_OFF,
#endif
    DISPLAY_SLEEP_10S,
    DISPLAY_SLEEP_30S,
    DISPLAY_SLEEP_1M,
    DISPLAY_SLEEP_2M,
    DISPLAY_SLEEP_5M,
    DISPLAY_SLEEP_ALWAYS_ON,

    DISPLAY_SLEEP_NUM,
};

enum
{
    DISPLAY_BRIGHTNESS_LOW,
    DISPLAY_BRIGHTNESS_MEDIUM,
    DISPLAY_BRIGHTNESS_HIGH,
    DISPLAY_BRIGHTNESS_VERYHIGH,

    DISPLAY_BRIGHTNESS_NUM,
};

enum
{
    RTC_TIMEZONE_M1200,
    RTC_TIMEZONE_M1100,
    RTC_TIMEZONE_M1000,
    RTC_TIMEZONE_M0900,
    RTC_TIMEZONE_M0800,
    RTC_TIMEZONE_M0700,
    RTC_TIMEZONE_M0600,
    RTC_TIMEZONE_M0500,
    RTC_TIMEZONE_M0400,
    RTC_TIMEZONE_M0300,
    RTC_TIMEZONE_M0200,
    RTC_TIMEZONE_M0100,
    RTC_TIMEZONE_P0000,
    RTC_TIMEZONE_P0100,
    RTC_TIMEZONE_P0200,
    RTC_TIMEZONE_P0300,
    RTC_TIMEZONE_P0400,
    RTC_TIMEZONE_P0500,
    RTC_TIMEZONE_P0600,
    RTC_TIMEZONE_P0700,
    RTC_TIMEZONE_P0800,
    RTC_TIMEZONE_P0900,
    RTC_TIMEZONE_P1000,
    RTC_TIMEZONE_P1100,
    RTC_TIMEZONE_P1200,
    RTC_TIMEZONE_P1300,
    RTC_TIMEZONE_P1400,

    RTC_TIMEZONE_NUM,
};

enum
{
    RTC_TIMEFORMAT_24HOUR,
    RTC_TIMEFORMAT_12HOUR,

    RTC_TIMEFORMAT_NUM,
};

#if defined(BATTERY_REMOVABLE)
enum
{
    BATTERYTYPE_NI_MH,
    BATTERYTYPE_ALKALINE,

    BATTERYTYPE_NUM,
};
#endif

#if defined(DATA_MODE)
enum
{
    DATAMODE_OFF,
    DATAMODE_ON,

    DATAMODE_NUM,
};
#endif

enum
{
    GAMESTRENGTH_1,
    GAMESTRENGTH_2,
    GAMESTRENGTH_3,
    GAMESTRENGTH_4,
    GAMESTRENGTH_5,
    GAMESTRENGTH_6,
    GAMESTRENGTH_7,
    GAMESTRENGTH_8,

    GAMESTRENGTH_NUM,
};

typedef struct
{
    unsigned int entryEmpty : 1;

    unsigned int pulseSound : 3;
#if defined(VIBRATION)
    unsigned int pulseVibration : 1;
#endif
#if defined(PULSE_LED)
    unsigned int pulseLED : 1;
#endif
    unsigned int pulseDisplayFlash : 1;
    unsigned int pulseThreshold : 4;

    unsigned int units : 2;
    unsigned int averaging : 5;
    unsigned int instantaneousAveraging : 3;

    unsigned int rateAlarm : 4;
    unsigned int doseAlarm : 4;
    unsigned int overrangeAlarm : 1;
    unsigned int alarmSignaling : 4;

    unsigned int tubeSensitivity : 8;
    unsigned int tubeDeadTimeCompensation : 7;
#if defined(TUBE_HV_PWM)
    unsigned int tubeHVProfile : 2;
    unsigned int tubeHVFrequency : 3;
    unsigned int tubeHVDutyCycle : 9;
#endif

    unsigned int datalogInterval : 3;

#if defined(DISPLAY_MONOCHROME)
    unsigned int displayContrast : 3;
#elif defined(DISPLAY_COLOR)
    unsigned int displayTheme : 2;
#if defined(DISPLAY_EXTRA_COLOR_SCHEMES) 
    unsigned int displayFNIRSI : 2;
#endif   
#endif
    unsigned int displayBrightness : 2;
    unsigned int displaySleep : 3;

    unsigned int rtcTimeZone : 5;
    unsigned int rtcTimeFormat : 1;

#if defined(BATTERY_REMOVABLE)
    unsigned int batteryType : 1;
#endif

#if defined(DATA_MODE)
    unsigned int dataMode : 1;
#endif

    unsigned int gameStrength : 3;
} Settings
#if defined(STM32G0) || defined(STM32L4)
    __attribute__((aligned(8)))
#endif
    ;

extern Settings settings;

extern const View settingsMenuView;

void initSettings(void);
void resetSettings(void);

void updateSettingsPeriod(void);

void writeSettings(void);

void onSettingsSubMenuBack(const Menu *menu);

#endif
