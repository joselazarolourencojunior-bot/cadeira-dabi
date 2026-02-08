# Guia de Início Rápido - Cadeira Odontológica STM8S105K4T6C

## 📋 Pré-requisitos

### Software Necessário
1. **SDCC (Small Device C Compiler)**
   - Download: http://sdcc.sourceforge.net/
   - Versão recomendada: 4.0 ou superior
   
2. **stm8flash** (para programação)
   - Windows: https://github.com/vdudouyt/stm8flash/releases
   - Linux: compilar do source
   
3. **ST-LINK Utility** (alternativa)
   - Download: https://www.st.com/en/development-tools/stsw-link004.html

4. **Editor recomendado**
   - VS Code com extensões C/C++
   - Notepad++
   - Qualquer editor de texto

### Hardware Necessário
- [x] STM8S105K4T6C
- [x] Programador ST-LINK V2
- [x] Fonte de alimentação 5V para o MCU
- [x] Fonte 24V para os motores
- [x] Drivers H-Bridge (L298N ou similar)
- [x] Botões e switches
- [x] LEDs e resistores
- [x] Buzzer
- [x] Motores DC 24V

## 🚀 Primeiros Passos

### 1. Instalação do SDCC (Windows)

```bash
# Baixe o instalador do SDCC
# Execute o instalador
# Adicione ao PATH do Windows:
# C:\Program Files\SDCC\bin
```

Verifique a instalação:
```bash
sdcc --version
```

### 2. Instalação do stm8flash (Windows)

```bash
# Baixe o executável stm8flash.exe
# Coloque em: C:\Program Files\stm8flash\
# Adicione ao PATH
```

### 3. Clone/Download do Projeto

```bash
cd "C:\Users\Usuario\Desktop\Lazaro 18-09-25\PROJETOS\Projetos_Flutter_VSCODE\"
# Você já está no diretório Cadeira_ML
```

### 4. Estrutura do Projeto

```
Cadeira_ML/
├── src/               # Código fonte
│   ├── main.c
│   ├── motor_control.c
│   ├── user_interface.c
│   └── safety.c
├── inc/               # Headers
│   ├── main.h
│   ├── config.h
│   ├── motor_control.h
│   ├── user_interface.h
│   └── safety.h
├── lib/               # Bibliotecas STM8 (adicionar depois)
├── docs/              # Documentação
├── build/             # Arquivos compilados (gerado)
├── Makefile           # Script de compilação
└── README.md
```

## ⚙️ Configuração

### 1. Baixar STM8 Standard Peripheral Library

A biblioteca SPL (Standard Peripheral Library) deve ser baixada do site da ST:

1. Acesse: https://www.st.com/en/embedded-software/stsw-stm8069.html
2. Baixe o arquivo (requer cadastro gratuito)
3. Extraia e copie as pastas:
   - `STM8S_StdPeriph_Driver/inc` -> `Cadeira_ML/lib/STM8S_StdPeriph_Driver/inc`
   - `STM8S_StdPeriph_Driver/src` -> `Cadeira_ML/lib/STM8S_StdPeriph_Driver/src`

### 2. Configurar stm8s.h

Crie o arquivo `lib/STM8S_StdPeriph_Driver/inc/stm8s.h` com:

```c
#define STM8S105
#include "stm8s_conf.h"
```

### 3. Ajustar Makefile (se necessário)

O Makefile já está configurado, mas você pode ajustar:
- Caminhos das ferramentas
- Flags de compilação
- Nome do projeto

## 🔨 Compilação

### Compilar o projeto

```bash
make clean
make all
```

**Saída esperada:**
```
Compilando src/main.c...
Compilando src/motor_control.c...
Compilando src/user_interface.c...
Compilando src/safety.c...
Linkando...
Build completo!
Arquivo gerado: build/cadeira_odonto.ihx
```

### Verificar tamanho

```bash
make size
```

## 📡 Conexão do Hardware

### Programador ST-LINK V2

Conexões do ST-LINK ao STM8S105:
```
ST-LINK V2          STM8S105K4T6C
──────────          ──────────────
3.3V/5V     ──────> VDD (Pino 8 ou 32)
GND         ──────> VSS (Pino 7 ou 31)
SWIM        ──────> SWIM (Pino configurable)
RST         ──────> NRST (Pino reset)
```

### Diagrama de Pinagem Básico

```
        STM8S105K4T6C (LQFP32)
              ┌─────┐
     PE5  1 ──┤     ├── 32 VDD
     PE4  2 ──┤     ├── 31 VSS
     PE3  3 ──┤     ├── 30 PA1 (MOTOR_UP)
     PE2  4 ──┤     ├── 29 PA2 (MOTOR_DOWN)
     PE1  5 ──┤     ├── 28 PA3 (MOTOR_BACKREST)
     PE0  6 ──┤     ├── 27 PA4
     VSS  7 ──┤     ├── 26 PA5
     VDD  8 ──┤     ├── 25 PA6
    NRST  9 ──┤     ├── 24 PB0 (BTN_UP)
    ...
```

## 🔥 Programação do MCU

### Método 1: Via Makefile

```bash
make flash
```

### Método 2: Via stm8flash manual

```bash
stm8flash -c stlinkv2 -p stm8s105k4 -w build/cadeira_odonto.ihx
```

### Método 3: Via ST Visual Programmer (STVP)

1. Abra STVP
2. Configure: Device -> STM8S105K4
3. File -> Open -> Selecione `build/cadeira_odonto.ihx`
4. Program -> Current Tab

## ✅ Teste Inicial

### 1. Teste de Alimentação
- Conecte 5V ao MCU
- LED de status deve piscar 3 vezes na inicialização
- LED permanece aceso após inicialização

### 2. Teste de Botões
- Pressione BTN_UP -> LED de status deve piscar
- Pressione BTN_DOWN -> LED de status deve piscar
- Todos os botões devem ter resposta

### 3. Teste de Buzzer
- Na inicialização, deve soar 2 beeps curtos

### 4. Teste de Segurança
- Pressione botão de emergência
- LED de erro deve acender
- Buzzer deve soar beep longo
- Pressione BTN_RESET para limpar

## 🐛 Troubleshooting

### Erro: "sdcc: not found"
**Solução:** Adicione SDCC ao PATH do sistema

### Erro: "stm8flash: not found"
**Solução:** Instale stm8flash e adicione ao PATH

### Erro: "Error: cannot open connection"
**Solução:**
- Verifique conexões do ST-LINK
- Verifique se o MCU está alimentado
- Use driver correto do ST-LINK

### Erro de compilação: "stm8s.h not found"
**Solução:** Baixe a STM8 SPL e coloque na pasta `lib/`

### MCU não responde após programação
**Solução:**
- Verifique alimentação (5V estável)
- Verifique se .ihx foi gravado corretamente
- Tente reset manual (conecte NRST a GND por 1s)

### Motores não funcionam
**Solução:**
- Verifique alimentação 24V dos drivers
- Verifique conexões dos drivers H-Bridge
- Teste pinos PWM com osciloscópio/LED

## 📊 Monitoramento Serial

### Conectar UART para Debug

1. Use conversor USB-Serial (FTDI, CH340, etc.)
2. Conecte:
   - TX do conversor -> RX (PD6) do STM8
   - RX do conversor -> TX (PD5) do STM8
   - GND -> GND

3. Abra terminal serial:
   ```bash
   # Windows (PuTTY, Tera Term)
   # Linux
   screen /dev/ttyUSB0 9600
   # ou
   minicom -D /dev/ttyUSB0 -b 9600
   ```

## 📈 Próximos Passos

1. **Teste individual de motores**
   - Verifique cada motor separadamente
   - Ajuste velocidades no config.h

2. **Calibração de limites**
   - Configure sensores de fim de curso
   - Teste movimentos completos

3. **Ajuste de segurança**
   - Calibre limite de corrente
   - Teste parada de emergência
   - Ajuste timeouts

4. **Interface completa**
   - Adicione display LCD (futuro)
   - Implemente salvamento de posições
   - Configure comunicação serial completa

## 📚 Recursos Adicionais

### Documentação
- `docs/HARDWARE.md` - Especificações de hardware
- `docs/SOFTWARE.md` - Arquitetura do software
- `README.md` - Visão geral do projeto

### Links Úteis
- [STM8S Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271.pdf)
- [STM8S105 Datasheet](https://www.st.com/resource/en/datasheet/stm8s105k4.pdf)
- [SDCC Manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)
- [STM8 Forum](https://www.st.com/content/st_com/en/support/learning/stm8-education.html)

## 🆘 Suporte

Para problemas ou dúvidas:
1. Verifique a documentação em `docs/`
2. Revise o código de exemplo
3. Consulte o datasheet do STM8S105
4. Verifique conexões de hardware

## ⚠️ Avisos Importantes

🔴 **SEGURANÇA EM PRIMEIRO LUGAR**
- Este é um equipamento médico/odontológico
- Realize testes completos antes do uso
- Nunca teste com pacientes antes da certificação
- Siga todas as normas de segurança aplicáveis
- Use componentes de qualidade certificada

🔴 **ELÉTRICA**
- Não conecte/desconecte com sistema energizado
- Use fonte estabilizada
- Implemente proteção contra curto-circuito
- Nunca exceda a corrente máxima dos pinos (10mA)

---

**Autor**: Lazaro  
**Versão**: 1.0  
**Data**: Fevereiro 2026

**Bom desenvolvimento! 🚀**
