/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

void Error_Handler(void);

#define watch_dog_Pin GPIO_PIN_13
#define watch_dog_GPIO_Port GPIOC
#define sobe_assento_Pin GPIO_PIN_0
#define sobe_assento_GPIO_Port GPIOA
#define desce_encosto_Pin GPIO_PIN_1
#define desce_encosto_GPIO_Port GPIOA
#define desce_encostoA2_Pin GPIO_PIN_2
#define desce_encostoA2_GPIO_Port GPIOA
#define desce_assento_Pin GPIO_PIN_3
#define desce_assento_GPIO_Port GPIOA
#define bt_refletor_Pin GPIO_PIN_4
#define bt_refletor_GPIO_Port GPIOA
#define vz_Pin GPIO_PIN_5
#define vz_GPIO_Port GPIOA
#define pt_Pin GPIO_PIN_6
#define pt_GPIO_Port GPIOA
#define rele_sobe_assento_Pin GPIO_PIN_0
#define rele_sobe_assento_GPIO_Port GPIOB
#define rele_desce_encosto_Pin GPIO_PIN_1
#define rele_desce_encosto_GPIO_Port GPIOB
#define rele_desce_assento_Pin GPIO_PIN_11
#define rele_desce_assento_GPIO_Port GPIOB

/* CUBA: IN = PB12 / OUT = PA12 */
#define cuba_in2_Pin GPIO_PIN_12
#define cuba_in2_GPIO_Port GPIOB
#define out_cuba_Pin GPIO_PIN_12
#define out_cuba_GPIO_Port GPIOA

#define rele_sobe_encosto_Pin GPIO_PIN_3
#define rele_sobe_encosto_GPIO_Port GPIOB
#define rele_refletor_Pin GPIO_PIN_4
#define rele_refletor_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
