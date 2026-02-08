# Estrutura do Código em Arquivo Único

## 📄 main.c - Arquivo Completo (~1000 linhas)

Todo o sistema está implementado em **`src/main.c`** para facilitar o entendimento e manutenção.

### 🗂️ Organização do Arquivo

```
┌─────────────────────────────────────────────────────────────┐
│                        main.c                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  SEÇÃO 1: INCLUDES                                          │
│  ├─ stm8s.h                                                 │
│  └─ stdint.h                                                │
│                                                             │
│  SEÇÃO 2: CONFIGURAÇÕES E DEFINIÇÕES (Linhas ~15-115)      │
│  ├─ Versão do sistema                                       │
│  ├─ Configurações dos motores                               │
│  ├─ Configurações dos botões                                │
│  ├─ Configurações dos sensores                              │
│  ├─ Configurações da interface                              │
│  ├─ Configurações de segurança                              │
│  └─ Macros úteis                                            │
│                                                             │
│  SEÇÃO 3: TIPOS E ENUMERAÇÕES (Linhas ~117-157)            │
│  ├─ ErrorCode_t      (códigos de erro)                      │
│  ├─ MotorDirection_t (direções dos motores)                 │
│  ├─ SystemState_t    (estados do sistema)                   │
│  └─ MotorState_t     (estrutura de estado do motor)         │
│                                                             │
│  SEÇÃO 4: VARIÁVEIS GLOBAIS (Linhas ~159-180)              │
│  ├─ Estado do sistema                                       │
│  ├─ Estado dos motores                                      │
│  ├─ Interface do usuário                                    │
│  └─ Segurança                                               │
│                                                             │
│  SEÇÃO 5: PROTÓTIPOS DE FUNÇÕES (Linhas ~182-225)          │
│  ├─ Sistema Principal (9 funções)                           │
│  ├─ Controle de Motores (11 funções)                        │
│  ├─ Interface do Usuário (12 funções)                       │
│  └─ Segurança (11 funções)                                  │
│                                                             │
│  SEÇÃO 6: FUNÇÃO PRINCIPAL (Linhas ~227-250)               │
│  └─ void main(void) - Loop infinito                         │
│                                                             │
│  SEÇÃO 7: IMPLEMENTAÇÃO - SISTEMA (Linhas ~252-430)        │
│  ├─ System_Init()      - Inicialização                      │
│  ├─ System_Process()   - Máquina de estados                 │
│  ├─ Clock_Config()     - Configuração do clock              │
│  ├─ GPIO_Config()      - Configuração dos GPIOs             │
│  ├─ Timer_Config()     - Configuração dos timers            │
│  ├─ UART_Config()      - Configuração da UART               │
│  └─ delay_ms()         - Função de delay                    │
│                                                             │
│  SEÇÃO 8: IMPLEMENTAÇÃO - MOTORES (Linhas ~432-630)        │
│  ├─ Motor_Init()                                            │
│  ├─ Motor_MoveUp()                                          │
│  ├─ Motor_MoveDown()                                        │
│  ├─ Motor_TiltBackrest()                                    │
│  ├─ Motor_TiltLegrest()                                     │
│  ├─ Motor_StopAll()                                         │
│  ├─ Motor_GetMainDirection()                                │
│  ├─ Motor_IsAnyRunning()                                    │
│  ├─ Motor_SetPWM()        (static)                          │
│  ├─ Motor_UpdateRamp()    (static)                          │
│  └─ Motor_Update()                                          │
│                                                             │
│  SEÇÃO 9: IMPLEMENTAÇÃO - INTERFACE (Linhas ~632-820)      │
│  ├─ UI_Init()                                               │
│  ├─ UI_ReadButtons()                                        │
│  ├─ UI_DebounceButton()   (static)                          │
│  ├─ UI_Beep()                                               │
│  ├─ UI_StatusLedOn/Off/Toggle()                             │
│  ├─ UI_ErrorLedOn/Off()                                     │
│  ├─ UI_ShowMessage()                                        │
│  ├─ UI_ShowError()                                          │
│  └─ UI_Update()                                             │
│                                                             │
│  SEÇÃO 10: IMPLEMENTAÇÃO - SEGURANÇA (Linhas ~822-1000)    │
│  ├─ Safety_Init()                                           │
│  ├─ Safety_CheckEmergencyStop()                             │
│  ├─ Safety_CheckLimitsUp/Down()                             │
│  ├─ Safety_CheckLimits()                                    │
│  ├─ Safety_CheckOvercurrent()                               │
│  ├─ Safety_CheckTimeout()                                   │
│  ├─ Safety_ResetTimeout()                                   │
│  ├─ Safety_ResetEmergencyStop()                             │
│  ├─ Safety_ReadCurrent()    (static)                        │
│  ├─ Safety_GetCurrentReading()                              │
│  └─ Safety_FullCheck()                                      │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## 📋 Seções Detalhadas

### SEÇÃO 1-2: Configurações (Linhas 1-115)
Todas as definições e configurações do hardware em um só lugar:
```c
#define MOTOR_UP_PORT           GPIOA
#define MOTOR_UP_PIN            GPIO_PIN_1
#define MOTOR_SPEED_NORMAL      60
#define BTN_UP                  (1 << 0)
#define MAX_MOTOR_CURRENT       5000
```

**Vantagem:** Fácil ajustar configurações sem procurar em múltiplos arquivos.

### SEÇÃO 3-4: Tipos e Variáveis (Linhas 117-180)
Definição de todos os tipos e variáveis globais:
```c
typedef enum {
    SYSTEM_IDLE,
    SYSTEM_MOVING_UP,
    // ...
} SystemState_t;

static SystemState_t systemState = SYSTEM_IDLE;
static MotorState_t mainMotor = {0};
```

**Vantagem:** Visão completa do estado do sistema.

### SEÇÃO 5: Protótipos (Linhas 182-225)
Todas as funções declaradas organizadamente:
```c
/* Sistema Principal */
void System_Init(void);
void System_Process(void);

/* Controle de Motores */
void Motor_Init(void);
void Motor_MoveUp(uint8_t speed);

/* Interface do Usuário */
void UI_Init(void);
uint8_t UI_ReadButtons(void);

/* Segurança */
void Safety_Init(void);
uint8_t Safety_CheckLimits(void);
```

**Vantagem:** Índice rápido de todas as funções disponíveis.

### SEÇÃO 6: main() (Linhas 227-250)
Função principal simples e clara:
```c
void main(void)
{
    System_Init();
    
    while (1)
    {
        System_Process();
        if (Safety_CheckEmergencyStop()) {
            systemState = SYSTEM_EMERGENCY_STOP;
            Motor_StopAll();
            UI_ShowError(ERROR_EMERGENCY_STOP);
        }
        UI_Update();
        delay_ms(10);
    }
}
```

**Vantagem:** Fluxo do programa visível de relance.

### SEÇÕES 7-10: Implementações (Linhas 252-1000)
Todas as funções implementadas sequencialmente por módulo.

## 🎯 Como Navegar no Código

### VS Code - Atalhos Úteis
- `Ctrl + P` → Digite "@" → Lista todas as funções
- `Ctrl + F` → Buscar função específica
- `Ctrl + G` → Ir para linha específica
- `Ctrl + Shift + O` → Outline/Estrutura do arquivo

### Ordem de Leitura Recomendada
1. **Primeiro:** SEÇÃO 2 (Configurações) - Entenda os #defines
2. **Segundo:** SEÇÃO 3 (Tipos) - Conheça as estruturas
3. **Terceiro:** SEÇÃO 6 (main) - Veja o fluxo principal
4. **Quarto:** SEÇÃO 7 (System_Process) - Entenda a máquina de estados
5. **Depois:** Funções específicas conforme necessário

## 🔧 Modificação do Código

### Para Adicionar Novo Botão:
1. Adicione o #define na SEÇÃO 2:
   ```c
   #define BTN_NEW_PIN    GPIO_PIN_6
   #define BTN_NEW        (1 << 6)
   ```

2. Configure GPIO na `GPIO_Config()`:
   ```c
   GPIO_Init(GPIOB, BTN_NEW_PIN, GPIO_MODE_IN_PU_NO_IT);
   ```

3. Adicione leitura em `UI_ReadButtons()`:
   ```c
   if (UI_DebounceButton(BTN_NEW_PIN, 6))
       buttons |= BTN_NEW;
   ```

4. Use em `System_Process()`:
   ```c
   if (buttons & BTN_NEW) {
       // Ação do novo botão
   }
   ```

### Para Adicionar Novo Motor:
1. Defina pinos na SEÇÃO 2
2. Adicione variável global na SEÇÃO 4:
   ```c
   static MotorState_t newMotor = {0};
   ```
3. Inicialize em `Motor_Init()`
4. Crie funções de controle na SEÇÃO 8
5. Pare em `Motor_StopAll()`

### Para Adicionar Novo Estado:
1. Adicione em `SystemState_t` (SEÇÃO 3):
   ```c
   SYSTEM_NEW_STATE,
   ```
2. Adicione case em `System_Process()`:
   ```c
   case SYSTEM_NEW_STATE:
       // Lógica do novo estado
       break;
   ```

## 📊 Estatísticas do Código

| Métrica | Valor |
|---------|-------|
| Linhas totais | ~1000 |
| Funções públicas | 35 |
| Funções privadas | 6 |
| Defines | ~40 |
| Tipos/Structs | 4 |
| Estados | 7 |

## ✅ Vantagens do Código Único

### ✔️ Desenvolvimento
- Fácil de navegar (um único arquivo)
- Busca rápida por funções
- Menos arquivos para gerenciar
- Copy-paste direto para outros projetos

### ✔️ Compilação
- Compilação mais rápida
- Menos overhead de linking
- Otimizações mais eficientes do compilador

### ✔️ Manutenção
- Todas as mudanças em um lugar
- Fácil ver dependências
- Refatoração simplificada

### ✔️ Aprendizado
- Fluxo completo visível
- Entendimento top-down
- Menos confusão com includes

## ⚠️ Considerações

### Quando Separar em Arquivos?
Considere separar se:
- Projeto crescer > 2000 linhas
- Múltiplos desenvolvedores trabalhando
- Necessitar reutilizar módulos em outros projetos
- Compilação ficar muito lenta

### Como Separar Depois?
1. Crie arquivos `.h` e `.c` para cada seção
2. Mova funções e defines correspondentes
3. Adicione includes adequados
4. Atualize Makefile para compilar novos arquivos

## 🆘 Troubleshooting

### "Undefined reference to..."
- Verifique se a função está implementada
- Confira protótipo na SEÇÃO 5

### "Conflicting types for..."
- Protótipo não corresponde à implementação
- Verifique assinatura da função

### "Multiple definition of..."
- Variável declarada fora de static
- Adicione `static` antes da variável global

## 📚 Recursos

- [HARDWARE.md](HARDWARE.md) - Especificações de hardware
- [SOFTWARE.md](SOFTWARE.md) - Arquitetura detalhada
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Solução de problemas

---

**Arquivo Único = Simplicidade Máxima! 🚀**

*Criado para facilitar o desenvolvimento e aprendizado.*
