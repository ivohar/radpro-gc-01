/*
 * Rad Pro
 * System
 *
 * (C) 2022-2025 Gissio
 *
 * License: MIT
 */

#if !defined(SYSTEM_H)
#define SYSTEM_H

#include <stdbool.h>
#include <stdint.h>

#include "view.h"

#define FIRMWARE_AUTHOR "Gissio"
#define FIRMWARE_NAME "Rad Pro GC-01"
#define FIRMWARE_VERSION "2.1-iha-c1"

extern const View lockView;
extern const View statisticsView;

void initSystem(void);

uint32_t getDeviceId(void);

void setLockMode(bool value);
bool isLockMode(void);

#endif
