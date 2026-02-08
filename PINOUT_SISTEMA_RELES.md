# 📌 PINOUT - Sistema de Controle de Cadeira Odontológica com Relés

## 🎯 STM8S105K4T6C - Microcontrolador (LQFP32)

**Versão:** 2.0  
**Data:** 08/Fevereiro/2026  
**Autor:** Lazaro

---

## 🔌 SAÍDAS - RELÉS (Ativo HIGH)

Os relés são acionados em **nível ALTO (HIGH)** e todos os pinos escolhidos possuem **True Open-Drain**, ideais para comandar relés com segurança.

| Função | Porta | Pino | Tipo | Descrição |
|--------|-------|------|------|-----------|
| **Relé Sobe Assento** | GPIOC | PC5 | True Open-Drain | Ativa motor para subir assento |
| **Relé Desce Assento** | GPIOC | PC6 | True Open-Drain | Ativa motor para descer assento |
| **Relé Sobe Encosto** | GPIOC | PC7 | True Open-Drain | Ativa motor para subir encosto |
| **Relé Desce Encosto** | GPIOD | PD1 | True Open-Drain | Ativa motor para descer encosto |
| **Relé Refletor** | GPIOD | PD2 | True Open-Drain | Liga/Desliga refletor (toggle) |

### 🔧 **Configuração GPIO:**
```c
GPIO_Init(RELE_XXX_PORT, RELE_XXX_PIN, GPIO_MODE_OUT_OD_LOW_FAST);
```
- **OUT_OD**: Open-Drain (coletor aberto)
- **LOW_FAST**: Inicializa em LOW (relé desligado), velocidade rápida

---

## 💡 SAÍDA - LED WATCHDOG

| Função | Porta | Pino | Tipo | Descrição |
|--------|-------|------|------|-----------|
| **LED Watchdog** | GPIOE | PE5 | Push-Pull | Pisca a cada 500ms indicando sistema ativo |

### 🔧 **Configuração GPIO:**
```c
GPIO_Init(LED_WATCHDOG_PORT, LED_WATCHDOG_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
```

---

## 🔘 ENTRADAS - BOTÕES (com Pull-Up Interno)

Todos os botões são **ativos em LOW** (pressionado = GND).

| Função | Porta | Pino | Descrição |
|--------|-------|------|-----------|
| **BTN Sobe Assento** | GPIOB | PB0 | Botão para subir assento (manual) |
| **BTN Desce Assento** | GPIOB | PB1 | Botão para descer assento (manual) |
| **BTN Sobe Encosto** | GPIOB | PB2 | Botão para subir encosto (manual) |
| **BTN Desce Encosto** | GPIOB | PB3 | Botão para descer encosto (manual) |
| **BTN Refletor** | GPIOB | PB4 | Botão toggle refletor (pressiona liga/desliga) |
| **BTN Volta a Zero** | GPIOB | PB5 | Movimento automático para posição inicial |
| **BTN Posição de Trabalho** | GPIOB | PB6 | Movimento automático para posição de trabalho |
| **BTN Parada Emergência** | GPIOB | PB7 | Botão de emergência (para tudo) |

### 🔧 **Configuração GPIO:**
```c
GPIO_Init(BTN_PORT, BTN_XXX_PIN, GPIO_MODE_IN_PU_NO_IT);
```
- **IN_PU**: Entrada com pull-up interno
- **NO_IT**: Sem interrupção externa

### 📝 **Leitura de Botão:**
```c
uint8_t buttonPressed = !GPIO_ReadInputPin(BTN_PORT, pin); // Inverte por causa do pull-up
```

---

## 🛡️ ENTRADAS - SENSORES DE LIMITE (com Pull-Up Interno)

Sensores tipo **fim de curso**, ativos em **LOW** (acionado = GND).

| Função | Porta | Pino | Descrição |
|--------|-------|------|-----------|
| **Limite Superior Assento** | GPIOC | PC3 | Sensor fim de curso superior assento |
| **Limite Inferior Assento** | GPIOC | PC4 | Sensor fim de curso inferior assento |
| **Limite Superior Encosto** | GPIOA | PA1 | Sensor fim de curso superior encosto |
| **Limite Inferior Encosto** | GPIOA | PA2 | Sensor fim de curso inferior encosto |

### 🔧 **Configuração GPIO:**
```c
GPIO_Init(LIMIT_XXX_PORT, LIMIT_XXX_PIN, GPIO_MODE_IN_PU_NO_IT);
```

### 📝 **Leitura de Sensor:**
```c
uint8_t limitAtingido = !GPIO_ReadInputPin(LIMIT_XXX_PORT, LIMIT_XXX_PIN); // Inverte
```

---

## 📊 DIAGRAMA DE CONEXÃO

```
┌──────────────────────────────────────────────────────────┐
│          STM8S105K4T6C (LQFP32)                         │
├──────────────────────────────────────────────────────────┤
│                                                           │
│  ┌─────────────┐                                         │
│  │  PORTA C    │  PC5 ──> Relé Sobe Assento            │
│  │ (Open-Drain)│  PC6 ──> Relé Desce Assento           │
│  └─────────────┘  PC7 ──> Relé Sobe Encosto            │
│                                                           │
│  ┌─────────────┐                                         │
│  │  PORTA D    │  PD1 ──> Relé Desce Encosto           │
│  │ (Open-Drain)│  PD2 ──> Relé Refletor                 │
│  └─────────────┘                                         │
│                                                           │
│  ┌─────────────┐                                         │
│  │  PORTA E    │  PE5 ──> LED Watchdog (500ms blink)    │
│  └─────────────┘                                         │
│                                                           │
│  ┌─────────────┐                                         │
│  │  PORTA B    │  PB0 <── BTN Sobe Assento              │
│  │  (Pull-Up)  │  PB1 <── BTN Desce Assento             │
│  │             │  PB2 <── BTN Sobe Encosto              │
│  │             │  PB3 <── BTN Desce Encosto             │
│  │             │  PB4 <── BTN Refletor (Toggle)         │
│  │             │  PB5 <── BTN Volta a Zero              │
│  │             │  PB6 <── BTN Posição de Trabalho       │
│  │             │  PB7 <── BTN Parada Emergência         │
│  └─────────────┘                                         │
│                                                           │
│  ┌─────────────┐                                         │
│  │  PORTA C    │  PC3 <── Limite Superior Assento       │
│  │  (Pull-Up)  │  PC4 <── Limite Inferior Assento       │
│  └─────────────┘                                         │
│                                                           │
│  ┌─────────────┐                                         │
│  │  PORTA A    │  PA1 <── Limite Superior Encosto       │
│  │  (Pull-Up)  │  PA2 <── Limite Inferior Encosto       │
│  └─────────────┘                                         │
│                                                           │
└──────────────────────────────────────────────────────────┘
```

---

## ⚙️ FUNCIONALIDADES DO SISTEMA

### 🔄 Controle de Relés com Interlock
- **Interlock de segurança**: Não permite sobe e desce simultâneos
- **Delay de 100ms** entre comandos opostos para proteger relés
- **Timeout de 30 segundos** de operação contínua

### 💡 LED Watchdog
- **Pisca a cada 500ms** quando sistema está ativo
- Indica que o microcontrolador está funcionando corretamente

### 🎚️ Controle do Refletor
- **Modo Toggle**: Aperta liga, aperta desliga
- **Debounce de 200ms** para evitar acionamentos múltiplos
- **Independente dos movimentos** (não desliga com Stop All)

### 🛡️ Sistema de Segurança
1. **Parada de Emergência**: Desliga todos os relés (exceto refletor)
2. **Sensores de Limite**: Para movimento ao atingir fim de curso
3. **Timeout**: Desliga após 30 segundos contínuos
4. **Debounce**: 50ms em todos os botões

### 🔀 Máquina de Estados
- **IDLE**: Aguardando comando
- **MOVENDO_ASSENTO**: Movimento manual do assento ativo
- **MOVENDO_ENCOSTO**: Movimento manual do encosto ativo
- **VOLTA_ZERO**: Movimento automático para posição inicial (limites inferiores)
- **POSICAO_TRABALHO**: Movimento automático para posição de trabalho (limites superiores)
- **EMERGENCY_STOP**: Parada de emergência acionada
- **ERROR**: Estado de erro (reset automático após 3s)

### 🎯 Funções Automáticas

#### **Volta a Zero - VZ (PB5)**
Pressione uma vez e o sistema executa:
- **Ação**: Aciona Desce Assento + Sobe Encosto simultaneamente
- **Duração**: 30 segundos fixos
- **Parada de emergência**: Qualquer botão pressionado durante o movimento = PARA TUDO
- **Indicação**: Sistema retorna a IDLE após 30s ou ao atingir limites
- **Efeito**: Libera o botão PT para ser usado

#### **Posição de Trabalho - PT (PB6)**
Pressione uma vez e o sistema executa:
- **Ação**: Aciona Sobe Assento + Desce Encosto simultaneamente
- **Duração**: 30 segundos fixos
- **Parada de emergência**: Qualquer botão pressionado durante o movimento = PARA TUDO
- **Indicação**: Sistema retorna a IDLE após 30s ou ao atingir limites
- **⚠️ PRÉ-REQUISITO**: Só funciona se VZ foi executado completamente antes!

### 🔄 Sequência de Operação Obrigatória

```
1. Ligar sistema → PT bloqueado
2. Pressionar VZ → Executa ciclo completo (30s) → PT liberado
3. Pressionar PT → Executa ciclo completo (30s) → PT bloqueado novamente
4. Para usar PT novamente → Executar VZ primeiro
```

**Importante**: 
- Qualquer movimento manual (botões PB0-PB3) bloqueia PT novamente
- Parada de emergência durante VZ ou PT bloqueia PT
- É necessário completar VZ toda vez antes de usar PT

---

## 📝 EXEMPLO DE USO NO CÓDIGO

### Ativar Relé:
```c
Rele_SobeAssento(RELE_ON);   // Liga relé
Rele_SobeAssento(RELE_OFF);  // Desliga relé
```

### Toggle Refletor:
```c
Rele_ToggleRefletor();  // Liga se estiver desligado, desliga se estiver ligado
```

### Verificar Botão:
```c
uint8_t buttons = UI_ReadButtons();
if (buttons & BTN_SOBE_ASSENTO) {
    // Botão pressionado
}
```

### Verificar Limite:
```c
if (Safety_CheckLimitAssentoSup()) {
    // Limite superior atingido!
}
```

---

## 🔗 BOAS PRÁTICAS IMPLEMENTADAS

✅ **Open-Drain para relés** - Protege contra curto-circuito  
✅ **Pull-up interno em botões** - Não precisa resistor externo  
✅ **Debounce por software** - Elimina ruído de contatos mecânicos  
✅ **Interlock de segurança** - Previne comando simultâneo de opostos  
✅ **Timeout automático** - Proteção contra travamento  
✅ **Watchdog LED visual** - Monitoramento de funcionamento  

---

**Desenvolvido por:** Lazaro  
**Hardware:** STM8S105K4T6C (16MHz HSI, 16KB Flash, 2KB RAM)  
**Compilador:** SDCC (Small Device C Compiler)  
