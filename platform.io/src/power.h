/*
 * Rad Pro
 * Power management
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#if !defined(POWER_H)
#define POWER_H

#include <stdbool.h>
#include <stdint.h>

#include "view.h"

extern View powerOnView;
extern View powerOffView;

#if defined(BATTERY_REMOVABLE)
extern View batteryTypeMenuView;
#endif

void initPower(void);

void setPowerEnabled(bool value);
bool isPowerOnReset(void);
bool isBatteryCharging(void);
#if defined(GC01)
bool isUSBPowered(float voltage);
#else
bool isUSBPowered(void);
#endif
uint32_t getBatteryNum(void);

void powerOn(bool isBoot);
void powerOff(bool showBatteryIndicator);
bool isInPowerOffView(void);

void updateBattery(void);
uint8_t getBatteryLevel(void);
float getBatteryVoltage(void);

#endif
