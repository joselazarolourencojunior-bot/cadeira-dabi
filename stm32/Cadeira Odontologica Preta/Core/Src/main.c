/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdint.h>
/* USER CODE END Includes */

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

/* USER CODE BEGIN PD */
#define BTN_DEBOUNCE_MS    50U
#define AUTO_MOVEMENT_MS   30000U
#define BTN_PRESSED_STATE  GPIO_PIN_RESET
#define WATCHDOG_TOGGLE_MS 500U
/* USER CODE END PD */

/* USER CODE BEGIN PV */
static uint32_t watchdogLastToggleMs = 0;

static SystemState_t systemState = SYSTEM_IDLE;
static uint32_t operationStartMs = 0;

static uint8_t refletorOn = 0;
static uint8_t cubaOutOn = 0;

static DebouncedButton_t btnSobeAssento  = { sobe_assento_GPIO_Port,    sobe_assento_Pin,    0, 0, 0 };
static DebouncedButton_t btnDesceEncosto = { desce_encosto_GPIO_Port,   desce_encosto_Pin,   0, 0, 0 };
static DebouncedButton_t btnSobeEncosto  = { desce_encostoA2_GPIO_Port, desce_encostoA2_Pin, 0, 0, 0 };
static DebouncedButton_t btnDesceAssento = { desce_assento_GPIO_Port,   desce_assento_Pin,   0, 0, 0 };
static DebouncedButton_t btnRefletor     = { bt_refletor_GPIO_Port,     bt_refletor_Pin,     0, 0, 0 };
static DebouncedButton_t btnVZ           = { vz_GPIO_Port,              vz_Pin,              0, 0, 0 };
static DebouncedButton_t btnPT           = { pt_GPIO_Port,              pt_Pin,              0, 0, 0 };
static DebouncedButton_t btnCuba         = { cuba_in2_GPIO_Port,        cuba_in2_Pin,        0, 0, 0 };

static uint8_t prevSobeAssento = 0;
static uint8_t prevDesceAssento = 0;
static uint8_t prevSobeEncosto = 0;
static uint8_t prevDesceEncosto = 0;
static uint8_t prevRefletor = 0;
static uint8_t prevVZ = 0;
static uint8_t prevPT = 0;
static uint8_t prevCuba = 0;
/* USER CODE END PV */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
static uint8_t EdgeFromStable(uint8_t stable, uint8_t *prevStable);
static void StopAllMotors(void);
static void StartVoltaZero(uint32_t now);
static void StartPosicaoTrabalho(uint32_t now);
static void DebounceButton_Update(DebouncedButton_t *btn, uint32_t now);
static void Buttons_Update(uint32_t now);
static void ManualControl_Update(void);
static void UpdateNonMotorOutputs(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
static uint8_t EdgeFromStable(uint8_t stable, uint8_t *prevStable)
{
  uint8_t edge = (stable && !(*prevStable)) ? 1U : 0U;
  *prevStable = stable;
  return edge;
}

static void StopAllMotors(void)
{
  HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_RESET);
}

static void StartVoltaZero(uint32_t now)
{
  StopAllMotors();
  HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_SET);
  operationStartMs = now;
  systemState = SYSTEM_VOLTA_ZERO;
}

static void StartPosicaoTrabalho(uint32_t now)
{
  StopAllMotors();
  HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_SET);
  operationStartMs = now;
  systemState = SYSTEM_POSICAO_TRABALHO;
}

static void DebounceButton_Update(DebouncedButton_t *btn, uint32_t now)
{
  uint8_t raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == BTN_PRESSED_STATE) ? 1U : 0U;

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
  DebounceButton_Update(&btnCuba, now);
}

static void ManualControl_Update(void)
{
  if (btnSobeAssento.stable && !btnDesceAssento.stable)
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_RESET);
  }
  else if (btnDesceAssento.stable && !btnSobeAssento.stable)
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_RESET);
  }

  if (btnSobeEncosto.stable && !btnDesceEncosto.stable)
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_RESET);
  }
  else if (btnDesceEncosto.stable && !btnSobeEncosto.stable)
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_RESET);
  }
}

static void UpdateNonMotorOutputs(void)
{
  HAL_GPIO_WritePin(GPIOB, rele_refletor_Pin, refletorOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(out_cuba_GPIO_Port, out_cuba_Pin, cubaOutOn ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();

  SystemClock_Config();
  MX_GPIO_Init();

  watchdogLastToggleMs = HAL_GetTick();

  btnSobeAssento.lastRaw  = (HAL_GPIO_ReadPin(btnSobeAssento.port, btnSobeAssento.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnDesceAssento.lastRaw = (HAL_GPIO_ReadPin(btnDesceAssento.port, btnDesceAssento.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnSobeEncosto.lastRaw  = (HAL_GPIO_ReadPin(btnSobeEncosto.port, btnSobeEncosto.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnDesceEncosto.lastRaw = (HAL_GPIO_ReadPin(btnDesceEncosto.port, btnDesceEncosto.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnRefletor.lastRaw     = (HAL_GPIO_ReadPin(btnRefletor.port, btnRefletor.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnVZ.lastRaw           = (HAL_GPIO_ReadPin(btnVZ.port, btnVZ.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnPT.lastRaw           = (HAL_GPIO_ReadPin(btnPT.port, btnPT.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnCuba.lastRaw         = (HAL_GPIO_ReadPin(btnCuba.port, btnCuba.pin) == BTN_PRESSED_STATE) ? 1U : 0U;

  btnSobeAssento.stable  = btnSobeAssento.lastRaw;
  btnDesceAssento.stable = btnDesceAssento.lastRaw;
  btnSobeEncosto.stable  = btnSobeEncosto.lastRaw;
  btnDesceEncosto.stable = btnDesceEncosto.lastRaw;
  btnRefletor.stable     = btnRefletor.lastRaw;
  btnVZ.stable           = btnVZ.lastRaw;
  btnPT.stable           = btnPT.lastRaw;
  btnCuba.stable         = btnCuba.lastRaw;

  prevSobeAssento = btnSobeAssento.stable;
  prevDesceAssento = btnDesceAssento.stable;
  prevSobeEncosto = btnSobeEncosto.stable;
  prevDesceEncosto = btnDesceEncosto.stable;
  prevRefletor = btnRefletor.stable;
  prevVZ = btnVZ.stable;
  prevPT = btnPT.stable;
  prevCuba = btnCuba.stable;

  while (1)
  {
    uint32_t now = HAL_GetTick();

    if ((now - watchdogLastToggleMs) >= WATCHDOG_TOGGLE_MS)
    {
      watchdogLastToggleMs = now;
      HAL_GPIO_TogglePin(watch_dog_GPIO_Port, watch_dog_Pin);
    }

    Buttons_Update(now);

    uint8_t edgeSobeAssento  = EdgeFromStable(btnSobeAssento.stable, &prevSobeAssento);
    uint8_t edgeDesceAssento = EdgeFromStable(btnDesceAssento.stable, &prevDesceAssento);
    uint8_t edgeSobeEncosto  = EdgeFromStable(btnSobeEncosto.stable, &prevSobeEncosto);
    uint8_t edgeDesceEncosto = EdgeFromStable(btnDesceEncosto.stable, &prevDesceEncosto);
    uint8_t edgeRefletor     = EdgeFromStable(btnRefletor.stable, &prevRefletor);
    uint8_t edgeVZ           = EdgeFromStable(btnVZ.stable, &prevVZ);
    uint8_t edgePT           = EdgeFromStable(btnPT.stable, &prevPT);
    uint8_t edgeCuba         = EdgeFromStable(btnCuba.stable, &prevCuba);

    if (edgeRefletor && systemState == SYSTEM_IDLE)
    {
      refletorOn = (uint8_t)!refletorOn;
    }

    if (edgeCuba && systemState == SYSTEM_IDLE)
    {
      cubaOutOn = (uint8_t)!cubaOutOn;
    }

    uint8_t anyKeyPressedEdge =
        (edgeSobeAssento || edgeDesceAssento || edgeSobeEncosto || edgeDesceEncosto ||
         edgeRefletor || edgeVZ || edgePT || edgeCuba) ? 1U : 0U;

    switch (systemState)
    {
      case SYSTEM_IDLE:
        if (edgeVZ)
          StartVoltaZero(now);
        else if (edgePT)
          StartPosicaoTrabalho(now);
        else
          ManualControl_Update();
        break;

      case SYSTEM_VOLTA_ZERO:
        if (anyKeyPressedEdge)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
        }
        else if ((now - operationStartMs) >= AUTO_MOVEMENT_MS)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
        }
        break;

      case SYSTEM_POSICAO_TRABALHO:
        if (anyKeyPressedEdge)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
        }
        else if ((now - operationStartMs) >= AUTO_MOVEMENT_MS)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
        }
        break;

      default:
        StopAllMotors();
        systemState = SYSTEM_IDLE;
        break;
    }

    UpdateNonMotorOutputs();
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(watch_dog_GPIO_Port, watch_dog_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin|rele_desce_encosto_Pin|rele_desce_assento_Pin|rele_sobe_encosto_Pin
                          |rele_refletor_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(out_cuba_GPIO_Port, out_cuba_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = watch_dog_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(watch_dog_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = sobe_assento_Pin|desce_encosto_Pin|desce_encostoA2_Pin|desce_assento_Pin
                          |bt_refletor_Pin|vz_Pin|pt_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = rele_sobe_assento_Pin|rele_desce_encosto_Pin|rele_desce_assento_Pin|rele_sobe_encosto_Pin
                          |rele_refletor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = cuba_in2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(cuba_in2_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = out_cuba_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(out_cuba_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
