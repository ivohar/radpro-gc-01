/*
 * Rad Pro
 * System
 *
 * (C) 2022-2024 Gissio
 *
 * License: MIT
 */

#if !defined(SYSTEM_H)
#define SYSTEM_H

#include <stdint.h>

#include "view.h"

#define FIRMWARE_AUTHOR "Gissio"
#define FIRMWARE_NAME "Rad Pro GC-01"
#define FIRMWARE_VERSION "2.0.4beta2-iha-c1"

extern const View statisticsView;

void initSystem(void);

uint32_t getDeviceId(void);

void startBootloader(void);

#endif
