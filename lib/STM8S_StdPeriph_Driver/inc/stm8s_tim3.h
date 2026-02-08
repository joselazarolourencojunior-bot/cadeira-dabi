/**
  ******************************************************************************
  * @file    stm8s_tim3.h
  * @brief   TIM3 peripheral driver header file for STM8S (placeholder)
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM8S_TIM3_H
#define __STM8S_TIM3_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Exported types ------------------------------------------------------------*/

/** TIM3 Prescaler */
typedef enum
{
  TIM3_PRESCALER_1 = ((uint8_t)0x00),
  TIM3_PRESCALER_2 = ((uint8_t)0x01),
  TIM3_PRESCALER_4 = ((uint8_t)0x02),
  TIM3_PRESCALER_8 = ((uint8_t)0x03)
} TIM3_Prescaler_TypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void TIM3_DeInit(void);
void TIM3_TimeBaseInit(TIM3_Prescaler_TypeDef TIM3_Prescaler, uint16_t TIM3_Period);
void TIM3_Cmd(FunctionalState NewState);

#endif /* __STM8S_TIM3_H */
