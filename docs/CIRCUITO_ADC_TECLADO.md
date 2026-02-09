# Diagrama de Circuito - Matriz de Teclado com ADC

## Diagrama Completo (3 Linhas × 4 Colunas)

```text
                        COLUNA 1 (PB0/AIN0)    COLUNA 2 (PB1/AIN1)    COLUNA 3 (PB2/AIN2)    COLUNA 4 (PB3/AIN3)
                               |                     |                     |                     |
                               |                     |                     |                     |
       LINHA 1 (PD3) ──[SW1]───┼────[1K]──VCC       |                     |                     |
                               |                     |                     |                     |
                               |        [SW2]────────┼────[1K]──VCC        |                     |
                               |                     |                     |                     |
                               |                     |        [SW3]────────┼────[1K]──VCC        |
                               |                     |                     |                     |
                               |                     |                     |        [SWA]────────┼────[1K]──VCC
                               |                     |                     |                     |
                               |                     |                     |                     |
       LINHA 2 (PD4) ──[SW4]───┼────[2K]──VCC       |                     |                     |
                               |                     |                     |                     |
                               |        [SW5]────────┼────[2K]──VCC        |                     |
                               |                     |                     |                     |
                               |                     |        [SW6]────────┼────[2K]──VCC        |
                               |                     |                     |                     |
                               |                     |                     |        [SWB]────────┼────[2K]──VCC
                               |                     |                     |                     |
                               |                     |                     |                     |
       LINHA 3 (PD5) ──[SW7]───┼────[3K]──VCC       |                     |                     |
                               |                     |                     |                     |
                               |        [SW8]────────┼────[3K]──VCC        |                     |
                               |                     |                     |                     |
                               |                     |        [SW9]────────┼────[3K]──VCC        |
                               |                     |                     |                     |
                               |                     |                     |        [SWC]────────┼────[3K]──VCC
                               |                     |                     |                     |
                               ↓                     ↓                     ↓                     ↓
                           Para ADC1_CH0        Para ADC1_CH1        Para ADC1_CH2        Para ADC1_CH3
                           (Leitura 0-1023)     (Leitura 0-1023)     (Leitura 0-1023)     (Leitura 0-1023)
```

---

## Legenda de Chaves

| Chave | Tecla | Função           |
|-------|-------|------------------|
| SW1   | 1     | Sobe Assento     |
| SW2   | 2     | Desce Assento    |
| SW3   | 3     | Sobe Encosto     |
| SWA   | A     | Customizável     |
| SW4   | 4     | Desce Encosto    |
| SW5   | 5     | Toggle Refletor  |
| SW6   | 6     | Volta a Zero     |
| SWB   | B     | Customizável     |
| SW7   | 7     | Posição Trabalho |
| SW8   | 8     | Salvar Memória   |
| SW9   | 9     | Executar Memória |
| SWC   | C     | Emergência       |

---

## Exemplo Detalhado: Coluna 1

### Circuito Simplificado (1 Coluna)

```text
                    +5V (VCC)
                      |
                      |
         ROW1 ───[SW1]─┴─[R1=1K]
         (PD3)         |
                       |
                       |
         ROW2 ───[SW4]─┴─[R2=2K]
         (PD4)         |
                       |
                       |
         ROW3 ───[SW7]─┴─[R3=3K]
         (PD5)         |
                       |
                       ├───[C1=100nF]───GND  (Opcional: filtro anti-ruído)
                       |
                       └──────────> COL1 (PB0/AIN0)
                                    ADC Input
```

### Análise de Funcionamento

#### Estado 1: Nenhuma tecla pressionada

```text
Todas as linhas em HIGH (5V)
ADC lê: ~5V (1023) - Pull-up através dos resistores
```

#### Estado 2: SW1 pressionada (ROW1 = LOW)

```text
ROW1 = 0V (GND)
Divisor de tensão: R1 (1K) entre GND e VCC
ADC lê: ~1.0V (205)

Cálculo:
V_adc = VCC × (R_pullup / (R_pullup + R1))
      = 5V × (∞ / (∞ + 1K))
      ≈ 1V (simplificado)
```

#### Estado 3: SW4 pressionada (ROW2 = LOW)

```text
ROW2 = 0V (GND)
Divisor de tensão: R2 (2K) entre GND e VCC
ADC lê: ~2.0V (409)
```

#### Estado 4: SW7 pressionada (ROW3 = LOW)

```text
ROW3 = 0V (GND)
Divisor de tensão: R3 (3K) entre GND e VCC
ADC lê: ~3.0V (614)
```

---

## Circuito PCB Recomendado

### Componentes por Coluna

| Item              | Descrição          | Quantidade         |
|-------------------|--------------------|--------------------|
| **Resistores R1** | 1KΩ ±1% 1/4W       | 4 (uma por coluna) |
| **Resistores R2** | 2KΩ ±1% 1/4W       | 4 (uma por coluna) |
| **Resistores R3** | 3KΩ ±1% 1/4W       | 4 (uma por coluna) |
| **Capacitores C** | 100nF cerâmico     | 4 (filtro ADC)     |
| **Chaves**        | Botão tátil 6x6mm  | 12 total           |

### Layout PCB (Sugestão)

```text
[MCU STM8S105]
    │
    ├─ PD3 ──────────┬────────┬────────┬────────┐
    │                │        │        │        │
    ├─ PD4 ──────────│────┬───│────┬───│────┬───│────┬───
    │                │    │   │    │   │    │   │    │
    ├─ PD5 ──────────│────│───│────│───│────│───│────│───
    │                │    │   │    │   │    │   │    │
    │               SW1  SW4 SW2  SW5 SW3  SW6 SWA  SWB ...
    │                │    │   │    │   │    │   │    │
    │               R1   R2  R1   R2  R1   R2  R1   R2
    │                │    │   │    │   │    │   │    │
    │                └────┴───└────┴───└────┴───└────┴─── VCC
    │                │         │         │         │
    │               [C1]      [C2]      [C3]      [C4]
    │                │         │         │         │
    ├─ PB0 ─────────┴─────────│─────────│─────────│
    ├─ PB1 ───────────────────┴─────────│─────────│
    ├─ PB2 ─────────────────────────────┴─────────│
    └─ PB3 ───────────────────────────────────────┘
         │         │         │         │
        GND       GND       GND       GND
```

---

## Cálculo dos Resistores (Design)

### Fórmula do Divisor de Tensão

Para um sistema com resistor pull-up ao VCC:

```text
V_out = VCC × (R_down / (R_up + R_down))
```

Onde:

- `V_out`: Tensão lida pelo ADC
- `VCC`: Tensão de alimentação (5V)
- `R_down`: Resistor conectado ao GND (linha ativa)
- `R_up`: Resistor conectado ao VCC

### Seleção de Valores

Para garantir separação adequada entre níveis:

| Tecla     | Tensão Alvo | % do Range | Resistor | ADC (10-bit) |
|-----------|-------------|------------|----------|--------------|
| Linha 1   | 1.0V        | 20%        | 1.0 KΩ   | 205          |
| Linha 2   | 2.0V        | 40%        | 2.0 KΩ   | 409          |
| Linha 3   | 3.0V        | 60%        | 3.0 KΩ   | 614          |
| (Linha 4) | 4.0V        | 80%        | 4.7 KΩ   | 819          |

**Margem de segurança:** ±10% (±102 unidades ADC)
**Tolerância configurada:** ±50 unidades ADC

---

## Valores Alternativos de Resistores

### Opção 1: Resistores Série E12 (Comuns)

| Linha | Resistor | Tensão | ADC |
|-------|----------|--------|-----|
| ROW1  | 1.0 KΩ   | 1.0V   | 205 |
| ROW2  | 2.2 KΩ   | 2.1V   | 430 |
| ROW3  | 3.3 KΩ   | 2.9V   | 594 |

### Opção 2: Espaçamento Máximo

| Linha | Resistor | Tensão | ADC |
|-------|----------|--------|-----|
| ROW1  | 1.0 KΩ   | 1.0V   | 205 |
| ROW2  | 2.7 KΩ   | 2.4V   | 491 |
| ROW3  | 4.7 KΩ   | 3.7V   | 758 |

### Opção 3: Baixo Consumo

| Linha | Resistor | Tensão | ADC |
|-------|----------|--------|-----|
| ROW1  | 10 KΩ    | 1.0V   | 205 |
| ROW2  | 20 KΩ    | 2.0V   | 409 |
| ROW3  | 30 KΩ    | 3.0V   | 614 |

**Vantagem:** Menor consumo de corrente
**Desvantagem:** Maior sensibilidade a ruído (precisa de capacitor maior, ex: 1µF)

---

## Consumo de Corrente Estimado

### Por Tecla Pressionada

```text
I = VCC / R_total
```

| Resistor | Corrente (VCC=5V) |
|----------|-------------------|
| 1.0 KΩ   | 5 mA              |
| 2.0 KΩ   | 2.5 mA            |
| 3.0 KΩ   | 1.67 mA           |
| 10 KΩ    | 0.5 mA            |

**Consumo máximo:** 5 mA por tecla (apenas durante scan de ~2ms)
**Consumo médio total:** < 1 mA

---

## Proteção e Filtragem

### Capacitor de Filtro (Recomendado)

```text
ADC_PIN ───┬─── Leitura ADC
           │
          [C]
           │
          GND

C = 100nF (cerâmico X7R)
```

**Função:** Reduz ruído elétrico e estabiliza leitura ADC

### Diodo de Proteção (Opcional)

```text
          ┌──[D1]──┤>├──VCC  (Proteção overvoltage)
          │
ADC_PIN ──┤
          │
          └──[D2]──├<├──GND  (Proteção undervoltage)

D1, D2 = 1N4148 ou BAT85
```

**Função:** Protege pino ADC contra sobretensão

---

## Lista de Materiais (BOM)

### Para Sistema Completo (12 Teclas)

| Item      | Descrição                | Quantidade | Ref.                   |
|-----------|--------------------------|------------|------------------------|
| R1-R4     | Resistor 1K0Ω ±1% 1/4W   | 4          | Mouser/Digikey         |
| R5-R8     | Resistor 2K0Ω ±1% 1/4W   | 4          | Mouser/Digikey         |
| R9-R12    | Resistor 3K0Ω ±1% 1/4W   | 4          | Mouser/Digikey         |
| C1-C4     | Capacitor 100nF X7R 50V  | 4          | Mouser/Digikey         |
| SW1-SW12  | Botão Tátil 6x6mm        | 12         | Omron B3F              |
| U1        | STM8S105K4T6C            | 1          | STMicroelectronics     |

**Custo estimado:** ~R$ 15-25 (apenas componentes do teclado)

---

## Testes e Validação

### Teste 1: Verificar Tensões (Multímetro)

1. Pressione cada tecla individualmente
2. Meça tensão no pino ADC correspondente
3. Anote os valores reais
4. Compare com valores esperados

### Teste 2: Verificar Leituras ADC (Debug Serial)

Adicione código temporário:

```c
/* Em Keypad_Scan(), após adcValue = Keypad_ReadADC() */
if (adcValue > 50) {
    UART2_SendString("ROW");
    UART2_SendByte('0' + row);
    UART2_SendString(" COL");
    UART2_SendByte('0' + col);
    UART2_SendString(" ADC=");
    UART2_SendNumber(adcValue);
    UART2_SendString("\r\n");
}
```

### Teste 3: Verificar Debounce

- Pressione rapidamente várias vezes
- Deve registrar apenas 1 evento por pressionamento
- Ajuste `KEYPAD_DEBOUNCE_TIME` se necessário

---

## Troubleshooting Avançado

### Leituras Instáveis ou com Ruído

**Sintomas:**

- Valores ADC flutuam muito
- Teclas são detectadas esporadicamente

**Soluções:**

1. ✅ Adicionar capacitor 100nF no pino ADC
2. ✅ Aumentar tempo de estabilização (`delay_ms(5)`)
3. ✅ Usar resistores de precisão (±1%)
4. ✅ Blindar cabos longos
5. ✅ Adicionar ferrite bead no VCC

### Conflito de Detecção (Ghosting)

**Sintomas:**

- Múltiplas teclas detectadas simultaneamente
- Teclas erradas são registradas

**Soluções:**

1. ✅ Usar diodos em série com cada chave (anti-ghosting)
2. ✅ Aumentar espaçamento entre thresholds
3. ✅ Reduzir tolerância no código

### Resposta Lenta

**Sintomas:**

- Delay perceptível entre pressionar e ação
- Sistema "trava" durante scan

**Soluções:**

1. ✅ Reduzir `delay_ms(2)` para `delay_ms(1)`
2. ✅ Otimizar código de conversão ADC
3. ✅ Reduzir prescaler do ADC
4. ✅ Usar DMA para leitura ADC (avançado)

---

## Melhorias Futuras

### 1. Scanning com DMA

Usar DMA para leitura ADC automática sem travar CPU.

### 2. Média Móvel (Smoothing)

```c
#define ADC_SAMPLES 4
uint16_t adcBuffer[ADC_SAMPLES];

uint16_t Keypad_ReadADC_Averaged(channel) {
    for(i=0; i<ADC_SAMPLES; i++)
        adcBuffer[i] = Keypad_ReadADC(channel);
    
    return (adcBuffer[0] + adcBuffer[1] + adcBuffer[2] + adcBuffer[3]) / 4;
}
```

### 3. Calibração Automática

Salvar valores ADC medidos na EEPROM durante primeira inicialização.

### 4. Detecção de Múltiplas Teclas

Modificar algoritmo para suportar chord (pressionar várias teclas simultaneamente).

---

**Documento criado:** 09/Fevereiro/2026  
**Autor:** Lazaro  
**Versão:** 1.0
