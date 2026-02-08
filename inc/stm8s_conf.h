/**
 ******************************************************************************
 * @file    stm8s_conf.h
 * @author  Lazaro
 * @version V1.0
 * @date    08-Fevereiro-2026
 * @brief   Library configuration file - define which peripherals to include
 ******************************************************************************
 */

#ifndef __STM8S_CONF_H
#define __STM8S_CONF_H

/* Uncomment the line below to enable peripheral header file inclusion */
#define _STM8S_H

/* Peripheral header files inclusion */
#include "stm8s_adc1.h"
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#include "stm8s_tim1.h"
#include "stm8s_tim2.h"
#include "stm8s_uart2.h"
#include "stm8s_flash.h"

/* Exported types and constants */

/* Check the used compiler */
#if defined(__CSMC__)
 #define _COSMIC_
#endif

#if defined(__RCST7__)
 #define _RAISONANCE_
#endif

#if defined(__ICCSTM8__)
 #define _IAR_
#endif

#if defined(__SDCC)
 #define _SDCC_
#endif

/* SDCC specific definitions */
#ifdef _SDCC_
 #define PUTCHAR_PROTOTYPE int putchar(int c)
 #define GETCHAR_PROTOTYPE int getchar(void)
 #define enableInterrupts()  __asm__("rim")
 #define disableInterrupts() __asm__("sim")
 #define nop()               __asm__("nop")
 #define trap()              __asm__("trap")
 #define wfi()               __asm__("wfi")
 #define halt()              __asm__("halt")
#endif

/* For COSMIC compiler */
#ifdef _COSMIC_
 #define enableInterrupts() _asm("rim")
 #define disableInterrupts() _asm("sim")
 #define nop() _asm("nop")
 #define trap() _asm("trap")
 #define wfi() _asm("wfi")
 #define halt() _asm("halt")
#endif

/* For Raisonance compiler */
#ifdef _RAISONANCE_
 #include <intrins.h>
 #define enableInterrupts() _rim_()
 #define disableInterrupts() _sim_()
 #define nop() _nop_()
 #define trap() _trap_()
 #define wfi() _wfi_()
 #define halt() _halt_()
#endif

/* For IAR compiler */
#ifdef _IAR_
 #include <intrinsics.h>
 #define enableInterrupts() __enable_interrupt()
 #define disableInterrupts() __disable_interrupt()
 #define nop() __no_operation()
 #define trap() __trap()
 #define wfi() __wait_for_interrupt()
 #define halt() __halt()
#endif

/* Assert */
#ifdef USE_FULL_ASSERT
 #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
 void assert_failed(uint8_t* file, uint32_t line);
#else
 #define assert_param(expr) ((void)0)
#endif

#endif /* __STM8S_CONF_H */

/************************ (C) COPYRIGHT Lazaro *****END OF FILE****/
