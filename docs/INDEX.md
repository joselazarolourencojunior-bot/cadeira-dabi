# 📚 Índice da Documentação

## 🎯 Início Rápido

| Documento                             | Descrição                 | Quando Usar       |
|---------------------------------------|---------------------------|-------------------|
| [README.md](../README.md)             | Visão geral do projeto    | Primeiro contato  |
| [QUICK_START.md](QUICK_START.md)      | Guia passo a passo        | Setup inicial     |
| [PROJECT_INFO.md](../PROJECT_INFO.md) | Informações completas     | Visão detalhada   |

## 💻 Código e Arquitetura

<!-- markdownlint-disable MD060 -->
| Documento | Descrição | Quando Usar |
|-----------|-----------|-------------|
| [CODIGO_UNICO.md](CODIGO_UNICO.md) | Estrutura do código em arquivo único | Entender organização |
| [RESUMO_VISUAL.md](RESUMO_VISUAL.md) | Mapa visual do código | Navegação rápida |
| [ARQUIVO_UNICO_VS_MULTIPLOS.md](ARQUIVO_UNICO_VS_MULTIPLOS.md) | Comparação de abordagens | Decisão de arquitetura |
| [SOFTWARE.md](SOFTWARE.md) | Arquitetura detalhada | Desenvolvimento |
<!-- markdownlint-enable MD060 -->

## 🔧 Hardware e Componentes

| Documento                  | Descrição                     | Quando Usar      |
|----------------------------|-------------------------------|------------------|
| [HARDWARE.md](HARDWARE.md) | Especificações completas      | Montagem física  |
| [BOM.md](BOM.md)           | Lista de materiais e custos   | Compras          |
| [MATRIZ_TECLADO.md](MATRIZ_TECLADO.md) | Matriz de teclado digital (legado) | Referência histórica |
| [MATRIZ_TECLADO_ADC.md](MATRIZ_TECLADO_ADC.md) | ⭐ Matriz de teclado com ADC | Sistema atual com leitura analógica |
| [CIRCUITO_ADC_TECLADO.md](CIRCUITO_ADC_TECLADO.md) | Diagramas e circuitos ADC | Design de hardware |

## 🆘 Suporte e Solução de Problemas

| Documento                                | Descrição                | Quando Usar        |
|------------------------------------------|--------------------------|--------------------| | [TROUBLESHOOTING.md](TROUBLESHOOTING.md) | Guia de problemas comuns | Quando algo falha  |

## 📂 Estrutura do Projeto

```text
Cadeira_ML/
│
├── 📄 README.md                    ← Comece aqui!
├── 📄 PROJECT_INFO.md             ← Visão completa
├── 📄 Makefile                    ← Build system
├── 📄 .gitignore
│
├── 📁 src/
│   ├── 📄 main.c                  ← TODO o código aqui! (~1000 linhas)
│   └── 📄 README.md               ← Info sobre o código
│
├── 📁 inc/
│   └── 📄 stm8s_conf.h            ← Config da SPL
│
├── 📁 lib/
│   └── 📁 STM8S_StdPeriph_Driver/ ← Biblioteca STM8 (baixar)
│
├── 📁 docs/                       ← Você está aqui!
│   ├── 📄 INDEX.md                ⭐ Este arquivo
│   ├── 📄 QUICK_START.md          ← Setup inicial
│   ├── 📄 HARDWARE.md             ← Hardware detalhado
│   ├── 📄 SOFTWARE.md             ← Arquitetura
│   ├── 📄 CODIGO_UNICO.md         ← Estrutura do código
│   ├── 📄 RESUMO_VISUAL.md        ← Mapa visual
│   ├── 📄 ARQUIVO_UNICO_VS_MULTIPLOS.md  ← Comparação
│   ├── 📄 BOM.md                  ← Lista de materiais
│   ├── 📄 MATRIZ_TECLADO.md       ← Matriz digital (legado)
│   ├── 📄 MATRIZ_TECLADO_ADC.md   ⭐ Matriz ADC (atual)
│   ├── 📄 CIRCUITO_ADC_TECLADO.md ← Diagramas e circuitos ADC
│   ├── 📄 ENCODER_VIRTUAL_MEMORIA.md  ← Encoder virtual
│   └── 📄 TROUBLESHOOTING.md      ← Solução de problemas
│
├── 📁 build/                      ← Arquivos compilados (gerado)
│   └── 📄 cadeira_odonto.ihx      ← Arquivo final
│
└── 📁 .vscode/                    ← Configuração VS Code
    ├── 📄 tasks.json
    ├── 📄 c_cpp_properties.json
    └── 📄 settings.json
```

## 🗺️ Roadmap de Leitura

### 🟢 Nível 1: Iniciante

1. [README.md](../README.md) - Entenda o projeto
2. [QUICK_START.md](QUICK_START.md) - Configure o ambiente
3. [CODIGO_UNICO.md](CODIGO_UNICO.md) - Entenda o código
4. [RESUMO_VISUAL.md](RESUMO_VISUAL.md) - Veja o mapa

### 🟡 Nível 2: Intermediário

1. [SOFTWARE.md](SOFTWARE.md) - Arquitetura completa
2. [HARDWARE.md](HARDWARE.md) - Detalhes de hardware
3. [ARQUIVO_UNICO_VS_MULTIPLOS.md](ARQUIVO_UNICO_VS_MULTIPLOS.md) - Decisões de design
4. [src/main.c](../src/main.c) - Leia o código

### 🔴 Nível 3: Avançado

1. [BOM.md](BOM.md) - Componentes e custos
2. [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Debug avançado
3. STM8S Datasheet - Hardware completo
4. Modificações customizadas

## 🎯 Busca Rápida

### Preciso entender

**O que é o projeto?**
→ [README.md](../README.md)

**Como compilar e programar?**
→ [QUICK_START.md](QUICK_START.md)

**Como o código está organizado?**
→ [CODIGO_UNICO.md](CODIGO_UNICO.md)

**Onde está cada função?**
→ [RESUMO_VISUAL.md](RESUMO_VISUAL.md)

**Como funciona a máquina de estados?**
→ [SOFTWARE.md](SOFTWARE.md) + [src/main.c](../src/main.c) linhas 290-388

**Quais pinos usar?**
→ [HARDWARE.md](HARDWARE.md)

**Quanto vai custar?**
→ [BOM.md](BOM.md)

**Não está funcionando!**
→ [TROUBLESHOOTING.md](TROUBLESHOOTING.md)

**Por que arquivo único?**
→ [ARQUIVO_UNICO_VS_MULTIPLOS.md](ARQUIVO_UNICO_VS_MULTIPLOS.md)

**Como adicionar um botão?**
→ [CODIGO_UNICO.md](CODIGO_UNICO.md) → Seção "Modificação do Código"

**Como adicionar um motor?**
→ [CODIGO_UNICO.md](CODIGO_UNICO.md) → Seção "Para Adicionar Novo Motor"

**Como funciona a segurança?**
→ [SOFTWARE.md](SOFTWARE.md) → "Sistema de Segurança"

**Como funciona o PWM?**
→ [SOFTWARE.md](SOFTWARE.md) → "Configurações de PWM"

## 📖 Documentação Externa

### STM8

- [STM8S105 Datasheet](https://www.st.com/resource/en/datasheet/stm8s105k4.pdf)
- [STM8S Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271.pdf)
- [STM8 Programming Manual](https://www.st.com/resource/en/programming_manual/cd00161709.pdf)

### Ferramentas

- [SDCC Manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)
- [ST-LINK Utility](https://www.st.com/en/development-tools/stsw-link004.html)
- [STM8 SPL](https://www.st.com/en/embedded-software/stsw-stm8069.html)

### Tutoriais

- [SDCC for STM8](https://github.com/gicking/STM8-SPL_SDCC_patch)
- [stm8flash Tool](https://github.com/vdudouyt/stm8flash)

## 🔧 Ferramentas Auxiliares

### Calculadoras Online

- [PWM Frequency Calculator](https://www.pjrc.com/teensy/td_pulse.html)
- [LED Resistor Calculator](https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-led-series-resistor)

### Simuladores

- [Proteus (STM8 support)](https://www.labcenter.com/)
- [Wokwi (Arduino, educational)](https://wokwi.com/)

## 📝 Template para Modificações

Quando modificar o código, documente:

```markdown
## Modificação: [Nome da Modificação]

**Data:** DD/MM/AAAA
**Autor:** Seu Nome

### O que foi mudado?
- ...

### Por que?
- ...

### Como testar?
1. ...
2. ...

### Arquivos afetados:
- src/main.c (linhas X-Y)
```

## 🤝 Contribuindo

Para contribuir com documentação:

1. Mantenha formatação consistente
2. Use markdown padrão
3. Adicione exemplos práticos
4. Teste os links
5. Atualize este INDEX.md

## 📧 Contato e Suporte

**Autor:** Lazaro  
**Projeto:** Cadeira Odontológica STM8S  
**Versão:** 1.0  
**Data:** Fevereiro 2026

---

## 🎓 Glossário Rápido

| Termo      | Significado                                      |
|------------|--------------------------------------------------|
| **STM8**   | Microcontrolador 8-bit da ST                     |
| **SPL**    | Standard Peripheral Library                      |
| **SDCC**   | Small Device C Compiler                          |
| **PWM**    | Pulse Width Modulation                           |
| **GPIO**   | General Purpose Input/Output                     |
| **UART**   | Universal Asynchronous Receiver/Transmitter      |
| **ADC**    | Analog to Digital Converter                      |
| **HSI**    | High Speed Internal oscillator                   |
| **SWIM**   | Single Wire Interface Module                     |
| **IHX**    | Intel HEX file format                            |

---

Boa leitura e bom desenvolvimento! 🚀

Documentação organizada para você encontrar tudo facilmente.
