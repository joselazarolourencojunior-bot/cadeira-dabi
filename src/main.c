/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  SYSTEM_IDLE = 0,
  SYSTEM_VOLTA_ZERO,
  SYSTEM_POSICAO_TRABALHO
} SystemState_t;

typedef struct
{
  GPIO_TypeDef *port;
  uint16_t pin;
  uint8_t stable;
  uint8_t lastRaw;
  uint32_t lastChangeMs;
} DebouncedButton_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BTN_SOBE_ASSENTO_PORT GPIOA
#define BTN_SOBE_ASSENTO_PIN  GPIO_PIN_0
#define BTN_DESCE_ENCOSTO_PORT GPIOA
#define BTN_DESCE_ENCOSTO_PIN  GPIO_PIN_1
#define BTN_SOBE_ENCOSTO_PORT GPIOA
#define BTN_SOBE_ENCOSTO_PIN  GPIO_PIN_2
#define BTN_DESCE_ASSENTO_PORT GPIOA
#define BTN_DESCE_ASSENTO_PIN  GPIO_PIN_3
#define BTN_REFLETOR_PORT GPIOA
#define BTN_REFLETOR_PIN  GPIO_PIN_4
#define BTN_VZ_PORT GPIOA
#define BTN_VZ_PIN  GPIO_PIN_5
#define BTN_PT_PORT GPIOA
#define BTN_PT_PIN  GPIO_PIN_6

#define RELE_SOBE_ASSENTO_PORT GPIOB
#define RELE_SOBE_ASSENTO_PIN  GPIO_PIN_0
#define RELE_DESCE_ENCOSTO_PORT GPIOB
#define RELE_DESCE_ENCOSTO_PIN  GPIO_PIN_1
#define RELE_SOBE_ENCOSTO_PORT GPIOB
#define RELE_SOBE_ENCOSTO_PIN  GPIO_PIN_2
#define RELE_DESCE_ASSENTO_PORT GPIOB
#define RELE_DESCE_ASSENTO_PIN  GPIO_PIN_11
#define RELE_REFLETOR_PORT GPIOB
#define RELE_REFLETOR_PIN  GPIO_PIN_4

#define WATCHDOG_TOGGLE_MS 500U
#define BTN_DEBOUNCE_MS 50U
#define AUTO_MOVEMENT_MS 30000U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint32_t watchdogLastToggleMs = 0;
static SystemState_t systemState = SYSTEM_IDLE;
static uint8_t vzCompletoCiclo = 0;
static uint32_t operationStartMs = 0;

static uint8_t refletorOn = 0;

static DebouncedButton_t btnSobeAssento = { BTN_SOBE_ASSENTO_PORT, BTN_SOBE_ASSENTO_PIN, 0, 0, 0 };
static DebouncedButton_t btnDesceAssento = { BTN_DESCE_ASSENTO_PORT, BTN_DESCE_ASSENTO_PIN, 0, 0, 0 };
static DebouncedButton_t btnSobeEncosto = { BTN_SOBE_ENCOSTO_PORT, BTN_SOBE_ENCOSTO_PIN, 0, 0, 0 };
static DebouncedButton_t btnDesceEncosto = { BTN_DESCE_ENCOSTO_PORT, BTN_DESCE_ENCOSTO_PIN, 0, 0, 0 };
static DebouncedButton_t btnRefletor = { BTN_REFLETOR_PORT, BTN_REFLETOR_PIN, 0, 0, 0 };
static DebouncedButton_t btnVZ = { BTN_VZ_PORT, BTN_VZ_PIN, 0, 0, 0 };
static DebouncedButton_t btnPT = { BTN_PT_PORT, BTN_PT_PIN, 0, 0, 0 };

static uint8_t btnRefletorPrevStable = 0;
static uint8_t btnVZPrevStable = 0;
static uint8_t btnPTPrevStable = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void StopAllMotors(void);
static void StartVoltaZero(uint32_t now);
static void StartPosicaoTrabalho(uint32_t now);
static void DebounceButton_Update(DebouncedButton_t *btn, uint32_t now);
static void Buttons_Update(uint32_t now);
static void ManualControl_Update(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void StopAllMotors(void)
{
  HAL_GPIO_WritePin(RELE_SOBE_ASSENTO_PORT,
                    RELE_SOBE_ASSENTO_PIN | RELE_DESCE_ASSENTO_PIN | RELE_SOBE_ENCOSTO_PIN | RELE_DESCE_ENCOSTO_PIN,
                    GPIO_PIN_RESET);
}

static void StartVoltaZero(uint32_t now)
{
  StopAllMotors();
  HAL_GPIO_WritePin(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN, GPIO_PIN_SET);
  operationStartMs = now;
  systemState = SYSTEM_VOLTA_ZERO;
  vzCompletoCiclo = 0;
}

static void StartPosicaoTrabalho(uint32_t now)
{
  StopAllMotors();
  HAL_GPIO_WritePin(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN, GPIO_PIN_SET);
  operationStartMs = now;
  systemState = SYSTEM_POSICAO_TRABALHO;
}

static void DebounceButton_Update(DebouncedButton_t *btn, uint32_t now)
{
  uint8_t raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == GPIO_PIN_RESET) ? 1 : 0;

  if (raw != btn->lastRaw)
  {
    btn->lastRaw = raw;
    btn->lastChangeMs = now;
    return;
  }

  if (btn->stable != raw)
  {
    if ((now - btn->lastChangeMs) >= BTN_DEBOUNCE_MS)
    {
      btn->stable = raw;
    }
  }
}

static void Buttons_Update(uint32_t now)
{
  DebounceButton_Update(&btnSobeAssento, now);
  DebounceButton_Update(&btnDesceAssento, now);
  DebounceButton_Update(&btnSobeEncosto, now);
  DebounceButton_Update(&btnDesceEncosto, now);
  DebounceButton_Update(&btnRefletor, now);
  DebounceButton_Update(&btnVZ, now);
  DebounceButton_Update(&btnPT, now);
}

static void ManualControl_Update(void)
{
  if (btnSobeAssento.stable && !btnDesceAssento.stable)
  {
    HAL_GPIO_WritePin(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN, GPIO_PIN_RESET);
  }
  else if (btnDesceAssento.stable && !btnSobeAssento.stable)
  {
    HAL_GPIO_WritePin(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN, GPIO_PIN_RESET);
  }

  if (btnSobeEncosto.stable && !btnDesceEncosto.stable)
  {
    HAL_GPIO_WritePin(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN, GPIO_PIN_RESET);
  }
  else if (btnDesceEncosto.stable && !btnSobeEncosto.stable)
  {
    HAL_GPIO_WritePin(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN, GPIO_PIN_RESET);
  }

  HAL_GPIO_WritePin(RELE_REFLETOR_PORT, RELE_REFLETOR_PIN, refletorOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t now = HAL_GetTick();
    if ((now - watchdogLastToggleMs) >= WATCHDOG_TOGGLE_MS)
    {
      watchdogLastToggleMs = now;
      HAL_GPIO_TogglePin(Watch_Dog_GPIO_Port, Watch_Dog_Pin);
    }

    Buttons_Update(now);
    uint8_t refletorPressedEdge = (btnRefletor.stable && !btnRefletorPrevStable) ? 1 : 0;
    uint8_t vzPressedEdge = (btnVZ.stable && !btnVZPrevStable) ? 1 : 0;
    uint8_t ptPressedEdge = (btnPT.stable && !btnPTPrevStable) ? 1 : 0;
    btnRefletorPrevStable = btnRefletor.stable;
    btnVZPrevStable = btnVZ.stable;
    btnPTPrevStable = btnPT.stable;

    uint8_t anyButtonPressed = (btnSobeAssento.stable || btnDesceAssento.stable || btnSobeEncosto.stable ||
                                btnDesceEncosto.stable || btnRefletor.stable || btnVZ.stable || btnPT.stable)
                                   ? 1
                                   : 0;

    switch (systemState)
    {
      case SYSTEM_IDLE:
        if (refletorPressedEdge)
        {
          refletorOn = (uint8_t)!refletorOn;
        }

        if (vzPressedEdge)
        {
          StartVoltaZero(now);
        }
        else if (ptPressedEdge && vzCompletoCiclo)
        {
          StartPosicaoTrabalho(now);
        }
        else
        {
          ManualControl_Update();
        }
        break;

      case SYSTEM_VOLTA_ZERO:
        if (anyButtonPressed)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
          vzCompletoCiclo = 0;
        }
        else if ((now - operationStartMs) >= AUTO_MOVEMENT_MS)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
          vzCompletoCiclo = 1;
        }
        break;

      case SYSTEM_POSICAO_TRABALHO:
        if (anyButtonPressed)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
          vzCompletoCiclo = 0;
        }
        else if ((now - operationStartMs) >= AUTO_MOVEMENT_MS)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
          vzCompletoCiclo = 0;
        }
        break;

      default:
        StopAllMotors();
        systemState = SYSTEM_IDLE;
        vzCompletoCiclo = 0;
        break;
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Watch_Dog_GPIO_Port, Watch_Dog_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Watch_Dog_Pin */
  GPIO_InitStruct.Pin = Watch_Dog_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Watch_Dog_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  HAL_GPIO_WritePin(RELE_SOBE_ASSENTO_PORT,
                    RELE_SOBE_ASSENTO_PIN | RELE_DESCE_ENCOSTO_PIN | RELE_SOBE_ENCOSTO_PIN | RELE_DESCE_ASSENTO_PIN | RELE_REFLETOR_PIN,
                    GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = RELE_SOBE_ASSENTO_PIN | RELE_DESCE_ENCOSTO_PIN | RELE_SOBE_ENCOSTO_PIN | RELE_DESCE_ASSENTO_PIN | RELE_REFLETOR_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RELE_SOBE_ASSENTO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BTN_SOBE_ASSENTO_PIN | BTN_DESCE_ENCOSTO_PIN | BTN_SOBE_ENCOSTO_PIN | BTN_DESCE_ASSENTO_PIN | BTN_REFLETOR_PIN |
                        BTN_VZ_PIN | BTN_PT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  btnSobeAssento.lastRaw = (HAL_GPIO_ReadPin(btnSobeAssento.port, btnSobeAssento.pin) == GPIO_PIN_RESET) ? 1 : 0;
  btnSobeAssento.stable = btnSobeAssento.lastRaw;

  btnDesceAssento.lastRaw = (HAL_GPIO_ReadPin(btnDesceAssento.port, btnDesceAssento.pin) == GPIO_PIN_RESET) ? 1 : 0;
  btnDesceAssento.stable = btnDesceAssento.lastRaw;

  btnSobeEncosto.lastRaw = (HAL_GPIO_ReadPin(btnSobeEncosto.port, btnSobeEncosto.pin) == GPIO_PIN_RESET) ? 1 : 0;
  btnSobeEncosto.stable = btnSobeEncosto.lastRaw;

  btnDesceEncosto.lastRaw = (HAL_GPIO_ReadPin(btnDesceEncosto.port, btnDesceEncosto.pin) == GPIO_PIN_RESET) ? 1 : 0;
  btnDesceEncosto.stable = btnDesceEncosto.lastRaw;

  btnRefletor.lastRaw = (HAL_GPIO_ReadPin(btnRefletor.port, btnRefletor.pin) == GPIO_PIN_RESET) ? 1 : 0;
  btnRefletor.stable = btnRefletor.lastRaw;
  btnRefletorPrevStable = btnRefletor.stable;

  btnVZ.lastRaw = (HAL_GPIO_ReadPin(btnVZ.port, btnVZ.pin) == GPIO_PIN_RESET) ? 1 : 0;
  btnVZ.stable = btnVZ.lastRaw;
  btnVZPrevStable = btnVZ.stable;

  btnPT.lastRaw = (HAL_GPIO_ReadPin(btnPT.port, btnPT.pin) == GPIO_PIN_RESET) ? 1 : 0;
  btnPT.stable = btnPT.lastRaw;
  btnPTPrevStable = btnPT.stable;


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
