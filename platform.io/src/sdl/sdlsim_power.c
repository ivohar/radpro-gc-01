/*
 * Rad Pro
 * Simulator power
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#if defined(SIMULATOR)

#include <stdio.h>

#include "../power.h"

void initPowerController(void)
{
}

void setPower(bool value)
{
}

bool isDevicePowered(void)
{
    return false;
}

bool isBatteryCharging(void)
{
    return false;
}

#endif
