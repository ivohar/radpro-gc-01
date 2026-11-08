/*
 * Rad Pro
 * STM32 System Stubs for CMake build
 *
 * (C) 2022-2026 Gissio
 *
 * License: MIT
 */

#if defined(STM32)

// SystemInit() is provided by the CMSIS system_stm32xxxx.c file, which the
// CMake build compiles together with the CMSIS startup file (vector table).

/**
 * @brief  C library initialization stub
 * @note   Called by __libc_init_array (from the CMSIS startup file). Normally
 *         provided by crti.o, which is not linked because of -nostdlib.
 */
__attribute__((used, noinline))
void _init(void)
{
    // Empty stub
    __asm__ volatile ("" ::: "memory");
}

#endif
