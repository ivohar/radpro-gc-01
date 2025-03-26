/*
 * Rad Pro
 * Events
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#include <stdint.h>

#include "adc.h"
#include "buzzer.h"
#include "cmath.h"
#include "comm.h"
#include "datalog.h"
#include "display.h"
#include "events.h"
#include "game.h"
#include "keyboard.h"
#include "led.h"
#include "measurements.h"
#include "rng.h"
#include "settings.h"
#include "tube.h"
#include "vibration.h"

#define PULSE_SOUND_CLICKS_TICKS ((uint32_t)(0.001 * SYSTICK_FREQUENCY))
#define PULSE_SOUND_CHIRPS_TICKS ((uint32_t)(0.004 * SYSTICK_FREQUENCY))
#define PULSE_SOUND_BEEPS_TICKS ((uint32_t)(0.015 * SYSTICK_FREQUENCY))
#define PULSE_VIBRATION_TICKS ((uint32_t)(0.050 * SYSTICK_FREQUENCY))
#define PULSE_LED_TICKS ((uint32_t)(0.050 * SYSTICK_FREQUENCY))
#define PULSE_DISPLAY_FLASH_TICKS ((uint32_t)(0.050 * SYSTICK_FREQUENCY))

#define ALARM_TICKS ((uint32_t)(0.200 * SYSTICK_FREQUENCY))

#define POWERON_TEST_TICKS ((uint32_t)(0.100 * SYSTICK_FREQUENCY))
#define POWERON_TEST_TICKS_EXT ((uint32_t)(0.150 * SYSTICK_FREQUENCY))

typedef enum
{
    TIMER_OFF = -1,
    TIMER_ELAPSED,
    TIMER_RUNNING,
} TimerState;

volatile uint32_t eventsCurrentTick;

static struct
{
    bool pulseThreshold;

    bool lastPulseTimeCountInitialized;
    uint32_t lastPulseTimeCount;
    uint32_t deadTimeCount;

    volatile int32_t buzzerTimer;
    volatile int32_t buzzerNoiseTimer;

    int32_t requestedDisplayBacklightTimer;
    bool requestedDisplayBacklightReset;
    volatile int32_t displayBacklightTimer;

#if defined(PULSE_LED)
    volatile int32_t pulseLEDTimer;
#endif

#if defined(ALERT_LED)
    volatile int32_t alertLEDTimer;
#endif

#if defined(VIBRATION)
    volatile int32_t vibrationTimer;
#endif

    int32_t keyboardTimer;

    int32_t periodTimer;
    volatile uint32_t periodUpdate;
    uint32_t lastPeriodUpdate;
} events;

static uint32_t getPulsesSoundMenuItem();

static const Menu pulsesMenu;
static const Menu pulsesSoundMenu;
static const Menu pulsesThresholdMenu;

static int32_t displayTimerValues[] = {
#if defined(DISPLAY_MONOCHROME)
    1,
#endif
    10 * SYSTICK_FREQUENCY,
    30 * SYSTICK_FREQUENCY,
    60 * SYSTICK_FREQUENCY,
    120 * SYSTICK_FREQUENCY,
    300 * SYSTICK_FREQUENCY,
    -1,
};

static uint32_t pulseSoundTicks[] = {
    0,
    PULSE_SOUND_CLICKS_TICKS,
    PULSE_SOUND_CHIRPS_TICKS,
    PULSE_SOUND_BEEPS_TICKS,
};

void resetEvents(void)
{
    selectMenuItem(&pulsesMenu,
                   0,
                   0);
    selectMenuItem(&pulsesSoundMenu,
                   getPulsesSoundMenuItem(),
                   PULSE_SOUND_NUM);
    selectMenuItem(&pulsesThresholdMenu,
                   settings.pulseThreshold,
                   RATEALARM_NUM);
}

void startEvents(void)
{
    events.deadTimeCount = PULSE_MEASUREMENT_FREQUENCY;
    events.periodTimer = SYSTICK_FREQUENCY;
    events.keyboardTimer = KEY_TICKS;
}

static TimerState updateTimer(volatile int32_t *timer)
{
    int32_t timerValue = *timer;

    if (timerValue == 0)
        return TIMER_OFF;
    else if (timerValue > 0)
    {
        timerValue--;

        *timer = timerValue;

        return (timerValue == 0) ? TIMER_ELAPSED : TIMER_RUNNING;
    }
    else
        return TIMER_RUNNING;
}

void onTick(void)
{
    // Measurement
    uint32_t pulseCount = 0;
    uint32_t pulseTimeCount;

    while (getTubePulse(&pulseTimeCount))
    {
        onRNGPulse(pulseTimeCount);

        if (events.lastPulseTimeCountInitialized)
        {
            uint32_t deltaTimeCount = pulseTimeCount - events.lastPulseTimeCount;

            if (deltaTimeCount < events.deadTimeCount)
                events.deadTimeCount = deltaTimeCount;
        }
        else
            events.lastPulseTimeCountInitialized = true;
        events.lastPulseTimeCount = pulseTimeCount;

        pulseCount++;
    }

    onMeasurementTick(pulseCount);

    if (updateTimer(&events.periodTimer) == TIMER_ELAPSED)
    {
        events.periodTimer = SYSTICK_FREQUENCY;
        events.periodUpdate++;

        onMeasurementPeriod();
    }

    // Keyboard
    if (updateTimer(&events.keyboardTimer) == TIMER_ELAPSED)
    {
        events.keyboardTimer = KEY_TICKS;

        onKeyboardTick();
    }

    // Buzzer
#if defined(SIMULATOR)
    updateBuzzer();
#endif

    switch (updateTimer(&events.buzzerTimer))
    {
    case TIMER_ELAPSED:
        setBuzzer(false);

        break;

    case TIMER_RUNNING:
        switch (updateTimer(&events.buzzerNoiseTimer))
        {
        case TIMER_ELAPSED:
            setBuzzer(true);

            break;

        case TIMER_RUNNING:
            setBuzzer(getRandomBit());

            break;

        default:
            break;
        }

    default:
        break;
    }

    // Display
    if (updateTimer(&events.displayBacklightTimer) == TIMER_ELAPSED)
        setDisplayBacklight(false);

#if defined(PULSE_LED)
    // Pulse LED
    if (updateTimer(&events.pulseLEDTimer) == TIMER_ELAPSED)
        setPulseLED(false);
#endif

#if defined(ALERT_LED)
    // Alert LED
    if (updateTimer(&events.alertLEDTimer) == TIMER_ELAPSED)
        setAlertLED(false);
#endif

#if defined(VIBRATION)
    // Vibration
    if (updateTimer(&events.vibrationTimer) == TIMER_ELAPSED)
        setVibration(false);
#endif
}

uint32_t getTick(void)
{
    return eventsCurrentTick;
}

// Events

void syncTimerThread(void)
{
    sleep(1);
}

void dispatchEvents(void)
{
    sleep(0);

    dispatchCommEvents();

    uint32_t periodUpdate = events.periodUpdate;
    if (events.lastPeriodUpdate != periodUpdate)
    {
        events.lastPeriodUpdate = periodUpdate;

        updateMeasurements();
        updateADC();
        updateViewPeriod();
        updateSettingsPeriod();
    }

    updateDatalog();
    dispatchViewEvents();
}

// Dead time

float getTubeDeadTime(void)
{
    return events.deadTimeCount * timerCountToSeconds;
}

// Timers

static void setBuzzerTimer(int32_t value, int32_t noiseValue)
{
    if (value > events.buzzerTimer)
    {
        events.buzzerTimer = value;
        events.buzzerNoiseTimer = noiseValue;
    }
}

static void setDisplayBacklightTimer(int32_t value)
{
    if ((value == -1) ||
        ((events.displayBacklightTimer != -1) &&
         (value > events.displayBacklightTimer)))
    {
        if (value != 1)
            setDisplayBacklight(true);

        events.displayBacklightTimer = value;
    }
}

#if defined(PULSE_LED)
static void setPulseLEDTimer(int32_t value)
{
    if (value > events.pulseLEDTimer)
    {
        setPulseLED(true);

        events.pulseLEDTimer = value;
    }
}
#endif

#if defined(ALERT_LED)
static void setAlertLEDTimer(int32_t value)
{
    if (value > events.alertLEDTimer)
    {
        setAlertLED(true);

        events.alertLEDTimer = value;
    }
}
#endif

#if defined(VIBRATION)
static void setVibrationTimer(int32_t value)
{
    if (value > events.vibrationTimer)
    {
        setVibration(true);

        events.vibrationTimer = value;
    }
}
#endif

void requestDisplayBacklightTrigger(void)
{
    events.requestedDisplayBacklightTimer = displayTimerValues[settings.displaySleep];
    events.requestedDisplayBacklightReset = true;
}

static void requestDisplayBacklightAlarm(void)
{
    if ((events.requestedDisplayBacklightTimer >= 0) &&
        events.requestedDisplayBacklightTimer < ALARM_TICKS)
        events.requestedDisplayBacklightTimer = ALARM_TICKS;
}

bool isDisplayBacklightTriggerRequested(void)
{
    return events.requestedDisplayBacklightTimer != 0;
}

void triggerDisplayBacklight(void)
{
    if (events.requestedDisplayBacklightReset)
        events.displayBacklightTimer = 0;

    setDisplayBacklightTimer(events.requestedDisplayBacklightTimer);

    events.requestedDisplayBacklightTimer = 0;
    events.requestedDisplayBacklightReset = false;
}

void cancelDisplayBacklight(void)
{
    events.displayBacklightTimer = 1;
}

bool isDisplayBacklightActive(void)
{
    return events.displayBacklightTimer != 0;
}

void setPulseThreshold(bool value)
{
    events.pulseThreshold = value;
}

bool isPulseThresholdEnabled(void)
{
    return events.pulseThreshold;
}

void triggerPulse(void)
{
    if (!events.pulseThreshold)
    {
        if (settings.pulseSound & PULSE_SOUND_ON_MASK)
            setBuzzerTimer(pulseSoundTicks[settings.pulseSound & PULSE_SOUND_TYPE_MASK] + 1,
                           events.buzzerTimer + 1);

#if defined(VIBRATION)
        if (settings.pulseVibration)
            setVibrationTimer(PULSE_VIBRATION_TICKS);
#endif

#if defined(PULSE_LED)
        if (settings.pulseLED)
            setPulseLEDTimer(PULSE_LED_TICKS);
#endif

        if (settings.pulseDisplayFlash && isDisplayEnabled())
            setDisplayBacklightTimer(PULSE_DISPLAY_FLASH_TICKS);
    }
}

void triggerAlarm(void)
{
    syncTimerThread();

    if (settings.alarmSignaling & (1 << ALARMSIGNALING_SOUND))
        setBuzzerTimer(ALARM_TICKS, 1);

#if defined(VIBRATION)
    if (settings.alarmSignaling & (1 << ALARMSIGNALING_VIBRATION))
        setVibrationTimer(ALARM_TICKS);
#endif

#if defined(ALERT_LED)
    if (settings.alarmSignaling & (1 << ALARMSIGNALING_ALERT_LED))
        setAlertLEDTimer(ALARM_TICKS);
#elif defined(PULSE_LED)
    if (settings.alarmSignaling & (1 << ALARMSIGNALING_ALERT_LED))
        setPulseLEDTimer(ALARM_TICKS);
#endif

    if (settings.alarmSignaling & (1 << ALARMSIGNALING_DISPLAY_FLASH))
        requestDisplayBacklightAlarm();
}

void triggerVibration(void)
{
#if defined(VIBRATION)
    syncTimerThread();

    setVibrationTimer(POWERON_TEST_TICKS);
#endif
}

void BuzzerAndVibration(void)
{
    syncTimerThread();

    setBuzzerTimer(POWERON_TEST_TICKS, 1);
#if defined(VIBRATION)
    setVibrationTimer(POWERON_TEST_TICKS_EXT);
#endif
}

// Pulses sound menu

static const char *const pulsesSoundMenuOptions[] = {
    "Off",
    "Clicks",
    "Chirps",
    "Beeps",
    NULL,
};

void togglePulseClicks(void)
{
    settings.pulseSound ^= PULSE_SOUND_ON_MASK;
}

static uint32_t getPulsesSoundMenuItem()
{
    if (!(settings.pulseSound & PULSE_SOUND_ON_MASK))
        return 0;
    else
        return (settings.pulseSound & PULSE_SOUND_TYPE_MASK);
}

static const char *onPulsesSoundMenuGetOption(const Menu *menu,
                                              uint32_t index,
                                              MenuStyle *menuStyle)
{
    *menuStyle = (index == getPulsesSoundMenuItem());

    return pulsesSoundMenuOptions[index];
}

static void onPulsesSoundMenuSelect(const Menu *menu)
{
    if (!menu->state->selectedIndex)
        settings.pulseSound = PULSE_SOUND_OFF_CLICKS;
    else
        settings.pulseSound = menu->state->selectedIndex | PULSE_SOUND_ON_MASK;
}

static MenuState pulsesSoundMenuState;

static const Menu pulsesSoundMenu = {
    "Sound",
    &pulsesSoundMenuState,
    onPulsesSoundMenuGetOption,
    onPulsesSoundMenuSelect,
    onPulsesSubMenuBack,
};

const View pulsesSoundMenuView = {
    onMenuEvent,
    &pulsesSoundMenu,
};

// Pulses threshold menu

static const char *onPulseThresholdMenuGetOption(const Menu *menu,
                                                 uint32_t index,
                                                 MenuStyle *menuStyle)
{
    *menuStyle = (index == settings.pulseThreshold);

    if (index == 0)
        return "Off";
    else if (index < RATEALARM_NUM)
        return buildRateAlarmMenuOption(index);
    else
        return NULL;
}

static void onPulseThresholdMenuSelect(const Menu *menu)
{
    settings.pulseThreshold = menu->state->selectedIndex;
}

static MenuState pulseThresholdMenuState;

static const Menu pulsesThresholdMenu = {
    "Threshold",
    &pulseThresholdMenuState,
    onPulseThresholdMenuGetOption,
    onPulseThresholdMenuSelect,
    onPulsesSubMenuBack,
};

const View pulseThresholdMenuView = {
    onMenuEvent,
    &pulsesThresholdMenu,
};

// Pulses menu

enum
{
    PULSES_MENU_OPTIONS_SOUND,
#if defined(VIBRATION)
    PULSES_MENU_OPTIONS_VIBRATION,
#endif
#if defined(PULSE_LED)
    PULSES_MENU_OPTIONS_PULSE_LED,
#endif
    PULSES_MENU_OPTIONS_DISPLAY_FLASH,
    PULSES_MENU_OPTIONS_PULSE_THRESHOLD,
};

static const char *const pulsesMenuOptions[] = {
    "Sound",
#if defined(VIBRATION)
    "Vibration",
#endif
#if defined(PULSE_LED)
    "Pulse LED",
#endif
    "Display flash",
    "Threshold",
    NULL,
};

static const char *onPulsesMenuGetOption(const Menu *menu,
                                         uint32_t index,
                                         MenuStyle *menuStyle)
{
    switch (index)
    {
    case PULSES_MENU_OPTIONS_SOUND:
    case PULSES_MENU_OPTIONS_PULSE_THRESHOLD:
        *menuStyle = MENUSTYLE_SUBMENU;

        break;

#if defined(VIBRATION)
    case PULSES_MENU_OPTIONS_VIBRATION:
        *menuStyle = settings.pulseVibration;

        break;
#endif

#if defined(PULSE_LED)
    case PULSES_MENU_OPTIONS_PULSE_LED:
        *menuStyle = settings.pulseLED;

        break;
#endif

    case PULSES_MENU_OPTIONS_DISPLAY_FLASH:
        *menuStyle = settings.pulseDisplayFlash;

        break;
    }

    return pulsesMenuOptions[index];
}

static void onPulsesMenuSelect(const Menu *menu)
{
    switch (menu->state->selectedIndex)
    {
    case PULSES_MENU_OPTIONS_SOUND:
        setView(&pulsesSoundMenuView);

        break;

#if defined(VIBRATION)
    case PULSES_MENU_OPTIONS_VIBRATION:
        settings.pulseVibration = !settings.pulseVibration;

        break;
#endif

#if defined(PULSE_LED)
    case PULSES_MENU_OPTIONS_PULSE_LED:
        settings.pulseLED = !settings.pulseLED;

        break;
#endif

    case PULSES_MENU_OPTIONS_DISPLAY_FLASH:
        settings.pulseDisplayFlash = !settings.pulseDisplayFlash;

        break;

    case PULSES_MENU_OPTIONS_PULSE_THRESHOLD:
        setView(&pulseThresholdMenuView);

        break;
    }
}

static MenuState pulsesMenuState;

static const Menu pulsesMenu = {
    "Pulses",
    &pulsesMenuState,
    onPulsesMenuGetOption,
    onPulsesMenuSelect,
    onSettingsSubMenuBack,
};

const View pulsesMenuView = {
    onMenuEvent,
    &pulsesMenu,
};

void onPulsesSubMenuBack(const Menu *menu)
{
    setView(&pulsesMenuView);
}
