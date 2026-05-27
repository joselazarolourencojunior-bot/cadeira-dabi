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
#define PT_DEBOUNCE_MS     10U
#define AUTO_MOVEMENT_MS   60000U
#define BTN_PRESSED_STATE  GPIO_PIN_RESET
#define WATCHDOG_TOGGLE_MS 500U
#define CUBA_PULSE_MS      5000U
#define PT_LONG_PRESS_MS   5000U
#define PT_MAX_MOVE_MS     60000U
#define PT_FLASH_PAGE_SIZE 1024U
#define PT_MEM_MAGIC       0x454D5450UL
#define PT_MEM_VERSION     1UL
#define FLASHSIZE_BASE     0x1FFFF7E0UL

#if defined(cuba_in2_GPIO_Port) && defined(cuba_in2_Pin)
#define CUBA_IN_GPIO_Port cuba_in2_GPIO_Port
#define CUBA_IN_Pin       cuba_in2_Pin
#elif defined(cuba_in_GPIO_Port) && defined(cuba_in_Pin)
#define CUBA_IN_GPIO_Port cuba_in_GPIO_Port
#define CUBA_IN_Pin       cuba_in_Pin
#else
#error "CUBA input pin not defined (expected cuba_in2_* or cuba_in_*)"
#endif
/* USER CODE END PD */

/* USER CODE BEGIN PV */
static uint32_t watchdogLastToggleMs = 0;

static SystemState_t systemState = SYSTEM_IDLE;
static uint32_t operationStartMs = 0;

static uint8_t refletorOn = 0;
static uint8_t cubaOutOn = 0;
static uint32_t cubaActiveUntilMs = 0;
static GPIO_PinState ptPressedState = BTN_PRESSED_STATE;

static DebouncedButton_t btnSobeAssento  = { sobe_assento_GPIO_Port,    sobe_assento_Pin,    0, 0, 0 };
static DebouncedButton_t btnDesceEncosto = { desce_encosto_GPIO_Port,   desce_encosto_Pin,   0, 0, 0 };
static DebouncedButton_t btnSobeEncosto  = { desce_encostoA2_GPIO_Port, desce_encostoA2_Pin, 0, 0, 0 };
static DebouncedButton_t btnDesceAssento = { desce_assento_GPIO_Port,   desce_assento_Pin,   0, 0, 0 };
static DebouncedButton_t btnRefletor     = { bt_refletor_GPIO_Port,     bt_refletor_Pin,     0, 0, 0 };
static DebouncedButton_t btnVZ           = { vz_GPIO_Port,              vz_Pin,              0, 0, 0 };
static DebouncedButton_t btnPT           = { pt_GPIO_Port,              pt_Pin,              0, 0, 0 };
static DebouncedButton_t btnCuba         = { CUBA_IN_GPIO_Port,         CUBA_IN_Pin,         0, 0, 0 };

static uint8_t prevSobeAssento = 0;
static uint8_t prevDesceAssento = 0;
static uint8_t prevSobeEncosto = 0;
static uint8_t prevDesceEncosto = 0;
static uint8_t prevRefletor = 0;
static uint8_t prevVZ = 0;
static uint8_t prevPT = 0;
static uint8_t prevCuba = 0;

static uint32_t lastLoopMs = 0;
static uint8_t referenceValid = 0;
static int32_t seatPosMs = 0;
static int32_t backPosMs = 0;

static uint32_t ptPressStartMs = 0;
static uint8_t ptLongHandled = 0;

static uint32_t errorBlinkUntilMs = 0;
static uint32_t refletorBlinkUntilMs = 0;
static uint8_t refletorSuppressed = 0;
static uint8_t ptRecallAfterVZ = 0;

static uint8_t ptMemValid = 0;
static int32_t ptMemSeatMs = 0;
static int32_t ptMemBackMs = 0;
static uint32_t recallSeatUntilMs = 0;
static uint32_t recallBackUntilMs = 0;
/* USER CODE END PV */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* USER CODE BEGIN PFP */
static uint8_t EdgeFromStable(uint8_t stable, uint8_t *prevStable);
static void StopAllMotors(void);
static void StartVoltaZero(uint32_t now);
static void StartPosicaoTrabalho(uint32_t now);
static void DebounceButton_UpdateWithPressedState(DebouncedButton_t *btn, GPIO_PinState pressedState, uint32_t now);
static void Buttons_Update(uint32_t now);
static void ManualControl_Update(void);
static void UpdateNonMotorOutputs(void);
static uint8_t MotorsActive(void);
static void PositionTracking_Update(uint32_t now);
static uint8_t PT_Mem_Load(void);
static uint8_t PT_Mem_Save(int32_t seatMs, int32_t backMs);
static void PT_Recall_Start(uint32_t now);
static void PT_Recall_Update(uint32_t now);
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

static uint8_t MotorsActive(void)
{
  return (HAL_GPIO_ReadPin(GPIOB, rele_sobe_assento_Pin) == GPIO_PIN_SET ||
          HAL_GPIO_ReadPin(GPIOB, rele_desce_assento_Pin) == GPIO_PIN_SET ||
          HAL_GPIO_ReadPin(GPIOB, rele_sobe_encosto_Pin) == GPIO_PIN_SET ||
          HAL_GPIO_ReadPin(GPIOB, rele_desce_encosto_Pin) == GPIO_PIN_SET)
             ? 1U
             : 0U;
}

static void PositionTracking_Update(uint32_t now)
{
  if (!referenceValid)
  {
    lastLoopMs = now;
    return;
  }

  uint32_t dt = now - lastLoopMs;
  lastLoopMs = now;
  if (dt == 0U)
    return;

  uint8_t seatUp = (HAL_GPIO_ReadPin(GPIOB, rele_sobe_assento_Pin) == GPIO_PIN_SET) ? 1U : 0U;
  uint8_t seatDown = (HAL_GPIO_ReadPin(GPIOB, rele_desce_assento_Pin) == GPIO_PIN_SET) ? 1U : 0U;
  uint8_t backUp = (HAL_GPIO_ReadPin(GPIOB, rele_sobe_encosto_Pin) == GPIO_PIN_SET) ? 1U : 0U;
  uint8_t backDown = (HAL_GPIO_ReadPin(GPIOB, rele_desce_encosto_Pin) == GPIO_PIN_SET) ? 1U : 0U;

  if (seatUp && !seatDown)
    seatPosMs += (int32_t)dt;
  else if (seatDown && !seatUp)
    seatPosMs -= (int32_t)dt;

  if (backDown && !backUp)
    backPosMs += (int32_t)dt;
  else if (backUp && !backDown)
    backPosMs -= (int32_t)dt;
}

static uint32_t PT_Flash_PageAddress(void)
{
  uint16_t kb = *(__IO uint16_t *)FLASHSIZE_BASE;
  uint32_t endAddr = FLASH_BASE + ((uint32_t)kb * 1024U);
  return endAddr - PT_FLASH_PAGE_SIZE;
}

static uint8_t PT_Mem_Load(void)
{
  uint32_t base = PT_Flash_PageAddress();
  uint32_t magic = *(__IO uint32_t *)(base + 0U);
  uint32_t version = *(__IO uint32_t *)(base + 4U);
  uint32_t seatRaw = *(__IO uint32_t *)(base + 8U);
  uint32_t backRaw = *(__IO uint32_t *)(base + 12U);
  uint32_t check = *(__IO uint32_t *)(base + 16U);

  if (magic != PT_MEM_MAGIC || version != PT_MEM_VERSION)
    return 0U;

  if (check != (magic ^ version ^ seatRaw ^ backRaw))
    return 0U;

  ptMemSeatMs = (int32_t)seatRaw;
  ptMemBackMs = (int32_t)backRaw;
  return 1U;
}

static uint8_t PT_Mem_Save(int32_t seatMs, int32_t backMs)
{
  uint32_t base = PT_Flash_PageAddress();

  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef erase = {0};
  uint32_t pageError = 0;
  erase.TypeErase = FLASH_TYPEERASE_PAGES;
  erase.PageAddress = base;
  erase.NbPages = 1;
  if (HAL_FLASHEx_Erase(&erase, &pageError) != HAL_OK)
  {
    HAL_FLASH_Lock();
    return 0U;
  }

  uint32_t seatRaw = (uint32_t)seatMs;
  uint32_t backRaw = (uint32_t)backMs;
  uint32_t check = PT_MEM_MAGIC ^ PT_MEM_VERSION ^ seatRaw ^ backRaw;

  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, base + 0U, PT_MEM_MAGIC) != HAL_OK ||
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, base + 4U, PT_MEM_VERSION) != HAL_OK ||
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, base + 8U, seatRaw) != HAL_OK ||
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, base + 12U, backRaw) != HAL_OK ||
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, base + 16U, check) != HAL_OK)
  {
    HAL_FLASH_Lock();
    return 0U;
  }

  HAL_FLASH_Lock();
  return 1U;
}

static void StartVoltaZero(uint32_t now)
{
  StopAllMotors();
  HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_SET);
  operationStartMs = now;
  systemState = SYSTEM_VOLTA_ZERO;
}

static void PT_Recall_Start(uint32_t now)
{
  if (!referenceValid || !ptMemValid)
  {
    StopAllMotors();
    recallSeatUntilMs = now;
    recallBackUntilMs = now;
    return;
  }

  int32_t seatRemain = ptMemSeatMs - seatPosMs;
  int32_t backRemain = ptMemBackMs - backPosMs;

  StopAllMotors();

  uint32_t seatAbs = (seatRemain < 0) ? (uint32_t)(-seatRemain) : (uint32_t)seatRemain;
  uint32_t backAbs = (backRemain < 0) ? (uint32_t)(-backRemain) : (uint32_t)backRemain;

  recallSeatUntilMs = now + seatAbs;
  recallBackUntilMs = now + backAbs;

  if (seatRemain > 0)
    HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_SET);
  else if (seatRemain < 0)
    HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_SET);

  if (backRemain > 0)
    HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_SET);
  else if (backRemain < 0)
    HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_SET);
}

static void PT_Recall_Update(uint32_t now)
{
  if (now >= recallSeatUntilMs)
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_assento_Pin, GPIO_PIN_RESET);
  }

  if (now >= recallBackUntilMs)
  {
    HAL_GPIO_WritePin(GPIOB, rele_sobe_encosto_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_RESET);
  }
}

static void StartPosicaoTrabalho(uint32_t now)
{
  if (!ptMemValid)
  {
    StopAllMotors();
    HAL_GPIO_WritePin(GPIOB, rele_sobe_assento_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, rele_desce_encosto_Pin, GPIO_PIN_SET);
    recallSeatUntilMs = now + PT_MAX_MOVE_MS;
    recallBackUntilMs = now + PT_MAX_MOVE_MS;
    operationStartMs = now;
    systemState = SYSTEM_POSICAO_TRABALHO;
    return;
  }

  PT_Recall_Start(now);
  operationStartMs = now;
  systemState = SYSTEM_POSICAO_TRABALHO;
}

static void DebounceButton_UpdateWithPressedState(DebouncedButton_t *btn, GPIO_PinState pressedState, uint32_t now)
{
  uint8_t raw = (HAL_GPIO_ReadPin(btn->port, btn->pin) == pressedState) ? 1U : 0U;

  if (raw != btn->lastRaw)
  {
    btn->lastRaw = raw;
    btn->lastChangeMs = now;
    return;
  }

  if (btn->stable != raw)
  {
    uint32_t debounceMs = (btn == &btnPT) ? PT_DEBOUNCE_MS : BTN_DEBOUNCE_MS;
    if ((now - btn->lastChangeMs) >= debounceMs)
    {
      btn->stable = raw;
    }
  }
}

static void Buttons_Update(uint32_t now)
{
  DebounceButton_UpdateWithPressedState(&btnSobeAssento, BTN_PRESSED_STATE, now);
  DebounceButton_UpdateWithPressedState(&btnDesceAssento, BTN_PRESSED_STATE, now);
  DebounceButton_UpdateWithPressedState(&btnSobeEncosto, BTN_PRESSED_STATE, now);
  DebounceButton_UpdateWithPressedState(&btnDesceEncosto, BTN_PRESSED_STATE, now);
  DebounceButton_UpdateWithPressedState(&btnRefletor, BTN_PRESSED_STATE, now);
  DebounceButton_UpdateWithPressedState(&btnVZ, BTN_PRESSED_STATE, now);
  DebounceButton_UpdateWithPressedState(&btnPT, ptPressedState, now);
  DebounceButton_UpdateWithPressedState(&btnCuba, BTN_PRESSED_STATE, now);
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
  uint8_t motorsActive = MotorsActive();
  if (motorsActive)
  {
    if (refletorOn)
      refletorSuppressed = 1U;
  }
  else
  {
    refletorSuppressed = 0U;
  }

  uint8_t refletorOut = (refletorOn && !refletorSuppressed) ? 1U : 0U;
  if (HAL_GetTick() < refletorBlinkUntilMs)
    refletorOut = (refletorOut ? 0U : 1U);
  HAL_GPIO_WritePin(GPIOB, rele_refletor_Pin, refletorOut ? GPIO_PIN_SET : GPIO_PIN_RESET);
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
  lastLoopMs = watchdogLastToggleMs;
  referenceValid = 0U;
  seatPosMs = 0;
  backPosMs = 0;
  ptMemValid = PT_Mem_Load();

  ptPressedState = BTN_PRESSED_STATE;

  btnSobeAssento.lastRaw  = (HAL_GPIO_ReadPin(btnSobeAssento.port, btnSobeAssento.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnDesceAssento.lastRaw = (HAL_GPIO_ReadPin(btnDesceAssento.port, btnDesceAssento.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnSobeEncosto.lastRaw  = (HAL_GPIO_ReadPin(btnSobeEncosto.port, btnSobeEncosto.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnDesceEncosto.lastRaw = (HAL_GPIO_ReadPin(btnDesceEncosto.port, btnDesceEncosto.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnRefletor.lastRaw     = (HAL_GPIO_ReadPin(btnRefletor.port, btnRefletor.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnVZ.lastRaw           = (HAL_GPIO_ReadPin(btnVZ.port, btnVZ.pin) == BTN_PRESSED_STATE) ? 1U : 0U;
  btnPT.lastRaw           = (HAL_GPIO_ReadPin(btnPT.port, btnPT.pin) == ptPressedState) ? 1U : 0U;
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

    uint32_t wdMs = WATCHDOG_TOGGLE_MS;
    if (now < errorBlinkUntilMs)
      wdMs = 120U;

    if ((now - watchdogLastToggleMs) >= wdMs)
    {
      watchdogLastToggleMs = now;
      HAL_GPIO_TogglePin(watch_dog_GPIO_Port, watch_dog_Pin);
    }

    Buttons_Update(now);
    PositionTracking_Update(now);

    uint8_t edgeSobeAssento  = EdgeFromStable(btnSobeAssento.stable, &prevSobeAssento);
    uint8_t edgeDesceAssento = EdgeFromStable(btnDesceAssento.stable, &prevDesceAssento);
    uint8_t edgeSobeEncosto  = EdgeFromStable(btnSobeEncosto.stable, &prevSobeEncosto);
    uint8_t edgeDesceEncosto = EdgeFromStable(btnDesceEncosto.stable, &prevDesceEncosto);
    uint8_t edgeRefletor     = EdgeFromStable(btnRefletor.stable, &prevRefletor);
    uint8_t edgeVZ           = EdgeFromStable(btnVZ.stable, &prevVZ);
    uint8_t prevPTBefore     = prevPT;
    uint8_t edgePT           = EdgeFromStable(btnPT.stable, &prevPT);
    uint8_t fallPT           = (!btnPT.stable && prevPTBefore) ? 1U : 0U;
    uint8_t edgeCuba         = EdgeFromStable(btnCuba.stable, &prevCuba);

    if (edgePT)
    {
      ptPressStartMs = now;
      ptLongHandled = 0U;
    }

    if (btnPT.stable && !ptLongHandled && ptPressStartMs != 0U)
    {
      if ((now - ptPressStartMs) >= PT_LONG_PRESS_MS)
      {
        ptLongHandled = 1U;
        if (systemState == SYSTEM_IDLE && referenceValid)
        {
          if (PT_Mem_Save(seatPosMs, backPosMs))
          {
            ptMemSeatMs = seatPosMs;
            ptMemBackMs = backPosMs;
            ptMemValid = 1U;
            refletorBlinkUntilMs = now + 300U;
          }
        }
        else
        {
          errorBlinkUntilMs = now + 2000U;
        }
      }
    }

    if (fallPT && ptPressStartMs != 0U)
    {
      uint32_t held = now - ptPressStartMs;
      if (!ptLongHandled && held < PT_LONG_PRESS_MS)
      {
        if (systemState == SYSTEM_IDLE && referenceValid && ptMemValid)
        {
          StartPosicaoTrabalho(now);
        }
        else if (systemState == SYSTEM_IDLE && !referenceValid && ptMemValid)
        {
          errorBlinkUntilMs = now + 2000U;
          ptRecallAfterVZ = 1U;
          StartVoltaZero(now);
        }
        else if (systemState == SYSTEM_IDLE && !ptMemValid)
        {
          StartPosicaoTrabalho(now);
        }
      }
      ptPressStartMs = 0U;
    }

    if (edgeRefletor && systemState == SYSTEM_IDLE)
    {
      refletorOn = (uint8_t)!refletorOn;
    }

    if (edgeCuba && systemState == SYSTEM_IDLE)
    {
      if (cubaActiveUntilMs < now)
        cubaActiveUntilMs = now;
      cubaActiveUntilMs += CUBA_PULSE_MS;
    }

    cubaOutOn = (now < cubaActiveUntilMs) ? 1U : 0U;

    uint8_t anyKeyPressedEdge =
        (edgeSobeAssento || edgeDesceAssento || edgeSobeEncosto || edgeDesceEncosto ||
         edgeRefletor || edgeVZ || edgePT || edgeCuba) ? 1U : 0U;

    switch (systemState)
    {
      case SYSTEM_IDLE:
        if (edgeVZ)
          StartVoltaZero(now);
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
          referenceValid = 1U;
          seatPosMs = 0;
          backPosMs = 0;
          lastLoopMs = now;
          refletorOn = 0U;
          refletorSuppressed = 0U;
          if (ptRecallAfterVZ)
          {
            ptRecallAfterVZ = 0U;
            StartPosicaoTrabalho(now);
          }
          else
          {
          systemState = SYSTEM_IDLE;
          }
        }
        break;

      case SYSTEM_POSICAO_TRABALHO:
        if (anyKeyPressedEdge)
        {
          StopAllMotors();
          systemState = SYSTEM_IDLE;
        }
        else if ((now - operationStartMs) >= PT_MAX_MOVE_MS)
        {
          StopAllMotors();
          refletorOn = 1U;
          refletorSuppressed = 0U;
          systemState = SYSTEM_IDLE;
        }
        else
        {
          PT_Recall_Update(now);
          if ((HAL_GPIO_ReadPin(GPIOB, rele_sobe_assento_Pin) == GPIO_PIN_RESET) &&
              (HAL_GPIO_ReadPin(GPIOB, rele_desce_assento_Pin) == GPIO_PIN_RESET) &&
              (HAL_GPIO_ReadPin(GPIOB, rele_sobe_encosto_Pin) == GPIO_PIN_RESET) &&
              (HAL_GPIO_ReadPin(GPIOB, rele_desce_encosto_Pin) == GPIO_PIN_RESET))
          {
            refletorOn = 1U;
            refletorSuppressed = 0U;
            systemState = SYSTEM_IDLE;
          }
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

  GPIO_InitStruct.Pin = CUBA_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CUBA_IN_GPIO_Port, &GPIO_InitStruct);

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
