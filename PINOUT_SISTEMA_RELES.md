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

### 🔧 Configuração GPIO

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

### 🔧 Configuração GPIO LED

```c
GPIO_Init(LED_WATCHDOG_PORT, LED_WATCHDOG_PIN, GPIO_MODE_OUT_PP_LOW_FAST);
```

---

## 🔘 ENTRADAS - BOTÕES (com Pull-Up Interno)

Todos os botões são **ativos em LOW** (pressionado = GND).

| Função | Porta | Pino | Descrição |
|--------|-------|------|-----------|
| **BTN Refletor** | GPIOB | PB4 | Botão toggle refletor (pressiona liga/desliga) |
| **BTN Volta a Zero (VZ)** | GPIOB | PB5 | Movimento automático para posição inicial |
| **BTN Posição de Trabalho (PT)** | GPIOB | PB6 | Movimento automático para posição de trabalho |

### 🔧 Configuração GPIO Botões

```c
GPIO_Init(BTN_PORT, BTN_XXX_PIN, GPIO_MODE_IN_PU_NO_IT);
```

- **IN_PU**: Entrada com pull-up interno
- **NO_IT**: Sem interrupção externa

### 📝 Leitura de Botão

```c
uint8_t buttonPressed = !GPIO_ReadInputPin(BTN_PORT, pin); // Inverte por causa do pull-up
```

---

## 🔄 MATRIZ DE CHAVEAMENTO (Preparado para M1, M2, M3)

Sistema preparado para receber matriz de chaveamento com 3 saídas digitais.

| Função | Porta | Pino | Descrição |
|--------|-------|------|-----------|
| **M1** | A definir | - | Saída de chaveamento 1 |
| **M2** | A definir | - | Saída de chaveamento 2 |
| **M3** | A definir | - | Saída de chaveamento 3 |

### 📝 Nota Matriz

A implementação detalhada da matriz será configurada conforme especificação do hardware.

---

## 📊 DIAGRAMA DE CONEXÃO

```text
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
│  │  PORTA B    │  PB4 <── BTN Refletor (Toggle)         │
│  │  (Pull-Up)  │  PB5 <── BTN Volta a Zero (VZ)         │
│  │             │  PB6 <── BTN Posição de Trabalho (PT)  │
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

1. **Parada de Emergência**: Durante VZ ou PT, qualquer botão pressionado para TUDO imediatamente
2. **Limite Virtual (Encoder por Tempo)**: Sistema calcula posição por tempo de operação
3. **Timeout**: Movimentos automáticos duram 30 segundos fixos
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

#### Volta a Zero - VZ (PB5)

Pressione uma vez e o sistema executa:

- **Ação**: Aciona Desce Assento + Sobe Encosto simultaneamente
- **Duração**: 30 segundos fixos
- **Parada de emergência**: Qualquer botão pressionado durante o movimento = PARA TUDO e desliga todos os relés
- **Indicação**: Sistema retorna a IDLE após 30s
- **Efeito**: Libera o botão PT para ser usado
- **Limite**: Calculado por tempo (encoder virtual)

#### Posição de Trabalho - PT (PB6)

Pressione uma vez e o sistema executa:

- **Ação**: Aciona Sobe Assento + Desce Encosto simultaneamente
- **Duração**: 30 segundos fixos
- **Parada de emergência**: Qualquer botão pressionado durante o movimento = PARA TUDO e desliga todos os relés
- **Indicação**: Sistema retorna a IDLE após 30s
- **⚠️ PRÉ-REQUISITO**: Só funciona se VZ foi executado completamente antes!
- **Limite**: Calculado por tempo (encoder virtual)

### 🔄 Sequência de Operação Obrigatória

```text
1. Ligar sistema → PT bloqueado
2. Pressionar VZ → Executa ciclo completo (30s) → PT liberado
3. Pressionar PT → Executa ciclo completo (30s) → PT bloqueado novamente
4. Para usar PT novamente → Executar VZ primeiro
```

**Importante**:

- Pressionar qualquer botão durante VZ ou PT = EMERGÊNCIA (para tudo, desliga todos os relés)
- Parada de emergência durante VZ ou PT bloqueia PT
- É necessário completar VZ toda vez antes de usar PT
- **NÃO existe botão físico de emergência** - emergência é automática ao pressionar qualquer botão durante operação automática

---

## 📝 EXEMPLO DE USO NO CÓDIGO

### Ativar Relé

```c
Rele_SobeAssento(RELE_ON);   // Liga relé
Rele_SobeAssento(RELE_OFF);  // Desliga relé
```

### Toggle Refletor

```c
Rele_ToggleRefletor();  // Liga se estiver desligado, desliga se estiver ligado
```

### Verificar Botão

```c
uint8_t buttons = UI_ReadButtons();
if (buttons & BTN_REFLETOR) {
    // Botão pressionado
}
```

### Posição Virtual (Encoder por Tempo)

```c
// Sistema calcula posição baseado no tempo de operação
// Não existem sensores físicos de limite
```

---

## 🔗 BOAS PRÁTICAS IMPLEMENTADAS

✅ **Open-Drain para relés** - Protege contra curto-circuito  
✅ **Pull-up interno em botões** - Não precisa resistor externo  
✅ **Debounce por software** - Elimina ruído de contatos mecânicos  
✅ **Interlock de segurança** - Previne comando simultâneo de opostos  
✅ **Timeout automático** - Proteção contra travamento (30s fixos)  
✅ **Watchdog LED visual** - Monitoramento de funcionamento  
✅ **Encoder virtual por tempo** - Calcula posição sem sensores físicos  
✅ **Emergência automática** - Qualquer botão durante VZ/PT para tudo  

---

**Desenvolvido por:** Lazaro  
**Hardware:** STM8S105K4T6C (16MHz HSI, 16KB Flash, 2KB RAM)  
**Compilador:** SDCC (Small Device C Compiler)

