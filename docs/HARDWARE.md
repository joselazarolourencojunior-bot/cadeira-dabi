# Documentação de Hardware - Cadeira Odontológica

## Especificações do Microcontrolador STM8S105K4T6C

### Características Principais
- **Família**: STM8S (8-bit)
- **Core**: STM8 CPU
- **Frequência Máxima**: 16 MHz
- **Memória Flash**: 16 KB
- **RAM**: 2 KB
- **EEPROM**: 1 KB
- **Package**: LQFP32

### Periféricos Disponíveis
- **Timers**: 
  - 1x TIM1 (16-bit avançado)
  - 1x TIM2 (16-bit uso geral)
  - 1x TIM4 (8-bit básico)
- **ADC**: 1x 10-bit (10 canais)
- **UART**: 2x full-duplex
- **SPI**: 1x Master/Slave
- **I²C**: 1x Master/Slave
- **GPIOs**: 32 pinos I/O
- **Watchdog**: Independente e de janela

## Pinout do Projeto

### PORTA A (Controle de Motores - PWM)
| Pino | Função            | Tipo   | Descrição                      |
|------|-------------------|--------|--------------------------------|
| PA1  | MOTOR_UP          | Output | Controle motor elevação UP     |
| PA2  | MOTOR_DOWN        | Output | Controle motor elevação DOWN   |
| PA3  | MOTOR_BACKREST    | Output | Controle motor encosto         |
| PA4  | MOTOR_LEGREST     | Output | Controle motor apoio pernas    |

### PORTA B (Botões de Controle)
| Pino | Função            | Tipo   | Descrição                      |
|------|-------------------|--------|--------------------------------|
| PB0  | BTN_UP            | Input  | Botão subir cadeira            |
| PB1  | BTN_DOWN          | Input  | Botão descer cadeira           |
| PB2  | BTN_BACKREST_UP   | Input  | Botão encosto para cima        |
| PB3  | BTN_BACKREST_DOWN | Input  | Botão encosto para baixo       |
| PB4  | BTN_LEGREST_UP    | Input  | Botão pernas para cima         |
| PB5  | BTN_LEGREST_DOWN  | Input  | Botão pernas para baixo        |
| PB7  | BTN_RESET         | Input  | Botão de reset                 |

### PORTA C (Sensores de Limite)
| Pino | Função              | Tipo   | Descrição                    |
|------|---------------------|--------|------------------------------|
| PC2  | EMERGENCY_STOP      | Input  | Botão parada emergência      |
| PC3  | LIMIT_UP            | Input  | Fim de curso superior        |
| PC4  | LIMIT_DOWN          | Input  | Fim de curso inferior        |
| PC5  | LIMIT_BACKREST_UP   | Input  | Fim de curso encosto cima    |
| PC6  | LIMIT_BACKREST_DOWN | Input  | Fim de curso encosto baixo   |
| PC7  | LIMIT_LEGREST_UP    | Input  | Fim de curso pernas cima     |

### PORTA D (LEDs e UART)
| Pino | Função       | Tipo   | Descrição                        |
|------|--------------|--------|----------------------------------|
| PD0  | LED_STATUS   | Output | LED indicador de status          |
| PD1  | LED_ERROR    | Output | LED indicador de erro            |
| PD5  | UART2_TX     | Output | Transmissão UART                 |
| PD6  | UART2_RX     | Input  | Recepção UART                    |

### PORTA E (Buzzer e outros)
| Pino | Função       | Tipo   | Descrição                        |
|------|--------------|--------|----------------------------------|
| PE0  | BUZZER       | Output | Buzzer para feedback sonoro      |

### Canais ADC
| Canal | Pino | Função          | Descrição                    |
|-------|------|-----------------|------------------------------|
| AIN3  | PD2  | CURRENT_SENSE   | Sensor de corrente motores   |

## Lista de Componentes Recomendados

### Drivers de Motor
- **Driver H-Bridge**: L298N ou BTS7960 (para cada motor)
- **Capacitores de filtro**: 100nF cerâmico + 100uF eletrolítico por motor
- **Diodos de proteção**: 1N4007 ou similar

### Sensores
- **Fim de curso**: Micro-switch mecânico ou sensor Hall
- **Sensor de corrente**: ACS712 (5A ou 20A)
- **Botão de emergência**: Tipo cogumelo, NA+NF

### Interface do Usuário
- **LEDs**: LED 5mm vermelho e verde
- **Resistores LEDs**: 470Ω
- **Buzzer**: Buzzer ativo 5V
- **Botões**: Push button tactile 6x6mm

### Alimentação
- **Fonte Principal**: 24V/10A para motores
- **Regulador 5V**: LM7805 ou buck converter para lógica
- **Capacitores**: 1000uF/50V na entrada, 100uF/25V na saída

### Proteção
- **Fusíveis**: Fusível rápido 10A para motores
- **TVS Diodes**: Para proteção de ESD
- **Optoacopladores**: PC817 para isolação galvânica (opcional)

## Esquema de Conexão dos Motores

```
STM8S (PWM) -> Driver H-Bridge -> Motor DC
     |              |
   GPIO          Enable/IN1/IN2
```

### Configuração Típica L298N
- PWM do STM8 -> Enable (velocidade)
- GPIO1 -> IN1 (direção)
- GPIO2 -> IN2 (direção oposta)

## Requisitos de Alimentação

### Microcontrolador
- **Tensão**: 3.3V ou 5V
- **Corrente**: ~20mA (típico)

### Motores (cada)
- **Tensão**: 24V DC
- **Corrente**: 3-5A (dependendo do motor)
- **Potência**: 75-120W

### Total do Sistema
- **Alimentação lógica**: 5V/500mA
- **Alimentação motores**: 24V/15A (com margem)

## Proteções Implementadas

1. **Sobrecorrente**: Monitoramento via ADC
2. **Fim de curso**: Sensores mecânicos/magnéticos
3. **Parada de emergência**: Botão de segurança
4. **Timeout**: Limite de tempo operação contínua
5. **Watchdog**: Reset automático em caso de travamento

## Notas de Montagem

1. Usar trilhas largas para alimentação dos motores (mínimo 2mm)
2. Separar planos de terra analógico e digital
3. Colocar capacitores de desacoplamento próximos ao MCU
4. Proteger entradas de botões com resistores de pull-up/down
5. Usar cabos blindados para sensores de fim de curso
6. Implementar proteção contra inversão de polaridade

## Programação e Debug

### Interface de Programação
- **SWIM**: Single Wire Interface Module
- **Pinos necessários**: SWIM, NRST, GND, VCC
- **Programador**: ST-LINK V2

### Pinout SWIM
- SWIM: PE0 (compartilhado, usar jumper)
- NRST: Reset pin
- VCC: 3.3V ou 5V
- GND: Ground

## Considerações de Segurança

⚠️ **ATENÇÃO**: Este é um equipamento médico/odontológico
- Seguir normas NBR IEC 60601
- Implementar isolação galvânica quando necessário
- Realizar testes rigorosos de segurança
- Usar componentes certificados
- Manter documentação completa

---
**Autor**: Lazaro  
**Data**: Fevereiro 2026  
**Versão**: 1.0
