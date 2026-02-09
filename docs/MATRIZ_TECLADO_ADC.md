# Matriz de Teclado com ADC (Leitura Analógica)

## Visão Geral

O sistema foi modificado para usar **conversores ADC (Analógico-Digital)** nas colunas da matriz de teclado, permitindo detectar **múltiplas chaves em paralelo** através de diferentes níveis de tensão criados por resistores.

## Vantagens do Sistema ADC

✅ **Múltiplas chaves por coluna** - Uma mesma coluna pode ter várias chaves  
✅ **Economia de pinos** - Menos pinos GPIO necessários  
✅ **Flexibilidade** - Fácil adicionar mais teclas sem mudar hardware  
✅ **Identificação precisa** - Cada chave tem seu nível de tensão único  

---

## Arquitetura do Hardware

### Conexões

#### Linhas (Saídas Digitais - GPIOD)
- **ROW1**: PD3 (GPIO_PIN_3) - Digital Output
- **ROW2**: PD4 (GPIO_PIN_4) - Digital Output
- **ROW3**: PD5 (GPIO_PIN_5) - Digital Output

#### Colunas (Entradas Analógicas ADC - GPIOB)
- **COL1**: PB0 (AIN0) - ADC Channel 0
- **COL2**: PB1 (AIN1) - ADC Channel 1
- **COL3**: PB2 (AIN2) - ADC Channel 2
- **COL4**: PB3 (AIN3) - ADC Channel 3

⚠️ **ATENÇÃO**: Os pinos PB0-PB3 foram realocados para ADC. Verifique se não estão sendo usados para outras funções!

---

## Princípio de Funcionamento

### Circuito com Resistores (Divisor de Tensão)

Para cada coluna, múltiplas chaves são conectadas em paralelo com **resistores diferentes**, criando níveis de tensão únicos:

```
     VCC (5V)
        |
        +----[R1=1K]----[SW1]----+
        |                        |
        +----[R2=2K]----[SW2]----+----> ADC_PIN
        |                        |
        +----[R3=3K]----[SW3]----+
        |                        |
       GND <--- ROW (Scan Line) GND
```

### Como Funciona

1. **Linha Ativa (LOW)**: Durante o scan, uma linha é colocada em GND (0V)
2. **Chave Pressionada**: Quando uma chave é pressionada, conecta o divisor de tensão ao GND
3. **Leitura ADC**: O ADC lê a tensão no pino da coluna
4. **Identificação**: O firmware compara o valor ADC com thresholds para identificar qual tecla foi pressionada

### Exemplo de Tensões

| Chave | Resistor | Tensão Aproximada | Valor ADC (10-bit) |
|-------|----------|-------------------|-------------------|
| **Tecla 1/2/3/A** | 1KΩ | ~1.0V | 205 |
| **Tecla 4/5/6/B** | 2KΩ | ~2.0V | 409 |
| **Tecla 7/8/9/C** | 3KΩ | ~3.0V | 614 |

**Cálculo**: `ADC_Value = (Tensão / 5V) × 1023`

---

## Configuração no Código

### 1. Definição dos Thresholds

No arquivo [main.c](../src/main.c), localize a tabela `keypadAdcThresholds`:

```c
static const uint16_t keypadAdcThresholds[KEYPAD_ROWS][KEYPAD_COLS] = {
    /* COL1    COL2    COL3    COL4 */
    { 205,    205,    205,    205 },  /* ROW1: Teclas 1, 2, 3, A - ~1V (R1=1K) */
    { 409,    409,    409,    409 },  /* ROW2: Teclas 4, 5, 6, B - ~2V (R2=2K) */
    { 614,    614,    614,    614 }   /* ROW3: Teclas 7, 8, 9, C - ~3V (R3=3K) */
};
```

### 2. Tolerância de Detecção

```c
#define KEYPAD_ADC_TOLERANCE    50    /* ±50 unidades ADC */
```

Isso significa que uma leitura entre `205-50` e `205+50` (155 a 255) será reconhecida como a primeira tecla.

---

## Como Calibrar o Sistema

### Passo 1: Escolher Valores de Resistores

Escolha resistores que criem tensões bem espaçadas:

**Exemplo com divisor de tensão (VCC = 5V):**
- R1 = 1KΩ  → V ≈ 1.0V  → ADC = 205
- R2 = 2KΩ  → V ≈ 2.0V  → ADC = 409  
- R3 = 3KΩ  → V ≈ 3.0V  → ADC = 614
- R4 = 4.7KΩ → V ≈ 3.8V → ADC = 780

**Fórmula do divisor de tensão:**
```
V_adc = VCC × (R_série / (R_série + R_chave))
```

### Passo 2: Medir Valores Reais

Use um multímetro ou o ADC para medir as tensões reais com cada chave pressionada:

1. Pressione uma tecla por vez
2. Leia o valor ADC no terminal serial ou debugger
3. Anote os valores medidos

### Passo 3: Atualizar Thresholds

Substitua os valores na tabela `keypadAdcThresholds` pelos valores medidos.

**Exemplo de código para debug:**
```c
/* Adicione no Keypad_Scan() para debug */
if (adcValue > 50) {  /* Ignora ruído */
    printf("ROW%d COL%d: ADC=%d\n", row, col, adcValue);
}
```

### Passo 4: Ajustar Tolerância

Se as detecções estiverem instáveis:
- **Aumentar tolerância**: `#define KEYPAD_ADC_TOLERANCE 80`
- **Diminuir tolerância**: `#define KEYPAD_ADC_TOLERANCE 30`

---

## Exemplo de Circuito Completo (1 Coluna)

```
ROW1 (PD3) ----[SW1]----+
                        |
                        +---- 1KΩ ---- VCC (5V)
                        |
ROW2 (PD4) ----[SW2]----+
                        |
                        +---- 2KΩ ---- VCC (5V)
                        |
ROW3 (PD5) ----[SW3]----+
                        |
                        +---- 3KΩ ---- VCC (5V)
                        |
                        +----> COL1 (PB0/AIN0)
```

**Funcionamento:**
1. Todas as linhas em HIGH (5V) → ADC lê ~5V (1023)
2. ROW1 vai LOW (0V):
   - SW1 pressionado → ADC lê ~1V (205)
   - SW1 não pressionado → ADC lê ~5V (1023)
3. Processo se repete para ROW2 e ROW3

---

## Especificações Técnicas

### ADC - STM8S105K4T6C

| Parâmetro | Valor |
|-----------|-------|
| Resolução | 10 bits (0-1023) |
| Tensão referência | 5V (VDD) |
| Prescaler | /18 (Fadc = 888 kHz) |
| Tempo de conversão | ~14 µs |
| Canais usados | AIN0, AIN1, AIN2, AIN3 |
| Alinhamento | Right (direita) |

### Timing

| Parâmetro | Valor |
|-----------|-------|
| Debounce | 50 ms |
| Estabilização ADC | 2 ms |
| Tempo total de scan | ~24 ms (3 linhas × 4 cols × 2ms) |

---

## Tabela de Mapeamento de Funções

| Tecla | Linha | Coluna | Resistor | ADC | Função |
|-------|-------|--------|----------|-----|--------|
| **1** | ROW1 | COL1 | 1KΩ | 205 | Sobe Assento |
| **2** | ROW1 | COL2 | 1KΩ | 205 | Desce Assento |
| **3** | ROW1 | COL3 | 1KΩ | 205 | Sobe Encosto |
| **A** | ROW1 | COL4 | 1KΩ | 205 | Customizável |
| **4** | ROW2 | COL1 | 2KΩ | 409 | Desce Encosto |
| **5** | ROW2 | COL2 | 2KΩ | 409 | Toggle Refletor |
| **6** | ROW2 | COL3 | 2KΩ | 409 | Volta a Zero |
| **B** | ROW2 | COL4 | 2KΩ | 409 | Customizável |
| **7** | ROW3 | COL1 | 3KΩ | 614 | Posição Trabalho |
| **8** | ROW3 | COL2 | 3KΩ | 614 | Salvar Memória |
| **9** | ROW3 | COL3 | 3KΩ | 614 | Executar Memória |
| **C** | ROW3 | COL4 | 3KΩ | 614 | Emergência (STOP) |

---

## Troubleshooting

### Problema: Teclas não respondem

**Soluções:**
1. Verificar conexões dos resistores
2. Medir tensões com multímetro
3. Ajustar thresholds no código
4. Verificar se ADC está inicializado
5. Aumentar tempo de estabilização (`delay_ms(2)` para `delay_ms(5)`)

### Problema: Detecções falsas

**Soluções:**
1. Aumentar tolerância: `KEYPAD_ADC_TOLERANCE`
2. Adicionar capacitor de filtro (100nF) entre ADC_PIN e GND
3. Usar resistores de precisão (1% tolerance)
4. Verificar ruído elétrico no circuito

### Problema: Múltiplas teclas detectadas simultaneamente

**Causa:** Valores ADC muito próximos

**Soluções:**
1. Usar resistores com valores mais espaçados
2. Reduzir tolerância no código
3. Garantir que apenas uma tecla por linha seja pressionada

---

## Expansão do Sistema

### Adicionar Mais Teclas (Mesma Coluna)

Para adicionar uma 4ª tecla na mesma coluna:

1. **Hardware**: Adicionar resistor de valor diferente (ex: 4.7KΩ)
2. **Software**: Criar nova linha na matriz ou usar outro método de mapeamento

**Limitação:** A resolução ADC de 10 bits permite ~10-15 níveis distintos com boa margem de tolerância.

### Adicionar Mais Colunas

Usar outros canais ADC disponíveis:
- AIN4 (PB4)
- AIN5 (PB5)
- AIN6 (PC4)
- etc.

---

## Referências

- [STM8S105 Datasheet](https://www.st.com/resource/en/datasheet/stm8s105k4.pdf)
- [STM8S Standard Peripheral Library - ADC](https://www.st.com/en/embedded-software/stsw-stm8069.html)
- [Divisor de Tensão - Wikipedia](https://pt.wikipedia.org/wiki/Divisor_de_tens%C3%A3o)

---

## Notas Importantes

⚠️ **CONFLITO DE PINOS**: Os pinos PB0-PB3 eram usados para botões. Verifique seu hardware e realoque conforme necessário.

⚠️ **ESTABILIZAÇÃO**: O tempo de estabilização de 2ms é crítico para leituras ADC precisas.

⚠️ **CALIBRAÇÃO**: Sempre calibre o sistema com os resistores reais do seu circuito.

📝 **DOCUMENTAÇÃO**: Mantenha um registro dos valores ADC medidos para cada tecla.

---

**Última atualização:** 09/Fevereiro/2026  
**Versão do firmware:** 1.0
