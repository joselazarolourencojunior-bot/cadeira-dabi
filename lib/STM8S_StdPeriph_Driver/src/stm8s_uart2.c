/**
 ******************************************************************************
 * @file    stm8s_uart2.c
 * @brief   UART2 peripheral driver for STM8S
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm8s_uart2.h"

/* Private defines -----------------------------------------------------------*/
/* UART2 registers */
#define UART2_BASE      0x5240
#define UART2_SR_ADDR   (UART2_BASE + 0x00)
#define UART2_DR_ADDR   (UART2_BASE + 0x01)
#define UART2_BRR1_ADDR (UART2_BASE + 0x02)
#define UART2_BRR2_ADDR (UART2_BASE + 0x03)
#define UART2_CR1_ADDR  (UART2_BASE + 0x04)
#define UART2_CR2_ADDR  (UART2_BASE + 0x05)
#define UART2_CR3_ADDR  (UART2_BASE + 0x06)

#define UART2_SR   (*(volatile uint8_t *)UART2_SR_ADDR)
#define UART2_DR   (*(volatile uint8_t *)UART2_DR_ADDR)
#define UART2_BRR1 (*(volatile uint8_t *)UART2_BRR1_ADDR)
#define UART2_BRR2 (*(volatile uint8_t *)UART2_BRR2_ADDR)
#define UART2_CR1  (*(volatile uint8_t *)UART2_CR1_ADDR)
#define UART2_CR2  (*(volatile uint8_t *)UART2_CR2_ADDR)
#define UART2_CR3  (*(volatile uint8_t *)UART2_CR3_ADDR)

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Deinitializes the UART2 peripheral.
  * @param  None
  * @retval None
  */
void UART2_DeInit(void)
{
    UART2_CR1 = 0x00;
    UART2_CR2 = 0x00;
    UART2_CR3 = 0x00;
    UART2_BRR1 = 0x00;
    UART2_BRR2 = 0x00;
    UART2_SR = 0xC0;
}

/**
  * @brief  Initializes the UART2 according to the specified parameters.
  * @param  BaudRate: The baudrate.
  * @param  WordLength: the word length
  * @param  StopBits: the number of stop bits
  * @param  Parity: the parity
  * @param  SyncMode: the synchronous mode
  * @param  Mode: the UART2 mode TX/RX enable
  * @retval None
  */
void UART2_Init(uint32_t BaudRate,
                UART2_WordLength_TypeDef WordLength,
                UART2_StopBits_TypeDef StopBits,
                UART2_Parity_TypeDef Parity,
                UART2_SyncMode_TypeDef SyncMode,
                UART2_Mode_TypeDef Mode)
{
    uint32_t BaudRate_Mantissa = 0;
    uint32_t BaudRate_Mantissa100 = 0;
    
    /* Disable UART */
    UART2_CR1 &= (uint8_t)(~0x20);
    
    /* Clear the word length, parity, stop bits and mode bits */
    UART2_CR1 = 0;
    UART2_CR3 = 0;
    
    /* Set the word length bit */
    UART2_CR1 |= (uint8_t)WordLength;
    
    /* Set the parity and stop bits */
    UART2_CR3 |= (uint8_t)((uint8_t)StopBits | (uint8_t)Parity);
    
    /* Set the synchronous mode */
    if ((uint8_t)(SyncMode & (uint8_t)0x80) == (uint8_t)0x00)
    {
        UART2_CR3 |= (uint8_t)SyncMode;
    }
    
    /* Set the mode (TX/RX) */
    UART2_CR2 |= (uint8_t)Mode;
    
    /* Set the baud rate divisor */
    /* F_CPU = 16000000 Hz */
    BaudRate_Mantissa = ((uint32_t)16000000) / BaudRate;
    BaudRate_Mantissa100 = (((uint32_t)16000000) * 100) / BaudRate;
    
    /* Set BRR2 first (contains LSB and MSB) */
    UART2_BRR2 = (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100)) << 4) / 100) & (uint8_t)0x0F);
    UART2_BRR2 |= (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0);
    
    /* Set BRR1 (contains mantissa bits [11:4]) */
    UART2_BRR1 = (uint8_t)BaudRate_Mantissa;
}

/**
  * @brief  Enable or Disable the UART2 peripheral.
  * @param  NewState: The new state of the UART peripheral.
  * @retval None
  */
void UART2_Cmd(FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        UART2_CR1 |= 0x20;  /* UART2 Enable */
    }
    else
    {
        UART2_CR1 &= (uint8_t)(~0x20);  /* UART2 Disable */
    }
}

/**
  * @brief  Transmits 8 bit data through the UART2 peripheral.
  * @param  Data: the data to transmit.
  * @retval None
  */
void UART2_SendData8(uint8_t Data)
{
    UART2_DR = Data;
}

/**
  * @brief  Returns the most recent received data by the UART2 peripheral.
  * @param  None
  * @retval The received data.
  */
uint8_t UART2_ReceiveData8(void)
{
    return UART2_DR;
}

/**
  * @brief  Transmits break characters.
  * @param  None
  * @retval None
  */
void UART2_SendBreak(void)
{
    UART2_CR2 |= 0x01;  /* Send break */
}

/**
  * @brief  Checks whether the specified UART2 flag is set or not.
  * @param  UART2_FLAG: specifies the flag to check.
  * @retval FlagStatus (SET or RESET)
  */
FlagStatus UART2_GetFlagStatus(UART2_Flag_TypeDef UART2_FLAG)
{
    FlagStatus status = RESET;
    
    if ((UART2_SR & (uint8_t)UART2_FLAG) != (uint8_t)0x00)
    {
        status = SET;
    }
    else
    {
        status = RESET;
    }
    
    return status;
}

/**
  * @brief  Clears the UART2 flags.
  * @param  UART2_FLAG: specifies the flag to clear
  * @retval None
  */
void UART2_ClearFlag(UART2_Flag_TypeDef UART2_FLAG)
{
    /* Clear the flag by writing 0 to SR and reading DR */
    UART2_SR = (uint8_t)~(UART2_FLAG);
    (void)(UART2_DR);
}
