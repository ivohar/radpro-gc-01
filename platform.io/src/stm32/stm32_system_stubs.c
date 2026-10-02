/*
 * Rad Pro
 * STM32 System Stubs for CMake build
 *
 * (C) 2022-2026 Gissio
 *
 * License: MIT
 */

#if defined(STM32)

/**
 * @brief  System initialization function stub
 * @note   This is called by Reset_Handler before main().
 *         The actual system initialization is done in initSystem()
 */
__attribute__((used, noinline))
void SystemInit(void)
{
    // Empty stub - actual initialization is done in initSystem()
    // Add volatile to prevent optimization
    __asm__ volatile ("" ::: "memory");
}

/**
 * @brief  C library initialization stub
 * @note   Required by __libc_init_array
 */
__attribute__((used, noinline))
void _init(void)
{
    // Empty stub
    __asm__ volatile ("" ::: "memory");
}

#endif
