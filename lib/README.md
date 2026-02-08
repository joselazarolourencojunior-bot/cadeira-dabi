# Biblioteca STM8S Standard Peripheral Library (SPL)

## 📚 Sobre

Esta pasta contém a STM8S Standard Peripheral Library adaptada para uso com o compilador SDCC no projeto de Cadeira Odontológica.

## 📂 Estrutura

```text
lib/
└── STM8S_StdPeriph_Driver/
    ├── inc/           # Arquivos header (.h)
    │   ├── stm8s.h             # Header principal
    │   ├── stm8s_gpio.h        # GPIO
    │   ├── stm8s_clk.h         # Clock
    │   ├── stm8s_tim1.h        # Timer 1
    │   ├── stm8s_tim2.h        # Timer 2
    │   ├── stm8s_tim3.h        # Timer 3 (placeholder)
    │   ├── stm8s_uart2.h       # UART2
    │   ├── stm8s_adc1.h        # ADC
    │   └── ...                 # Outros periféricos
    └── src/           # Arquivos fonte (.c)
        ├── stm8s_gpio.c
        ├── stm8s_clk.c
        ├── stm8s_tim1.c
        ├── stm8s_tim2.c
        ├── stm8s_tim3.c
        ├── stm8s_uart2.c
        ├── stm8s_adc1.c
        └── ...
```

## ⚙️ Configuração

O arquivo `inc/stm8s_conf.h` no diretório do projeto controla quais periféricos são incluídos na compilação.

### Periféricos Habilitados no Projeto

- **GPIO** - Controle dos pinos digitais
- **CLK** - Gerenciamento de clock
- **TIM1** - Timer 1 para PWM e temporização
- **TIM2** - Timer 2 para debounce e delays
- **UART2** - Comunicação serial/debug
- **ADC1** - Conversor analógico-digital
- **FLASH** - Gravação em memória EEPROM

## 🔧 Adaptações para SDCC

A biblioteca foi adaptada para o compilador SDCC com as seguintes modificações:

1. **Definição do compilador**: Adicionado suporte para `__SDCC` no arquivo `stm8s.h`
2. **Macros específicas**: Definições vazias para `FAR`, `NEAR`, `TINY`, `EEPROM`
3. **Interrupções**: Macros para habilitar/desabilitar interrupções com assembly inline
4. **UART2**: Implementação customizada para STM8S105K4
5. **TIM3**: Header placeholder (não usado no projeto mas necessário para compatibilidade)

## 📝 Observações

- A biblioteca original é da STMicroelectronics
- Fonte principal: Repositórios GitHub com adaptações para SDCC
- Versão base: STM8S SPL v2.3.0
- Licença: MCD-ST Liberty SW License Agreement V2

## 🚀 Uso no Projeto

O Makefile automaticamente inclui os arquivos da biblioteca durante a compilação:

```makefile
CFLAGS += -I$(LIB_DIR)/STM8S_StdPeriph_Driver/inc
SOURCES += $(wildcard $(LIB_DIR)/STM8S_StdPeriph_Driver/src/*.c)
```

## 🔗 Referências

- [STM8S105 Datasheet](https://www.st.com/resource/en/datasheet/stm8s105k4.pdf)
- [STM8S Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271.pdf)
- [SDCC Manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)
- [Repositório com patches para SDCC](https://github.com/gicking/STM8-SPL_SDCC_patch)

## ⚠️ Importante

Não modifique os arquivos desta biblioteca diretamente. Para configurar quais periféricos usar, edite o arquivo `inc/stm8s_conf.h` no diretório raiz do projeto.

---

**Instalada em:** Fevereiro 2026  
**Projeto:** Cadeira Odontológica STM8S105K4T6C
