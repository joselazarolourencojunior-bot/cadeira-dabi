/**
 ******************************************************************************
 * @file    main.c
 * @author  Lazaro
 * @version V1.0
 * @date    08-Fevereiro-2026
 * @brief   Sistema de Controle de Cadeira Odontológica - TUDO EM UM ARQUIVO
 *          Microcontrolador: STM8S105K4T6C
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stdint.h"

/* ============================================================================
   CONFIGURAÇÕES E DEFINIÇÕES
   ============================================================================ */

/* Versão do sistema */
#define SYSTEM_VERSION      "1.0"
#define SYSTEM_NAME         "Cadeira Odontologica - Reles"
#define F_CPU               16000000UL

/* Configurações dos Relés (Ativo HIGH) - Usando pinos True Open-Drain */
#define RELE_SOBE_ASSENTO_PORT      GPIOC
#define RELE_SOBE_ASSENTO_PIN       GPIO_PIN_5
#define RELE_DESCE_ASSENTO_PORT     GPIOC
#define RELE_DESCE_ASSENTO_PIN      GPIO_PIN_6
#define RELE_SOBE_ENCOSTO_PORT      GPIOC
#define RELE_SOBE_ENCOSTO_PIN       GPIO_PIN_7
#define RELE_DESCE_ENCOSTO_PORT     GPIOD
#define RELE_DESCE_ENCOSTO_PIN      GPIO_PIN_1
#define RELE_REFLETOR_PORT          GPIOD
#define RELE_REFLETOR_PIN           GPIO_PIN_2

/* LED Watchdog */
#define LED_WATCHDOG_PORT           GPIOE
#define LED_WATCHDOG_PIN            GPIO_PIN_5

/* Configurações dos Botões (com pull-up interno) */
#define BTN_PORT                    GPIOB
/* NOTA: Botões manuais PB0-PB3 NÃO existem fisicamente no hardware */
/* #define BTN_SOBE_ASSENTO_PIN        GPIO_PIN_0 */
/* #define BTN_DESCE_ASSENTO_PIN       GPIO_PIN_1 */
/* #define BTN_SOBE_ENCOSTO_PIN        GPIO_PIN_2 */
/* #define BTN_DESCE_ENCOSTO_PIN       GPIO_PIN_3 */
#define BTN_REFLETOR_PIN            GPIO_PIN_4
#define BTN_VOLTA_ZERO_PIN          GPIO_PIN_5
#define BTN_POSICAO_TRABALHO_PIN    GPIO_PIN_6
/* NOTA: Botão de emergência PB7 NÃO existe fisicamente */
/* A emergência funciona ao pressionar QUALQUER botão durante VZ ou PT */
/* #define BTN_EMERGENCIA_PIN          GPIO_PIN_7 */

/* #define BTN_SOBE_ASSENTO            (1 << 0) */
/* #define BTN_DESCE_ASSENTO           (1 << 1) */
/* #define BTN_SOBE_ENCOSTO            (1 << 2) */
/* #define BTN_DESCE_ENCOSTO           (1 << 3) */
#define BTN_REFLETOR                (1 << 4)
#define BTN_VOLTA_ZERO              (1 << 5)
#define BTN_POSICAO_TRABALHO        (1 << 6)
/* #define BTN_EMERGENCIA              (1 << 7) */
#define BTN_DEBOUNCE_TIME           50

/* NOTA: Sensores de limite físicos NÃO existem */
/* O sistema usa ENCODER VIRTUAL POR TEMPO para controle de posição */
/* #define LIMIT_ASSENTO_SUP_PORT      GPIOC */
/* #define LIMIT_ASSENTO_SUP_PIN       GPIO_PIN_3 */
/* #define LIMIT_ASSENTO_INF_PORT      GPIOC */
/* #define LIMIT_ASSENTO_INF_PIN       GPIO_PIN_4 */
/* #define LIMIT_ENCOSTO_SUP_PORT      GPIOA */
/* #define LIMIT_ENCOSTO_SUP_PIN       GPIO_PIN_1 */
/* #define LIMIT_ENCOSTO_INF_PORT      GPIOA */
/* #define LIMIT_ENCOSTO_INF_PIN       GPIO_PIN_2 */

/* ============================================================================
   MATRIZ DE CHAVEAMENTO - M1, M2, M3 (PREPARADO PARA IMPLEMENTAÇÃO FUTURA)
   ============================================================================ */
/* NOTA: Definir pinos conforme especificação do hardware */
/* #define MATRIZ_M1_PORT             GPIOx */
/* #define MATRIZ_M1_PIN              GPIO_PIN_x */
/* #define MATRIZ_M2_PORT             GPIOx */
/* #define MATRIZ_M2_PIN              GPIO_PIN_x */
/* #define MATRIZ_M3_PORT             GPIOx */
/* #define MATRIZ_M3_PIN              GPIO_PIN_x */

/* Configurações da Matriz de Teclado 3x4 com ADC */
/* Linhas (Outputs - Scan) */
#define KEYPAD_ROW_PORT             GPIOD
#define KEYPAD_ROW1_PIN             GPIO_PIN_3
#define KEYPAD_ROW2_PIN             GPIO_PIN_4
#define KEYPAD_ROW3_PIN             GPIO_PIN_5

/* Colunas (Inputs Analógicos - ADC) */
/* Usando 4 canais ADC para detectar níveis de tensão diferentes */
#define KEYPAD_COL1_PORT            GPIOB
#define KEYPAD_COL1_PIN             GPIO_PIN_0
#define KEYPAD_COL1_ADC_CHANNEL     ADC1_CHANNEL_0  /* AIN0 */

#define KEYPAD_COL2_PORT            GPIOB
#define KEYPAD_COL2_PIN             GPIO_PIN_1
#define KEYPAD_COL2_ADC_CHANNEL     ADC1_CHANNEL_1  /* AIN1 */

#define KEYPAD_COL3_PORT            GPIOB
#define KEYPAD_COL3_PIN             GPIO_PIN_2
#define KEYPAD_COL3_ADC_CHANNEL     ADC1_CHANNEL_2  /* AIN2 */

#define KEYPAD_COL4_PORT            GPIOB
#define KEYPAD_COL4_PIN             GPIO_PIN_3
#define KEYPAD_COL4_ADC_CHANNEL     ADC1_CHANNEL_3  /* AIN3 */

/* Configurações do ADC para Matriz de Teclado */
#define KEYPAD_ADC_RESOLUTION       1024    /* Resolução 10-bit */
#define KEYPAD_ADC_VREF_MV          5000    /* Tensão de referência 5V (em mV) */
#define KEYPAD_ADC_TOLERANCE        50      /* Tolerância para detecção (em unidades ADC) */

/* Mapeamento das Teclas (3 linhas x 4 colunas = 12 teclas) */
/*
 *  Layout da Matriz:
 *      COL1  COL2  COL3  COL4
 * ROW1  1     2     3     A
 * ROW2  4     5     6     B
 * ROW3  7     8     9     C
 * 
 * Pode ser remapeado conforme necessidade
 */
#define KEY_1       0x01
#define KEY_2       0x02
#define KEY_3       0x03
#define KEY_A       0x0A
#define KEY_4       0x04
#define KEY_5       0x05
#define KEY_6       0x06
#define KEY_B       0x0B
#define KEY_7       0x07
#define KEY_8       0x08
#define KEY_9       0x09
#define KEY_C       0x0C
#define KEY_NONE    0x00

#define KEYPAD_DEBOUNCE_TIME    50  /* ms - tempo de debounce */
#define KEYPAD_ROWS             3
#define KEYPAD_COLS             4

/* Tabela de Níveis ADC para Detecção de Teclas
 * Cada coluna pode ter até 4 teclas em paralelo com resistores diferentes
 * Os valores representam níveis ADC esperados para cada tecla (0-1023)
 * 
 * Exemplo de circuito com resistores:
 *   VCC (5V) ----[R1]----+----[Tecla1]----GND
 *                        |
 *                        +----[R2+Tecla2]----GND
 *                        |
 *                        +----[R3+Tecla3]----GND
 *                        |
 *                        +----ADC_PIN
 * 
 * AJUSTE ESTES VALORES CONFORME SEU CIRCUITO!
 */
#define KEYPAD_ADC_LEVELS       4   /* Número máximo de níveis por coluna */

/* Níveis ADC para cada tecla em cada linha/coluna
 * Formato: keypadAdcThresholds[linha][coluna] = valor_ADC_esperado
 */
static const uint16_t keypadAdcThresholds[KEYPAD_ROWS][KEYPAD_COLS] = {
    /* COL1    COL2    COL3    COL4 */
    { 205,    205,    205,    205 },  /* ROW1: Teclas 1, 2, 3, A - ~1V (R1=1K)   */
    { 409,    409,    409,    409 },  /* ROW2: Teclas 4, 5, 6, B - ~2V (R2=2K)   */
    { 614,    614,    614,    614 }   /* ROW3: Teclas 7, 8, 9, C - ~3V (R3=3K)   */
};

/* Configurações de Temporização */
#define WATCHDOG_BLINK_TIME     500  /* ms - pisca a cada 500ms */
#define REFLETOR_TOGGLE_DELAY   200  /* ms - debounce do toggle */
#define RELE_MIN_PULSE_TIME     100  /* ms - tempo mínimo de acionamento */
#define AUTO_MOVEMENT_TIME      30000 /* ms - 30 segundos para VZ e PT */
#define BTN_REFLETOR_MEMORIZAR  10000 /* ms - 10 segundos para memorizar */
#define BTN_REFLETOR_EXECUTAR   5000  /* ms - 5 segundos para executar */
#define REFLETOR_BLINK_LONGO    800   /* ms - piscada longa */
#define REFLETOR_BLINK_RAPIDO   200   /* ms - piscada rápida */

/* Configurações do Encoder Virtual (baseado em tempo) */
#define TEMPO_CURSO_TOTAL_ASSENTO   30000  /* ms - tempo total de curso do assento */
#define TEMPO_CURSO_TOTAL_ENCOSTO   30000  /* ms - tempo total de curso do encosto */
#define POSICAO_MIN                 0      /* Posição mínima (0ms) */
#define POSICAO_MAX_ASSENTO         30000  /* Posição máxima assento (30000ms) */
#define POSICAO_MAX_ENCOSTO         30000  /* Posição máxima encosto (30000ms) */
#define SAVE_INTERVAL               5000   /* ms - Salva na EEPROM a cada 5 segundos */

/* Endereços EEPROM para armazenamento */
#define EEPROM_ADDR_POSICAO_ASSENTO     0x4000  /* Endereço para posição assento */
#define EEPROM_ADDR_POSICAO_ENCOSTO     0x4004  /* Endereço para posição encosto */
#define EEPROM_ADDR_CALIBRADO           0x4008  /* Flag de calibração */
#define EEPROM_ADDR_MEMORIA_ASSENTO     0x4009  /* Posição memorizada assento */
#define EEPROM_ADDR_MEMORIA_ENCOSTO     0x400D  /* Posição memorizada encosto */
#define EEPROM_ADDR_MEMORIA_VALIDA      0x4011  /* Flag de memória válida */
#define EEPROM_MAGIC_NUMBER             0xAB    /* Número mágico para validação */

/* Configurações de Segurança */
#define MAX_CONTINUOUS_TIME     30000   /* 30 segundos máximo */
#define IDLE_TIMEOUT            300000  /* 5 minutos */
#define INTERLOCK_DELAY         100     /* ms entre acionamentos opostos */

/* Macros */
#define nop()               __asm__("nop")
#define enableInterrupts()  __asm__("rim")
#define disableInterrupts() __asm__("sim")

/* ============================================================================
   TIPOS E ENUMERAÇÕES
   ============================================================================ */

typedef enum {
    ERROR_NONE = 0,
    ERROR_EMERGENCY_STOP,
    ERROR_LIMIT_EXCEEDED,
    ERROR_TIMEOUT,
    ERROR_SYSTEM,
    ERROR_INTERLOCK
} ErrorCode_t;

typedef enum {
    RELE_OFF = 0,
    RELE_ON = 1
} ReleState_t;

typedef enum {
    SYSTEM_IDLE = 0,
    SYSTEM_MOVENDO_ASSENTO,
    SYSTEM_MOVENDO_ENCOSTO,
    SYSTEM_VOLTA_ZERO,
    SYSTEM_POSICAO_TRABALHO,
    SYSTEM_MEMORIA_POSICAO,
    SYSTEM_EMERGENCY_STOP,
    SYSTEM_ERROR
} SystemState_t;

typedef struct {
    ReleState_t sobeAssento;
    ReleState_t desceAssento;
    ReleState_t sobeEncosto;
    ReleState_t desceEncosto;
    ReleState_t refletor;
    uint32_t startTime;
    uint32_t lastInterlockTime;
} ReleControl_t;

typedef struct {
    uint32_t posicaoAssento;      /* Posição atual do assento em ms (0-30000) */
    uint32_t posicaoEncosto;      /* Posição atual do encosto em ms (0-30000) */
    uint32_t lastUpdateTime;      /* Último tempo de atualização */
    uint32_t lastSaveTime;        /* Último tempo de salvamento na EEPROM */
    uint8_t calibrado;            /* Flag: sistema foi calibrado? */
    uint8_t movimentoAtivo;       /* Flag: algum movimento ativo? */
} EncoderVirtual_t;

/* ============================================================================
   VARIÁVEIS GLOBAIS
   ============================================================================ */

/* Estado do Sistema */
static SystemState_t systemState = SYSTEM_IDLE;
static uint32_t systemTick = 0;
static uint8_t emergencyStopFlag = 0;

/* Controle dos Relés */
static ReleControl_t reles = {0};

/* Encoder Virtual */
static EncoderVirtual_t encoder = {0};

/* Interface do Usuário */
static uint8_t buttonState = 0;
static uint8_t lastButtonState = 0;
static uint16_t debounceCounter[8] = {0};
static uint8_t refletorState = 0;
static uint8_t lastRefletorButton = 0;
static uint32_t refletorPressStartTime = 0;
static uint8_t refletorLongPressActive = 0;

/* Memória de Posição */
static uint32_t memoriaAssentoPos = 0;
static uint32_t memoriaEncostoPos = 0;
static uint8_t memoriaValida = 0;

/* Matriz de Teclado */
static uint8_t keypadCurrentKey = KEY_NONE;
static uint8_t keypadLastKey = KEY_NONE;
static uint32_t keypadLastPressTime = 0;
static uint8_t keypadPressed = 0;

/* LED Watchdog */
static uint32_t watchdogTimer = 0;
static uint8_t watchdogState = 0;

/* Segurança */
static uint32_t operationStartTime = 0;
static uint8_t autoMovementActive = 0;
static uint8_t vzCompletoCiclo = 0; /* Flag: VZ foi completado? */
static uint8_t emergencyStopActive = 0;
static uint16_t currentReading = 0;

/* ============================================================================
   PROTÓTIPOS DE FUNÇÕES
   ============================================================================ */

/* Sistema Principal */
void System_Init(void);
void System_Process(void);
void Clock_Config(void);
void GPIO_Config(void);
void Timer_Config(void);
void delay_ms(uint16_t ms);

/* Controle de Relés */
void Rele_Init(void);
void Rele_SobeAssento(ReleState_t state);
void Rele_DesceAssento(ReleState_t state);
void Rele_SobeEncosto(ReleState_t state);
void Rele_DesceEncosto(ReleState_t state);
void Rele_Refletor(ReleState_t state);
void Rele_ToggleRefletor(void);
void Rele_StopAll(void);
uint8_t Rele_IsAnyActive(void);
static uint8_t Rele_CheckInterlock(void);

/* Encoder Virtual e Memória */
void Encoder_Init(void);
void Encoder_Update(void);
void Encoder_Calibrar(void);
void Encoder_SaveToEEPROM(void);
void Encoder_LoadFromEEPROM(void);
uint32_t Encoder_GetPosicaoAssento(void);
uint32_t Encoder_GetPosicaoEncosto(void);
uint8_t Encoder_GetPercentualAssento(void);
uint8_t Encoder_GetPercentualEncosto(void);
static void EEPROM_WriteUint32(uint16_t addr, uint32_t value);
static uint32_t EEPROM_ReadUint32(uint16_t addr);

/* Watchdog e Interface */
void Watchdog_Init(void);
void Watchdog_Update(void);
uint8_t UI_ReadButtons(void);
static uint8_t UI_DebounceButton(uint8_t pin, uint8_t index);
void UI_ProcessRefletorLongPress(void);

/* Memória de Posição */
void Memoria_Salvar(void);
void Memoria_Executar(void);
void Memoria_LoadFromEEPROM(void);
void Memoria_SaveToEEPROM(void);
void Refletor_BlinkPattern(uint8_t count, uint16_t duration);

/* Matriz de Teclado com ADC */
void Keypad_Init(void);
void Keypad_ADC_Init(void);
uint16_t Keypad_ReadADC(ADC1_Channel_TypeDef channel);
uint8_t Keypad_Scan(void);
uint8_t Keypad_GetKey(void);
void Keypad_ProcessKey(uint8_t key);
static uint8_t Keypad_AnalyzeADC(uint16_t adcValue, uint8_t row, uint8_t col);

/* Segurança */
void Safety_Init(void);
uint8_t Safety_CheckEmergencyStop(void);
uint8_t Safety_CheckLimitAssentoSup(void);
uint8_t Safety_CheckLimitAssentoInf(void);
uint8_t Safety_CheckLimitEncostoSup(void);
uint8_t Safety_CheckLimitEncostoInf(void);
uint8_t Safety_CheckTimeout(uint32_t currentTime);
void Safety_ResetTimeout(void);
uint8_t Safety_FullCheck(void);

/* ============================================================================
   FUNÇÃO PRINCIPAL
   ============================================================================ */

/**
 * @brief  Função principal
 * @param  None
 * @retval None
 */
void main(void)
{
    /* Inicialização do sistema */
    System_Init();
    
    /* Loop principal */
    while (1)
    {
        /* Incrementa tick do sistema */
        systemTick++;
        
        /* Atualiza watchdog LED */
        Watchdog_Update();
        
        /* Atualiza encoder virtual */
        Encoder_Update();
        
        /* Lê e processa matriz de teclado */
        uint8_t key = Keypad_GetKey();
        if (key != KEY_NONE)
        {
            Keypad_ProcessKey(key);
        }
        
        /* Processa pressionamento longo do refletor */
        UI_ProcessRefletorLongPress();
        
        /* Verifica parada de emergência */
        if (Safety_CheckEmergencyStop())
        {
            systemState = SYSTEM_EMERGENCY_STOP;
            Rele_StopAll();
        }
        
        /* Processa o sistema */
        System_Process();
        
        /* Verifica timeout de segurança */
        if (Rele_IsAnyActive() && Safety_CheckTimeout(systemTick))
        {
            Rele_StopAll();
            systemState = SYSTEM_ERROR;
        }
        
        /* Pequeno delay para não sobrecarregar o sistema */
        delay_ms(10);
    }
}

/* ============================================================================
   IMPLEMENTAÇÃO - SISTEMA PRINCIPAL
   ============================================================================ */

/**
 * @brief  Inicializa o sistema
 * @param  None
 * @retval None
 */
void System_Init(void)
{
    /* Desabilita interrupções durante inicialização */
    disableInterrupts();
    
    /* Configuração do clock */
    Clock_Config();
    
    /* Configuração dos GPIOs */
    GPIO_Config();
    
    /* Configuração dos Timers */
    Timer_Config();
    
    /* Inicializa módulos */
    Rele_Init();
    Watchdog_Init();
    Safety_Init();
    Encoder_Init();
    Keypad_Init();
    Memoria_LoadFromEEPROM();
    
    /* Habilita interrupções */
    enableInterrupts();
    
    /* Delay para estabilização */
    delay_ms(100);
    
    systemState = SYSTEM_IDLE;
}

/**
 * @brief  Processa a máquina de estados do sistema
 * @param  None
 * @retval None
 */
void System_Process(void)
{
    uint8_t buttons = UI_ReadButtons();
    
    switch (systemState)
    {
        case SYSTEM_IDLE:
            /* ========== MOVIMENTO MANUAL - NÃO EXISTE NO HARDWARE ========== */
            /* Botões PB0-PB3 não existem fisicamente */
            
            /* Botão Sobe Assento - NÃO EXISTE */
            /*
            if (buttons & BTN_SOBE_ASSENTO)
            {
                if (!Safety_CheckLimitAssentoSup())
                {
                    Rele_SobeAssento(RELE_ON);
                    systemState = SYSTEM_MOVENDO_ASSENTO;
                    Safety_ResetTimeout();
                }
            }
            */
            
            /* Botão Desce Assento - NÃO EXISTE */
            /*
            else if (buttons & BTN_DESCE_ASSENTO)
            {
                if (!Safety_CheckLimitAssentoInf())
                {
                    Rele_DesceAssento(RELE_ON);
                    systemState = SYSTEM_MOVENDO_ASSENTO;
                    Safety_ResetTimeout();
                }
            }
            */
            
            /* Botão Sobe Encosto - NÃO EXISTE */
            /*
            else if (buttons & BTN_SOBE_ENCOSTO)
            {
                if (!Safety_CheckLimitEncostoSup())
                {
                    Rele_SobeEncosto(RELE_ON);
                    systemState = SYSTEM_MOVENDO_ENCOSTO;
                    Safety_ResetTimeout();
                }
            }
            */
            
            /* Botão Desce Encosto - NÃO EXISTE */
            /*
            else if (buttons & BTN_DESCE_ENCOSTO)
            {
                if (!Safety_CheckLimitEncostoInf())
                {
                    Rele_DesceEncosto(RELE_ON);
                    systemState = SYSTEM_MOVENDO_ENCOSTO;
                    Safety_ResetTimeout();
                }
            }
            */
            
            /* ========== BOTÕES QUE EXISTEM NO HARDWARE ========== */
            
            /* Botão Refletor - agora processado por UI_ProcessRefletorLongPress() */
            
            /* Botão Volta a Zero - Movimento automático por 30 segundos */
            if (buttons & BTN_VOLTA_ZERO)
            {
                systemState = SYSTEM_VOLTA_ZERO;
                operationStartTime = systemTick;
                autoMovementActive = 1;
                vzCompletoCiclo = 0; /* Resetando - VZ iniciando */
                Safety_ResetTimeout();
            }
            
            /* Botão Posição de Trabalho - SÓ funciona se VZ foi completado antes */
            if (buttons & BTN_POSICAO_TRABALHO)
            {
                /* Verifica se VZ foi executado completamente antes */
                if (vzCompletoCiclo)
                {
                    systemState = SYSTEM_POSICAO_TRABALHO;
                    operationStartTime = systemTick;
                    autoMovementActive = 1;
                    Safety_ResetTimeout();
                }
                /* Se VZ não foi feito, ignora o comando PT */
                /* Opcional: pode adicionar bip de erro aqui */
            }
            break;
            
        /* ========== ESTADOS DE MOVIMENTO MANUAL - NÃO USADOS (hardware não tem botões manuais) ========== */
        case SYSTEM_MOVENDO_ASSENTO:
        case SYSTEM_MOVENDO_ENCOSTO:
            /* Esta lógica não será executada pois os botões não existem */
            /* Mantida para compatibilidade de código */
            /*
            // Para quando soltar o botão
            if (!(buttons & (BTN_SOBE_ASSENTO | BTN_DESCE_ASSENTO | BTN_SOBE_ENCOSTO | BTN_DESCE_ENCOSTO)))
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                vzCompletoCiclo = 0; // Movimento manual - requer VZ antes de PT
            }
            
            // Verifica limites durante movimento
            if (Safety_FullCheck())
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                vzCompletoCiclo = 0; // Movimento manual - requer VZ antes de PT
            }
            */
            
            /* Força volta para IDLE se entrar aqui por algum motivo */
            Rele_StopAll();
            systemState = SYSTEM_IDLE;
            break;
            
        case SYSTEM_VOLTA_ZERO:
            /* Movimento automático VZ: Desce Assento + Sobe Encosto por 30 segundos */
            
            /* Verifica se algum botão foi pressionado = EMERGÊNCIA */
            if (buttons != 0)
            {
                Rele_StopAll();
                systemState = SYSTEM_EMERGENCY_STOP;
                autoMovementActive = 0;
                vzCompletoCiclo = 0; /* VZ interrompido - PT bloqueado */
                break;
            }
            
            /* Ativa os relés: Desce Assento + Sobe Encosto */
            Rele_DesceAssento(RELE_ON);
            Rele_SobeEncosto(RELE_ON);
            
            /* Verifica timeout de 30 segundos */
            if ((systemTick - operationStartTime) >= AUTO_MOVEMENT_TIME)
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                autoMovementActive = 0;
                vzCompletoCiclo = 1; /* VZ completou com sucesso - PT liberado! */
                Encoder_Calibrar(); /* Calibra encoder - posição zero */
            }
            
            /* Verifica limites de segurança */
            if (Safety_CheckLimitAssentoInf() || Safety_CheckLimitEncostoSup())
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                autoMovementActive = 0;
                vzCompletoCiclo = 1; /* VZ completou (atingiu limites) - PT liberado! */
                Encoder_Calibrar(); /* Calibra encoder - posição zero */
            }
            break;
            
        case SYSTEM_POSICAO_TRABALHO:
            /* Movimento automático PT: Sobe Assento + Desce Encosto por 30 segundos */
            
            /* Verifica se algum botão foi pressionado = EMERGÊNCIA */
            if (buttons != 0)
            {
                Rele_StopAll();
                systemState = SYSTEM_EMERGENCY_STOP;
                autoMovementActive = 0;
                vzCompletoCiclo = 0; /* PT interrompido - precisa fazer VZ novamente */
                break;
            }
            
            /* Ativa os relés: Sobe Assento + Desce Encosto */
            Rele_SobeAssento(RELE_ON);
            Rele_DesceEncosto(RELE_ON);
            
            /* Verifica timeout de 30 segundos */
            if ((systemTick - operationStartTime) >= AUTO_MOVEMENT_TIME)
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                autoMovementActive = 0;
                vzCompletoCiclo = 0; /* Após PT, precisa fazer VZ novamente antes do próximo PT */
            }
            
            /* Verifica limites de segurança */
            if (Safety_CheckLimitAssentoSup() || Safety_CheckLimitEncostoInf())
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                autoMovementActive = 0;
                vzCompletoCiclo = 0; /* Após PT, precisa fazer VZ novamente */
            }
            break;
            
        case SYSTEM_MEMORIA_POSICAO:
            /* Movimento automático para posição memorizada */
            
            /* Verifica se algum botão foi pressionado = EMERGÊNCIA */
            if (buttons != 0)
            {
                Rele_StopAll();
                systemState = SYSTEM_EMERGENCY_STOP;
                autoMovementActive = 0;
                break;
            }
            
            /* Verifica se já atingiu as posições alvo */
            uint8_t assentoOk = 0;
            uint8_t encostoOk = 0;
            
            /* Controla assento */
            if (encoder.posicaoAssento < memoriaAssentoPos - 500)
            {
                /* Precisa subir */
                Rele_SobeAssento(RELE_ON);
            }
            else if (encoder.posicaoAssento > memoriaAssentoPos + 500)
            {
                /* Precisa descer */
                Rele_DesceAssento(RELE_ON);
            }
            else
            {
                /* Posição OK */
                Rele_SobeAssento(RELE_OFF);
                Rele_DesceAssento(RELE_OFF);
                assentoOk = 1;
            }
            
            /* Controla encosto */
            if (encoder.posicaoEncosto < memoriaEncostoPos - 500)
            {
                /* Precisa subir */
                Rele_SobeEncosto(RELE_ON);
            }
            else if (encoder.posicaoEncosto > memoriaEncostoPos + 500)
            {
                /* Precisa descer */
                Rele_DesceEncosto(RELE_ON);
            }
            else
            {
                /* Posição OK */
                Rele_SobeEncosto(RELE_OFF);
                Rele_DesceEncosto(RELE_OFF);
                encostoOk = 1;
            }
            
            /* Se ambos OK, finaliza movimento */
            if (assentoOk && encostoOk)
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                autoMovementActive = 0;
            }
            
            /* Timeout de segurança (30 segundos) */
            if ((systemTick - operationStartTime) >= AUTO_MOVEMENT_TIME)
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                autoMovementActive = 0;
            }
            
            /* Verifica limites de segurança */
            if (Safety_FullCheck())
            {
                Rele_StopAll();
                systemState = SYSTEM_IDLE;
                autoMovementActive = 0;
            }
            break;
            
        case SYSTEM_EMERGENCY_STOP:
            /* Sistema em parada de emergência */
            Rele_StopAll();
            vzCompletoCiclo = 0; /* Após emergência, requer VZ novamente */
            
            /* Aguarda reset da emergência */
            if (!Safety_CheckEmergencyStop())
            {
                delay_ms(500); /* Debounce */
                if (!Safety_CheckEmergencyStop())
                {
                    systemState = SYSTEM_IDLE;
                    emergencyStopFlag = 0;
                }
            }
            break;
            
        case SYSTEM_ERROR:
            /* Estado de erro - relés desligados */
            Rele_StopAll();
            vzCompletoCiclo = 0; /* Após erro, requer VZ novamente */
            /* Reset automático após 3 segundos */
            if ((systemTick - operationStartTime) > 3000)
            {
                systemState = SYSTEM_IDLE;
            }
            break;
            
        default:
            systemState = SYSTEM_IDLE;
            break;
    }
}

/**
 * @brief  Configura o clock do sistema
 * @param  None
 * @retval None
 */
void Clock_Config(void)
{
    /* Configura HSI (16MHz) como clock source */
    CLK_DeInit();
    
    /* Habilita HSI */
    CLK_HSICmd(ENABLE);
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
    
    /* Configura divisor do clock */
    CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    
    /* Habilita clocks dos periféricos necessários */
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, ENABLE);
}

/**
 * @brief  Configura os pinos GPIO
 * @param  None
 * @retval None
 */
void GPIO_Config(void)
{
    /* ========== SAÍDAS - RELÉS (Ativo HIGH, True Open-Drain) ========== */
    GPIO_Init(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
    GPIO_Init(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
    GPIO_Init(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
    GPIO_Init(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
    GPIO_Init(RELE_REFLETOR_PORT, RELE_REFLETOR_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
    
    /* ========== SAÍDA - LED WATCHDOG ========== */
    GPIO_Init(LED_WATCHDOG_PORT, LED_WATCHDOG_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
    
    /* ========== ENTRADAS - BOTÕES (com pull-up interno) ========== */
    /* NOTA: Apenas 3 botões físicos existem no hardware (PB4, PB5, PB6) */
    /* GPIO_Init(BTN_PORT, BTN_SOBE_ASSENTO_PIN, GPIO_MODE_IN_PU_NO_IT);    // PB0 - NÃO EXISTE */
    /* GPIO_Init(BTN_PORT, BTN_DESCE_ASSENTO_PIN, GPIO_MODE_IN_PU_NO_IT);   // PB1 - NÃO EXISTE */
    /* GPIO_Init(BTN_PORT, BTN_SOBE_ENCOSTO_PIN, GPIO_MODE_IN_PU_NO_IT);    // PB2 - NÃO EXISTE */
    /* GPIO_Init(BTN_PORT, BTN_DESCE_ENCOSTO_PIN, GPIO_MODE_IN_PU_NO_IT);   // PB3 - NÃO EXISTE */
    GPIO_Init(BTN_PORT, BTN_REFLETOR_PIN, GPIO_MODE_IN_PU_NO_IT);          /* PB4 - Refletor */
    GPIO_Init(BTN_PORT, BTN_VOLTA_ZERO_PIN, GPIO_MODE_IN_PU_NO_IT);        /* PB5 - Volta Zero */
    GPIO_Init(BTN_PORT, BTN_POSICAO_TRABALHO_PIN, GPIO_MODE_IN_PU_NO_IT);  /* PB6 - Posição Trabalho */
    /* GPIO_Init(BTN_PORT, BTN_EMERGENCIA_PIN, GPIO_MODE_IN_PU_NO_IT);      // PB7 - NÃO EXISTE */
    
    /* ========== SENSORES DE LIMITE - NÃO EXISTEM NO HARDWARE ========== */
    /* NOTA: Sistema usa ENCODER VIRTUAL POR TEMPO - sem sensores físicos */
    /* GPIO_Init(LIMIT_ASSENTO_SUP_PORT, LIMIT_ASSENTO_SUP_PIN, GPIO_MODE_IN_PU_NO_IT); // PC3 */
    /* GPIO_Init(LIMIT_ASSENTO_INF_PORT, LIMIT_ASSENTO_INF_PIN, GPIO_MODE_IN_PU_NO_IT); // PC4 */
    /* GPIO_Init(LIMIT_ENCOSTO_SUP_PORT, LIMIT_ENCOSTO_SUP_PIN, GPIO_MODE_IN_PU_NO_IT); // PA1 */
    /* GPIO_Init(LIMIT_ENCOSTO_INF_PORT, LIMIT_ENCOSTO_INF_PIN, GPIO_MODE_IN_PU_NO_IT); // PA2 */
    
    /* ========== MATRIZ DE TECLADO 3x4 COM ADC ========== */
    /* Linhas - Saídas digitais (inicialmente HIGH) */
    GPIO_Init(KEYPAD_ROW_PORT, KEYPAD_ROW1_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(KEYPAD_ROW_PORT, KEYPAD_ROW2_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    GPIO_Init(KEYPAD_ROW_PORT, KEYPAD_ROW3_PIN, GPIO_MODE_OUT_PP_HIGH_FAST);
    
    /* Colunas - Entradas analógicas (ADC) - Sem pull-up */
    GPIO_Init(KEYPAD_COL1_PORT, KEYPAD_COL1_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(KEYPAD_COL2_PORT, KEYPAD_COL2_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(KEYPAD_COL3_PORT, KEYPAD_COL3_PIN, GPIO_MODE_IN_FL_NO_IT);
    GPIO_Init(KEYPAD_COL4_PORT, KEYPAD_COL4_PIN, GPIO_MODE_IN_FL_NO_IT);
}

/**
 * @brief  Configura os timers
 * @param  None
 * @retval None
 */
void Timer_Config(void)
{
    /* Timer2 para base de tempo do watchdog */
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 1000); /* 1ms tick @ 16MHz */
    TIM2_Cmd(ENABLE);
}

/**
 * @brief  Configura a UART
 * @param  None
 * @retval None
 */
void UART_Config(void)
{
    /* UART2 para comunicação/debug */
    UART2_DeInit();
    UART2_Init(9600, 
               UART2_WORDLENGTH_8D, 
               UART2_STOPBITS_1, 
               UART2_PARITY_NO,
               UART2_SYNCMODE_CLOCK_DISABLE, 
               UART2_MODE_TXRX_ENABLE);
    UART2_Cmd(ENABLE);
}

/**
 * @brief  Função de delay em milissegundos
 * @param  ms: tempo em milissegundos
 * @retval None
 */
void delay_ms(uint16_t ms)
{
    uint16_t i;
    for (i = 0; i < ms; i++)
    {
        /* Aproximadamente 1ms @ 16MHz */
        uint16_t j;
        for (j = 0; j < 1600; j++)
        {
            nop();
        }
    }
}

/* ============================================================================
   IMPLEMENTAÇÃO - CONTROLE DE RELÉS
   ============================================================================ */

/**
 * @brief  Inicializa o sistema de controle de relés
 * @param  None
 * @retval None
 */
void Rele_Init(void)
{
    /* Garante que todos os relés estão desligados (LOW) */
    GPIO_WriteLow(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN);
    GPIO_WriteLow(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN);
    GPIO_WriteLow(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN);
    GPIO_WriteLow(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN);
    GPIO_WriteLow(RELE_REFLETOR_PORT, RELE_REFLETOR_PIN);
    
    /* Inicializa estrutura de controle */
    reles.sobeAssento = RELE_OFF;
    reles.desceAssento = RELE_OFF;
    reles.sobeEncosto = RELE_OFF;
    reles.desceEncosto = RELE_OFF;
    reles.refletor = RELE_OFF;
    reles.startTime = 0;
    reles.lastInterlockTime = 0;
}

/**
 * @brief  Controla relé Sobe Assento
 * @param  state: RELE_ON ou RELE_OFF
 * @retval None
 */
void Rele_SobeAssento(ReleState_t state)
{
    if (state == RELE_ON)
    {
        /* Verifica interlock - não pode subir e descer ao mesmo tempo */
        if (reles.desceAssento == RELE_ON)
        {
            Rele_DesceAssento(RELE_OFF);
            delay_ms(INTERLOCK_DELAY);
        }
        GPIO_WriteHigh(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN);
        reles.sobeAssento = RELE_ON;
        reles.startTime = systemTick;
    }
    else
    {
        GPIO_WriteLow(RELE_SOBE_ASSENTO_PORT, RELE_SOBE_ASSENTO_PIN);
        reles.sobeAssento = RELE_OFF;
    }
}

/**
 * @brief  Controla relé Desce Assento
 * @param  state: RELE_ON ou RELE_OFF
 * @retval None
 */
void Rele_DesceAssento(ReleState_t state)
{
    if (state == RELE_ON)
    {
        /* Verifica interlock - não pode subir e descer ao mesmo tempo */
        if (reles.sobeAssento == RELE_ON)
        {
            Rele_SobeAssento(RELE_OFF);
            delay_ms(INTERLOCK_DELAY);
        }
        GPIO_WriteHigh(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN);
        reles.desceAssento = RELE_ON;
        reles.startTime = systemTick;
    }
    else
    {
        GPIO_WriteLow(RELE_DESCE_ASSENTO_PORT, RELE_DESCE_ASSENTO_PIN);
        reles.desceAssento = RELE_OFF;
    }
}

/**
 * @brief  Controla relé Sobe Encosto
 * @param  state: RELE_ON ou RELE_OFF
 * @retval None
 */
void Rele_SobeEncosto(ReleState_t state)
{
    if (state == RELE_ON)
    {
        /* Verifica interlock */
        if (reles.desceEncosto == RELE_ON)
        {
            Rele_DesceEncosto(RELE_OFF);
            delay_ms(INTERLOCK_DELAY);
        }
        GPIO_WriteHigh(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN);
        reles.sobeEncosto = RELE_ON;
        reles.startTime = systemTick;
    }
    else
    {
        GPIO_WriteLow(RELE_SOBE_ENCOSTO_PORT, RELE_SOBE_ENCOSTO_PIN);
        reles.sobeEncosto = RELE_OFF;
    }
}

/**
 * @brief  Controla relé Desce Encosto
 * @param  state: RELE_ON ou RELE_OFF
 * @retval None
 */
void Rele_DesceEncosto(ReleState_t state)
{
    if (state == RELE_ON)
    {
        /* Verifica interlock */
        if (reles.sobeEncosto == RELE_ON)
        {
            Rele_SobeEncosto(RELE_OFF);
            delay_ms(INTERLOCK_DELAY);
        }
        GPIO_WriteHigh(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN);
        reles.desceEncosto = RELE_ON;
        reles.startTime = systemTick;
    }
    else
    {
        GPIO_WriteLow(RELE_DESCE_ENCOSTO_PORT, RELE_DESCE_ENCOSTO_PIN);
        reles.desceEncosto = RELE_OFF;
    }
}

/**
 * @brief  Controla relé Refletor (modo direto)
 * @param  state: RELE_ON ou RELE_OFF
 * @retval None
 */
void Rele_Refletor(ReleState_t state)
{
    if (state == RELE_ON)
    {
        GPIO_WriteHigh(RELE_REFLETOR_PORT, RELE_REFLETOR_PIN);
        reles.refletor = RELE_ON;
    }
    else
    {
        GPIO_WriteLow(RELE_REFLETOR_PORT, RELE_REFLETOR_PIN);
        reles.refletor = RELE_OFF;
    }
    refletorState = state;
}

/**
 * @brief  Toggle do refletor (liga/desliga)
 * @param  None
 * @retval None
 */
void Rele_ToggleRefletor(void)
{
    if (refletorState == RELE_OFF)
    {
        Rele_Refletor(RELE_ON);
    }
    else
    {
        Rele_Refletor(RELE_OFF);
    }
}

/**
 * @brief  Para todos os relés
 * @param  None
 * @retval None
 */
void Rele_StopAll(void)
{
    Rele_SobeAssento(RELE_OFF);
    Rele_DesceAssento(RELE_OFF);
    Rele_SobeEncosto(RELE_OFF);
    Rele_DesceEncosto(RELE_OFF);
    /* Refletor não é desligado no StopAll */
}

/**
 * @brief  Verifica se algum relé está ativo
 * @param  None
 * @retval uint8_t: 1 se algum relé está ativo, 0 caso contrário
 */
uint8_t Rele_IsAnyActive(void)
{
    return (reles.sobeAssento || reles.desceAssento || 
            reles.sobeEncosto || reles.desceEncosto);
}

/**
 * @brief  Verifica condições de interlock
 * @param  None
 * @retval uint8_t: 1 se há violação de interlock, 0 caso contrário
 */
static uint8_t Rele_CheckInterlock(void)
{
    /* Não pode ter sobe e desce ativos simultaneamente */
    if ((reles.sobeAssento && reles.desceAssento) ||
        (reles.sobeEncosto && reles.desceEncosto))
    {
        return 1; /* Interlock violado */
    }
    return 0;
}

/* ============================================================================
   IMPLEMENTAÇÃO - WATCHDOG LED
   ============================================================================ */

/**
 * @brief  Inicializa o watchdog LED
 * @param  None
 * @retval None
 */
void Watchdog_Init(void)
{
    watchdogTimer = 0;
    watchdogState = 0;
    GPIO_WriteLow(LED_WATCHDOG_PORT, LED_WATCHDOG_PIN);
}

/**
 * @brief  Atualiza o watchdog LED (pisca periodicamente)
 * @param  None
 * @retval None
 */
void Watchdog_Update(void)
{
    if ((systemTick - watchdogTimer) >= WATCHDOG_BLINK_TIME)
    {
        watchdogTimer = systemTick;
        
        if (watchdogState)
        {
            GPIO_WriteLow(LED_WATCHDOG_PORT, LED_WATCHDOG_PIN);
            watchdogState = 0;
        }
        else
        {
            GPIO_WriteHigh(LED_WATCHDOG_PORT, LED_WATCHDOG_PIN);
            watchdogState = 1;
        }
    }
}

/* ============================================================================
   IMPLEMENTAÇÃO - ENCODER VIRTUAL E MEMÓRIA
   ============================================================================ */

/**
 * @brief  Inicializa o sistema de encoder virtual
 * @param  None
 * @retval None
 */
void Encoder_Init(void)
{
    /* Desbloqueia EEPROM para escrita */
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    
    /* Carrega dados da EEPROM */
    Encoder_LoadFromEEPROM();
    
    /* Inicializa timestamps */
    encoder.lastUpdateTime = systemTick;
    encoder.lastSaveTime = systemTick;
    encoder.movimentoAtivo = 0;
    
    /* Se não calibrado, inicia em posição zero */
    if (!encoder.calibrado)
    {
        encoder.posicaoAssento = 0;
        encoder.posicaoEncosto = 0;
        /* Será calibrado no primeiro ciclo VZ */
    }
}

/**
 * @brief  Atualiza posições do encoder virtual baseado no tempo
 * @param  None
 * @retval None
 */
void Encoder_Update(void)
{
    uint32_t deltaTime = systemTick - encoder.lastUpdateTime;
    encoder.lastUpdateTime = systemTick;
    
    /* Atualiza posição do assento */
    if (reles.sobeAssento == RELE_ON)
    {
        encoder.posicaoAssento += deltaTime;
        if (encoder.posicaoAssento > POSICAO_MAX_ASSENTO)
            encoder.posicaoAssento = POSICAO_MAX_ASSENTO;
        encoder.movimentoAtivo = 1;
    }
    else if (reles.desceAssento == RELE_ON)
    {
        if (encoder.posicaoAssento >= deltaTime)
            encoder.posicaoAssento -= deltaTime;
        else
            encoder.posicaoAssento = 0;
        encoder.movimentoAtivo = 1;
    }
    
    /* Atualiza posição do encosto */
    if (reles.sobeEncosto == RELE_ON)
    {
        encoder.posicaoEncosto += deltaTime;
        if (encoder.posicaoEncosto > POSICAO_MAX_ENCOSTO)
            encoder.posicaoEncosto = POSICAO_MAX_ENCOSTO;
        encoder.movimentoAtivo = 1;
    }
    else if (reles.desceEncosto == RELE_ON)
    {
        if (encoder.posicaoEncosto >= deltaTime)
            encoder.posicaoEncosto -= deltaTime;
        else
            encoder.posicaoEncosto = 0;
        encoder.movimentoAtivo = 1;
    }
    
    /* Se não há movimento, reseta flag */
    if (reles.sobeAssento == RELE_OFF && reles.desceAssento == RELE_OFF &&
        reles.sobeEncosto == RELE_OFF && reles.desceEncosto == RELE_OFF)
    {
        encoder.movimentoAtivo = 0;
    }
    
    /* Salva na EEPROM periodicamente (a cada 5 segundos) */
    if ((systemTick - encoder.lastSaveTime) >= SAVE_INTERVAL)
    {
        Encoder_SaveToEEPROM();
        encoder.lastSaveTime = systemTick;
    }
}

/**
 * @brief  Calibra o sistema (executado no fim do ciclo VZ)
 * @param  None
 * @retval None
 */
void Encoder_Calibrar(void)
{
    /* Após VZ, assume posição zero */
    encoder.posicaoAssento = 0;
    encoder.posicaoEncosto = 0;
    encoder.calibrado = 1;
    
    /* Salva imediatamente */
    Encoder_SaveToEEPROM();
}

/**
 * @brief  Salva posições na EEPROM
 * @param  None
 * @retval None
 */
void Encoder_SaveToEEPROM(void)
{
    /* Salva posição do assento */
    EEPROM_WriteUint32(EEPROM_ADDR_POSICAO_ASSENTO, encoder.posicaoAssento);
    
    /* Salva posição do encosto */
    EEPROM_WriteUint32(EEPROM_ADDR_POSICAO_ENCOSTO, encoder.posicaoEncosto);
    
    /* Salva flag de calibração com número mágico */
    FLASH_ProgramByte(EEPROM_ADDR_CALIBRADO, encoder.calibrado ? EEPROM_MAGIC_NUMBER : 0x00);
}

/**
 * @brief  Carrega posições da EEPROM
 * @param  None
 * @retval None
 */
void Encoder_LoadFromEEPROM(void)
{
    /* Verifica se há dados válidos (número mágico) */
    uint8_t calibFlag = FLASH_ReadByte(EEPROM_ADDR_CALIBRADO);
    
    if (calibFlag == EEPROM_MAGIC_NUMBER)
    {
        /* Dados válidos - carrega posições */
        encoder.posicaoAssento = EEPROM_ReadUint32(EEPROM_ADDR_POSICAO_ASSENTO);
        encoder.posicaoEncosto = EEPROM_ReadUint32(EEPROM_ADDR_POSICAO_ENCOSTO);
        encoder.calibrado = 1;
        
        /* Validação dos limites */
        if (encoder.posicaoAssento > POSICAO_MAX_ASSENTO)
            encoder.posicaoAssento = POSICAO_MAX_ASSENTO;
        if (encoder.posicaoEncosto > POSICAO_MAX_ENCOSTO)
            encoder.posicaoEncosto = POSICAO_MAX_ENCOSTO;
    }
    else
    {
        /* Dados inválidos - inicializa em zero */
        encoder.posicaoAssento = 0;
        encoder.posicaoEncosto = 0;
        encoder.calibrado = 0;
    }
}

/**
 * @brief  Obtém posição atual do assento em ms
 * @param  None
 * @retval uint32_t: posição em ms (0-30000)
 */
uint32_t Encoder_GetPosicaoAssento(void)
{
    return encoder.posicaoAssento;
}

/**
 * @brief  Obtém posição atual do encosto em ms
 * @param  None
 * @retval uint32_t: posição em ms (0-30000)
 */
uint32_t Encoder_GetPosicaoEncosto(void)
{
    return encoder.posicaoEncosto;
}

/**
 * @brief  Obtém posição do assento em percentual
 * @param  None
 * @retval uint8_t: posição em % (0-100)
 */
uint8_t Encoder_GetPercentualAssento(void)
{
    return (uint8_t)((encoder.posicaoAssento * 100UL) / POSICAO_MAX_ASSENTO);
}

/**
 * @brief  Obtém posição do encosto em percentual
 * @param  None
 * @retval uint8_t: posição em % (0-100)
 */
uint8_t Encoder_GetPercentualEncosto(void)
{
    return (uint8_t)((encoder.posicaoEncosto * 100UL) / POSICAO_MAX_ENCOSTO);
}

/**
 * @brief  Escreve uint32 na EEPROM (4 bytes)
 * @param  addr: endereço base
 * @param  value: valor a ser escrito
 * @retval None
 */
static void EEPROM_WriteUint32(uint16_t addr, uint32_t value)
{
    FLASH_ProgramByte(addr + 0, (uint8_t)(value >> 24));
    FLASH_ProgramByte(addr + 1, (uint8_t)(value >> 16));
    FLASH_ProgramByte(addr + 2, (uint8_t)(value >> 8));
    FLASH_ProgramByte(addr + 3, (uint8_t)(value));
}

/**
 * @brief  Lê uint32 da EEPROM (4 bytes)
 * @param  addr: endereço base
 * @retval uint32_t: valor lido
 */
static uint32_t EEPROM_ReadUint32(uint16_t addr)
{
    uint32_t value = 0;
    value |= ((uint32_t)FLASH_ReadByte(addr + 0)) << 24;
    value |= ((uint32_t)FLASH_ReadByte(addr + 1)) << 16;
    value |= ((uint32_t)FLASH_ReadByte(addr + 2)) << 8;
    value |= ((uint32_t)FLASH_ReadByte(addr + 3));
    return value;
}

/* ============================================================================
   IMPLEMENTAÇÃO - INTERFACE DO USUÁRIO (BOTÕES)
   ============================================================================ */

/**
 * @brief  Lê o estado dos botões com debounce
 * @param  None
 * @retval uint8_t: máscara com o estado dos botões
 */
uint8_t UI_ReadButtons(void)
{
    uint8_t buttons = 0;
    
    /* Lê cada botão com debounce (botões ativos em LOW) */
    /* NOTA: Apenas 3 botões físicos existem (PB4, PB5, PB6) */
    
    /* Botões de movimento manual - NÃO EXISTEM NO HARDWARE */
    /* if (UI_DebounceButton(BTN_SOBE_ASSENTO_PIN, 0))
        buttons |= BTN_SOBE_ASSENTO; */
        
    /* if (UI_DebounceButton(BTN_DESCE_ASSENTO_PIN, 1))
        buttons |= BTN_DESCE_ASSENTO; */
        
    /* if (UI_DebounceButton(BTN_SOBE_ENCOSTO_PIN, 2))
        buttons |= BTN_SOBE_ENCOSTO; */
        
    /* if (UI_DebounceButton(BTN_DESCE_ENCOSTO_PIN, 3))
        buttons |= BTN_DESCE_ENCOSTO; */
        
    /* Botões que existem fisicamente */
    if (UI_DebounceButton(BTN_REFLETOR_PIN, 4))
        buttons |= BTN_REFLETOR;
        
    if (UI_DebounceButton(BTN_VOLTA_ZERO_PIN, 5))
        buttons |= BTN_VOLTA_ZERO;
        
    if (UI_DebounceButton(BTN_POSICAO_TRABALHO_PIN, 6))
        buttons |= BTN_POSICAO_TRABALHO;
    
    /* Botão de emergência - NÃO EXISTE NO HARDWARE */
    /* Emergência = Qualquer botão durante VZ ou PT */
    /* if (UI_DebounceButton(BTN_EMERGENCIA_PIN, 7))
        buttons |= BTN_EMERGENCIA; */
    
    return buttons;
}

/**
 * @brief  Debounce de um botão individual
 * @param  pin: pino do botão
 * @param  index: índice do contador de debounce
 * @retval uint8_t: 1 se botão está pressionado, 0 caso contrário
 */
static uint8_t UI_DebounceButton(uint8_t pin, uint8_t index)
{
    /* Lê estado do botão (ativo em LOW devido ao pull-up) */
    uint8_t buttonPressed = !GPIO_ReadInputPin(BTN_PORT, pin);
    
    if (buttonPressed)
    {
        if (debounceCounter[index] < BTN_DEBOUNCE_TIME)
        {
            debounceCounter[index]++;
        }
        
        if (debounceCounter[index] >= BTN_DEBOUNCE_TIME)
        {
            return 1; /* Botão confirmado como pressionado */
        }
    }
    else
    {
        debounceCounter[index] = 0;
    }
    
    return 0;
}

/* ============================================================================
   IMPLEMENTAÇÃO - SEGURANÇA
   ============================================================================ */

/**
 * @brief  Inicializa o módulo de segurança
 * @param  None
 * @retval None
 */
void Safety_Init(void)
{
    /* Inicializa variáveis */
    operationStartTime = 0;
    emergencyStopActive = 0;
}

/**
 * @brief  Verifica se o botão de emergência está acionado
 * @param  None
 * @retval uint8_t: 1 se emergência ativa, 0 caso contrário
 * @note   BOTÃO DE EMERGÊNCIA NÃO EXISTE NO HARDWARE
 *         Emergência = Qualquer botão pressionado durante VZ ou PT
 */
uint8_t Safety_CheckEmergencyStop(void)
{
    /* Botão físico de emergência não existe */
    /* uint8_t emergencyPin = !GPIO_ReadInputPin(BTN_PORT, BTN_EMERGENCIA_PIN); */
    
    /* Emergência é tratada na lógica de PT e VZ quando qualquer botão é pressionado */
    /* Esta função mantida para compatibilidade - sempre retorna 0 */
    return 0;
}

/**
 * @brief  Verifica sensor limite superior do assento
 * @param  None
 * @retval uint8_t: 1 se limite atingido, 0 caso contrário
 * @note   SENSORES DE LIMITE NÃO EXISTEM NO HARDWARE
 *         Sistema usa ENCODER VIRTUAL POR TEMPO
 */
uint8_t Safety_CheckLimitAssentoSup(void)
{
    /* Sensor físico não existe - retorna sempre 0 */
    /* return (!GPIO_ReadInputPin(LIMIT_ASSENTO_SUP_PORT, LIMIT_ASSENTO_SUP_PIN)); */
    return 0; /* Limite controlado por tempo (encoder virtual) */
}

/**
 * @brief  Verifica sensor limite inferior do assento
 * @param  None
 * @retval uint8_t: 1 se limite atingido, 0 caso contrário
 * @note   SENSORES DE LIMITE NÃO EXISTEM NO HARDWARE
 *         Sistema usa ENCODER VIRTUAL POR TEMPO
 */
uint8_t Safety_CheckLimitAssentoInf(void)
{
    /* Sensor físico não existe - retorna sempre 0 */
    /* return (!GPIO_ReadInputPin(LIMIT_ASSENTO_INF_PORT, LIMIT_ASSENTO_INF_PIN)); */
    return 0; /* Limite controlado por tempo (encoder virtual) */
}

/**
 * @brief  Verifica sensor limite superior do encosto
 * @param  None
 * @retval uint8_t: 1 se limite atingido, 0 caso contrário
 * @note   SENSORES DE LIMITE NÃO EXISTEM NO HARDWARE
 *         Sistema usa ENCODER VIRTUAL POR TEMPO
 */
uint8_t Safety_CheckLimitEncostoSup(void)
{
    /* Sensor físico não existe - retorna sempre 0 */
    /* return (!GPIO_ReadInputPin(LIMIT_ENCOSTO_SUP_PORT, LIMIT_ENCOSTO_SUP_PIN)); */
    return 0; /* Limite controlado por tempo (encoder virtual) */
}

/**
 * @brief  Verifica sensor limite inferior do encosto
 * @param  None
 * @retval uint8_t: 1 se limite atingido, 0 caso contrário
 * @note   SENSORES DE LIMITE NÃO EXISTEM NO HARDWARE
 *         Sistema usa ENCODER VIRTUAL POR TEMPO
 */
uint8_t Safety_CheckLimitEncostoInf(void)
{
    /* Sensor físico não existe - retorna sempre 0 */
    /* return (!GPIO_ReadInputPin(LIMIT_ENCOSTO_INF_PORT, LIMIT_ENCOSTO_INF_PIN)); */
    return 0; /* Limite controlado por tempo (encoder virtual) */
}

/**
 * @brief  Verifica timeout de operação contínua
 * @param  currentTime: tempo atual do sistema
 * @retval uint8_t: 1 se timeout, 0 caso contrário
 */
uint8_t Safety_CheckTimeout(uint32_t currentTime)
{
    if (reles.startTime == 0)
        return 0;
    
    /* Verifica se excedeu tempo máximo de operação contínua */
    if ((currentTime - reles.startTime) > MAX_CONTINUOUS_TIME)
    {
        return 1; /* Timeout! */
    }
    
    return 0;
}

/**
 * @brief  Reseta o contador de timeout
 * @param  None
 * @retval None
 */
void Safety_ResetTimeout(void)
{
    operationStartTime = systemTick;
}

/**
 * @brief  Verifica todas as condições de segurança
 * @param  None
 * @retval uint8_t: 1 se algum problema detectado, 0 se tudo ok
 */
uint8_t Safety_FullCheck(void)
{
    /* Verifica emergência */
    if (Safety_CheckEmergencyStop())
        return 1;
    
    /* Verifica limites baseado no movimento atual */
    if (reles.sobeAssento && Safety_CheckLimitAssentoSup())
        return 1;
    
    if (reles.desceAssento && Safety_CheckLimitAssentoInf())
        return 1;
    
    if (reles.sobeEncosto && Safety_CheckLimitEncostoSup())
        return 1;
    
    if (reles.desceEncosto && Safety_CheckLimitEncostoInf())
        return 1;
    
    /* Verifica interlock */
    if (Rele_CheckInterlock())
        return 1;
    
    return 0; /* Tudo ok */
}

/* ============================================================================
   IMPLEMENTAÇÃO - MEMÓRIA DE POSIÇÃO COM BOTÃO REFLETOR
   ============================================================================ */

/**
 * @brief  Processa pressionamento longo do botão refletor
 * @param  None
 * @retval None
 */
void UI_ProcessRefletorLongPress(void)
{
    uint8_t refletorPressed = !GPIO_ReadInputPin(BTN_PORT, BTN_REFLETOR_PIN);
    
    /* Botão foi pressionado */
    if (refletorPressed)
    {
        /* Inicia contagem se ainda não estava pressionado */
        if (!refletorLongPressActive)
        {
            refletorPressStartTime = systemTick;
            refletorLongPressActive = 1;
        }
        
        uint32_t pressDuration = systemTick - refletorPressStartTime;
        
        /* 10 segundos - MEMORIZA posição */
        if (pressDuration >= BTN_REFLETOR_MEMORIZAR)
        {
            /* Memoriza apenas uma vez */
            static uint8_t memorizado = 0;
            if (!memorizado)
            {
                Memoria_Salvar();
                Refletor_BlinkPattern(1, REFLETOR_BLINK_LONGO);
                memorizado = 1;
            }
        }
    }
    else
    {
        /* Botão foi solto */
        if (refletorLongPressActive)
        {
            uint32_t pressDuration = systemTick - refletorPressStartTime;
            
            /* Entre 5 e 10 segundos - EXECUTA posição memorizada */
            if (pressDuration >= BTN_REFLETOR_EXECUTAR && pressDuration < BTN_REFLETOR_MEMORIZAR)
            {
                if (memoriaValida)
                {
                    Memoria_Executar();
                    Refletor_BlinkPattern(2, REFLETOR_BLINK_RAPIDO);
                }
            }
            /* Menos de 5 segundos - Toggle normal do refletor */
            else if (pressDuration < BTN_REFLETOR_EXECUTAR)
            {
                Rele_ToggleRefletor();
            }
            
            refletorLongPressActive = 0;
            refletorPressStartTime = 0;
        }
    }
}

/**
 * @brief  Salva posição atual na memória
 * @param  None
 * @retval None
 */
void Memoria_Salvar(void)
{
    /* Salva posições atuais */
    memoriaAssentoPos = encoder.posicaoAssento;
    memoriaEncostoPos = encoder.posicaoEncosto;
    memoriaValida = 1;
    
    /* Salva na EEPROM */
    Memoria_SaveToEEPROM();
}

/**
 * @brief  Executa movimento para posição memorizada
 * @param  None
 * @retval None
 */
void Memoria_Executar(void)
{
    if (memoriaValida && systemState == SYSTEM_IDLE)
    {
        systemState = SYSTEM_MEMORIA_POSICAO;
        operationStartTime = systemTick;
        autoMovementActive = 1;
        Safety_ResetTimeout();
    }
}

/**
 * @brief  Carrega posição memorizada da EEPROM
 * @param  None
 * @retval None
 */
void Memoria_LoadFromEEPROM(void)
{
    /* Verifica se há memória válida */
    uint8_t validFlag = FLASH_ReadByte(EEPROM_ADDR_MEMORIA_VALIDA);
    
    if (validFlag == EEPROM_MAGIC_NUMBER)
    {
        /* Carrega posições memorizadas */
        memoriaAssentoPos = EEPROM_ReadUint32(EEPROM_ADDR_MEMORIA_ASSENTO);
        memoriaEncostoPos = EEPROM_ReadUint32(EEPROM_ADDR_MEMORIA_ENCOSTO);
        
        /* Validação dos limites */
        if (memoriaAssentoPos <= POSICAO_MAX_ASSENTO && 
            memoriaEncostoPos <= POSICAO_MAX_ENCOSTO)
        {
            memoriaValida = 1;
        }
        else
        {
            memoriaValida = 0;
        }
    }
    else
    {
        memoriaValida = 0;
        memoriaAssentoPos = 0;
        memoriaEncostoPos = 0;
    }
}

/**
 * @brief  Salva posição memorizada na EEPROM
 * @param  None
 * @retval None
 */
void Memoria_SaveToEEPROM(void)
{
    /* Salva posições */
    EEPROM_WriteUint32(EEPROM_ADDR_MEMORIA_ASSENTO, memoriaAssentoPos);
    EEPROM_WriteUint32(EEPROM_ADDR_MEMORIA_ENCOSTO, memoriaEncostoPos);
    
    /* Salva flag de validade */
    FLASH_ProgramByte(EEPROM_ADDR_MEMORIA_VALIDA, EEPROM_MAGIC_NUMBER);
}

/**
 * @brief  Pisca o refletor em padrão específico
 * @param  count: número de piscadas
 * @param  duration: duração de cada piscada em ms
 * @retval None
 */
void Refletor_BlinkPattern(uint8_t count, uint16_t duration)
{
    uint8_t originalState = refletorState;
    uint8_t i;
    
    for (i = 0; i < count; i++)
    {
        /* Liga */
        Rele_Refletor(RELE_ON);
        delay_ms(duration);
        
        /* Desliga */
        Rele_Refletor(RELE_OFF);
        delay_ms(duration);
    }
    
    /* Restaura estado original */
    Rele_Refletor(originalState);
}

/* ============================================================================
   IMPLEMENTAÇÃO - MATRIZ DE TECLADO 3x4
   ============================================================================ */

/**
 * @brief  Tabela de mapeamento da matriz de teclado
 *         [linha][coluna] = código da tecla
 */
static const uint8_t keypadMap[KEYPAD_ROWS][KEYPAD_COLS] = {
    {KEY_1, KEY_2, KEY_3, KEY_A},  /* Linha 1 */
    {KEY_4, KEY_5, KEY_6, KEY_B},  /* Linha 2 */
    {KEY_7, KEY_8, KEY_9, KEY_C}   /* Linha 3 */
};

/**
 * @brief  Inicializa a matriz de teclado
 * @param  None
 * @retval None
 */
void Keypad_Init(void)
{
    /* Inicializa ADC para leitura das colunas */
    Keypad_ADC_Init();
    
    /* Inicializa variáveis */
    keypadCurrentKey = KEY_NONE;
    keypadLastKey = KEY_NONE;
    keypadLastPressTime = 0;
    keypadPressed = 0;
}

/**
 * @brief  Inicializa o ADC para leitura das colunas da matriz
 * @param  None
 * @retval None
 */
void Keypad_ADC_Init(void)
{
    /* Desliga o ADC antes de configurar */
    ADC1_DeInit();
    
    /* Configura o ADC:
     * - Prescaler = 18 (Fadc = Fcpu/18 = 16MHz/18 = 888kHz)
     * - Modo de conversão única
     * - Alinhamento à direita (10 bits)
     * - Schmitt trigger desabilitado nos pinos ADC
     */
    ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
              ADC1_CHANNEL_0,              /* Canal será selecionado dinamicamente */
              ADC1_PRESSEL_FCPU_D18,       /* Prescaler /18 */
              ADC1_EXTTRIG_TIM,            /* Trigger externo (não usado) */
              DISABLE,                      /* Trigger externo desabilitado */
              ADC1_ALIGN_RIGHT,            /* Alinhamento à direita */
              ADC1_SCHMITTTRIG_CHANNEL0 | ADC1_SCHMITTTRIG_CHANNEL1 |
              ADC1_SCHMITTTRIG_CHANNEL2 | ADC1_SCHMITTTRIG_CHANNEL3,
              DISABLE);                     /* Schmitt trigger desabilitado */
    
    /* Habilita o ADC */
    ADC1_Cmd(ENABLE);
}

/**
 * @brief  Lê valor ADC de um canal específico
 * @param  channel: Canal ADC a ser lido
 * @retval uint16_t: Valor ADC (0-1023)
 */
uint16_t Keypad_ReadADC(ADC1_Channel_TypeDef channel)
{
    uint16_t adcValue = 0;
    
    /* Seleciona o canal */
    ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE, channel, ADC1_ALIGN_RIGHT);
    
    /* Inicia conversão */
    ADC1_StartConversion();
    
    /* Aguarda conversão completar */
    while (ADC1_GetFlagStatus(ADC1_FLAG_EOC) == RESET);
    
    /* Lê o valor convertido (10 bits) */
    adcValue = ADC1_GetConversionValue();
    
    /* Limpa flag de conversão completa */
    ADC1_ClearFlag(ADC1_FLAG_EOC);
    
    return adcValue;
}

/**
 * @brief  Analisa valor ADC e determina se corresponde a uma tecla
 * @param  adcValue: Valor ADC lido
 * @param  row: Linha atual sendo scaneada
 * @param  col: Coluna sendo analisada
 * @retval uint8_t: 1 se tecla detectada, 0 caso contrário
 */
static uint8_t Keypad_AnalyzeADC(uint16_t adcValue, uint8_t row, uint8_t col)
{
    uint16_t threshold = keypadAdcThresholds[row][col];
    
    /* Verifica se o valor ADC está dentro da tolerância do threshold */
    if ((adcValue >= (threshold - KEYPAD_ADC_TOLERANCE)) && 
        (adcValue <= (threshold + KEYPAD_ADC_TOLERANCE)))
    {
        return 1;  /* Tecla detectada */
    }
    
    return 0;  /* Nenhuma tecla detectada */
}

/**
 * @brief  Faz scan da matriz de teclado usando ADC
 * @param  None
 * @retval uint8_t: código da tecla pressionada ou KEY_NONE
 * 
 * @note   Funcionamento:
 *         1. Para cada linha, ativa LOW (GND)
 *         2. Lê valores ADC das 4 colunas
 *         3. Compara com thresholds para identificar tecla
 *         4. Múltiplas chaves por coluna são diferenciadas por resistores
 */
uint8_t Keypad_Scan(void)
{
    uint8_t row, col;
    uint16_t adcValue;
    ADC1_Channel_TypeDef adcChannels[KEYPAD_COLS] = {
        KEYPAD_COL1_ADC_CHANNEL,
        KEYPAD_COL2_ADC_CHANNEL,
        KEYPAD_COL3_ADC_CHANNEL,
        KEYPAD_COL4_ADC_CHANNEL
    };
    
    /* Scan cada linha */
    for (row = 0; row < KEYPAD_ROWS; row++)
    {
        /* Coloca todas as linhas em HIGH (5V) */
        GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW1_PIN);
        GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW2_PIN);
        GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW3_PIN);
        
        /* Coloca linha atual em LOW (GND) */
        switch (row)
        {
            case 0:
                GPIO_WriteLow(KEYPAD_ROW_PORT, KEYPAD_ROW1_PIN);
                break;
            case 1:
                GPIO_WriteLow(KEYPAD_ROW_PORT, KEYPAD_ROW2_PIN);
                break;
            case 2:
                GPIO_WriteLow(KEYPAD_ROW_PORT, KEYPAD_ROW3_PIN);
                break;
        }
        
        /* Delay para estabilização do circuito (importante para ADC) */
        delay_ms(2);  /* 2ms para estabilização */
        
        /* Lê valores ADC de cada coluna */
        for (col = 0; col < KEYPAD_COLS; col++)
        {
            /* Lê valor ADC da coluna */
            adcValue = Keypad_ReadADC(adcChannels[col]);
            
            /* Analisa se o valor corresponde a uma tecla pressionada */
            if (Keypad_AnalyzeADC(adcValue, row, col))
            {
                /* Tecla encontrada - restaura linhas e retorna */
                GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW1_PIN);
                GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW2_PIN);
                GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW3_PIN);
                
                return keypadMap[row][col];
            }
        }
    }
    
    /* Restaura todas as linhas em HIGH */
    GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW1_PIN);
    GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW2_PIN);
    GPIO_WriteHigh(KEYPAD_ROW_PORT, KEYPAD_ROW3_PIN);
    
    return KEY_NONE;
}

/**
 * @brief  Obtém tecla pressionada com debounce
 * @param  None
 * @retval uint8_t: código da tecla ou KEY_NONE
 */
uint8_t Keypad_GetKey(void)
{
    uint8_t key = Keypad_Scan();
    
    /* Se nenhuma tecla pressionada */
    if (key == KEY_NONE)
    {
        keypadPressed = 0;
        keypadCurrentKey = KEY_NONE;
        return KEY_NONE;
    }
    
    /* Verifica se é a mesma tecla */
    if (key == keypadCurrentKey)
    {
        /* Verifica debounce */
        if (!keypadPressed && (systemTick - keypadLastPressTime) >= KEYPAD_DEBOUNCE_TIME)
        {
            keypadPressed = 1;
            keypadLastKey = key;
            return key;
        }
    }
    else
    {
        /* Nova tecla detectada */
        keypadCurrentKey = key;
        keypadLastPressTime = systemTick;
        keypadPressed = 0;
    }
    
    return KEY_NONE;
}

/**
 * @brief  Processa tecla pressionada
 * @param  key: código da tecla
 * @retval None
 */
void Keypad_ProcessKey(uint8_t key)
{
    /* Processa apenas no estado IDLE para segurança */
    if (systemState != SYSTEM_IDLE)
        return;
    
    switch (key)
    {
        case KEY_1:
            /* Tecla 1 - Sobe Assento */
            if (!Safety_CheckLimitAssentoSup())
            {
                Rele_SobeAssento(RELE_ON);
                delay_ms(100);  /* Pulso curto */
                Rele_SobeAssento(RELE_OFF);
            }
            break;
            
        case KEY_2:
            /* Tecla 2 - Desce Assento */
            if (!Safety_CheckLimitAssentoInf())
            {
                Rele_DesceAssento(RELE_ON);
                delay_ms(100);
                Rele_DesceAssento(RELE_OFF);
            }
            break;
            
        case KEY_3:
            /* Tecla 3 - Sobe Encosto */
            if (!Safety_CheckLimitEncostoSup())
            {
                Rele_SobeEncosto(RELE_ON);
                delay_ms(100);
                Rele_SobeEncosto(RELE_OFF);
            }
            break;
            
        case KEY_4:
            /* Tecla 4 - Desce Encosto */
            if (!Safety_CheckLimitEncostoInf())
            {
                Rele_DesceEncosto(RELE_ON);
                delay_ms(100);
                Rele_DesceEncosto(RELE_OFF);
            }
            break;
            
        case KEY_5:
            /* Tecla 5 - Toggle Refletor */
            Rele_ToggleRefletor();
            break;
            
        case KEY_6:
            /* Tecla 6 - Volta a Zero */
            systemState = SYSTEM_VOLTA_ZERO;
            operationStartTime = systemTick;
            autoMovementActive = 1;
            vzCompletoCiclo = 0;
            Safety_ResetTimeout();
            break;
            
        case KEY_7:
            /* Tecla 7 - Posição de Trabalho */
            if (vzCompletoCiclo)
            {
                systemState = SYSTEM_POSICAO_TRABALHO;
                operationStartTime = systemTick;
                autoMovementActive = 1;
                Safety_ResetTimeout();
            }
            break;
            
        case KEY_8:
            /* Tecla 8 - Salvar Memória */
            Memoria_Salvar();
            Refletor_BlinkPattern(1, REFLETOR_BLINK_LONGO);
            break;
            
        case KEY_9:
            /* Tecla 9 - Executar Memória */
            if (memoriaValida)
            {
                Memoria_Executar();
                Refletor_BlinkPattern(2, REFLETOR_BLINK_RAPIDO);
            }
            break;
            
        case KEY_A:
            /* Tecla A - Função customizável */
            /* TODO: Definir função */
            break;
            
        case KEY_B:
            /* Tecla B - Função customizável */
            /* TODO: Definir função */
            break;
            
        case KEY_C:
            /* Tecla C - Parada de Emergência */
            systemState = SYSTEM_EMERGENCY_STOP;
            Rele_StopAll();
            break;
            
        default:
            break;
    }
}

/* ============================================================================
   FIM DO ARQUIVO
   ============================================================================ */

/************************ (C) COPYRIGHT Lazaro 2026 *****END OF FILE****/
