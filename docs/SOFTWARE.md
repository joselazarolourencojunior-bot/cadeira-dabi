# Documentação de Software - Cadeira Odontológica

## Arquitetura do Software

### Módulos Principais

```
┌─────────────────────────────────────────┐
│            main.c                       │
│     (Estado da Máquina Principal)       │
└────────────┬────────────────────────────┘
             │
    ┌────────┼────────┬──────────┐
    │        │        │          │
    ▼        ▼        ▼          ▼
┌────────┐ ┌────┐ ┌────────┐ ┌────────┐
│ Motor  │ │ UI │ │ Safety │ │ Config │
│Control │ │    │ │        │ │        │
└────────┘ └────┘ └────────┘ └────────┘
```

### 1. main.c
**Responsabilidades:**
- Inicialização do sistema
- Máquina de estados principal
- Coordenação entre módulos
- Loop principal de execução

**Estados do Sistema:**
- `SYSTEM_IDLE`: Sistema em espera
- `SYSTEM_MOVING_UP`: Cadeira subindo
- `SYSTEM_MOVING_DOWN`: Cadeira descendo
- `SYSTEM_TILTING_BACKREST`: Ajustando encosto
- `SYSTEM_TILTING_LEGREST`: Ajustando apoio de pernas
- `SYSTEM_EMERGENCY_STOP`: Parada de emergência ativa
- `SYSTEM_ERROR`: Estado de erro

### 2. motor_control.c/h
**Responsabilidades:**
- Controle PWM dos motores
- Gerenciamento de velocidade
- Aceleração/desaceleração suave
- Status dos motores

**Funções Principais:**
```c
void Motor_Init(void);
void Motor_MoveUp(uint8_t speed);
void Motor_MoveDown(uint8_t speed);
void Motor_TiltBackrest(MotorDirection_t direction);
void Motor_TiltLegrest(MotorDirection_t direction);
void Motor_StopAll(void);
MotorDirection_t Motor_GetMainDirection(void);
uint8_t Motor_IsAnyRunning(void);
void Motor_Update(void);
```

### 3. user_interface.c/h
**Responsabilidades:**
- Leitura de botões com debounce
- Controle de LEDs
- Controle do buzzer
- Feedback ao usuário

**Funções Principais:**
```c
void UI_Init(void);
uint8_t UI_ReadButtons(void);
void UI_Beep(uint16_t duration);
void UI_StatusLedOn/Off/Toggle(void);
void UI_ShowMessage(const char* message);
void UI_ShowError(ErrorCode_t error);
void UI_Update(void);
```

### 4. safety.c/h
**Responsabilidades:**
- Verificação de sensores de limite
- Monitoramento de corrente
- Detecção de parada de emergência
- Timeouts de segurança

**Funções Principais:**
```c
void Safety_Init(void);
uint8_t Safety_CheckEmergencyStop(void);
uint8_t Safety_CheckLimitsUp/Down(void);
uint8_t Safety_CheckLimits(void);
uint8_t Safety_CheckOvercurrent(void);
uint8_t Safety_CheckTimeout(uint32_t currentTime);
void Safety_ResetTimeout(void);
uint8_t Safety_FullCheck(void);
```

### 5. config.h
**Responsabilidades:**
- Configurações globais
- Definições de hardware
- Constantes do sistema
- Enumerações e tipos

## Fluxo de Execução

### Inicialização
```
1. Desabilitar interrupções
2. Configurar Clock (16MHz)
3. Configurar GPIOs
4. Configurar Timers (PWM)
5. Configurar UART
6. Inicializar módulos
   - Motor_Init()
   - UI_Init()
   - Safety_Init()
7. Habilitar interrupções
8. Feedback de inicialização (beeps + LEDs)
9. Entrar no loop principal
```

### Loop Principal
```
while(1) {
    1. Processar máquina de estados
    2. Verificar parada de emergência
    3. Atualizar interface do usuário
    4. Delay de 10ms
}
```

### Máquina de Estados

#### Estado IDLE
```
- Lê botões
- Se BTN_UP pressionado:
  → Verifica limites
  → Move para cima
  → Transição para MOVING_UP
- Se BTN_DOWN pressionado:
  → Verifica limites
  → Move para baixo
  → Transição para MOVING_DOWN
- Se BTN_BACKREST pressionado:
  → Ajusta encosto
  → Transição para TILTING_BACKREST
```

#### Estados de Movimento
```
- Monitora botões
- Se botão solto:
  → Para motores
  → Retorna para IDLE
- Verifica limites continuamente
- Se limite atingido:
  → Para motores
  → Beep de alerta
  → Retorna para IDLE
```

#### Estado de Emergência
```
- Para todos os motores imediatamente
- Mostra erro
- Aguarda reset manual
- Após reset e botão liberado:
  → Retorna para IDLE
```

## Configurações de PWM

### Timer2 (Motores)
```c
Prescaler: 16
Auto-reload: 1000
Frequência PWM: 1 kHz
Duty Cycle: 0-1000 (0-100%)
```

### Velocidades Pré-configuradas
- **Lenta**: 30% (duty = 300)
- **Normal**: 60% (duty = 600)
- **Rápida**: 90% (duty = 900)

## Sistema de Segurança

### Níveis de Proteção

#### Nível 1: Prevenção
- Verificação antes de iniciar movimento
- Validação de limites
- Detecção de sobrecarga

#### Nível 2: Detecção
- Monitoramento contínuo durante operação
- Sensores de fim de curso
- Leitura de corrente via ADC

#### Nível 3: Resposta
- Parada imediata
- Alertas sonoros e visuais
- Registro de erro

### Timeouts
- **Operação contínua**: 30 segundos
- **Inatividade**: 5 minutos (sleep mode futuro)
- **Debounce**: 50ms

### Monitoramento de Corrente
```
ADC Channel 3 -> Leitura 0-1023
Conversão: (ADC * 5000) / 1023 = mA
Limite: 5000mA (5A)
```

## Comunicação UART

### Configuração
```
Baud Rate: 9600
Data Bits: 8
Stop Bits: 1
Parity: None
Flow Control: None
```

### Propósito
- Debug e diagnóstico
- Configuração remota (futuro)
- Logs de operação (futuro)
- Integração com sistema externo (futuro)

## Mensagens de Debug

### Formato (futuro)
```
[TIMESTAMP] [LEVEL] Mensagem
Exemplo: [00:12:34] [INFO] Sistema iniciado
```

### Níveis de Log
- **INFO**: Informações gerais
- **WARN**: Avisos
- **ERROR**: Erros recuperáveis
- **FATAL**: Erros críticos

## Melhorias Futuras

### Funcionalidades Planejadas
1. **Display LCD**: Mostrar status e mensagens
2. **Memória de posições**: Salvar posições favoritas na EEPROM
3. **Interface serial completa**: Comandos via UART
4. **Modo de teste**: Para diagnóstico e manutenção
5. **Logging em EEPROM**: Histórico de operações
6. **Modo sleep**: Economia de energia
7. **CAN Bus**: Comunicação com outros dispositivos
8. **App Mobile**: Controle via Bluetooth (BLE)

### Otimizações
1. **Interrupções**: Migrar leitura de botões para interrupção
2. **DMA**: Para transferências UART
3. **Low Power**: Modos de baixo consumo
4. **Filtros digitais**: Para leitura de ADC mais estável

## Compilação e Programação

### Requisitos
```bash
# Instalar SDCC
# Windows: Download de sdcc.sourceforge.net
# Linux: apt-get install sdcc

# Instalar stm8flash
git clone https://github.com/vdudouyt/stm8flash.git
cd stm8flash
make
make install
```

### Comandos
```bash
# Compilar
make clean
make all

# Programar
make flash

# Ver tamanho
make size
```

### Estrutura de Build
```
build/
├── obj/              # Arquivos objeto (.rel)
└── cadeira_odonto.ihx # Arquivo final Intel HEX
```

## Troubleshooting

### Problema: LED de erro piscando
**Causa**: Sistema em estado de erro
**Solução**: Pressionar BTN_RESET

### Problema: Motores não respondem
**Causa**: Parada de emergência ativa
**Solução**: Verificar botão de emergência e resetar

### Problema: Movimento lento
**Causa**: Corrente alta detectada
**Solução**: Verificar motores e carga mecânica

### Problema: Não compila
**Causa**: SDCC não instalado ou PATH incorreto
**Solução**: Verificar instalação do SDCC

### Problema: Não programa
**Causa**: ST-LINK não conectado
**Solução**: Verificar conexões SWIM/NRST/VCC/GND

## Convenções de Código

### Nomenclatura
- **Funções**: `Module_ActionObject()` - PascalCase
- **Variáveis locais**: `camelCase`
- **Variáveis globais**: `g_variableName`
- **Constantes**: `UPPER_CASE`
- **Tipos**: `TypeName_t`

### Comentários
```c
/**
 * @brief  Descrição breve
 * @param  nome: descrição do parâmetro
 * @retval Descrição do retorno
 */
```

### Organização de Arquivos
```c
/* Includes */
/* Private typedef */
/* Private defines */
/* Private variables */
/* Private function prototypes */
/* Public functions */
/* Private functions */
```

---
**Autor**: Lazaro  
**Data**: Fevereiro 2026  
**Versão**: 1.0
