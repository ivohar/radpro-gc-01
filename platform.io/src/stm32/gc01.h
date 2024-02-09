/*
 * Rad Pro
 * FNIRSI GC-01 pin definitions
 *
 * (C) 2022-2024 Gissio
 *
 * License: MIT
 */

#define SYSCLK_FREQUENCY 72000000
#define AHB_FREQUENCY SYSCLK_FREQUENCY
#define APB1_FREQUENCY (SYSCLK_FREQUENCY / 2)
#define TIM_FREQUENCY SYSCLK_FREQUENCY
#define DEADTIME_TIM_FREQUENCY 8000000

#define PAYLOAD_BASE 0x08004000
#define PAYLOAD_SIZE (0xa000 - 0x4)

#define PWR_EN_PORT GPIOA
#define PWR_EN_PIN GPIO2
#define PWR_BAT_PORT GPIOA
#define PWR_BAT_PIN GPIO0
#define PWR_BAT_CHANNEL 0
#define PWR_BAT_SCALE_FACTOR 2.0F
#define PWR_CHRG_PORT GPIOA
#define PWR_CHRG_PIN GPIO4
#define PWR_CHRG_PULLUP

#define TUBE_HV_PORT GPIOA
#define TUBE_HV_PIN GPIO3
#define TUBE_HV_TIMER_RCC RCC_TIM2
#define TUBE_HV_TIMER TIM2
#define TUBE_HV_TIMER_CCMR TIM_CCMR2
#define TUBE_HV_TIMER_CCMR_MODE TIM_CCMR2_OC4M_PWM2
#define TUBE_HV_TIMER_CCER_CC TIM_CCER_CC4E
#define TUBE_HV_TIMER_CCR TIM_CCR4
#define TUBE_DET_PORT GPIOC
#define TUBE_DET_PIN GPIO10
#define TUBE_DET_PULLUP
#define TUBE_DET_EXTI EXTI10
#define TUBE_DET_EXTI_IRQ NVIC_EXTI15_10_IRQ
#define TUBE_DET_EXTICR_REG AFIO_EXTICR3
#define TUBE_DET_EXTICR_VALUE (2 << (2 * AFIO_EXTICR_FIELDSIZE))
#define TUBE_DET_IRQ_HANDLER exti15_10_isr
#define TUBE_DET_TIMER_LOW_RCC RCC_TIM3
#define TUBE_DET_TIMER_LOW TIM3
#define TUBE_DET_TIMER_HIGH_RCC RCC_TIM4
#define TUBE_DET_TIMER_HIGH TIM4

#define KEY_POWER_PORT GPIOA
#define KEY_POWER_PIN GPIO1
#define KEY_ENTER_PORT GPIOC
#define KEY_ENTER_PIN GPIO1
#define KEY_BACK_PORT GPIOC
#define KEY_BACK_PIN GPIO3
#define KEY_UP_PORT GPIOC
#define KEY_UP_PIN GPIO0
#define KEY_DOWN_PORT GPIOC
#define KEY_DOWN_PIN GPIO2

#define PULSE_LED_PORT GPIOA
#define PULSE_LED_PIN GPIO15

#define LCD_RESX_PORT GPIOC
#define LCD_RESX_PIN GPIO5
#define LCD_RDX_PORT GPIOC
#define LCD_RDX_PIN GPIO6
#define LCD_WRX_PORT GPIOC
#define LCD_WRX_PIN GPIO7
#define LCD_DCX_PORT GPIOC
#define LCD_DCX_PIN GPIO8
#define LCD_CSX_PORT GPIOC
#define LCD_CSX_PIN GPIO9
#define LCD_DATA_PORT GPIOB
#define LCD_DATA_PINS 0xffff
#define LCD_BACKLIGHT_PORT GPIOA
#define LCD_BACKLIGHT_PIN GPIO10
#define LCD_BACKLIGHT_TIMER_RCC RCC_TIM1
#define LCD_BACKLIGHT_TIMER TIM1
#define LCD_BACKLIGHT_TIMER_CCMR TIM_CCMR2
#define LCD_BACKLIGHT_TIMER_CCMR_MODE TIM_CCMR2_OC3M_PWM1
#define LCD_BACKLIGHT_TIMER_CCER_CC TIM_CCER_CC3E
#define LCD_BACKLIGHT_TIMER_CCR TIM_CCR3
#define LCD_BACKLIGHT_PWM_PERIOD BUZZ_PWM_PERIOD

#define BUZZ_PORT GPIOA
#define BUZZ_PIN GPIO8
#define BUZZ_TIMER_RCC RCC_TIM1
#define BUZZ_TIMER TIM1
#define BUZZ_TIMER_CCMR TIM_CCMR1
#define BUZZ_TIMER_CCMR_MODE TIM_CCMR1_OC1M_PWM1
#define BUZZ_TIMER_CCER_CC TIM_CCER_CC1E
#define BUZZ_TIMER_CCR TIM_CCR1
#define BUZZ_PWM_FREQUENCY 2000
#define BUZZ_PWM_PERIOD (TIM_FREQUENCY / BUZZ_PWM_FREQUENCY)

#define VIBRATOR_PORT GPIOA
#define VIBRATOR_PIN GPIO9

#define USB_INTERFACE RCC_USB
#define USB_VID 0x0483
#define USB_PID 0x5740
#define USB_DEVICE_RELEASE 0x0200
#define USB_DM_PORT GPIOA
#define USB_DM_PIN GPIO11
#define USB_DP_PORT GPIOA
#define USB_DP_PIN GPIO12