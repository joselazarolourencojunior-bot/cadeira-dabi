# Cadeira Odontológica - Sistema STM8S105K4T6C

> **Status**: Em Desenvolvimento  
> **Versão**: 1.0  
> **Autor**: Lazaro  
> **Data**: Fevereiro 2026

## 📋 Índice
- [Sobre o Projeto](#sobre-o-projeto)
- [Funcionalidades](#funcionalidades)
- [Hardware](#hardware)
- [Software](#software)
- [Início Rápido](#início-rápido)
- [Documentação](#documentação)
- [Licença](#licença)

## 🎯 Sobre o Projeto

Sistema embarcado completo para controle de cadeira odontológica utilizando o microcontrolador **STM8S105K4T6C** da ST Microelectronics.

O projeto implementa:
- ✅ Controle PWM de motores DC
- ✅ Interface de usuário com botões
- ✅ Sistema de segurança multinível
- ✅ Monitoramento de corrente
- ✅ Sensores de fim de curso
- ✅ Parada de emergência
- ✅ Feedback sonoro e visual

## ⚡ Funcionalidades

### Movimentos Controlados
- **Cadeira Principal**: Subir/Descer com controle de velocidade
- **Encosto**: Inclinação ajustável
- **Apoio de Pernas**: Movimento independente
- **Velocidades**: 3 níveis (Lenta, Normal, Rápida)

### Segurança
- 🛑 Botão de parada de emergência
- 🔒 Sensores de fim de curso em todas as posições
- ⚡ Monitoramento de sobrecorrente
- ⏱️ Timeout de operação contínua (30s)
- 🔔 Alertas sonoros e visuais

### Interface
- 🎮 7 botões de controle
- 💡 LEDs de status e erro
- 🔊 Buzzer para feedback
- 📡 UART para comunicação serial (9600 bps)

## 🔧 Hardware

### Microcontrolador
- **Modelo**: STM8S105K4T6C
- **Core**: STM8 (8-bit)
- **Clock**: 16 MHz
- **Flash**: 16 KB
- **RAM**: 2 KB
- **EEPROM**: 1 KB
- **Package**: LQFP32

### Componentes Principais
- Driver H-Bridge: L298N ou BTS7960 (x3)
- Motores DC 24V (x3)
- Sensores fim de curso (x6)
- Botão emergência tipo cogumelo
- LEDs indicadores
- Buzzer ativo 5V
- Fonte 24V/10A (motores)
- Fonte 5V/500mA (lógica)

### Pinout Resumido
```
PORTA A: Controle de Motores (PWM)
PORTA B: Botões de Interface
PORTA C: Sensores de Limite + Emergência
PORTA D: LEDs + UART
PORTA E: Buzzer
```

📄 Detalhes completos: [docs/HARDWARE.md](docs/HARDWARE.md)

## 💻 Software

### Arquitetura
```
main.c              → Máquina de estados principal
motor_control.c     → Controle PWM e movimentos
user_interface.c    → Botões, LEDs, beeper
safety.c            → Sensores e proteções
config.h            → Configurações do sistema
```

### Compilador
- **SDCC** (Small Device C Compiler) v4.0+
- Suporte alternativo: IAR, Cosmic

### Bibliotecas
- STM8 Standard Peripheral Library (SPL)

📄 Detalhes completos: [docs/SOFTWARE.md](docs/SOFTWARE.md)

## 🚀 Início Rápido

### Pré-requisitos
```bash
# Instale o SDCC
# Windows: http://sdcc.sourceforge.net/

# Instale o stm8flash
# https://github.com/vdudouyt/stm8flash

# Programador
ST-LINK V2 ou compatível
```

### Clonar/Abrir Projeto
```bash
cd "C:\Users\Usuario\Desktop\Lazaro 18-09-25\PROJETOS\Projetos_Flutter_VSCODE\Cadeira_ML"
```

### Compilar
```bash
make clean
make all
```

### Programar
```bash
make flash
```

📄 Guia completo: [docs/QUICK_START.md](docs/QUICK_START.md)

## 📚 Documentação

### Estrutura de Arquivos
```
Cadeira_ML/
├── 📁 src/                 # Código fonte C
│   ├── main.c
│   ├── motor_control.c
│   ├── user_interface.c
│   └── safety.c
│
├── 📁 inc/                 # Headers
│   ├── main.h
│   ├── config.h
│   ├── motor_control.h
│   ├── user_interface.h
│   └── safety.h
│
├── 📁 lib/                 # Bibliotecas STM8 SPL
│   └── STM8S_StdPeriph_Driver/
│       ├── inc/
│       └── src/
│
├── 📁 docs/                # Documentação
│   ├── HARDWARE.md        # Especificações de hardware
│   ├── SOFTWARE.md        # Arquitetura de software
│   └── QUICK_START.md     # Guia de início rápido
│
├── 📁 build/               # Compilados (gerado)
│
├── 📄 Makefile             # Build system
├── 📄 README.md            # Este arquivo
└── 📄 .gitignore
```

### Documentos Disponíveis

| Documento | Descrição |
|-----------|-----------|
| [HARDWARE.md](docs/HARDWARE.md) | Especificações completas de hardware, pinout, componentes |
| [SOFTWARE.md](docs/SOFTWARE.md) | Arquitetura do software, fluxos, APIs |
| [QUICK_START.md](docs/QUICK_START.md) | Guia passo a passo para começar |

### Datasheets e Referências
- [STM8S105 Datasheet](https://www.st.com/resource/en/datasheet/stm8s105k4.pdf)
- [STM8S Reference Manual](https://www.st.com/resource/en/reference_manual/cd00190271.pdf)
- [SDCC User Manual](http://sdcc.sourceforge.net/doc/sdccman.pdf)

## 🛠️ Desenvolvimento

### Comandos Make Disponíveis
```bash
make all      # Compila o projeto
make clean    # Remove arquivos de build
make flash    # Compila e programa o STM8
make size     # Mostra tamanho do programa
make help     # Mostra ajuda
```

### Estrutura de Build
```
build/
├── obj/                    # Arquivos objeto (.rel)
└── cadeira_odonto.ihx     # Arquivo Intel HEX final
```

### Convenções de Código
- Funções: `Module_ActionObject()` (PascalCase)
- Variáveis: `camelCase`
- Constantes: `UPPER_CASE`
- Tipos: `TypeName_t`

## 🔌 Conexão e Programação

### ST-LINK V2 → STM8S105
```
ST-LINK          STM8S105
───────          ────────
3.3V/5V   →      VDD
GND       →      VSS
SWIM      →      SWIM
RST       →      NRST
```

### Programação
```bash
# Via Makefile
make flash

# Ou direto com stm8flash
stm8flash -c stlinkv2 -p stm8s105k4 -w build/cadeira_odonto.ihx
```

## ⚠️ Avisos de Segurança

> **🔴 ATENÇÃO**: Este é um equipamento médico/odontológico

- ⚠️ Realize testes completos antes do uso
- ⚠️ Nunca teste com pacientes sem certificação
- ⚠️ Siga normas NBR IEC 60601
- ⚠️ Use componentes certificados
- ⚠️ Mantenha documentação completa
- ⚠️ Implemente isolação galvânica
- ⚠️ Teste sistemas de segurança regularmente

## 🔄 Versões

### v1.0 (Atual)
- ✅ Controle básico de 3 motores
- ✅ Interface com 7 botões
- ✅ Sistema de segurança multinível
- ✅ Sensores de fim de curso
- ✅ Parada de emergência
- ✅ Monitoramento de corrente
- ✅ Feedback sonoro/visual
- ✅ Comunicação UART básica

### Planejado (v2.0)
- 🔲 Display LCD 16x2
- 🔲 Memória de posições (EEPROM)
- 🔲 Interface serial completa
- 🔲 Modo de teste e diagnóstico
- 🔲 Logging de operações
- 🔲 Modo sleep/baixo consumo
- 🔲 Comunicação CAN Bus
- 🔲 Controle via App Mobile (BLE)

## 🐛 Problemas Conhecidos

Nenhum no momento. Reporte problemas via issues.

## 📧 Contato

**Autor**: Lazaro  
**Email**: [seu-email-aqui]  
**Projeto**: Cadeira Odontológica STM8S  
**Data**: Fevereiro 2026

## 📄 Licença

Projeto proprietário - Todos os direitos reservados.

---

## 🎓 Agradecimentos

- ST Microelectronics pela excelente documentação
- Comunidade SDCC pelo compilador open-source
- Desenvolvedores do stm8flash

---

**Desenvolvido com ❤️ para a área odontológica**

```
  ___          _      _            
 / __|__ _  __| |___ (_)_ _ __ _   
| (__/ _` |/ _` / -_)| | '_/ _` |  
 \___\__,_|\__,_\___|_|_| \__,_|   
                                   
  ___    _         _        _     _         
 / _ \ __| |___ _ _| |_ ___ | |___ (_)__ __ _ 
| (_) / _` / _ \ ' \  _/ _ \| / _ \| / _/ _` |
 \___/\__,_\___/_||_\__\___/|_\___/|_\__\__,_|
                                              
```
