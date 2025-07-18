/*
 * Rad Pro
 * Display
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#include <limits.h>

#include <mcu-renderer.h>

#include "cmath.h"
#include "cstring.h"
#include "display.h"
#include "events.h"
#include "measurements.h"
#include "power.h"
#include "rtc.h"
#include "settings.h"
#include "system.h"
#include "tube.h"
#include "view.h"

// Fonts

#include FONT_SMALL
#include FONT_MEDIUM
#include FONT_LARGE
#include FONT_SYMBOLS

// Display metrics

#if defined(DISPLAY_128X64)

#define DISPLAY_LANDSCAPE

#define CONTENT_PADDING 3

#define TITLEBAR_HEIGHT 8
#define TITLEBAR_TIME_WIDTH (20 + 2)
#define TITLEBAR_TIME_PADDING 1
#define TITLEBAR_NOTIFICATIONICON_WIDTH 9
#define TITLEBAR_NOTIFICATIONICON_PADDING 2
#define TITLEBAR_BATTERYICON_WIDTH 9
#define TITLEBAR_SHADOW_HEIGHT 0

#define MENU_LINE_HEIGHT 14
#define MENU_SUBMENU_WIDTH 5
#define MENU_CHECKED_WIDTH 8

#define MEASUREMENT_PADDING 7
#define MEASUREMENT_VALUE_HEIGHT 36
#define MEASUREMENT_VALUE_OFFSET_X (CONTENT_WIDTH - 6)
#define MEASUREMENT_VALUE_OFFSET_Y 2
#define MEASUREMENT_UNIT_X 78
#define MEASUREMENT_CONFIDENCE_HEIGHT (CONTENT_BOTTOM - MEASUREMENT_UNIT_BOTTOM)
#define MEASUREMENT_CONFIDENCE_OFFSET_Y 0
#define MEASUREMENT_INFO_KEY_Y MEASUREMENT_VALUE_BOTTOM
#define MEASUREMENT_INFO_KEY_HEIGHT FONT_MEDIUM_LINE_HEIGHT
#define MEASUREMENT_INFO_KEY_OFFSET_Y (FONT_MEDIUM_LINE_HEIGHT - FONT_SMALL_LINE_HEIGHT)
#define MEASUREMENT_INFO_VALUE_HEIGHT MEASUREMENT_CONFIDENCE_HEIGHT
#define MEASUREMENT_STATE_X 32
#define MEASUREMENT_BAR_LABEL_Y MEASUREMENT_VALUE_BOTTOM
#define MEASUREMENT_BAR_LABEL_HEIGHT (FONT_SMALL_LINE_HEIGHT + 1)
#define MEASUREMENT_BAR_TICKS_HEIGHT 2
#define MEASUREMENT_BAR_TICKS_UPPER_HEIGHT 1
#define MEASUREMENT_BAR_TICKS_LOWERMARGIN_HEIGHT 0
#define MEASUREMENT_BAR_FRAME_HEIGHT 9
#define MEASUREMENT_BAR_FRAME_RIGHT MEASUREMENT_BAR_RIGHT

#define HISTORY_WIDTH 122
#define HISTORY_HEIGHT 42
#define HISTORY_TOP_LABEL_OFFSET_X (HISTORY_WIDTH - 1)
#define HISTORY_TOP_LABEL_OFFSET_Y (HISTORY_Y - CONTENT_Y - FONT_SMALL_LINE_HEIGHT)
#define HISTORY_BOTTOM_LABEL_OFFSET_X (HISTORY_WIDTH - 1)
#define HISTORY_BOTTOM_LABEL_OFFSET_Y -1

#define GAME_BOARD_X 0
#define GAME_BOARD_Y 0
#define GAME_SQUARE_WIDTH 9
#define GAME_SQUARE_HEIGHT 8
#define GAME_HISTORY_FIRST_WIDTH 32
#define GAME_HISTORY_FIRST_OFFSET_X 8

#define STATISTICS_KEY_WIDTH (CONTENT_WIDTH / 2)

#elif defined(DISPLAY_320X240)

#define DISPLAY_LANDSCAPE

#define CONTENT_PADDING 12

#define TITLEBAR_HEIGHT 40
#define TITLEBAR_TIME_WIDTH (53 + 12)
#define TITLEBAR_TIME_PADDING 12
#define TITLEBAR_NOTIFICATIONICON_WIDTH 24
#define TITLEBAR_NOTIFICATIONICON_PADDING 12
#define TITLEBAR_BATTERYICON_WIDTH 24
#define TITLEBAR_SHADOW_HEIGHT 2

#define MENU_LINE_HEIGHT 50
#define MENU_SUBMENU_WIDTH 12
#define MENU_CHECKED_WIDTH 21

#define MEASUREMENT_PADDING 15
#define MEASUREMENT_VALUE_HEIGHT 122
#define MEASUREMENT_VALUE_OFFSET_X (CONTENT_WIDTH - 10)
#define MEASUREMENT_VALUE_OFFSET_Y 3
#define MEASUREMENT_UNIT_X 206
#define MEASUREMENT_CONFIDENCE_HEIGHT (CONTENT_BOTTOM - MEASUREMENT_UNIT_BOTTOM)
#define MEASUREMENT_CONFIDENCE_OFFSET_Y -2
#define MEASUREMENT_INFO_KEY_Y MEASUREMENT_VALUE_BOTTOM
#define MEASUREMENT_INFO_KEY_HEIGHT (CONTENT_HEIGHT - MEASUREMENT_VALUE_HEIGHT - MEASUREMENT_INFO_VALUE_HEIGHT)
#define MEASUREMENT_INFO_KEY_OFFSET_Y (MEASUREMENT_INFO_KEY_HEIGHT - FONT_SMALL_LINE_HEIGHT)
#define MEASUREMENT_INFO_VALUE_HEIGHT (FONT_SMALL_LINE_HEIGHT + 11)
#define MEASUREMENT_STATE_X 80
#define MEASUREMENT_BAR_LABEL_Y MEASUREMENT_VALUE_BOTTOM
#define MEASUREMENT_BAR_LABEL_HEIGHT (10 + FONT_SMALL_LINE_HEIGHT)
#define MEASUREMENT_BAR_TICKS_HEIGHT 8
#define MEASUREMENT_BAR_TICKS_UPPER_HEIGHT 4
#define MEASUREMENT_BAR_TICKS_LOWERMARGIN_HEIGHT 4
#define MEASUREMENT_BAR_FRAME_HEIGHT 20
#define MEASUREMENT_BAR_FRAME_RIGHT MEASUREMENT_BAR_RIGHT

#define HISTORY_WIDTH 302
#define HISTORY_HEIGHT 130
#define HISTORY_TOP_LABEL_OFFSET_X (HISTORY_WIDTH - 5)
#define HISTORY_TOP_LABEL_OFFSET_Y (HISTORY_Y - CONTENT_Y - FONT_SMALL_LINE_HEIGHT - 2)
#define HISTORY_BOTTOM_LABEL_OFFSET_X (HISTORY_WIDTH - 5)
#define HISTORY_BOTTOM_LABEL_OFFSET_Y 2

#define GAME_BOARD_X 0
#define GAME_BOARD_Y TITLEBAR_BOTTOM
#define GAME_SQUARE_WIDTH 25
#define GAME_SQUARE_HEIGHT 25
#define GAME_HISTORY_FIRST_WIDTH 66
#define GAME_HISTORY_FIRST_OFFSET_X 12

#define STATISTICS_KEY_WIDTH (CONTENT_WIDTH / 2)

#elif defined(DISPLAY_240X320)

#define DISPLAY_PORTRAIT

#define CONTENT_PADDING 8

#define TITLEBAR_HEIGHT 40
#define TITLEBAR_TIME_WIDTH (53 + 8)
#define TITLEBAR_TIME_PADDING 8
#define TITLEBAR_NOTIFICATIONICON_WIDTH 24
#define TITLEBAR_NOTIFICATIONICON_PADDING 8
#define TITLEBAR_BATTERYICON_WIDTH 24
#define TITLEBAR_SHADOW_HEIGHT 2

#define MENU_LINE_HEIGHT 46
#define MENU_SUBMENU_WIDTH 12
#define MENU_CHECKED_WIDTH 21

#define MEASUREMENT_PADDING 15
#define MEASUREMENT_VALUE_HEIGHT 150
#define MEASUREMENT_VALUE_OFFSET_X (CONTENT_WIDTH - 10)
#define MEASUREMENT_VALUE_OFFSET_Y 62
#define MEASUREMENT_UNIT_X CONTENT_X
#define MEASUREMENT_CONFIDENCE_HEIGHT FONT_SMALL_LINE_HEIGHT
#define MEASUREMENT_CONFIDENCE_OFFSET_Y -2
#define MEASUREMENT_INFO_KEY_Y MEASUREMENT_CONFIDENCE_BOTTOM
#define MEASUREMENT_INFO_KEY_HEIGHT 43
#define MEASUREMENT_INFO_KEY_OFFSET_Y (MEASUREMENT_INFO_KEY_HEIGHT - FONT_SMALL_LINE_HEIGHT)
#define MEASUREMENT_INFO_VALUE_HEIGHT (CONTENT_BOTTOM - MEASUREMENT_INFO_VALUE_Y)
#define MEASUREMENT_STATE_X (CONTENT_X + CONTENT_WIDTH / 2)
#define MEASUREMENT_BAR_LABEL_Y 241
#define MEASUREMENT_BAR_LABEL_HEIGHT (MEASUREMENT_INFO_KEY_HEIGHT - FONT_SMALL_LINE_HEIGHT / 2)
#define MEASUREMENT_BAR_TICKS_HEIGHT 8
#define MEASUREMENT_BAR_TICKS_UPPER_HEIGHT 4
#define MEASUREMENT_BAR_TICKS_LOWERMARGIN_HEIGHT 4
#define MEASUREMENT_BAR_FRAME_HEIGHT 20
#define MEASUREMENT_BAR_FRAME_RIGHT (MEASUREMENT_BAR_RIGHT - MEASUREMENT_PADDING)

#define HISTORY_WIDTH 202
#define HISTORY_HEIGHT 130
#define HISTORY_TOP_LABEL_OFFSET_X (HISTORY_WIDTH - 5)
#define HISTORY_TOP_LABEL_OFFSET_Y (HISTORY_Y - CONTENT_Y - FONT_SMALL_LINE_HEIGHT - 2)
#define HISTORY_BOTTOM_LABEL_OFFSET_X (HISTORY_WIDTH - 5)
#define HISTORY_BOTTOM_LABEL_OFFSET_Y 2

#define GAME_BOARD_X ((CONTENT_WIDTH - GAME_BOARD_WIDTH) / 2)
#define GAME_BOARD_Y (TITLEBAR_BOTTOM + GAME_BOARD_X)
#define GAME_SQUARE_WIDTH 25
#define GAME_SQUARE_HEIGHT 25
#define GAME_HISTORY_FIRST_WIDTH 68
#define GAME_HISTORY_FIRST_OFFSET_X 10

#define STATISTICS_KEY_WIDTH 110

#endif

// Common metrics

#define TITLEBAR_X 0
#define TITLEBAR_Y 0
#define TITLEBAR_WIDTH DISPLAY_WIDTH
#define TITLEBAR_BOTTOM (TITLEBAR_Y + TITLEBAR_HEIGHT)
#define TITLEBAR_PADDING CONTENT_PADDING
#define TITLEBAR_CONTENT_HEIGHT (TITLEBAR_HEIGHT - TITLEBAR_SHADOW_HEIGHT)
#define TITLEBAR_TITLE_OFFSET_X TITLEBAR_PADDING
#define TITLEBAR_TITLE_OFFSET_Y ((TITLEBAR_CONTENT_HEIGHT - FONT_SMALL_LINE_HEIGHT) / 2)
#define TITLEBAR_TIME_OFFSET_X (TITLEBAR_TIME_WIDTH - TITLEBAR_TIME_PADDING)
#define TITLEBAR_TIME_OFFSET_Y ((TITLEBAR_CONTENT_HEIGHT - FONT_SMALL_LINE_HEIGHT) / 2)
#define TITLEBAR_NOTIFICATIONICON_HEIGHT FONT_SYMBOLS_LINE_HEIGHT
#define TITLEBAR_NOTIFICATIONICON_OFFSET_X 0
#define TITLEBAR_NOTIFICATIONICON_OFFSET_Y ((TITLEBAR_CONTENT_HEIGHT - TITLEBAR_NOTIFICATIONICON_HEIGHT) / 2)
#define TITLEBAR_BATTERYICON_HEIGHT FONT_SYMBOLS_LINE_HEIGHT
#define TITLEBAR_BATTERYICON_OFFSET_X 0
#define TITLEBAR_BATTERYICON_OFFSET_Y ((TITLEBAR_CONTENT_HEIGHT - TITLEBAR_BATTERYICON_HEIGHT) / 2)

#define CONTENT_X 0
#define CONTENT_Y TITLEBAR_BOTTOM
#define CONTENT_WIDTH DISPLAY_WIDTH
#define CONTENT_HEIGHT (DISPLAY_HEIGHT - TITLEBAR_BOTTOM)
#define CONTENT_RIGHT (CONTENT_X + CONTENT_WIDTH)
#define CONTENT_BOTTOM (CONTENT_Y + CONTENT_HEIGHT)

#define MENU_PADDING CONTENT_PADDING
#define MENU_LINE_NUM (CONTENT_HEIGHT / MENU_LINE_HEIGHT)

#define MEASUREMENT_VALUE_WIDTH CONTENT_WIDTH
#define MEASUREMENT_VALUE_X CONTENT_X
#define MEASUREMENT_VALUE_Y CONTENT_Y
#define MEASUREMENT_VALUE_BOTTOM (MEASUREMENT_VALUE_Y + MEASUREMENT_VALUE_HEIGHT)
#define MEASUREMENT_UNIT_Y MEASUREMENT_VALUE_BOTTOM
#define MEASUREMENT_UNIT_WIDTH (CONTENT_WIDTH - MEASUREMENT_UNIT_X)
#define MEASUREMENT_UNIT_HEIGHT FONT_MEDIUM_LINE_HEIGHT
#define MEASUREMENT_UNIT_BOTTOM (MEASUREMENT_UNIT_Y + MEASUREMENT_UNIT_HEIGHT)
#define MEASUREMENT_UNIT_OFFSET_X (MEASUREMENT_UNIT_WIDTH - MEASUREMENT_PADDING)
#define MEASUREMENT_UNIT_OFFSET_Y 0
#define MEASUREMENT_CONFIDENCE_X MEASUREMENT_UNIT_X
#define MEASUREMENT_CONFIDENCE_Y MEASUREMENT_UNIT_BOTTOM
#define MEASUREMENT_CONFIDENCE_WIDTH MEASUREMENT_UNIT_WIDTH
#define MEASUREMENT_CONFIDENCE_OFFSET_X (MEASUREMENT_CONFIDENCE_WIDTH - MEASUREMENT_PADDING)
#define MEASUREMENT_CONFIDENCE_BOTTOM (MEASUREMENT_CONFIDENCE_Y + MEASUREMENT_CONFIDENCE_HEIGHT)
#define MEASUREMENT_INFO_X CONTENT_X
#define MEASUREMENT_INFO_WIDTH (MEASUREMENT_STATE_RIGHT - CONTENT_X)
#define MEASUREMENT_INFO_KEY_OFFSET_X MEASUREMENT_PADDING
#define MEASUREMENT_INFO_VALUE_Y (MEASUREMENT_INFO_KEY_Y + MEASUREMENT_INFO_KEY_HEIGHT)
#define MEASUREMENT_INFO_VALUE_WIDTH (MEASUREMENT_STATE_RIGHT - MEASUREMENT_INFO_X)
#define MEASUREMENT_INFO_VALUE_OFFSET_X MEASUREMENT_PADDING
#define MEASUREMENT_INFO_VALUE_OFFSET_Y 0
#define MEASUREMENT_STATE_Y MEASUREMENT_INFO_KEY_Y
#define MEASUREMENT_STATE_HEIGHT MEASUREMENT_INFO_KEY_HEIGHT
#if defined(DISPLAY_LANDSCAPE)
#define MEASUREMENT_STATE_WIDTH (MEASUREMENT_UNIT_X - MEASUREMENT_STATE_X)
#define MEASUREMENT_STATE_OFFSET_X (CONTENT_WIDTH / 2 - MEASUREMENT_STATE_X)
#elif defined(DISPLAY_PORTRAIT)
#define MEASUREMENT_STATE_WIDTH (CONTENT_X + CONTENT_WIDTH - MEASUREMENT_STATE_X)
#define MEASUREMENT_STATE_OFFSET_X (MEASUREMENT_STATE_WIDTH - MEASUREMENT_PADDING)
#endif
#define MEASUREMENT_STATE_OFFSET_Y MEASUREMENT_INFO_KEY_OFFSET_Y
#define MEASUREMENT_STATE_RIGHT (MEASUREMENT_STATE_X + MEASUREMENT_STATE_WIDTH)
#define MEASUREMENT_BAR_DECADES 7
#define MEASUREMENT_BAR_WIDTH (MEASUREMENT_STATE_RIGHT - CONTENT_X)
#define MEASUREMENT_BAR_RIGHT MEASUREMENT_STATE_RIGHT
#define MEASUREMENT_BAR_LABEL_OFFSET_Y (MEASUREMENT_BAR_LABEL_HEIGHT - FONT_SMALL_LINE_HEIGHT)
#define MEASUREMENT_BAR_TICKS_X (CONTENT_X + MEASUREMENT_PADDING + 2)
#define MEASUREMENT_BAR_TICKS_Y (MEASUREMENT_BAR_LABEL_Y + MEASUREMENT_BAR_LABEL_HEIGHT)
#define MEASUREMENT_BAR_TICKS_WIDTH (MEASUREMENT_BAR_FRAME_RIGHT - MEASUREMENT_BAR_TICKS_X - 2)
#define MEASUREMENT_BAR_TICKS_LOWER_HEIGHT (MEASUREMENT_BAR_TICKS_HEIGHT - MEASUREMENT_BAR_TICKS_UPPER_HEIGHT)
#define MEASUREMENT_BAR_FRAME_X (CONTENT_X + MEASUREMENT_PADDING)
#define MEASUREMENT_BAR_FRAME_Y (MEASUREMENT_BAR_OUTERMARGIN_Y + MEASUREMENT_BAR_TICKS_LOWERMARGIN_HEIGHT)
#define MEASUREMENT_BAR_FRAME_WIDTH (MEASUREMENT_BAR_FRAME_RIGHT - MEASUREMENT_BAR_FRAME_X)
#define MEASUREMENT_BAR_FRAME_BOTTOM (MEASUREMENT_BAR_FRAME_Y + MEASUREMENT_BAR_FRAME_HEIGHT)
#define MEASUREMENT_BAR_INNERFRAME_X (MEASUREMENT_BAR_FRAME_X + 1)
#define MEASUREMENT_BAR_INNERFRAME_Y (MEASUREMENT_BAR_FRAME_Y + 1)
#define MEASUREMENT_BAR_INNERFRAME_WIDTH (MEASUREMENT_BAR_FRAME_WIDTH - 2)
#define MEASUREMENT_BAR_INNERFRAME_HEIGHT (MEASUREMENT_BAR_FRAME_HEIGHT - 2)
#define MEASUREMENT_BAR_INSTRUMENT_X (MEASUREMENT_BAR_INNERFRAME_X + 1)
#define MEASUREMENT_BAR_INSTRUMENT_Y (MEASUREMENT_BAR_INNERFRAME_Y + 1)
#define MEASUREMENT_BAR_INSTRUMENT_WIDTH (MEASUREMENT_BAR_INNERFRAME_WIDTH - 2)
#define MEASUREMENT_BAR_INSTRUMENT_HEIGHT (MEASUREMENT_BAR_INNERFRAME_HEIGHT - 2)
#define MEASUREMENT_BAR_OUTERMARGIN_X CONTENT_X
#define MEASUREMENT_BAR_OUTERMARGIN_Y (MEASUREMENT_BAR_TICKS_Y + MEASUREMENT_BAR_TICKS_HEIGHT)
#define MEASUREMENT_BAR_OUTERMARGIN_WIDTH (MEASUREMENT_BAR_RIGHT - MEASUREMENT_BAR_OUTERMARGIN_X)
#define MEASUREMENT_BAR_OUTERMARGIN_HEIGHT (CONTENT_BOTTOM - MEASUREMENT_BAR_OUTERMARGIN_Y)
#define MEASUREMENT_BAR_INNERMARGIN_X (CONTENT_X + MEASUREMENT_PADDING)
#define MEASUREMENT_BAR_INNERMARGIN_Y MEASUREMENT_BAR_FRAME_Y
#define MEASUREMENT_BAR_INNERMARGIN_WIDTH MEASUREMENT_BAR_FRAME_WIDTH
#define MEASUREMENT_BAR_INNERMARGIN_HEIGHT MEASUREMENT_BAR_FRAME_HEIGHT

#define HISTORY_X ((CONTENT_WIDTH - HISTORY_WIDTH) / 2)
#define HISTORY_Y (CONTENT_Y + (CONTENT_HEIGHT - HISTORY_HEIGHT) / 2)
#define HISTORY_DATA_X (HISTORY_X + 1)
#define HISTORY_DATA_Y (HISTORY_Y + 1)
#define HISTORY_DATA_WIDTH (HISTORY_WIDTH - 2)
#define HISTORY_DATA_HEIGHT (HISTORY_HEIGHT - 2)
#define HISTORY_TOP_LABEL_X HISTORY_X
#define HISTORY_TOP_LABEL_Y CONTENT_Y
#define HISTORY_TOP_LABEL_WIDTH HISTORY_WIDTH
#define HISTORY_TOP_LABEL_HEIGHT (HISTORY_Y - HISTORY_TOP_LABEL_Y)
#define HISTORY_BOTTOM_LABEL_X HISTORY_X
#define HISTORY_BOTTOM_LABEL_Y (HISTORY_Y + HISTORY_HEIGHT)
#define HISTORY_BOTTOM_LABEL_WIDTH HISTORY_WIDTH
#define HISTORY_BOTTOM_LABEL_HEIGHT (CONTENT_BOTTOM - HISTORY_BOTTOM_LABEL_Y)
#define HISTORY_SPACE_LEFT_X 0
#define HISTORY_SPACE_LEFT_Y HISTORY_TOP_LABEL_Y
#define HISTORY_SPACE_LEFT_WIDTH HISTORY_X
#define HISTORY_SPACE_LEFT_HEIGHT (CONTENT_BOTTOM - HISTORY_TOP_LABEL_Y)
#define HISTORY_SPACE_RIGHT_X (HISTORY_X + HISTORY_WIDTH)
#define HISTORY_SPACE_RIGHT_Y HISTORY_TOP_LABEL_Y
#define HISTORY_SPACE_RIGHT_WIDTH (CONTENT_WIDTH - HISTORY_SPACE_RIGHT_X)
#define HISTORY_SPACE_RIGHT_HEIGHT HISTORY_SPACE_LEFT_HEIGHT

#define RNG_DATA_X CONTENT_X
#define RNG_DATA_Y CONTENT_Y
#define RNG_DATA_WIDTH CONTENT_WIDTH
#define RNG_DATA_HEIGHT ((CONTENT_HEIGHT + FONT_SMALL_LINE_HEIGHT) / 2)
#define RNG_DATA_OFFSET_X CONTENT_PADDING
#define RNG_DATA_OFFSET_Y (RNG_DATA_HEIGHT - FONT_SMALL_LINE_HEIGHT)
#define RNG_STATE_X CONTENT_X
#define RNG_STATE_Y (RNG_DATA_Y + RNG_DATA_HEIGHT)
#define RNG_STATE_WIDTH CONTENT_WIDTH
#define RNG_STATE_HEIGHT (CONTENT_BOTTOM - RNG_STATE_Y)
#define RNG_STATE_OFFSET_X (CONTENT_WIDTH / 2)
#define RNG_STATE_OFFSET_Y ((RNG_STATE_HEIGHT - FONT_MEDIUM_LINE_HEIGHT) / 2)

#define STATISTICS_ENTRY_HEIGHT FONT_SMALL_LINE_HEIGHT
#define STATISTICS_ENTRIES_HEIGHT (STATISTICS_ENTRY_NUM * STATISTICS_ENTRY_HEIGHT)
#define STATISTICS_TOPMARGIN_HEIGHT ((CONTENT_HEIGHT - STATISTICS_ENTRIES_HEIGHT) / 2)
#define STATISTICS_BOTTOMMARGIN_HEIGHT (CONTENT_HEIGHT - STATISTICS_TOPMARGIN_HEIGHT - STATISTICS_ENTRIES_HEIGHT)
#define STATISTICS_KEY_X 0
#define STATISTICS_KEY_OFFSET_X CONTENT_PADDING
#define STATISTICS_VALUE_X STATISTICS_KEY_WIDTH
#define STATISTICS_VALUE_WIDTH (CONTENT_WIDTH - STATISTICS_KEY_WIDTH)
#define STATISTICS_VALUE_OFFSET_X 0

#define GAME_BOARD_WIDTH (8 * GAME_SQUARE_WIDTH)
#define GAME_BOARD_HEIGHT (8 * GAME_SQUARE_HEIGHT)
#define GAME_BOARD_RIGHT (GAME_BOARD_X + GAME_BOARD_WIDTH)
#define GAME_BOARD_BOTTOM (GAME_BOARD_Y + GAME_BOARD_HEIGHT)
#define GAME_HISTORY_SECOND_WIDTH (GAME_HISTORY_WIDTH - GAME_HISTORY_FIRST_WIDTH)
#if defined(DISPLAY_LANDSCAPE)
#define GAME_TOP_TIMER_X GAME_BOARD_RIGHT
#define GAME_TOP_TIMER_Y GAME_BOARD_Y
#define GAME_TOP_TIMER_WIDTH (CONTENT_RIGHT - GAME_BOARD_RIGHT)
#define GAME_TOP_TIMER_HEIGHT (GAME_HISTORY_Y - GAME_BOARD_Y)
#define GAME_TOP_TIMER_OFFSET_X (FONT_SMALL_LINE_HEIGHT / 2)
#define GAME_TOP_TIMER_OFFSET_Y ((2 * GAME_SQUARE_HEIGHT - FONT_SMALL_LINE_HEIGHT) / 2)
#define GAME_HISTORY_X GAME_TOP_TIMER_X
#define GAME_HISTORY_Y (GAME_BOARD_Y + (GAME_BOARD_HEIGHT - GAME_HISTORY_HEIGHT) / 2)
#define GAME_HISTORY_WIDTH (CONTENT_RIGHT - GAME_BOARD_RIGHT)
#define GAME_HISTORY_HEIGHT (3 * FONT_SMALL_LINE_HEIGHT)
#define GAME_BOTTOM_TIMER_X GAME_BOARD_RIGHT
#define GAME_BOTTOM_TIMER_Y (GAME_HISTORY_Y + GAME_HISTORY_HEIGHT)
#define GAME_BOTTOM_TIMER_WIDTH (CONTENT_RIGHT - GAME_BOARD_RIGHT)
#define GAME_BOTTOM_TIMER_HEIGHT (GAME_BOARD_Y + GAME_BOARD_HEIGHT - GAME_BOTTOM_TIMER_Y)
#define GAME_BOTTOM_TIMER_OFFSET_X GAME_TOP_TIMER_OFFSET_X
#define GAME_BOTTOM_TIMER_OFFSET_Y (GAME_BOTTOM_TIMER_HEIGHT - 2 * GAME_SQUARE_HEIGHT + GAME_TOP_TIMER_OFFSET_Y)
#elif defined(DISPLAY_PORTRAIT)
#define GAME_TOP_TIMER_X GAME_BOARD_X
#define GAME_TOP_TIMER_Y CONTENT_Y
#define GAME_TOP_TIMER_WIDTH GAME_BOARD_WIDTH
#define GAME_TOP_TIMER_HEIGHT (GAME_BOARD_Y - GAME_TOP_TIMER_Y)
#define GAME_TOP_TIMER_OFFSET_X (GAME_TOP_TIMER_WIDTH - FONT_SMALL_LINE_HEIGHT / 2)
#define GAME_TOP_TIMER_OFFSET_Y 0
#define GAME_HISTORY_X GAME_BOARD_X
#define GAME_HISTORY_Y GAME_BOARD_BOTTOM
#define GAME_HISTORY_WIDTH (GAME_BOARD_WIDTH - GAME_BOTTOM_TIMER_WIDTH)
#define GAME_HISTORY_HEIGHT (CONTENT_BOTTOM - GAME_BOARD_BOTTOM)
#define GAME_BOTTOM_TIMER_X (GAME_BOARD_RIGHT - GAME_BOTTOM_TIMER_WIDTH)
#define GAME_BOTTOM_TIMER_Y (GAME_BOARD_Y + GAME_BOARD_HEIGHT)
#define GAME_BOTTOM_TIMER_WIDTH (3 * GAME_SQUARE_WIDTH)
#define GAME_BOTTOM_TIMER_HEIGHT (CONTENT_BOTTOM - GAME_BOARD_BOTTOM)
#define GAME_BOTTOM_TIMER_OFFSET_X (GAME_BOTTOM_TIMER_WIDTH - FONT_SMALL_LINE_HEIGHT / 2)
#define GAME_BOTTOM_TIMER_OFFSET_Y 0
#define GAME_LEFTMARGIN_X CONTENT_X
#define GAME_LEFTMARGIN_Y CONTENT_Y
#define GAME_LEFTMARGIN_WIDTH (GAME_BOARD_X - CONTENT_X)
#define GAME_LEFTMARGIN_HEIGHT CONTENT_HEIGHT
#define GAME_RIGHTMARGIN_X (CONTENT_X + GAME_BOARD_RIGHT)
#define GAME_RIGHTMARGIN_Y CONTENT_Y
#define GAME_RIGHTMARGIN_WIDTH (CONTENT_RIGHT - GAME_BOARD_RIGHT)
#define GAME_RIGHTMARGIN_HEIGHT CONTENT_HEIGHT
#endif

// Colors

typedef enum
{
    COLOR_ELEMENT_ACTIVE,
    COLOR_ELEMENT_NEUTRAL,

    COLOR_CONTAINER_BACKGROUND,
    COLOR_CONTAINER_GLOBAL,
    COLOR_CONTAINER_GLOBAL_SHADOW1,
    COLOR_CONTAINER_GLOBAL_SHADOW2,
    COLOR_CONTAINER_GLOBAL_SHADOW1_MENU,
    COLOR_CONTAINER_GLOBAL_SHADOW2_MENU,

    COLOR_INSTRUMENT_ENHANCED_PRIMARY,
    COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_PRIMARY,
    COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_TERTIARY,
    COLOR_INSTRUMENT_FRAME_PRIMARY,
    COLOR_INSTRUMENT_FRAME_TERTIARY,

    COLOR_INSTRUMENT_ENHANCED_PRIMARY_ALERTZONE1,
    COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_PRIMARY_ALERTZONE1,
    COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_TERTIARY_ALERTZONE1,
    COLOR_INSTRUMENT_FRAME_PRIMARY_ALERTZONE1,
    COLOR_INSTRUMENT_FRAME_TERTIARY_ALERTZONE1,

    COLOR_INSTRUMENT_ENHANCED_PRIMARY_ALERTZONE2,
    COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_PRIMARY_ALERTZONE2,
    COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_TERTIARY_ALERTZONE2,
    COLOR_INSTRUMENT_FRAME_PRIMARY_ALERTZONE2,
    COLOR_INSTRUMENT_FRAME_TERTIARY_ALERTZONE2,

    COLOR_INSTRUMENT_TICKMARK_PRIMARY,
    COLOR_INSTRUMENT_TICKMARK_SECONDARY,

    COLOR_RUNNING,
    COLOR_ALARM,

    COLOR_NORMAL_ENABLED_BACKGROUND,
    COLOR_FLAT_CHECKED_BACKGROUND,
    COLOR_ON_FLAT_ACTIVE,
    COLOR_ON_FLAT_NEUTRAL,

    COLOR_SELECTED_ENABLED_BACKGROUND,
    COLOR_GAME_SQUARE_BLACK,
    COLOR_GAME_SQUARE_WHITE,
    COLOR_GAME_PIECE_BLACK,
    COLOR_GAME_PIECE_WHITE,
} Color;

#define COLOR_INSTRUMENT_OFFSET 5

#if defined(DISPLAY_COLOR)

// Online color blender: https://pinetools.com/blend-colors
// Online RGB565 color picker: https://rgbcolorpicker.com/565

#if defined(DISPLAY_EXTRA_COLOR_SCHEMES)
static const mr_color_t displayColors[][4] = {
{mr_get_color(0x292929), mr_get_color(0xF2F0F2), mr_get_color(0x7C7C00), mr_get_color(0xD068D0)},
{mr_get_color(0x747274), mr_get_color(0x9A9A9A), mr_get_color(0x7C7B00), mr_get_color(0x308C30)},
{mr_get_color(0xF0F0F0), mr_get_color(0x1F1F1F), mr_get_color(0x000000), mr_get_color(0xB8DCB8)},
{mr_get_color(0xF9FBF9), mr_get_color(0x262726), mr_get_color(0x191900), mr_get_color(0xE0F4E0)},
{mr_get_color(0xBDBDBD), mr_get_color(0x181818), mr_get_color(0x000000), mr_get_color(0x804080)},
{mr_get_color(0xD6D6D6), mr_get_color(0x1F1F1F), mr_get_color(0x000000), mr_get_color(0x40A040)},
{mr_get_color(0xBDBDBD), mr_get_color(0x202020), mr_get_color(0x000000), mr_get_color(0x804080)},
{mr_get_color(0xD6D6D6), mr_get_color(0x282828), mr_get_color(0x000000), mr_get_color(0x40A040)},
{mr_get_color(0x466B99), mr_get_color(0x5696C7), mr_get_color(0x006834), mr_get_color(0x906CD0)},
{mr_get_color(0xC1C6DB), mr_get_color(0x3B4550), mr_get_color(0x142F07), mr_get_color(0xB07858)},
{mr_get_color(0xADB3BB), mr_get_color(0x222C37), mr_get_color(0x415807), mr_get_color(0xC00068)},
{mr_get_color(0xFFFFFF), mr_get_color(0x333233), mr_get_color(0x191900), mr_get_color(0xF8FCF8)},
{mr_get_color(0xC3C2C3), mr_get_color(0x141214), mr_get_color(0x515500), mr_get_color(0xC860C8)},
{mr_get_color(0x416591), mr_get_color(0x45789F), mr_get_color(0x005B2A), mr_get_color(0x705070)},
{mr_get_color(0xB6BBC3), mr_get_color(0x2F3740), mr_get_color(0x0F2505), mr_get_color(0x2834C8)},
{mr_get_color(0xA4A9B1), mr_get_color(0x1B232C), mr_get_color(0x344D05), mr_get_color(0x50C4E8)},
{mr_get_color(0xDBDFDB), mr_get_color(0x292929), mr_get_color(0x141400), mr_get_color(0x58B458)},
{mr_get_color(0xB8B8B8), mr_get_color(0x101010), mr_get_color(0x414800), mr_get_color(0x402040)},
{mr_get_color(0x3D5F8A), mr_get_color(0x37607F), mr_get_color(0x005123), mr_get_color(0x503418)},
{mr_get_color(0xACB1B8), mr_get_color(0x262C33), mr_get_color(0x0C1B05), mr_get_color(0xB0F440)},
{mr_get_color(0x9B9FA7), mr_get_color(0x161C23), mr_get_color(0x2A3E05), mr_get_color(0xE88C70)},
{mr_get_color(0xC3C2C3), mr_get_color(0x212121), mr_get_color(0x0F1100), mr_get_color(0xC860C8)},
{mr_get_color(0xADADAD), mr_get_color(0x0D0F0D), mr_get_color(0x343900), mr_get_color(0xC0E0C0)},
{mr_get_color(0x474647), mr_get_color(0xE6E6E6), mr_get_color(0x7C7C00), mr_get_color(0x98C898)},
{mr_get_color(0x7C7C7C), mr_get_color(0x9C9C9C), mr_get_color(0x686700), mr_get_color(0x80C080)},
{mr_get_color(0x358C35), mr_get_color(0x138A00), mr_get_color(0x396E00), mr_get_color(0x101C10)},
{mr_get_color(0xC72B2B), mr_get_color(0xE52626), mr_get_color(0x7F000C), mr_get_color(0xF86CD8)},
{mr_get_color(0xFFFFFF), mr_get_color(0x333233), mr_get_color(0x000000), mr_get_color(0xF8FCF8)},
{mr_get_color(0xA8ACB2), mr_get_color(0x3C5275), mr_get_color(0x005B2D), mr_get_color(0x80D8F8)},
{mr_get_color(0x232426), mr_get_color(0xF2F0F2), mr_get_color(0x7C7C00), mr_get_color(0xB05CC0)},
{mr_get_color(0x7E7F7E), mr_get_color(0xA3A4A3), mr_get_color(0x7C7B00), mr_get_color(0x98D098)},
{mr_get_color(0x446894), mr_get_color(0x5779A2), mr_get_color(0x3E6952), mr_get_color(0x805C90)},
{mr_get_color(0xAD937C), mr_get_color(0xB89070), mr_get_color(0x505000), mr_get_color(0xC04080)},
{mr_get_color(0xBDA88D), mr_get_color(0xD0B088), mr_get_color(0x646400), mr_get_color(0x80C040)},
{mr_get_color(0x000000), mr_get_color(0x000000), mr_get_color(0x000000), mr_get_color(0x000000)},
{mr_get_color(0xFFFFFF), mr_get_color(0xFFFFFF), mr_get_color(0x7C7C00), mr_get_color(0xF8FCF8)}
};    
#else
static const mr_color_t displayColors[][3] = {    
    // Element active
    {mr_get_color(0x1a1a1a),
     mr_get_color(0xf2f0f2), // 0xf2f2f2
     mr_get_color(0x474700)},

    // Element neutral (on container background)
    {mr_get_color(0x666366), // 0x666666
     mr_get_color(0x9a9a9a),
     mr_get_color(0x474600)}, // 0x2e2e00 // orig 0x2e2f00

    // Container background [global background]
    {mr_get_color(0xf7f7f7),
     mr_get_color(0x1f1f1f),
     mr_get_color(0x000000)},

    // Container global [title bar background]
    {mr_get_color(0xfcfdfc), // 0xfcfcfc
     mr_get_color(0x262726), // 0x262626
     mr_get_color(0x0a0a00)},

    // Container global shadow 1
    {mr_get_color(0xd0d0d0),
     mr_get_color(0x181818),
     mr_get_color(0x000000)},

    // Container global shadow 2
    {mr_get_color(0xe8e8e8),
     mr_get_color(0x1f1f1f),
     mr_get_color(0x000000)},

    // Container global shadow 1 menu
    {mr_get_color(0xd0d0d0),
     mr_get_color(0x202020),
     mr_get_color(0x000000)},

    // Container global shadow 2 menu
    {mr_get_color(0xe8e8e8),
     mr_get_color(0x282828),
     mr_get_color(0x000000)},

    // Instrument enhanced primary
    {mr_get_color(0x325b9a),
     mr_get_color(0x5696c7),
     mr_get_color(0x003315)},

    // Instrument enhanced tertiary (on instrument frame primary)
    {mr_get_color(0xd6deeb),
     mr_get_color(0x3b4550),
     mr_get_color(0x081303)}, // 0x081203

    // Instrument enhanced tertiary (on instrument frame tertiary)
    {mr_get_color(0xb8c0cd),
     mr_get_color(0x222c37),
     mr_get_color(0x1a2703)},

    // Instrument frame primary
    {mr_get_color(0xffffff),
     mr_get_color(0x333233), // 0x333333
     mr_get_color(0x0a0a00)},

    // Instrument frame tertiary
    {mr_get_color(0xd9d8d9), // 0xd9d9d9
     mr_get_color(0x141214), // 0x141414
     mr_get_color(0x212400)},

    // Instrument enhanced primary alert zone 1
    {mr_get_color(0x2e548e),
     mr_get_color(0x45789f),
     mr_get_color(0x002911)},

    // Instrument enhanced tertiary (on instrument frame primary) alert zone 1
    {mr_get_color(0xc5cdd9),
     mr_get_color(0x2f3740),
     mr_get_color(0x060f02)}, // 0x060e02

    // Instrument enhanced tertiary (on instrument frame tertiary) alert zone 1
    {mr_get_color(0xaab1bd),
     mr_get_color(0x1b232c),
     mr_get_color(0x151f02)},

    // Instrument frame primary alert zone 1
    {mr_get_color(0xebedeb), // 0xebebeb
     mr_get_color(0x292929), // 0x2b2b2b
     mr_get_color(0x080800)},

    // Instrument frame tertiary alert zone 1
    {mr_get_color(0xc8c8c8),
     mr_get_color(0x101010),
     mr_get_color(0x1a1d00)},

    // Instrument enhanced primary alert zone 2
    {mr_get_color(0x2a4d83),
     mr_get_color(0x37607f),
     mr_get_color(0x00210e)},

    // Instrument enhanced tertiary (on instrument frame primary) alert zone 2
    {mr_get_color(0xb6bdc8),
     mr_get_color(0x262c33),
     mr_get_color(0x050b02)},

    // Instrument enhanced tertiary (on instrument frame tertiary) alert zone 2
    {mr_get_color(0x9da3ae),
     mr_get_color(0x161c23),
     mr_get_color(0x111902)},

    // Instrument frame primary alert zone 2
    {mr_get_color(0xd9d8d9), // 0xd9d9d9
     mr_get_color(0x212121),
     mr_get_color(0x060700)}, // 0x060600

    // Instrument frame tertiary alert zone 2
    {mr_get_color(0xb8b8b8),
     mr_get_color(0x0d0f0d), // 0x0d0d0d
     mr_get_color(0x151700)},

    // Instrument tickmark primary
    {mr_get_color(0x333233), // 0x333333
     mr_get_color(0xe6e6e6),
     mr_get_color(0x474700)},

    // Instrument tickmark secondary
    {mr_get_color(0x707070),
     mr_get_color(0x9c9c9c),
     mr_get_color(0x333200)}, // 0x0x333300

    // Alarm
    {mr_get_color(0x228722),
     mr_get_color(0x138a00),
     mr_get_color(0x173800)},

    // Alarm
    {mr_get_color(0xdf1b1b),
     mr_get_color(0xe52626),
     mr_get_color(0x4d0005)},

    // Normal enabled background [menu background]
    {mr_get_color(0xffffff),
     mr_get_color(0x333233), // 0x333333
     mr_get_color(0x000000)},

    // Flat checked background (on container global) [menu selected background]
    {mr_get_color(0xb0b6bf),  // Changed alpha to 20% // orig 0xd6dde9 
     mr_get_color(0x3c5275),  // Changed alpha to 50% // orig 0x324563 
     mr_get_color(0x002912)}, // Changed alpha to 50% // orig 0x00230f 

    // On flat active (on container global) [menu selected foreground]
    {mr_get_color(0x161718),
     mr_get_color(0xf2f0f2), // 0xf2f2f2
     mr_get_color(0x474700)},

    // On flat neutral (on container global) [menu selected gadget]
    {mr_get_color(0x737473),  // 0x737373
     mr_get_color(0xa3a4a3),  // 0xa3a3a3
     mr_get_color(0x474600)}, // 0x333300 // orig 0x333200

    // Selected enabled background [game square selected]
    {mr_get_color(0x305792),
     mr_get_color(0x5779a2),
     mr_get_color(0x193422)},

    // Game square black
    {mr_get_color(0xb89070),
     mr_get_color(0xb89070),
     mr_get_color(0x202000)},

    // Game square white
    {mr_get_color(0xd0b088),
     mr_get_color(0xd0b088),
     mr_get_color(0x303000)},

    // Game piece black
    {mr_get_color(0x000000),
     mr_get_color(0x000000),
     mr_get_color(0x000000)},

    // Game piece white
    {mr_get_color(0xffffff),
     mr_get_color(0xffffff),
     mr_get_color(0x484800)},
};
#endif

#endif

void onDisplaySubMenuBack(const Menu *menu);

// Definitions

const uint32_t menuLineNum = MENU_LINE_NUM;

static const Menu displayMenu;
#if defined(DISPLAY_MONOCHROME)
static const Menu displayContrastMenu;
#elif defined(DISPLAY_COLOR)
static const Menu displayThemeMenu;
#endif
static const Menu displayBrightnessMenu;
static const Menu displaySleepMenu;

// Variables

mr_t mr;

// Functions

void resetDisplay(void)
{
    selectMenuItem(&displayMenu,
                   0,
                   0);

#if defined(DISPLAY_MONOCHROME)
    selectMenuItem(&displayContrastMenu,
                   settings.displayContrast,
                   DISPLAY_CONTRAST_NUM);
#elif defined(DISPLAY_COLOR)
    selectMenuItem(&displayThemeMenu,
                   settings.displayTheme,
                   DISPLAY_THEME_NUM);
#endif

    selectMenuItem(&displaySleepMenu,
                   settings.displaySleep,
                   DISPLAY_SLEEP_NUM);
    selectMenuItem(&displayBrightnessMenu,
                   settings.displayBrightness,
                   DISPLAY_BRIGHTNESS_NUM);
}

// Drawing functions

static inline mr_color_t getFillColor(Color color)
{
#if defined(DISPLAY_MONOCHROME)
    if ((color == COLOR_INSTRUMENT_ENHANCED_PRIMARY) ||
        (color == COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_PRIMARY) ||
        (color == COLOR_INSTRUMENT_FRAME_TERTIARY) ||
        (color == COLOR_INSTRUMENT_TICKMARK_PRIMARY) ||
        (color == COLOR_INSTRUMENT_TICKMARK_SECONDARY) ||
        (color == COLOR_FLAT_CHECKED_BACKGROUND) ||
        (color == COLOR_GAME_SQUARE_BLACK))
        return 0xffff;
    else
        return 0x0000;
#elif defined(DISPLAY_COLOR)
    return displayColors[color][settings.displayTheme];
#endif
}

static void setFillColor(Color color)
{
#if defined(DISPLAY_MONOCHROME)
    mr_color_t fillColor = getFillColor(color);
    mr_color_t strokeColor = fillColor ? 0x0000 : 0xffff;

    mr_set_fill_color(&mr, fillColor);
    mr_set_stroke_color(&mr, strokeColor);
#elif defined(DISPLAY_COLOR)
    mr_set_fill_color(&mr, getFillColor(color));
#endif
}

static void setStrokeColor(Color color)
{
#if defined(DISPLAY_COLOR)
    mr_set_stroke_color(&mr, getFillColor(color));
#endif
}

static void drawRectangle(const mr_rectangle_t *rectangle)
{
    mr_draw_rectangle(&mr, rectangle);
}

static void drawMargin(const mr_rectangle_t *outerMargin,
                       const mr_rectangle_t *innerMargin)
{
    mr_rectangle_t rectangle;

    mr_point_t outerRectangleEnd = {
        outerMargin->x + outerMargin->width,
        outerMargin->y + outerMargin->height};

    mr_point_t innerRectangleEnd = {
        innerMargin->x + innerMargin->width,
        innerMargin->y + innerMargin->height};

    // Top
    rectangle.x = outerMargin->x;
    rectangle.y = outerMargin->y;
    rectangle.width = outerMargin->width;
    rectangle.height = innerMargin->y - outerMargin->y;

    mr_draw_rectangle(&mr, &rectangle);

    // Bottom
    rectangle.y = innerRectangleEnd.y;
    rectangle.height = outerRectangleEnd.y - innerRectangleEnd.y;

    mr_draw_rectangle(&mr, &rectangle);

    // Left
    rectangle.y = innerMargin->y;
    rectangle.width = innerMargin->x - outerMargin->x;
    rectangle.height = innerMargin->height;

    mr_draw_rectangle(&mr, &rectangle);

    // Right
    rectangle.x = innerRectangleEnd.x;
    rectangle.width = outerRectangleEnd.y - innerRectangleEnd.y;

    mr_draw_rectangle(&mr, &rectangle);
}

static void drawFrame(const mr_rectangle_t *rectangle)
{
    mr_rectangle_t innerMargin = {
        rectangle->x + 1,
        rectangle->y + 1,
        rectangle->width - 2,
        rectangle->height - 2};

    drawMargin(rectangle,
               &innerMargin);
}

static void drawImage(const mr_rectangle_t *rectangle,
                      const mr_color_t *imageBuffer)
{
    mr_draw_image(&mr, rectangle, imageBuffer);
}

static void setFont(const uint8_t *font)
{
    mr_set_font(&mr, font);
}

static uint16_t getTextWidth(const char *str)
{
    return mr_get_utf8_text_width(&mr, (const uint8_t *)str);
}

#if defined(DISPLAY_MONOCHROME)
static void drawGrayedRectangle(const mr_rectangle_t *rectangle)
{
    mr_rectangle_t r;
    r.width = 1;
    r.height = 1;

    setFillColor(COLOR_CONTAINER_GLOBAL);

    for (r.y = rectangle->y;
         r.y < (rectangle->y + rectangle->height);
         r.y++)
    {
        for (r.x = rectangle->x;
             r.x < (rectangle->x + rectangle->width);
             r.x++)
        {
            if ((r.x ^ r.y) & 0b1)
                continue;

            mr_draw_rectangle(&mr, &r);
        }
    }
}
#endif

static void drawText(const char *str,
                     const mr_rectangle_t *rectangle,
                     const mr_point_t *offset)
{
    uint32_t availableWidth = rectangle->width - offset->x;

    if (getTextWidth(str) <= availableWidth)
        mr_draw_utf8_text(&mr, (const uint8_t *)str, rectangle, offset);
    else
    {
        char buffer[64];
        strcpy(buffer, str);

        for (int32_t i = (strlen(str) - 2); i > 0; i--)
        {
            strcpy(buffer + i, getString(STRING_ELLIPSIS));
            if (getTextWidth(buffer) <= availableWidth)
            {
                mr_draw_utf8_text(&mr, (const uint8_t *)buffer, rectangle, offset);

                break;
            }
        }
    }
}

static void drawCenteredText(const char *str,
                             const mr_rectangle_t *rectangle,
                             const mr_point_t *offset)
{
    mr_point_t centeredOffset = *offset;

    centeredOffset.x -= getTextWidth(str) / 2;

    drawText(str,
             rectangle,
             &centeredOffset);
}

static void drawRightAlignedText(const char *str,
                                 const mr_rectangle_t *rectangle,
                                 const mr_point_t *offset)
{
    mr_point_t rightAlignedOffset = *offset;

    rightAlignedOffset.x -= getTextWidth(str);

    drawText(str,
             rectangle,
             &rightAlignedOffset);
}

static void drawBatteryIcon(const mr_rectangle_t *rectangle,
                            const mr_point_t *offset)
{
    bool chargingBattery = isBatteryCharging();

    Color color;
    uint8_t level = getBatteryLevel();

    if (isUSBPowered() || chargingBattery)
        color = COLOR_RUNNING;
    else if (!chargingBattery && (level == 0))
        color = COLOR_ALARM;
    else
        color = COLOR_ELEMENT_NEUTRAL;

#if !defined(FONT_SYMBOLS_NOCHARGING)
    if (chargingBattery)
        level += BATTERY_LEVEL_NUM;
#endif

    char buffer[8];
    strclr(buffer);
    strcatChar(buffer, '0' + level);

    setFont(font_symbols);
    setStrokeColor(color);
    drawText(buffer,
             rectangle,
             offset);
}

void drawPowerOff(bool displayEnabled)
{
    setFillColor(COLOR_CONTAINER_BACKGROUND);

    const mr_rectangle_t rectangle = {
        0,
        0,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
    };
    const mr_point_t offset = {
        ((DISPLAY_WIDTH - TITLEBAR_BATTERYICON_WIDTH) / 2),
        ((DISPLAY_HEIGHT - FONT_SMALL_LINE_HEIGHT) / 2),
    };

    if (displayEnabled)
        drawBatteryIcon(&rectangle, &offset);
    else
        drawRectangle(&rectangle);
}

void drawTitleBar(const char *title,
                  bool isMenu)
{
    mr_rectangle_t rectangle = {
        TITLEBAR_WIDTH,
        TITLEBAR_Y,
        0,
        TITLEBAR_CONTENT_HEIGHT};

    setFillColor(COLOR_CONTAINER_GLOBAL);

    // Icons
    rectangle.width = TITLEBAR_BATTERYICON_WIDTH + TITLEBAR_PADDING;
    rectangle.x -= rectangle.width;
    const mr_point_t titlebarBatteryOffset = {
        TITLEBAR_BATTERYICON_OFFSET_X,
        TITLEBAR_BATTERYICON_OFFSET_Y,
    };
    drawBatteryIcon(&rectangle, &titlebarBatteryOffset);

    char buffer[8];
    mr_color_t color = COLOR_ELEMENT_NEUTRAL;
    strclr(buffer);
    if (isLockMode())
        strcatChar(buffer, '<');
    else if (isAlarm())
    {
        strcatChar(buffer, ';');
        color = COLOR_ALARM;
    }
    else if (isAlarmEnabled())
        strcatChar(buffer, ':');

    if (buffer[0])
    {
        rectangle.width = TITLEBAR_NOTIFICATIONICON_WIDTH + TITLEBAR_NOTIFICATIONICON_PADDING;
        rectangle.x -= rectangle.width;
        const mr_point_t titlebarNotificationOffset = {
            TITLEBAR_NOTIFICATIONICON_OFFSET_X,
            TITLEBAR_NOTIFICATIONICON_OFFSET_Y,
        };
        setStrokeColor(color);
        drawText(buffer,
                 &rectangle,
                 &titlebarNotificationOffset);
    }

    // Time
    RTCDateTime dateTime;
    getDeviceDateTime(&dateTime);

    strclr(buffer);
    if (dateTime.year >= RTC_YEAR_MIN)
    {
        if (settings.rtcTimeFormat == RTC_TIMEFORMAT_24HOUR)
            strcatUInt32(buffer, dateTime.hour, 2);
        else
        {
            uint32_t hour = dateTime.hour % 12;
            strcatUInt32(buffer, (hour == 0) ? 12 : hour, 1);
        }
        strcatChar(buffer, ':');
        strcatUInt32(buffer, dateTime.minute, 2);
    }

    rectangle.width = TITLEBAR_TIME_WIDTH;
    rectangle.x -= rectangle.width;
    const mr_point_t titlebarTimeOffset = {
        TITLEBAR_TIME_OFFSET_X,
        TITLEBAR_TIME_OFFSET_Y,
    };
    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_ACTIVE);
    drawRightAlignedText(buffer,
                         &rectangle,
                         &titlebarTimeOffset);

    // Title
    rectangle.width = rectangle.x - TITLEBAR_X;
    rectangle.x = TITLEBAR_X;
    const mr_point_t titlebarTitleOffset = {
        TITLEBAR_TITLE_OFFSET_X,
        TITLEBAR_TITLE_OFFSET_Y,
    };
    drawText(title,
             &rectangle,
             &titlebarTitleOffset);

    // Shadow
#if TITLEBAR_SHADOW_HEIGHT >= 2
    const mr_rectangle_t titlebarShadow1Rectangle = {
        TITLEBAR_X,
        TITLEBAR_BOTTOM - 2,
        TITLEBAR_WIDTH,
        1,
    };
    setFillColor(!isMenu
                     ? COLOR_CONTAINER_GLOBAL_SHADOW1
                     : COLOR_CONTAINER_GLOBAL_SHADOW1_MENU);
    drawRectangle(&titlebarShadow1Rectangle);
#endif

#if TITLEBAR_SHADOW_HEIGHT >= 1
    const mr_rectangle_t titlebarShadow2Rectangle = {
        TITLEBAR_X,
        TITLEBAR_BOTTOM - 1,
        TITLEBAR_WIDTH,
        1,
    };
    setFillColor(!isMenu
                     ? COLOR_CONTAINER_GLOBAL_SHADOW2
                     : COLOR_CONTAINER_GLOBAL_SHADOW2_MENU);
    drawRectangle(&titlebarShadow2Rectangle);
#endif

    // Set background
    setFillColor(COLOR_CONTAINER_BACKGROUND);
}

static void drawValueAndUnit(const char *valueString,
                             const char *unitString,
                             const mr_rectangle_t *rectangle,
                             const mr_point_t *offset,
                             bool isHold)
{
    mr_rectangle_t partialRectangle = *rectangle;

    partialRectangle.width = offset->x +
                             getTextWidth(valueString);

    setStrokeColor(isHold
                       ? COLOR_ELEMENT_ACTIVE
                       : COLOR_INSTRUMENT_ENHANCED_PRIMARY);
    drawText(valueString,
             &partialRectangle,
             offset);

    partialRectangle.x += partialRectangle.width;
    partialRectangle.width = rectangle->width - partialRectangle.width;

    mr_point_t partialOffset = {
        0,
        offset->y};

    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    drawText(unitString,
             &partialRectangle,
             &partialOffset);
}

void drawNotification(const char *title,
                      const char *subtitle)
{
    // Top line
    const mr_rectangle_t topRectangle = {
        0,
        0,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT / 2};

    const mr_point_t topOffset = {
        DISPLAY_WIDTH / 2,
        DISPLAY_HEIGHT / 2 - FONT_MEDIUM_LINE_HEIGHT};

    setFont(font_medium);
    setStrokeColor(COLOR_ELEMENT_ACTIVE);
    setFillColor(COLOR_CONTAINER_GLOBAL);
    drawCenteredText(title,
                     &topRectangle,
                     &topOffset);

    // Bottom line
    const mr_rectangle_t bottomRectangle = {
        0,
        DISPLAY_HEIGHT / 2,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT / 2};

    const mr_point_t bottomOffset = {
        DISPLAY_WIDTH / 2,
        FONT_MEDIUM_LINE_HEIGHT - FONT_SMALL_LINE_HEIGHT};

    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    drawCenteredText(subtitle,
                     &bottomRectangle,
                     &bottomOffset);
}

void drawMenu(const Menu *menu)
{
    drawTitleBar(menu->title, true);

    uint32_t startIndex = menu->state->startIndex;
    uint32_t selectedIndex = menu->state->selectedIndex;

    mr_rectangle_t rectangle;
    rectangle.height = MENU_LINE_HEIGHT;
    rectangle.y = CONTENT_Y;

    const mr_point_t menuOptionOffset = {
        MENU_PADDING,
        (MENU_LINE_HEIGHT - FONT_MEDIUM_LINE_HEIGHT) / 2};

    const mr_point_t menuGadgetOffset = {
        MENU_PADDING,
        (MENU_LINE_HEIGHT - FONT_SYMBOLS_LINE_HEIGHT) / 2};

    for (uint32_t i = 0; i < MENU_LINE_NUM; i++)
    {
        uint32_t index = startIndex + i;

        MenuStyle menuStyle;
        const char *menuItem = menu->onGetOption(menu,
                                                 index,
                                                 &menuStyle);

        if (!menuItem)
            break;

        rectangle.x = CONTENT_WIDTH;

        setFont(font_symbols);

        setFillColor((index == selectedIndex)
                         ? COLOR_FLAT_CHECKED_BACKGROUND
                         : COLOR_NORMAL_ENABLED_BACKGROUND);

        // Submenu
        if (menuStyle & MENUSTYLE_SUBMENU)
        {
            rectangle.width = MENU_SUBMENU_WIDTH + 2 * MENU_PADDING;
            rectangle.x -= rectangle.width;

            setStrokeColor(COLOR_ON_FLAT_NEUTRAL);
            drawText("=",
                     &rectangle,
                     &menuGadgetOffset);
        }

        // Checked
        if (menuStyle & MENUSTYLE_CHECKED)
        {
            rectangle.width = MENU_CHECKED_WIDTH + 2 * MENU_PADDING;
            rectangle.x -= rectangle.width;

            setStrokeColor(COLOR_INSTRUMENT_ENHANCED_PRIMARY);
            drawText(">",
                     &rectangle,
                     &menuGadgetOffset);
        }

        // Text
        rectangle.width = rectangle.x;
        rectangle.x = 0;

        setFont(font_medium);

        setStrokeColor(COLOR_ON_FLAT_ACTIVE);
        drawText(menuItem,
                 &rectangle,
                 &menuOptionOffset);

        rectangle.y += MENU_LINE_HEIGHT;
    }

    // Bottom space
    rectangle.x = CONTENT_X;
    rectangle.width = CONTENT_WIDTH;
    rectangle.height = CONTENT_BOTTOM - rectangle.y;

    setFillColor(COLOR_NORMAL_ENABLED_BACKGROUND);
    drawRectangle(&rectangle);
}

void drawTestMode(const char lines[8][32])
{
    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_ACTIVE);
    setFillColor(COLOR_NORMAL_ENABLED_BACKGROUND);

    mr_rectangle_t rectangle;
    const mr_point_t offset = {
        0,
        0};

    for (uint32_t y = 0; y < 4; y++)
        for (uint32_t x = 0; x < 2; x++)
        {
            rectangle = (mr_rectangle_t){
                (DISPLAY_WIDTH / 2) * x,
                FONT_SMALL_LINE_HEIGHT * y,
                (DISPLAY_WIDTH / 2),
                FONT_SMALL_LINE_HEIGHT};

            drawText(lines[y * 2 + x],
                     &rectangle,
                     &offset);
        }

    rectangle = (mr_rectangle_t){
        0,
        (4 * FONT_SMALL_LINE_HEIGHT),
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT - (4 * FONT_SMALL_LINE_HEIGHT)};

    drawRectangle(&rectangle);
}

void drawMeasurementValue(const char *valueString,
                          const char *unitString,
                          float confidence,
                          MeasurementStyle style)
{
    // Value
    const mr_rectangle_t valueRectangle = {
        MEASUREMENT_VALUE_X,
        MEASUREMENT_VALUE_Y,
        MEASUREMENT_VALUE_WIDTH,
        MEASUREMENT_VALUE_HEIGHT};
    const mr_point_t valueOffset = {
        MEASUREMENT_VALUE_OFFSET_X,
        MEASUREMENT_VALUE_OFFSET_Y};

    setFont(font_large);
    setStrokeColor(
        (style == MEASUREMENTSTYLE_ALARM)
            ? COLOR_ALARM
        : (style == MEASUREMENTSTYLE_HOLD)
            ? COLOR_ELEMENT_ACTIVE
            : COLOR_INSTRUMENT_ENHANCED_PRIMARY);
    drawRightAlignedText(valueString,
                         &valueRectangle,
                         &valueOffset);

#if defined(DISPLAY_MONOCHROME)
    if (style == MEASUREMENTSTYLE_ALARM)
        drawGrayedRectangle(&valueRectangle);
#endif

    // Unit
    const mr_rectangle_t unitRectangle = {
        MEASUREMENT_UNIT_X,
        MEASUREMENT_UNIT_Y,
        MEASUREMENT_UNIT_WIDTH,
        MEASUREMENT_UNIT_HEIGHT};

    const mr_point_t unitOffset = {
        MEASUREMENT_UNIT_OFFSET_X,
        MEASUREMENT_UNIT_OFFSET_Y};

    setFont(font_medium);
    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    drawRightAlignedText(unitString,
                         &unitRectangle,
                         &unitOffset);

    // Confidence
    char confidenceString[8];
    char confidenceUnit[8];

    if (confidence != 0)
    {
        strcpy(confidenceString, getString(STRING_PLUSMINUS));
        strcatFloat(confidenceString,
                    100 * confidence,
                    confidence < 0.1F ? 1 : 0);
        strcpy(confidenceUnit, getString(STRING_PERCENT));
    }
    else
    {
        strclr(confidenceString);
        strclr(confidenceUnit);
    }

    setFont(font_small);

    const mr_rectangle_t confidenceRectangle = {
        MEASUREMENT_CONFIDENCE_X,
        MEASUREMENT_CONFIDENCE_Y,
        MEASUREMENT_CONFIDENCE_WIDTH,
        MEASUREMENT_CONFIDENCE_HEIGHT};

    mr_point_t confidenceOffset = {
        MEASUREMENT_CONFIDENCE_OFFSET_X -
            getTextWidth(confidenceString) -
            getTextWidth(confidenceUnit),
        MEASUREMENT_CONFIDENCE_OFFSET_Y};

    drawValueAndUnit(confidenceString,
                     confidenceUnit,
                     &confidenceRectangle,
                     &confidenceOffset,
                     style == MEASUREMENTSTYLE_HOLD);
}

void drawMeasurementInfo(const char *infoKeyString,
                         const char *infoValueString,
                         const char *infoUnitString,
                         const char *stateString,
                         MeasurementStyle style)
{
    const mr_rectangle_t infoKeyRectangle = {
        MEASUREMENT_INFO_X,
        MEASUREMENT_INFO_KEY_Y,
        MEASUREMENT_INFO_WIDTH,
        MEASUREMENT_INFO_KEY_HEIGHT};

    const mr_point_t infoKeyOffset = {
        MEASUREMENT_INFO_KEY_OFFSET_X,
        MEASUREMENT_INFO_KEY_OFFSET_Y};

    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    drawText(infoKeyString,
             &infoKeyRectangle,
             &infoKeyOffset);

    const mr_rectangle_t stateRectangle = {
        MEASUREMENT_STATE_X,
        MEASUREMENT_STATE_Y,
        MEASUREMENT_STATE_WIDTH,
        MEASUREMENT_STATE_HEIGHT};

    const mr_point_t stateOffset = {
        MEASUREMENT_STATE_OFFSET_X,
        MEASUREMENT_STATE_OFFSET_Y};

    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
#if defined(DISPLAY_LANDSCAPE)
    drawCenteredText(stateString,
                     &stateRectangle,
                     &stateOffset);
#elif defined(DISPLAY_PORTRAIT)
    drawRightAlignedText(stateString,
                         &stateRectangle,
                         &stateOffset);
#endif

    const mr_rectangle_t infoValueRectangle = {
        MEASUREMENT_INFO_X,
        MEASUREMENT_INFO_VALUE_Y,
        MEASUREMENT_INFO_WIDTH,
        MEASUREMENT_INFO_VALUE_HEIGHT};

    const mr_point_t infoValueOffset = {
        MEASUREMENT_INFO_VALUE_OFFSET_X,
        MEASUREMENT_INFO_VALUE_OFFSET_Y};

    setStrokeColor((style == MEASUREMENTSTYLE_HOLD)
                       ? COLOR_ELEMENT_ACTIVE
                       : COLOR_INSTRUMENT_ENHANCED_PRIMARY);
    drawValueAndUnit(infoValueString,
                     infoUnitString,
                     &infoValueRectangle,
                     &infoValueOffset,
                     style == MEASUREMENTSTYLE_HOLD);
}

static uint32_t getMeasurementBarInstrumentValue(float value,
                                                 float minValue)
{
    if (value <= minValue)
        return 0;

    float x = value / minValue;
    float y = log10f(x);
    uint32_t instrumentValue = y *
                               MEASUREMENT_BAR_INSTRUMENT_WIDTH /
                               MEASUREMENT_BAR_DECADES;

    if (instrumentValue > MEASUREMENT_BAR_INSTRUMENT_WIDTH)
        return MEASUREMENT_BAR_INSTRUMENT_WIDTH;

    return instrumentValue;
}

void drawMeasurementBar(float value,
                        int32_t minExponent,
                        float alertZone1Value,
                        float alertZone2Value)
{
    mr_rectangle_t rectangle;
    mr_color_t color;

    float minValue = powf(10.0F, minExponent);
    uint32_t instrumentValue = getMeasurementBarInstrumentValue(value, minValue);
#if defined(DISPLAY_COLOR)
    uint32_t instrumentAlertZone1Value = getMeasurementBarInstrumentValue(alertZone1Value, minValue);
    uint32_t instrumentAlertZone2Value = getMeasurementBarInstrumentValue(alertZone2Value, minValue);
#endif

    // Labels & ticks
    uint32_t xTick = 0;
    uint32_t xLabel = 0;
    uint32_t xLabelCenter = 0;
    bool wasLargeTick = false;
    for (uint32_t i = 0; i < (2 * (MEASUREMENT_BAR_DECADES + 1) + 1); i++)
    {
        bool isTick = i & 0b1;
        uint32_t index = i >> 1;
        uint32_t exponentIndex = minExponent + index;
        uint32_t metricPowerIndex = (30 + exponentIndex) % 3;
        bool isLargeTick = isTick && (metricPowerIndex == 0);

        uint32_t xTickNext = MEASUREMENT_BAR_TICKS_X +
                             (MEASUREMENT_BAR_TICKS_WIDTH - 1) * index /
                                 MEASUREMENT_BAR_DECADES +
                             isTick;
        if (i == 2 * (MEASUREMENT_BAR_DECADES + 1))
            xTickNext = MEASUREMENT_BAR_RIGHT;

        if (wasLargeTick)
        {
            char buffer[8];
            strclr(buffer);
            strcatDecimalPowerWithMetricPrefix(buffer, exponentIndex - 1, -3);

            uint32_t width = (i < (2 * (MEASUREMENT_BAR_DECADES - 1)))
                                 ? xTickNext - xLabel
                                 : MEASUREMENT_BAR_RIGHT - xLabel;

            rectangle = (mr_rectangle_t){
                xLabel,
                MEASUREMENT_BAR_LABEL_Y,
                width,
                MEASUREMENT_BAR_LABEL_HEIGHT};

            mr_point_t offset = {
                xLabelCenter - xLabel,
                MEASUREMENT_BAR_LABEL_OFFSET_Y,
            };

            setStrokeColor(COLOR_ELEMENT_ACTIVE);
            setFillColor(COLOR_CONTAINER_BACKGROUND);
            drawCenteredText(buffer,
                             &rectangle,
                             &offset);

            xLabel = xTickNext;
        }

        xLabelCenter = xTickNext;
        wasLargeTick = isLargeTick;

        uint32_t tickWidth = isTick
                                 ? 1
                                 : xTickNext - xTick;
        color = isLargeTick
                    ? COLOR_INSTRUMENT_TICKMARK_PRIMARY
                    : COLOR_CONTAINER_BACKGROUND;

        rectangle = (mr_rectangle_t){
            xTick,
            MEASUREMENT_BAR_TICKS_Y,
            tickWidth,
            MEASUREMENT_BAR_TICKS_UPPER_HEIGHT};

        setFillColor(color);
        drawRectangle(&rectangle);

        color = isLargeTick
                    ? COLOR_INSTRUMENT_TICKMARK_PRIMARY
                : isTick
                    ? COLOR_INSTRUMENT_TICKMARK_SECONDARY
                    : COLOR_CONTAINER_BACKGROUND;

        rectangle.y += MEASUREMENT_BAR_TICKS_UPPER_HEIGHT;
        rectangle.height = MEASUREMENT_BAR_TICKS_LOWER_HEIGHT;

        setFillColor(color);
        drawRectangle(&rectangle);

        xTick = xTickNext;
    }

    // Bar outer frame
    const mr_rectangle_t barFrameRectangle = {
        MEASUREMENT_BAR_FRAME_X,
        MEASUREMENT_BAR_FRAME_Y,
        MEASUREMENT_BAR_FRAME_WIDTH,
        MEASUREMENT_BAR_FRAME_HEIGHT};

    setFillColor(COLOR_INSTRUMENT_FRAME_TERTIARY);
    drawFrame(&barFrameRectangle);

    // Bar inner frame
    rectangle = (mr_rectangle_t){
        MEASUREMENT_BAR_INNERFRAME_X,
        MEASUREMENT_BAR_INNERFRAME_Y,
        1,
        MEASUREMENT_BAR_INNERFRAME_HEIGHT};

    setFillColor(COLOR_INSTRUMENT_FRAME_PRIMARY);
    drawRectangle(&rectangle);

    rectangle.x = MEASUREMENT_BAR_INNERFRAME_X + MEASUREMENT_BAR_INNERFRAME_WIDTH - 1;

    setFillColor(COLOR_INSTRUMENT_FRAME_PRIMARY + 2 * COLOR_INSTRUMENT_OFFSET);
    drawRectangle(&rectangle);

    // Bar
    uint32_t xBar = 0;
    while (xBar < MEASUREMENT_BAR_INSTRUMENT_WIDTH)
    {
        uint32_t xBarNext = 0;
        if (xBar < instrumentValue)
            xBarNext = instrumentValue;
        else
            xBarNext = MEASUREMENT_BAR_INSTRUMENT_WIDTH;

        // Color
        uint32_t alertZoneColor;
#if defined(DISPLAY_MONOCHROME)
        alertZoneColor = 0;
#elif defined(DISPLAY_COLOR)
        if (xBar < instrumentAlertZone1Value)
        {
            if (xBarNext > instrumentAlertZone1Value)
                xBarNext = instrumentAlertZone1Value;

            alertZoneColor = 0;
        }
        else if (xBar < instrumentAlertZone2Value)
        {
            if (xBarNext > instrumentAlertZone2Value)
                xBarNext = instrumentAlertZone2Value;

            alertZoneColor = COLOR_INSTRUMENT_OFFSET;
        }
        else
            alertZoneColor = 2 * COLOR_INSTRUMENT_OFFSET;
#endif

        // Draw
        rectangle = (mr_rectangle_t){
            MEASUREMENT_BAR_INSTRUMENT_X + xBar,
            MEASUREMENT_BAR_INSTRUMENT_Y,
            xBarNext - xBar,
            MEASUREMENT_BAR_INSTRUMENT_HEIGHT,
        };

        setFillColor(alertZoneColor + ((xBar < instrumentValue)
                                           ? COLOR_INSTRUMENT_ENHANCED_PRIMARY
                                           : COLOR_INSTRUMENT_FRAME_PRIMARY));
        drawRectangle(&rectangle);

        rectangle.y = MEASUREMENT_BAR_INNERFRAME_Y;
        rectangle.height = 1;

        setFillColor(alertZoneColor + COLOR_INSTRUMENT_FRAME_PRIMARY);
        drawRectangle(&rectangle);

        rectangle.y = MEASUREMENT_BAR_INNERFRAME_Y +
                      MEASUREMENT_BAR_INNERFRAME_HEIGHT - 1;

        drawRectangle(&rectangle);

        xBar = xBarNext;
    }

    // Margin
    const mr_rectangle_t barOuterMargin = {
        MEASUREMENT_BAR_OUTERMARGIN_X,
        MEASUREMENT_BAR_OUTERMARGIN_Y,
        MEASUREMENT_BAR_OUTERMARGIN_WIDTH,
        MEASUREMENT_BAR_OUTERMARGIN_HEIGHT};

    const mr_rectangle_t barInnerMargin = {
        MEASUREMENT_BAR_INNERMARGIN_X,
        MEASUREMENT_BAR_INNERMARGIN_Y,
        MEASUREMENT_BAR_INNERMARGIN_WIDTH,
        MEASUREMENT_BAR_INNERMARGIN_HEIGHT};

    setFillColor(COLOR_CONTAINER_BACKGROUND);
    drawMargin(&barOuterMargin,
               &barInnerMargin);
}

static int16_t getHistoryY(uint8_t value,
                           uint32_t dataExponentMinValue,
                           uint32_t dataScale)
{
    return value
               ? ((value - dataExponentMinValue) * dataScale) >> 8
               : 0;
}

void drawHistory(float scale,
                 const char *unitString,
                 uint32_t timeTickNum,
                 const uint8_t *data,
                 uint8_t alertZone1Value,
                 uint8_t alertZone2Value)
{
    // Analysis
    uint32_t dataMax = 0;
    uint32_t dataMin = UCHAR_MAX;
    for (uint32_t i = 0; i < HISTORY_BUFFER_SIZE; i++)
    {
        uint32_t value = data[i];

        if (value)
        {
            if (value > dataMax)
                dataMax = value;

            if (value < dataMin)
                dataMin = value;
        }
    }

    char topLabelString[16];
    char bottomLabelString[16];
    int32_t dataExponentMinValue = 0;
    uint32_t dataTickNum = 1;

    strclr(topLabelString);
    strclr(bottomLabelString);

    if (dataMax > 0)
    {
        uint32_t cpsToRateUnit = (uint32_t)(HISTORY_DECADE * (16 + log10f(scale * HISTORY_VALUE_MIN))) + 1;

        int32_t exponentMax = (int32_t)((cpsToRateUnit + dataMax) / HISTORY_DECADE - 16 + 1);
        int32_t exponentMin = (int32_t)((cpsToRateUnit + dataMin) / HISTORY_DECADE - 16);

        dataExponentMinValue = (int32_t)((exponentMin + 16) * HISTORY_DECADE - cpsToRateUnit);
        dataTickNum = exponentMax - exponentMin;

        strcatDecimalPowerWithMetricPrefix(topLabelString, exponentMax, -2);
        strcat(topLabelString, unitString);

        strcatDecimalPowerWithMetricPrefix(bottomLabelString, exponentMin, -2);
        strcat(bottomLabelString, unitString);
    }

    uint32_t dataRange = HISTORY_DECADE * dataTickNum;

    // Labels
    const mr_rectangle_t topLabelRectangle = {
        HISTORY_TOP_LABEL_X,
        HISTORY_TOP_LABEL_Y,
        HISTORY_TOP_LABEL_WIDTH,
        HISTORY_TOP_LABEL_HEIGHT};

    const mr_point_t topLabelOffset = {
        HISTORY_TOP_LABEL_OFFSET_X,
        HISTORY_TOP_LABEL_OFFSET_Y};

    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_ACTIVE);
    drawRightAlignedText(topLabelString,
                         &topLabelRectangle,
                         &topLabelOffset);

    const mr_rectangle_t bottomLabelRectangle = {
        HISTORY_BOTTOM_LABEL_X,
        HISTORY_BOTTOM_LABEL_Y,
        HISTORY_BOTTOM_LABEL_WIDTH,
        HISTORY_BOTTOM_LABEL_HEIGHT};

    const mr_point_t bottomLabelOffset = {
        HISTORY_BOTTOM_LABEL_OFFSET_X,
        HISTORY_BOTTOM_LABEL_OFFSET_Y};

    drawRightAlignedText(bottomLabelString,
                         &bottomLabelRectangle,
                         &bottomLabelOffset);

    // Space and frame
    const mr_rectangle_t leftSpaceRectangle = {
        HISTORY_SPACE_LEFT_X,
        HISTORY_SPACE_LEFT_Y,
        HISTORY_SPACE_LEFT_WIDTH,
        HISTORY_SPACE_LEFT_HEIGHT};

    drawRectangle(&leftSpaceRectangle);

    const mr_rectangle_t rightSpaceRectangle = {
        HISTORY_SPACE_RIGHT_X,
        HISTORY_SPACE_RIGHT_Y,
        HISTORY_SPACE_RIGHT_WIDTH,
        HISTORY_SPACE_RIGHT_HEIGHT};

    drawRectangle(&rightSpaceRectangle);

    const mr_rectangle_t frameRectangle = {
        HISTORY_X,
        HISTORY_Y,
        HISTORY_WIDTH,
        HISTORY_HEIGHT};

    setFillColor(COLOR_INSTRUMENT_FRAME_TERTIARY);
    drawFrame(&frameRectangle);

    // Value tick table
    uint8_t valueTickTable[32];
    uint32_t valueTickTableIndex = 0;

    for (uint32_t yTickIndex = 1;
         yTickIndex <= timeTickNum;
         yTickIndex++)
    {
        int16_t yTickNext = yTickIndex * HISTORY_DATA_HEIGHT / dataTickNum;

        valueTickTable[valueTickTableIndex++] = yTickNext;
        valueTickTable[valueTickTableIndex++] = yTickNext + 1;
    }

    // History
    uint32_t dataScale = 257 * HISTORY_DATA_HEIGHT / dataRange;

    int16_t timeTickIndex = 1;
    int16_t timeTickNext = HISTORY_DATA_WIDTH / timeTickNum;

    int16_t yDataLast = 0;
#if defined(DISPLAY_COLOR)
    int16_t yAlertZone1 = getHistoryY(alertZone1Value,
                                      dataExponentMinValue,
                                      dataScale);
    int16_t yAlertZone2 = getHistoryY(alertZone2Value,
                                      dataExponentMinValue,
                                      dataScale);
#endif

    for (int16_t x = 0; x < HISTORY_DATA_WIDTH; x++)
    {
        // Time tick
        bool timeTickDraw = false;
        if (x == timeTickNext)
        {
            timeTickIndex++;
            timeTickNext = timeTickIndex * HISTORY_DATA_WIDTH / timeTickNum;
            timeTickDraw = true;
        }

        // Value tick
        valueTickTableIndex = 0;

        // Data
        int16_t yData = getHistoryY(data[x],
                                    dataExponentMinValue,
                                    dataScale);

        if (x == 0)
            yDataLast = yData;

        int16_t yDataBottom;
        int16_t yDataTop;
        if (yData < yDataLast)
        {
            yDataBottom = yData;
            yDataTop = yDataLast;
        }
        else
        {
            yDataBottom = yDataLast;
            yDataTop = yData;
        }
        yDataBottom--;
        yDataLast = yData;

        // Render
        mr_color_t imageBuffer[HISTORY_DATA_HEIGHT];

        int16_t y = 0;
        while (y < HISTORY_DATA_HEIGHT)
        {
            mr_color_t color = COLOR_INSTRUMENT_FRAME_PRIMARY;
            int16_t yNext = 0;

            // Render time and value ticks
            int16_t yTickNext;
            if (timeTickDraw)
            {
                yTickNext = HISTORY_DATA_HEIGHT;

                color = COLOR_INSTRUMENT_FRAME_TERTIARY;
            }
            else
            {
                yTickNext = valueTickTable[valueTickTableIndex];

                if (valueTickTableIndex & 0b1)
                    color = COLOR_INSTRUMENT_FRAME_TERTIARY;

                if (y == yTickNext)
                    valueTickTableIndex++;
            }
            yNext = yTickNext;

            // Render data
            int16_t yDataNext;
            if (y < yDataBottom)
            {
                yDataNext = yDataBottom;

                if (color == COLOR_INSTRUMENT_FRAME_PRIMARY)
                    color = COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_PRIMARY;
                else
                    color = COLOR_INSTRUMENT_ENHANCED_TERTIARY_OVER_TERTIARY;
            }
            else if (y < yDataTop)
            {
                yDataNext = yDataTop;

                color = COLOR_INSTRUMENT_ENHANCED_PRIMARY;
            }
            else
                yDataNext = HISTORY_DATA_HEIGHT;
            if (yDataNext < yNext)
                yNext = yDataNext;

#if !defined(DISPLAY_MONOCHROME)
            // Render alert zones
            int16_t yAlertZoneNext;
            if (y < yAlertZone1)
                yAlertZoneNext = yAlertZone1;
            else if (y < yAlertZone2)
            {
                yAlertZoneNext = yAlertZone2;

                color += COLOR_INSTRUMENT_OFFSET;
            }
            else
            {
                yAlertZoneNext = HISTORY_DATA_HEIGHT;

                color += 2 * COLOR_INSTRUMENT_OFFSET;
            }
            if (yAlertZoneNext < yNext)
                yNext = yAlertZoneNext;
#endif
            // Render to imagebuffer
            mr_color_t fillColor = getFillColor(color);
            for (int32_t i = y; i < yNext; i++)
                imageBuffer[(HISTORY_DATA_HEIGHT - 1) - i] = fillColor;

            y = yNext;
        }

        // Draw
        mr_rectangle_t historyImageRectangle = {
            HISTORY_DATA_X + x,
            HISTORY_DATA_Y,
            1,
            HISTORY_DATA_HEIGHT};

        drawImage(&historyImageRectangle,
                  imageBuffer);
    }
}

void drawRNG(const char *title,
             const char *rngString,
             const char *stateString)
{
    drawTitleBar(title, false);

    // RNG string
    const mr_rectangle_t rngDataRectangle = {
        RNG_DATA_X,
        RNG_DATA_Y,
        RNG_DATA_WIDTH,
        RNG_DATA_HEIGHT};

    const mr_point_t rngDataOffset = {
        RNG_DATA_OFFSET_X,
        RNG_DATA_OFFSET_Y};

    setFont(font_small);
    setStrokeColor(COLOR_INSTRUMENT_ENHANCED_PRIMARY);
    drawText(rngString,
             &rngDataRectangle,
             &rngDataOffset);

    // State
    const mr_rectangle_t rngStateRectangle = {
        RNG_STATE_X,
        RNG_STATE_Y,
        RNG_STATE_WIDTH,
        RNG_STATE_HEIGHT};

    const mr_point_t rngStateOffset = {
        RNG_STATE_OFFSET_X,
        RNG_STATE_OFFSET_Y};

    setFont(font_medium);
    setStrokeColor(COLOR_ELEMENT_ACTIVE);
    drawCenteredText(stateString,
                     &rngStateRectangle,
                     &rngStateOffset);
}

enum
{
    STAT_TITLE,
    STAT_TUBE_TIME,
    STAT_TUBE_PULSES,
    STAT_DEAD_TIME,
    STAT_DEVICEID_1,
    STAT_DEVICEID_2,
    STAT_VOLTAGE,
};

struct StatisticsData
{
    const char *keyString;
    uint8_t type;
};

static struct StatisticsData statisticsData[] = {
#if defined(DISPLAY_LANDSCAPE)
    {STRING_TUBE_LIFE_TIME, STAT_TUBE_TIME},
    {STRING_PULSES, STAT_TUBE_PULSES},
    {STRING_DEAD_TIME, STAT_DEAD_TIME},
    {STRING_DEVICE_ID, STAT_DEVICEID_1},
    {"", STAT_DEVICEID_2},
    {STRING_VOLTAGE, STAT_VOLTAGE},
#elif defined(DISPLAY_PORTRAIT)
    {STRING_TUBE, STAT_TITLE},
    {STRING_LIFE_TIME, STAT_TUBE_TIME},
    {STRING_PULSES, STAT_TUBE_PULSES},
    {STRING_DEAD_TIME_SHORT, STAT_DEAD_TIME},
    {"", STAT_TITLE},
    {STRING_DEVICE, STAT_TITLE},
    {STRING_ID, STAT_DEVICEID_1},
    {"", STAT_DEVICEID_2},
    {STRING_VOLTAGE_SHORT, STAT_VOLTAGE},
#endif
};

#define STATISTICS_ENTRY_NUM (sizeof(statisticsData) / sizeof(struct StatisticsData))

void drawStatistics(void)
{
    drawTitleBar(getString(STRING_STATISTICS), false);

    setFont(font_small);

    char keyString[32];
    char valueString[32];
    char unitString[16];

    mr_rectangle_t rectangle;
    mr_point_t offset;

    uint32_t y = CONTENT_Y;

    for (uint32_t i = 0; i < STATISTICS_ENTRY_NUM; i++)
    {
        struct StatisticsData *entry = &statisticsData[i];

        strcpy(keyString, entry->keyString);
        strclr(valueString);
        strclr(unitString);

        switch (entry->type)
        {
        case STAT_TITLE:
            break;

        case STAT_TUBE_TIME:
            strcatTime(valueString, getTubeTime());

            break;

        case STAT_TUBE_PULSES:
            strcatUInt32(valueString, getTubePulseCount(), 0);

            break;

        case STAT_DEAD_TIME:
        {
            float deadTime = getTubeDeadTime();
            if (deadTime >= 1)
                strcpy(valueString, getString(STRING_NONE));
            else
            {
                strcpy(valueString, getString(STRING_LESSTHAN));
                strcatFloat(valueString, 1000000 * deadTime, 1);
                strcatChar(unitString, ' ');
                strcat(unitString, getString(STRING_MICROSECONDS));
            }

            break;
        }

        case STAT_DEVICEID_1:
            getDeviceId(valueString);
            valueString[12] = '\0';

            break;

        case STAT_DEVICEID_2:
            getDeviceId(valueString);
            strcpy(valueString, valueString + 12);

            break;

        case STAT_VOLTAGE:
            strcatFloat(valueString, getBatteryVoltage(), 3);
            strclr(unitString);
            strcatChar(unitString, ' ');
            strcat(unitString, getString(STRING_VOLTS));

            break;
        }

        // Key
        rectangle = (mr_rectangle_t){
            STATISTICS_KEY_X,
            y,
            STATISTICS_KEY_WIDTH,
            STATISTICS_ENTRY_HEIGHT};

        offset = (mr_point_t){
            STATISTICS_KEY_OFFSET_X,
            0};

        if (i == 0)
        {
            rectangle.height += STATISTICS_TOPMARGIN_HEIGHT;
            offset.y += STATISTICS_TOPMARGIN_HEIGHT;
            y += STATISTICS_TOPMARGIN_HEIGHT;
        }
        else if (i == (STATISTICS_ENTRY_NUM - 1))
            rectangle.height += STATISTICS_BOTTOMMARGIN_HEIGHT;

        setStrokeColor(
            (!valueString[0] && !unitString[0])
                ? COLOR_ELEMENT_ACTIVE
                : COLOR_ELEMENT_NEUTRAL);
        drawText(keyString,
                 &rectangle,
                 &offset);

        // Value
        rectangle.x = STATISTICS_VALUE_X;
        rectangle.width = STATISTICS_VALUE_WIDTH;
        offset.x = STATISTICS_VALUE_OFFSET_X;

        drawValueAndUnit(valueString,
                         unitString,
                         &rectangle,
                         &offset,
                         false);

        y += FONT_SMALL_LINE_HEIGHT;
    }
}

void drawGame(const uint8_t board[8][8],
              const char time[2][16],
              const char history[3][2][6])
{
    mr_rectangle_t rectangle;
    mr_point_t offset;

#if defined(DISPLAY_COLOR)
    drawTitleBar(getString(STRING_GAME), false);
#endif

    // Board
    static const char *const gamePieceMap = "@AACFBDE";

    setFont(font_symbols);

    rectangle.width = GAME_SQUARE_WIDTH;
    rectangle.height = GAME_SQUARE_HEIGHT;

    const mr_point_t gamePieceOffset = {
        0,
        0};

    for (uint32_t y = 0; y < 8; y++)
    {
        for (uint32_t x = 0; x < 8; x++)
        {
            uint8_t pieceCode = board[y][x];

            uint8_t pieceType = (pieceCode >> 0) & 0x7;
            bool pieceBlack = (pieceCode >> 3) & 0x1;
            bool pieceSelected = (pieceCode >> 4) & 0x1;
            bool squareBlack = (x + y) & 0x1;

            char pieceChar = gamePieceMap[pieceType];

#if defined(DISPLAY_MONOCHROME)
            bool isInverted;
            if (pieceSelected)
            {
                pieceChar |= 0x10;
                isInverted = !pieceBlack;
            }
            else
            {
                pieceChar |= (pieceBlack ^ squareBlack ^ 1) << 3;
                isInverted = squareBlack;
            }
            setFillColor(isInverted
                             ? COLOR_GAME_SQUARE_BLACK
                             : COLOR_GAME_SQUARE_WHITE);
#elif defined(DISPLAY_COLOR)
            setStrokeColor(pieceBlack
                               ? COLOR_GAME_PIECE_BLACK
                               : COLOR_GAME_PIECE_WHITE);
            setFillColor(pieceSelected
                             ? COLOR_SELECTED_ENABLED_BACKGROUND
                         : squareBlack
                             ? COLOR_GAME_SQUARE_BLACK
                             : COLOR_GAME_SQUARE_WHITE);
#endif

            char pieceString[2];

            strclr(pieceString);
            strcatChar(pieceString, pieceChar);

            rectangle.x = GAME_BOARD_X + GAME_SQUARE_WIDTH * x;
            rectangle.y = GAME_BOARD_Y + GAME_SQUARE_HEIGHT * y;

            drawText(pieceString,
                     &rectangle,
                     &gamePieceOffset);
        }
    }

    // Top timer
    setFont(font_small);
    setStrokeColor(COLOR_ELEMENT_NEUTRAL);
    setFillColor(COLOR_CONTAINER_BACKGROUND);

    const mr_rectangle_t topTimerRectangle = {
        GAME_TOP_TIMER_X,
        GAME_TOP_TIMER_Y,
        GAME_TOP_TIMER_WIDTH,
        GAME_TOP_TIMER_HEIGHT};

    const mr_point_t topTimerOffset = {
        GAME_TOP_TIMER_OFFSET_X,
        GAME_TOP_TIMER_OFFSET_Y};

#if defined(DISPLAY_LANDSCAPE)
    drawText(time[0],
             &topTimerRectangle,
             &topTimerOffset);
#elif defined(DISPLAY_PORTRAIT)
    drawRightAlignedText(time[0],
                         &topTimerRectangle,
                         &topTimerOffset);
#endif

    // History
    rectangle.y = GAME_HISTORY_Y;
    rectangle.height = GAME_HISTORY_HEIGHT / 3;

    offset = (mr_point_t){
        0,
        0};

    for (uint32_t y = 0; y < 3; y++)
    {
        rectangle.x = GAME_HISTORY_X;
        rectangle.width = GAME_HISTORY_FIRST_WIDTH;
        offset.x = GAME_HISTORY_FIRST_OFFSET_X;

        drawText(history[y][0],
                 &rectangle,
                 &offset);

        rectangle.x += GAME_HISTORY_FIRST_WIDTH;
        rectangle.width = GAME_HISTORY_SECOND_WIDTH;
        offset.x = 0;

        drawText(history[y][1],
                 &rectangle,
                 &offset);

        rectangle.y += GAME_HISTORY_HEIGHT / 3;
    }

    // Bottom timer
    const mr_rectangle_t bottomTimerRectangle = {
        GAME_BOTTOM_TIMER_X,
        GAME_BOTTOM_TIMER_Y,
        GAME_BOTTOM_TIMER_WIDTH,
        GAME_BOTTOM_TIMER_HEIGHT};

    const mr_point_t bottomTimerOffset = {
        GAME_BOTTOM_TIMER_OFFSET_X,
        GAME_BOTTOM_TIMER_OFFSET_Y};

#if defined(DISPLAY_LANDSCAPE)
    drawText(time[1],
             &bottomTimerRectangle,
             &bottomTimerOffset);
#elif defined(DISPLAY_PORTRAIT)
    drawRightAlignedText(time[1],
                         &bottomTimerRectangle,
                         &bottomTimerOffset);
#endif

    // Spaces
#if defined(GAME_LEFTMARGIN_WIDTH)
    const mr_rectangle_t gameLeftMarginRectangle = {
        GAME_LEFTMARGIN_X,
        GAME_LEFTMARGIN_Y,
        GAME_LEFTMARGIN_WIDTH,
        GAME_LEFTMARGIN_HEIGHT};

    drawRectangle(&gameLeftMarginRectangle);
#endif

#if defined(GAME_RIGHTMARGIN_WIDTH)
    const mr_rectangle_t gameRightMarginRectangle = {
        GAME_RIGHTMARGIN_X,
        GAME_RIGHTMARGIN_Y,
        GAME_RIGHTMARGIN_WIDTH,
        GAME_RIGHTMARGIN_HEIGHT};

    drawRectangle(&gameRightMarginRectangle);
#endif
}

// Display contrast level menu

#if defined(DISPLAY_MONOCHROME)

static const char *onDisplayContrastMenuGetOption(const Menu *menu,
                                                  uint32_t index,
                                                  MenuStyle *menuStyle)
{
    if (index >= DISPLAY_CONTRAST_NUM)
        return NULL;

    *menuStyle = (index == settings.displayContrast);

    strcpy(menuOption, getString(STRING_CONTRAST_LEVEL));
    strcatChar(menuOption, ' ');
    strcatUInt32(menuOption, index + 1, 0);

    return menuOption;
}

static void onDisplayContrastMenuSelect(const Menu *menu)
{
    settings.displayContrast = menu->state->selectedIndex;

    updateDisplayContrast();
}

static MenuState displayContrastMenuState;

static const Menu displayContrastMenu = {
    getString(STRING_CONTRAST),
    &displayContrastMenuState,
    onDisplayContrastMenuGetOption,
    onDisplayContrastMenuSelect,
    onDisplaySubMenuBack,
};

const View displayContrastMenuView = {
    onMenuEvent,
    &displayContrastMenu,
};

#endif

// Display theme menu

#if defined(DISPLAY_COLOR)

static const char *const displayThemeMenuOptions[] = {
    getString(STRING_THEME_DAY),
    getString(STRING_THEME_DUSK),
    getString(STRING_THEME_NIGHT),
#if defined(DISPLAY_EXTRA_COLOR_SCHEMES) 
    getString(STRING_THEME_FNIRSI_BARBIE),
#endif    
    NULL,
};

static const char *onDisplayThemeMenuGetOption(const Menu *menu,
                                               uint32_t index,
                                               MenuStyle *menuStyle)
{
    *menuStyle = (index == settings.displayTheme);

    return displayThemeMenuOptions[index];
}

static void onDisplayThemeMenuSelect(const Menu *menu)
{
    settings.displayTheme = menu->state->selectedIndex;

    requestViewUpdate();
}

static MenuState displayThemeMenuState;

static const Menu displayThemeMenu = {
    getString(STRING_THEME),
    &displayThemeMenuState,
    onDisplayThemeMenuGetOption,
    onDisplayThemeMenuSelect,
    onDisplaySubMenuBack,
};

const View displayThemeMenuView = {
    onMenuEvent,
    &displayThemeMenu,
};

#endif

// Display brightness level menu

static const char *const displayBrightnessMenuOptions[] = {
    getString(STRING_VERY_LOW),
    getString(STRING_LOW),
    getString(STRING_MEDIUM),
    getString(STRING_HIGH),
    NULL,
};

static const char *onDisplayBrightnessMenuGetOption(const Menu *menu,
                                                    uint32_t index,
                                                    MenuStyle *menuStyle)
{
    *menuStyle = (index == settings.displayBrightness);

    return displayBrightnessMenuOptions[index];
}

static void onDisplayBrightnessMenuSelect(const Menu *menu)
{
    settings.displayBrightness = menu->state->selectedIndex;
}

static MenuState displayBrightnessMenuState;

static const Menu displayBrightnessMenu = {
    getString(STRING_BRIGHTNESS),
    &displayBrightnessMenuState,
    onDisplayBrightnessMenuGetOption,
    onDisplayBrightnessMenuSelect,
    onDisplaySubMenuBack,
};

const View displayBrightnessMenuView = {
    onMenuEvent,
    &displayBrightnessMenu,
};

// Display sleep menu

static const char *const displaySleepMenuOptions[] = {
#if defined(DISPLAY_MONOCHROME)
    getString(STRING_ALWAYS_OFF),
#endif
    getString(STRING_10_SECONDS),
    getString(STRING_30_SECONDS),
    getString(STRING_1_MINUTE),
    getString(STRING_2_MINUTES),
    getString(STRING_5_MINUTES),
    getString(STRING_ALWAYS_ON),
    NULL,
};

static const char *onDisplaySleepMenuGetOption(const Menu *menu,
                                               uint32_t index,
                                               MenuStyle *menuStyle)
{
    *menuStyle = (index == settings.displaySleep);

    return displaySleepMenuOptions[index];
}

static void onDisplaySleepMenuSelect(const Menu *menu)
{
    settings.displaySleep = menu->state->selectedIndex;
}

static MenuState displaySleepMenuState;

static const Menu displaySleepMenu = {
    getString(STRING_SLEEP),
    &displaySleepMenuState,
    onDisplaySleepMenuGetOption,
    onDisplaySleepMenuSelect,
    onDisplaySubMenuBack,
};

const View displaySleepMenuView = {
    onMenuEvent,
    &displaySleepMenu,
};

// Display menu

static const OptionView displayMenuOptions[] = {
#if defined(DISPLAY_COLOR)
    {getString(STRING_THEME), &displayThemeMenuView},
#endif
    {getString(STRING_BRIGHTNESS), &displayBrightnessMenuView},
#if defined(DISPLAY_MONOCHROME)
    {getString(STRING_CONTRAST), &displayContrastMenuView},
#endif
    {getString(STRING_SLEEP), &displaySleepMenuView},
    {NULL},
};

static const char *onDisplayMenuGetOption(const Menu *menu,
                                          uint32_t index,
                                          MenuStyle *menuStyle)
{
    *menuStyle = MENUSTYLE_SUBMENU;

    return displayMenuOptions[index].option;
}

static void onDisplayMenuSelect(const Menu *menu)
{
    setView(displayMenuOptions[menu->state->selectedIndex].view);
}

void onDisplaySubMenuBack(const Menu *menu)
{
    setView(&displayMenuView);
}

static MenuState displayMenuState;

static const Menu displayMenu = {
    getString(STRING_DISPLAY),
    &displayMenuState,
    onDisplayMenuGetOption,
    onDisplayMenuSelect,
    onSettingsSubMenuBack,
};

const View displayMenuView = {
    onMenuEvent,
    &displayMenu,
};
