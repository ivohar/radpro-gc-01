/*
 * Rad Pro
 * LED
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#if defined(PULSE_LED)

#include "menu.h"
#include "led.h"
#include "settings.h"
#include "tube.h"

static const Menu pulseLEDMenu;

void initLED(void)
{
    initLEDController();

    selectMenuItem(&pulseLEDMenu,
                   settings.pulseLED,
                   PULSE_LED_NUM);
}

// Pulse LED menu

static const char *const pulseLEDMenuOptions[] = {
    "Off",
    "On",
    NULL,
};

static const char *onPulseLEDMenuGetOption(const Menu *menu,
                                           uint32_t index,
                                           MenuStyle *menuStyle)
{
    *menuStyle = (index == settings.pulseLED);

    return pulseLEDMenuOptions[index];
}

static void onPulseLEDMenuSelect(const Menu *menu)
{
    settings.pulseLED = menu->state->selectedIndex;
}

static MenuState pulseLEDMenuState;

static const Menu pulseLEDMenu = {
    "Pulse LED",
    &pulseLEDMenuState,
    onPulseLEDMenuGetOption,
    onPulseLEDMenuSelect,
    onPulsesSubMenuBack,
};

const View pulseLEDMenuView = {
    onMenuEvent,
    &pulseLEDMenu,
};

#endif
