# 🔄 Changelog - Migração para Sistema ADC

**Data:** 09/Fevereiro/2026  
**Versão:** 1.0 → 1.1  
**Tipo:** Feature Update - Matriz de Teclado

---

## 📋 Resumo das Alterações

Sistema de matriz de teclado **migrado de leitura digital para leitura analógica (ADC)**, permitindo detectar múltiplas chaves em paralelo através de diferentes níveis de tensão.

---

## ✨ O Que Mudou

### 1. ⚡ Hardware - Pinos das Colunas

#### ❌ Antes (Digital)
```c
/* Colunas em GPIOF com pull-up digital */
#define KEYPAD_COL_PORT    GPIOF
#define KEYPAD_COL1_PIN    GPIO_PIN_4  /* PF4 */
#define KEYPAD_COL2_PIN    GPIO_PIN_5  /* PF5 */
#define KEYPAD_COL3_PIN    GPIO_PIN_6  /* PF6 */
#define KEYPAD_COL4_PIN    GPIO_PIN_7  /* PF7 */

/* Entradas com pull-up interno */
GPIO_Init(KEYPAD_COL_PORT, KEYPAD_COL1_PIN, GPIO_MODE_IN_PU_NO_IT);
```

#### ✅ Agora (ADC Analógico)
```c
/* Colunas em GPIOB com canais ADC */
#define KEYPAD_COL1_PORT        GPIOB
#define KEYPAD_COL1_PIN         GPIO_PIN_0  /* PB0 */
#define KEYPAD_COL1_ADC_CHANNEL ADC1_CHANNEL_0  /* AIN0 */

#define KEYPAD_COL2_PORT        GPIOB
#define KEYPAD_COL2_PIN         GPIO_PIN_1
#define KEYPAD_COL2_ADC_CHANNEL ADC1_CHANNEL_1

#define KEYPAD_COL3_PORT        GPIOB
#define KEYPAD_COL3_PIN         GPIO_PIN_2
#define KEYPAD_COL3_ADC_CHANNEL ADC1_CHANNEL_2

#define KEYPAD_COL4_PORT        GPIOB
#define KEYPAD_COL4_PIN         GPIO_PIN_3
#define KEYPAD_COL4_ADC_CHANNEL ADC1_CHANNEL_3

/* Entradas analógicas (sem pull-up) */
GPIO_Init(KEYPAD_COL1_PORT, KEYPAD_COL1_PIN, GPIO_MODE_IN_FL_NO_IT);
```

**🔴 IMPORTANTE:** Os pinos PB0-PB3 estavam usando outros recursos antes. Verifique conflitos!

---

### 2. 🔌 Hardware - Circuito com Resistores

#### ❌ Antes (Matriz Digital)
```
Chave conectada diretamente entre linha e coluna
Pull-up interno do MCU detecta 0/1 (LOW/HIGH)
```

#### ✅ Agora (Divisor de Tensão)
```
VCC (5V) ----[R1=1K]----[Tecla1]----GND
         |
         +---[R2=2K]----[Tecla2]----GND
         |
         +---[R3=3K]----[Tecla3]----GND
         |
         +----> ADC_PIN (lê tensão 0-5V)
```

**Cada tecla tem seu resistor exclusivo**, criando um **nível de tensão único**:
- Tecla com R1 = 1KΩ → ADC lê ~1.0V (205)
- Tecla com R2 = 2KΩ → ADC lê ~2.0V (409)
- Tecla com R3 = 3KΩ → ADC lê ~3.0V (614)

---

### 3. 💻 Software - Funções Modificadas

#### ❌ Antes
```c
/* Leitura digital simples */
static uint8_t Keypad_ReadColumn(void) {
    uint8_t cols = 0;
    if (GPIO_ReadInputPin(KEYPAD_COL_PORT, KEYPAD_COL1_PIN) == RESET)
        cols |= 0x01;
    return cols;
}
```

#### ✅ Agora
```c
/* Inicialização do ADC */
void Keypad_ADC_Init(void);

/* Leitura ADC de um canal */
uint16_t Keypad_ReadADC(ADC1_Channel_TypeDef channel);

/* Análise do valor ADC para detectar tecla */
static uint8_t Keypad_AnalyzeADC(uint16_t adcValue, uint8_t row, uint8_t col);
```

---

### 4. 📊 Software - Tabela de Thresholds

#### ✅ Nova Feature
```c
/* Tabela de níveis ADC esperados para cada tecla */
static const uint16_t keypadAdcThresholds[KEYPAD_ROWS][KEYPAD_COLS] = {
    /* COL1    COL2    COL3    COL4 */
    { 205,    205,    205,    205 },  /* ROW1: 1, 2, 3, A (~1V, R=1K) */
    { 409,    409,    409,    409 },  /* ROW2: 4, 5, 6, B (~2V, R=2K) */
    { 614,    614,    614,    614 }   /* ROW3: 7, 8, 9, C (~3V, R=3K) */
};

#define KEYPAD_ADC_TOLERANCE  50  /* ±50 unidades ADC de margem */
```

---

### 5. ⏱️ Software - Timing Ajustado

#### ❌ Antes
```c
/* Delay mínimo com NOPs */
nop(); nop(); nop(); nop(); nop();
```

#### ✅ Agora
```c
/* Delay de 2ms para estabilização do ADC */
delay_ms(2);
```

**Razão:** ADC precisa de tempo para estabilização elétrica do circuito antes da conversão.

---

## 🎯 Benefícios da Mudança

| Aspecto | Antes (Digital) | Agora (ADC) | Melhoria |
|---------|----------------|-------------|----------|
| **Teclas por coluna** | 1 | Múltiplas (3+) | 🟢 Expansível |
| **Pinos GPIO usados** | 7 (3 rows + 4 cols) | 7 (3 rows + 4 ADC) | 🟡 Igual |
| **Flexibilidade** | Baixa | Alta | 🟢 Fácil expandir |
| **Identificação** | Binária (ON/OFF) | Analógica (níveis) | 🟢 Múltiplos níveis |
| **Complexidade código** | Baixa | Média | 🔴 Mais complexo |
| **Calibração** | Não necessária | Necessária | 🔴 Requer ajuste |
| **Custo hardware** | Baixo (sem resistores) | Médio (+12 resistores) | 🔴 R$ 3-5 a mais |
| **Consumo energia** | Mínimo | Baixo (mA apenas durante scan) | 🟡 Levemente maior |

---

## 📋 Checklist de Migração

### Hardware

- [ ] **Adquirir resistores**
  - 4× 1.0 KΩ ±1% 1/4W
  - 4× 2.0 KΩ ±1% 1/4W
  - 4× 3.0 KΩ ±1% 1/4W
  - 4× Capacitor 100nF cerâmico (opcional, filtro ruído)

- [ ] **Reconectar pinos**
  - Mover colunas de PF4-PF7 para PB0-PB3
  - Verificar se PB0-PB3 estão livres
  - Manter linhas em PD3-PD5

- [ ] **Montar circuito**
  - Conectar resistores entre VCC e chaves
  - Conectar chaves entre resistores e linhas de scan
  - Adicionar capacitores de filtro (opcional)

### Software

- [x] **Código atualizado em** [main.c](../src/main.c)
  - [x] Definições de pinos ADC
  - [x] Função `Keypad_ADC_Init()`
  - [x] Função `Keypad_ReadADC()`
  - [x] Função `Keypad_AnalyzeADC()`
  - [x] Modificação em `Keypad_Scan()`
  - [x] Tabela `keypadAdcThresholds`

- [ ] **Calibração necessária**
  - [ ] Medir tensões reais com multímetro
  - [ ] Ler valores ADC de cada tecla
  - [ ] Ajustar tabela `keypadAdcThresholds[]`
  - [ ] Ajustar `KEYPAD_ADC_TOLERANCE` se necessário

- [ ] **Testes**
  - [ ] Compilar código sem erros
  - [ ] Flash no microcontrolador
  - [ ] Testar cada tecla individualmente
  - [ ] Verificar debounce está funcionando
  - [ ] Confirmar não há detecções falsas

---

## 🔧 Arquivos Modificados

### Código
- ✏️ [src/main.c](../src/main.c) - **MODIFICADO**
  - Linhas 74-113: Definições de hardware
  - Linhas 297-301: Protótipos de funções
  - Linhas 754-764: Inicialização GPIO
  - Linhas 1693-1832: Implementação Keypad com ADC

### Documentação
- 📄 [docs/MATRIZ_TECLADO_ADC.md](MATRIZ_TECLADO_ADC.md) - **NOVO** ⭐
- 📄 [docs/CIRCUITO_ADC_TECLADO.md](CIRCUITO_ADC_TECLADO.md) - **NOVO** ⭐
- 📄 [docs/MATRIZ_TECLADO.md](MATRIZ_TECLADO.md) - **ATUALIZADO** (marcado como legado)
- 📄 [docs/INDEX.md](INDEX.md) - **ATUALIZADO**
- 📄 [docs/CHANGELOG_ADC.md](CHANGELOG_ADC.md) - **NOVO** (este arquivo)

---

## 📚 Documentação de Referência

### Para Implementação
1. 📘 [MATRIZ_TECLADO_ADC.md](MATRIZ_TECLADO_ADC.md) - Explicação completa do sistema
2. 📐 [CIRCUITO_ADC_TECLADO.md](CIRCUITO_ADC_TECLADO.md) - Diagramas de circuito e BOM

### Para Troubleshooting
3. 🔧 [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Problemas gerais

### Referência Técnica
4. 📖 STM8S105 Datasheet - Especificações do ADC
5. 📖 STM8S Standard Peripheral Library - Funções ADC1_*

---

## ⚠️ Notas Importantes

### Conflitos de Pinos

**⚠️ ATENÇÃO:** Os pinos **PB0-PB3** foram realocados para ADC. Se esses pinos estavam sendo usados para **botões painel frontal**, você precisará:

1. **Opção A:** Realocar os botões para outros pinos GPIO disponíveis
2. **Opção B:** Usar outros canais ADC disponíveis:
   - AIN4 (PB4)
   - AIN5 (PB5)  
   - AIN2 (PC4)
   - AIN3 (PD2)
   - AIN6 (PD6)

### Calibração Obrigatória

Este sistema **requer calibração** com os resistores reais:

```
1. Monte o circuito
2. Pressione cada tecla e leia ADC
3. Anote valores medidos
4. Atualize tabela keypadAdcThresholds[]
5. Recompile e teste
```

**Não use valores padrão em produção!** Os valores exemplo (205, 409, 614) são aproximados.

---

## 🚀 Próximos Passos

1. ✅ **Você está aqui:** Código implementado
2. 🔲 **Hardware:** Monte o circuito com resistores
3. 🔲 **Teste:** Verifique funcionamento básico
4. 🔲 **Calibração:** Ajuste thresholds ADC
5. 🔲 **Validação:** Teste todas as 12 teclas
6. 🔲 **Produção:** Integre ao sistema completo

---

## 💡 Suporte

### Dúvidas sobre Implementação
- Consulte [MATRIZ_TECLADO_ADC.md](MATRIZ_TECLADO_ADC.md)

### Problemas com Circuito
- Consulte [CIRCUITO_ADC_TECLADO.md](CIRCUITO_ADC_TECLADO.md)

### Erros no Código
- Consulte [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

### Questões Técnicas ADC
- STM8S105 Datasheet, Seção 11 (ADC)
- AN2834 Application Note (STMicroelectronics)

---

**Desenvolvido por:** Lazaro  
**Data:** 09/Fevereiro/2026  
**Versão Firmware:** 1.0 → 1.1  
**Status:** ✅ Implementado, ⏳ Aguardando calibração em hardware
