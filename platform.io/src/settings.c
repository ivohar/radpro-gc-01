/*
 * Rad Pro
 * Settings
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#include <limits.h>
#include <stdbool.h>
#if defined(SIMULATOR)
#include <time.h>
#endif

#include "comm.h"
#include "cstring.h"
#include "datalog.h"
#include "display.h"
#include "flash.h"
#include "game.h"
#include "led.h"
#include "measurements.h"
#include "power.h"
#include "rng.h"
#include "rtc.h"
#include "settings.h"
#include "system.h"
#include "tube.h"

typedef struct
{
    Dose tube;
    Dose dose;
    Settings settings;
} FlashState;

Settings settings;

static FlashState *getFlashState(FlashIterator *iterator);

void initSettings(void)
{
    // Default values
#if defined(TUBE_HV_PWM)
    settings.tubeConversionFactor = TUBE_CONVERSIONFACTOR_DEFAULT;
#endif

#if defined(PULSE_LED)
    settings.pulseLED = PULSE_LED_ON;
#endif
    settings.pulseClicks = PULSE_CLICKS_CLICKS;
#if defined(DISPLAY_MONOCHROME)
    settings.displayContrast = DISPLAY_CONTRAST_DEFAULT;
#endif
#if defined(SIMULATOR)
    settings.displayBrightness = DISPLAY_BRIGHTNESS_VERYHIGH;
#else
    settings.displayBrightness = DISPLAY_BRIGHTNESS_HIGH;
#endif
    settings.displaySleep = DISPLAY_SLEEP_30S;
#if defined(SIMULATOR)
    time_t unixTime = time(NULL);
    struct tm *localTM = gmtime(&unixTime);
    time_t localTime = mktime(localTM);
    settings.rtcTimeZone = 12 + (unixTime - localTime) / 3600;
#else
    settings.rtcTimeZone = RTC_TIMEZONE_P0000;
#endif

    // Read flash state
    FlashIterator iterator;
    FlashState *flashState = getFlashState(&iterator);

    if (flashState)
    {
        setCumulativeDoseTime(flashState->dose.time);
        setCumulativeDosePulseCount(flashState->dose.pulseCount);
        setTubeTime(flashState->tube.time);
        setTubePulseCount(flashState->tube.pulseCount);
        settings = flashState->settings;
    }
}

static FlashState *getFlashState(FlashIterator *iterator)
{
    iterator->region = &flashSettingsRegion;
    setFlashPageHead(iterator);

    uint8_t *entry = getFlashPage(iterator->pageIndex);
    FlashState *flashState = NULL;

    for (iterator->index = 0;
         iterator->index <= (flashPageDataSize - sizeof(FlashState));
         iterator->index += sizeof(FlashState))
    {
        if (isFlashEmpty(entry, sizeof(FlashState)))
            break;

        flashState = (FlashState *)entry;

        entry += sizeof(FlashState);
    }

    return flashState;
}

void writeSettings(void)
{
    FlashIterator iterator;
    getFlashState(&iterator);

    FlashState flashState;
    flashState.dose.time = getCumulativeDoseTime();
    flashState.dose.pulseCount = getCumulativeDosePulseCount();
    flashState.tube.time = getTubeTime();
    flashState.tube.pulseCount = getTubePulseCount();
    flashState.settings = settings;

    writeFlashPage(&iterator,
                   (uint8_t *)&flashState,
                   sizeof(FlashState));
}

// Settings menu

static const OptionView settingsMenuOptions[] = {
    {"Pulses", &pulsesMenuView},
    {"Alarms", &alarmsMenuView},
    {"Measurements", &measurementsMenuView},
    {"Geiger tube", &tubeMenuView},
    {"Data log", &datalogMenuView},
    {"Display", &displayMenuView},
    {"Date and time", &dateAndTimeMenuView},
#if defined(BATTERY_REMOVABLE)
    {"Battery type", &batteryTypeMenuView},
#endif
    {"Random generator", &rngMenuView},
#if defined(GAME)
    {"Game", &gameMenuView},
#endif
    {"Statistics", &statisticsView},
#if defined(DATA_MODE)
    {"Data mode", &dataModeMenuView},
#endif
    {"Lock mode", &lockView},
    {NULL},
};

static const char *onSettingsMenuGetOption(const Menu *menu,
                                           uint32_t index,
                                           MenuStyle *menuStyle)
{
    *menuStyle = MENUSTYLE_SUBMENU;

    return settingsMenuOptions[index].option;
}

static void onSettingsMenuSelect(const Menu *menu)
{
    setView(settingsMenuOptions[menu->state->selectedIndex].view);
}

static void onSettingsMenuBack(const Menu *menu)
{
    setMeasurementView(-1);
}

void onSettingsSubMenuBack(const Menu *menu)
{
    setView(&settingsMenuView);
}

static MenuState settingsMenuState;

static const Menu settingsMenu = {
    "Settings",
    &settingsMenuState,
    onSettingsMenuGetOption,
    onSettingsMenuSelect,
    onSettingsMenuBack,
};

const View settingsMenuView = {
    onMenuEvent,
    &settingsMenu,
};
