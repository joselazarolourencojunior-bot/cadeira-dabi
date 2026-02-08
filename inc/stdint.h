/**
 ******************************************************************************
 * @file    stdint.h
 * @brief   Standard integer types for STM8 (8-bit microcontroller)
 * @note    Local implementation for SDCC compatibility
 ******************************************************************************
 */

#ifndef _STDINT_H
#define _STDINT_H

/* Exact-width integer types */
typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef signed short       int16_t;
typedef unsigned short     uint16_t;
typedef signed long        int32_t;
typedef unsigned long      uint32_t;

/* Minimum-width integer types */
typedef signed char        int_least8_t;
typedef unsigned char      uint_least8_t;
typedef signed short       int_least16_t;
typedef unsigned short     uint_least16_t;
typedef signed long        int_least32_t;
typedef unsigned long      uint_least32_t;

/* Fastest minimum-width integer types */
typedef signed char        int_fast8_t;
typedef unsigned char      uint_fast8_t;
typedef signed short       int_fast16_t;
typedef unsigned short     uint_fast16_t;
typedef signed long        int_fast32_t;
typedef unsigned long      uint_fast32_t;

/* Integer types capable of holding object pointers */
typedef signed short       intptr_t;
typedef unsigned short     uintptr_t;

/* Greatest-width integer types */
typedef signed long        intmax_t;
typedef unsigned long      uintmax_t;

/* Limits of exact-width integer types */
#define INT8_MIN           (-128)
#define INT8_MAX           (127)
#define UINT8_MAX          (255)
#define INT16_MIN          (-32768)
#define INT16_MAX          (32767)
#define UINT16_MAX         (65535u)
#define INT32_MIN          (-2147483648L)
#define INT32_MAX          (2147483647L)
#define UINT32_MAX         (4294967295UL)

/* Limits of minimum-width integer types */
#define INT_LEAST8_MIN     INT8_MIN
#define INT_LEAST8_MAX     INT8_MAX
#define UINT_LEAST8_MAX    UINT8_MAX
#define INT_LEAST16_MIN    INT16_MIN
#define INT_LEAST16_MAX    INT16_MAX
#define UINT_LEAST16_MAX   UINT16_MAX
#define INT_LEAST32_MIN    INT32_MIN
#define INT_LEAST32_MAX    INT32_MAX
#define UINT_LEAST32_MAX   UINT32_MAX

/* Limits of fastest minimum-width integer types */
#define INT_FAST8_MIN      INT8_MIN
#define INT_FAST8_MAX      INT8_MAX
#define UINT_FAST8_MAX     UINT8_MAX
#define INT_FAST16_MIN     INT16_MIN
#define INT_FAST16_MAX     INT16_MAX
#define UINT_FAST16_MAX    UINT16_MAX
#define INT_FAST32_MIN     INT32_MIN
#define INT_FAST32_MAX     INT32_MAX
#define UINT_FAST32_MAX    UINT32_MAX

/* Limits of integer types capable of holding object pointers */
#define INTPTR_MIN         INT16_MIN
#define INTPTR_MAX         INT16_MAX
#define UINTPTR_MAX        UINT16_MAX

/* Limits of greatest-width integer types */
#define INTMAX_MIN         INT32_MIN
#define INTMAX_MAX         INT32_MAX
#define UINTMAX_MAX        UINT32_MAX

/* Limits of other integer types */
#define SIZE_MAX           UINT16_MAX

/* Macros for integer constants */
#define INT8_C(c)          c
#define UINT8_C(c)         c##U
#define INT16_C(c)         c
#define UINT16_C(c)        c##U
#define INT32_C(c)         c##L
#define UINT32_C(c)        c##UL
#define INTMAX_C(c)        c##L
#define UINTMAX_C(c)       c##UL

#endif /* _STDINT_H */
