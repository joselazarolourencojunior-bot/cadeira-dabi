/**
 ******************************************************************************
 * @file    stm8s_uart2.h
 * @brief   UART2 peripheral driver header file for STM8S
 ******************************************************************************
 */

#ifndef __STM8S_UART2_H
#define __STM8S_UART2_H

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"

/* Exported types ------------------------------------------------------------*/

/** @addtogroup UART2_Exported_Types
  * @{
  */

typedef enum {
    UART2_WORDLENGTH_8D = (uint8_t)0x00,  /**< 8 bits Data */
    UART2_WORDLENGTH_9D = (uint8_t)0x10   /**< 9 bits Data */
} UART2_WordLength_TypeDef;

typedef enum {
    UART2_STOPBITS_1 = (uint8_t)0x00,     /**< One stop bit */
    UART2_STOPBITS_2 = (uint8_t)0x20      /**< Two stop bits */
} UART2_StopBits_TypeDef;

typedef enum {
    UART2_PARITY_NO     = (uint8_t)0x00,  /**< No parity */
    UART2_PARITY_EVEN   = (uint8_t)0x04,  /**< Even parity */
    UART2_PARITY_ODD    = (uint8_t)0x06   /**< Odd parity */
} UART2_Parity_TypeDef;

typedef enum {
    UART2_SYNCMODE_CLOCK_DISABLE = (uint8_t)0x80,  /**< Clock disabled */
    UART2_SYNCMODE_CLOCK_ENABLE  = (uint8_t)0x08   /**< Clock enabled */
} UART2_SyncMode_TypeDef;

typedef enum {
    UART2_MODE_RX_ENABLE = (uint8_t)0x08,     /**< RX Enable */
    UART2_MODE_TX_ENABLE = (uint8_t)0x04,     /**< TX Enable */
    UART2_MODE_TXRX_ENABLE = (uint8_t)0x0C   /**< TX RX Enable */
} UART2_Mode_TypeDef;

typedef enum {
    UART2_FLAG_TXE  = (uint16_t)0x0080,  /**< Transmit Data Register Empty flag */
    UART2_FLAG_TC   = (uint16_t)0x0040,  /**< Transmission Complete flag */
    UART2_FLAG_RXNE = (uint16_t)0x0020,  /**< Read Data Register Not Empty flag */
    UART2_FLAG_IDLE = (uint16_t)0x0010,  /**< Idle line detected flag */
    UART2_FLAG_OR   = (uint16_t)0x0008,  /**< OverRun error flag */
    UART2_FLAG_NF   = (uint16_t)0x0004,  /**< Noise error flag */
    UART2_FLAG_FE   = (uint16_t)0x0002,  /**< Framing Error flag */
    UART2_FLAG_PE   = (uint16_t)0x0001   /**< Parity Error flag */
} UART2_Flag_TypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

void UART2_DeInit(void);
void UART2_Init(uint32_t BaudRate,
                UART2_WordLength_TypeDef WordLength,
                UART2_StopBits_TypeDef StopBits,
                UART2_Parity_TypeDef Parity,
                UART2_SyncMode_TypeDef SyncMode,
                UART2_Mode_TypeDef Mode);
void UART2_Cmd(FunctionalState NewState);
void UART2_SendData8(uint8_t Data);
uint8_t UART2_ReceiveData8(void);
void UART2_SendBreak(void);
FlagStatus UART2_GetFlagStatus(UART2_Flag_TypeDef UART2_FLAG);
void UART2_ClearFlag(UART2_Flag_TypeDef UART2_FLAG);

#endif /* __STM8S_UART2_H */
