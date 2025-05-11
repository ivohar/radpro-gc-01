/*
 * Rad Pro
 * Keyboard
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#include <stdint.h>

#include "cmath.h"
#include "events.h"
#include "keyboard.h"
#include "settings.h"

#if defined(KEYBOARD_2KEYS) || defined(KEYBOARD_3KEYS)
#define KEY_PRESSED_LONG ((uint32_t)(0.25 * SYSTICK_FREQUENCY / KEY_TICKS))
#define KEY_PRESSED_EXTENDED ((uint32_t)(1.0 * SYSTICK_FREQUENCY / KEY_TICKS))
#elif defined(KEYBOARD_5KEYS)
#define KEY_REPEAT_START ((uint32_t)(0.5 * SYSTICK_FREQUENCY / KEY_TICKS))
#define KEY_REPEAT_PERIOD ((uint32_t)(0.05 * SYSTICK_FREQUENCY / KEY_TICKS))
#define KEY_PRESSED_LONG ((uint32_t)(1.0 * SYSTICK_FREQUENCY / KEY_TICKS))
#endif

#define EVENT_QUEUE_SIZE 16
#define EVENT_QUEUE_MASK (EVENT_QUEUE_SIZE - 1)

static struct
{
    bool isInitialized;

    bool wasKeyDown[KEY_NUM];

    Key pressedKey;
    uint32_t pressedTicks;

    bool isBacklightWake;

    KeyboardMode mode;

    volatile uint32_t eventQueueHead;
    volatile uint32_t eventQueueTail;
    volatile Event eventQueue[EVENT_QUEUE_SIZE];
} keyboard;

void initKeyboard(void)
{
    initKeyboardController();

    getKeyboardState(keyboard.wasKeyDown);

    keyboard.pressedKey = KEY_NONE;
    keyboard.pressedTicks = ((uint32_t)(10.0 * SYSTICK_FREQUENCY / KEY_TICKS));
    keyboard.isInitialized = true;
}

void onKeyboardTick(void)
{
    if (!keyboard.isInitialized)
        return;

    Event event = EVENT_NONE;

    bool isKeyDown[KEY_NUM];
    getKeyboardState(isKeyDown);

    bool isBacklightPressed = false;
    bool isBacklightReleased = false;

    for (int32_t i = 0; i < KEY_NUM; i++)
    {
        // Key down
        if (!keyboard.wasKeyDown[i] &&
            isKeyDown[i])
        {
#if defined(DISPLAY_MONOCHROME)
            if ((settings.displaySleep != DISPLAY_SLEEP_ALWAYS_OFF) &&
                !isDisplayBacklightActive())
#elif defined(DISPLAY_COLOR)
            if (!isDisplayBacklightActive())
#endif
                isBacklightPressed = true;

#if defined(KEYBOARD_5KEYS)
            if ((i == KEY_UP) || (i == KEY_DOWN))
                event = i;
#endif

            keyboard.pressedKey = i;
            keyboard.pressedTicks = 0;
        }

        // Key up
        if (keyboard.wasKeyDown[i] &&
            !isKeyDown[i] &&
            (i == keyboard.pressedKey))
        {
            isBacklightReleased = true;

#if defined(KEYBOARD_2KEYS)
            if (keyboard.mode == KEYBOARD_MODE_MEASUREMENT)
            {
                if (keyboard.pressedTicks < KEY_PRESSED_LONG)
                {
                    if (i == KEY_RIGHT)
                        event = EVENT_KEY_BACK;
                }
                if (keyboard.pressedTicks < KEY_PRESSED_EXTENDED)
                {
                    if (i == KEY_LEFT)
                        event = EVENT_KEY_DOWN;
                }
            }
            else
            {
                if (keyboard.pressedTicks < KEY_PRESSED_LONG)
                {
                    if (i == KEY_LEFT)
                        event = EVENT_KEY_UP;
                    else if (i == KEY_RIGHT)
                        event = EVENT_KEY_DOWN;
                }
            }
#elif defined(KEYBOARD_3KEYS)
            if (keyboard.mode == KEYBOARD_MODE_MEASUREMENT)
            {
                if (keyboard.pressedTicks < KEY_PRESSED_LONG)
                {
                    if (i == KEY_RIGHT)
                        event = EVENT_KEY_DOWN;
                }
                if (keyboard.pressedTicks < KEY_PRESSED_EXTENDED)
                {
                    if (i == KEY_LEFT)
                        event = EVENT_KEY_UP;
                    else if (i == KEY_RIGHT)
                        event = EVENT_KEY_DOWN;
                    else if (i == KEY_OK)
                        event = EVENT_KEY_BACK;
                }
            }
            else
            {
                if (keyboard.pressedTicks < KEY_PRESSED_LONG)
                {
                    if (i == KEY_LEFT)
                        event = EVENT_KEY_UP;
                    else if (i == KEY_RIGHT)
                        event = EVENT_KEY_DOWN;
                }
                if (keyboard.pressedTicks < KEY_PRESSED_EXTENDED)
                {
                    if (i == KEY_OK)
                        event = EVENT_KEY_SELECT;
                }
            }
#elif defined(KEYBOARD_5KEYS)
            if (keyboard.pressedTicks < KEY_PRESSED_LONG)
            {
                if (i == KEY_LEFT)
                    event = EVENT_KEY_BACK;
                else if (i == KEY_RIGHT)
                    event = EVENT_KEY_SELECT;
                else if (i == KEY_OK)
                {
                    if (keyboard.mode == KEYBOARD_MODE_MEASUREMENT)
                    {
                        if (!keyboard.isBacklightWake)
                            event = EVENT_KEY_TOGGLEBACKLIGHT;
                    }
                    else
                        event = EVENT_KEY_SELECT;
                }
            }
#endif

            keyboard.pressedKey = KEY_NONE;
        }

        keyboard.wasKeyDown[i] = isKeyDown[i];
    }

    if (keyboard.pressedKey != KEY_NONE)
    {
        keyboard.pressedTicks += 1;

        // Key timeout
#if defined(KEYBOARD_2KEYS)
        if (keyboard.mode == KEYBOARD_MODE_MEASUREMENT)
        {
            if (keyboard.pressedTicks == KEY_PRESSED_LONG)
            {
                if (!isKeyDown[KEY_LEFT] && isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_SELECT;
            }
            else if (keyboard.pressedTicks == KEY_PRESSED_EXTENDED)
            {
                if (isKeyDown[KEY_LEFT] && isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_TOGGLELOCK;
                else if (isKeyDown[KEY_LEFT])
                    event = EVENT_KEY_RESET;
                else if (isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_POWER;
            }
        }
        else
        {
            if (keyboard.pressedTicks == KEY_PRESSED_LONG)
            {
                if (isKeyDown[KEY_LEFT] && !isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_BACK;
                else if (!isKeyDown[KEY_LEFT] && isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_SELECT;
            }
            else if (keyboard.pressedTicks == KEY_PRESSED_EXTENDED)
            {
                if (isKeyDown[KEY_LEFT] && isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_TOGGLELOCK;
                else if (isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_POWER;
            }
        }
#elif defined(KEYBOARD_3KEYS)
        if (keyboard.mode == KEYBOARD_MODE_MEASUREMENT)
        {
            if (keyboard.pressedTicks == KEY_PRESSED_LONG)
            {
                if (isKeyDown[KEY_RIGHT] && !isKeyDown[KEY_LEFT])
                    event = EVENT_KEY_SELECT;
            }
            else if (keyboard.pressedTicks == KEY_PRESSED_EXTENDED)
            {
                if (isKeyDown[KEY_LEFT] && isKeyDown[KEY_OK])
                    event = EVENT_KEY_TOGGLELOCK;
                else if (isKeyDown[KEY_LEFT] && isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_TOGGLEPULSECLICKS;
                else if (isKeyDown[KEY_LEFT])
                    event = EVENT_KEY_RESET;
                else if (isKeyDown[KEY_OK])
                    event = EVENT_KEY_POWER;
            }
        }
        else
        {
            if (keyboard.pressedTicks == KEY_PRESSED_LONG)
            {
                if (isKeyDown[KEY_LEFT] && !isKeyDown[KEY_OK])
                    event = EVENT_KEY_BACK;
                else if (isKeyDown[KEY_RIGHT])
                    event = EVENT_KEY_SELECT;
            }
            else if (keyboard.pressedTicks == KEY_PRESSED_EXTENDED)
            {
                if (isKeyDown[KEY_LEFT] && isKeyDown[KEY_OK])
                    event = EVENT_KEY_TOGGLELOCK;
                else if (isKeyDown[KEY_OK])
                    event = EVENT_KEY_POWER;
            }
        }
#elif defined(KEYBOARD_5KEYS)
        if (keyboard.pressedTicks >= KEY_REPEAT_START)
        {
            uint32_t repeatTicks = (keyboard.pressedTicks - KEY_REPEAT_START) % KEY_REPEAT_PERIOD;
            if (repeatTicks == 0)
            {
                if ((keyboard.pressedKey == KEY_UP) ||
                    (keyboard.pressedKey == KEY_DOWN))
                    event = (Event)keyboard.pressedKey;
            }
        }

        if (keyboard.pressedTicks == KEY_PRESSED_LONG)
        {
            if (isKeyDown[KEY_LEFT] && isKeyDown[KEY_OK])
                event = EVENT_KEY_TOGGLELOCK;
            else if (keyboard.pressedKey == KEY_OK)
                event = EVENT_KEY_POWER;
            else if (keyboard.pressedKey == KEY_LEFT)
                event = EVENT_KEY_RESET;
            else if (keyboard.pressedKey == KEY_RIGHT)
                event = EVENT_KEY_TOGGLEPULSECLICKS;
        }
#endif
    }

    // Enqueue event
    bool ignoreEvent = keyboard.isBacklightWake &&
                       (event != EVENT_KEY_POWER) &&
                       (event != EVENT_KEY_TOGGLEBACKLIGHT);

    if (isBacklightPressed)
    {
        event = EVENT_KEY_TOGGLEBACKLIGHT;
        keyboard.isBacklightWake = true;
    }

    if (isBacklightReleased)
        keyboard.isBacklightWake = false;

    if ((event == EVENT_NONE) ||
        ignoreEvent)
        return;

    keyboard.eventQueue[keyboard.eventQueueHead] = event;
    keyboard.eventQueueHead = (keyboard.eventQueueHead + 1) & EVENT_QUEUE_MASK;
}

void setKeyboardMode(KeyboardMode mode)
{
#if defined(KEYBOARD_2KEYS)
    if (mode == KEYBOARD_MODE_MEASUREMENT)
        keyboard.pressedKey = KEY_NONE;
#else
    keyboard.pressedKey = KEY_NONE;
#endif

    keyboard.mode = mode;
}

Event getKeyboardEvent(void)
{
    if (keyboard.eventQueueHead != keyboard.eventQueueTail)
    {
        // Dequeue event
        Event event = keyboard.eventQueue[keyboard.eventQueueTail];
        keyboard.eventQueueTail = (keyboard.eventQueueTail + 1) & EVENT_QUEUE_MASK;

        return event;
    }

    return EVENT_NONE;
}
