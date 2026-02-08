# Sistema de Controle de Cadeira Odontológica
## Microcontrolador: STM8S105K4T6C

### ⚡ CÓDIGO COMPLETO EM UM ÚNICO ARQUIVO!
Todo o sistema está implementado no arquivo **`src/main.c`** (~1000 linhas)
- ✅ Máquina de estados principal
- ✅ Controle de motores PWM
- ✅ Interface do usuário (botões, LEDs, buzzer)
- ✅ Sistema de segurança completo
- ✅ Todas as configurações e definições

**Vantagens:**
- 📦 Fácil de entender o fluxo completo
- 🚀 Compilação mais rápida
- 📝 Manutenção simplificada
- 💾 Apenas um arquivo para editar

### Descrição do Projeto
Sistema embarcado para controle de cadeira odontológica utilizando o microcontrolador STM8S105K4T6C.

### Especificações do STM8S105K4T6C
- Core: STM8 de 8 bits
- Frequência: até 16 MHz
- Flash: 16 KB
- RAM: 2 KB
- EEPROM: 1 KB
- Timers: 3x 16-bit
- ADC: 10-bit, 10 canais
- UART: 2
- SPI: 1
- I²C: 1
- GPIOs: 32 pinos I/O

### Funcionalidades do Sistema
1. **Controle de Motores**
   - Motor de elevação da cadeira
   - Motor de inclinação do encosto
   - Motor de inclinação do apoio de pernas
   - Controle PWM para velocidade suave

2. **Interface do Usuário**
   - Botões de controle (subir/descer/inclinar)
   - Display LCD para status
   - LEDs indicadores
   - Buzzer para feedback sonoro

3. **Segurança**
   - Sensores de fim de curso
   - Detecção de sobrecarga
   - Sistema de parada de emergência
   - Proteção contra sobrecorrente

4. **Comunicação**
   - Interface UART para configuração
   - Possível integração com sistema externo

### Estrutura de Diretórios
```
├── src/              # Código fonte - APENAS main.c!
├── inc/              # Header da biblioteca STM8 SPL
├── lib/              # Bibliotecas STM8 (SPL)
├── docs/             # Documentação completa
├── build/            # Arquivos compilados
└── .vscode/          # Configuração VS Code
```

### Compilação
```bash
# Windows (PowerShell)
make clean
make all
make flash

# Linux/Mac
make clean && make all && make flash
```

### Pinout Planejado
- **PA1-PA3**: Controle de motores (PWM)
- **PB0-PB7**: Interface de botões
- **PC3-PC7**: Sensores fim de curso
- **PD5-PD6**: UART (TX/RX)
- **PE0-PE3**: Display LCD
- **ADC Channels**: Sensores de corrente

### Autor
Lazaro
Data: 2025

### Licença
Projeto proprietário - Todos os direitos reservados

---

## 📚 Documentação Completa

### Índice Principal
🔗 **[docs/INDEX.md](docs/INDEX.md)** - Navegue por toda a documentação!

### Documentos Principais

| Documento | Descrição | Quando Usar |
|-----------|-----------|-------------|
| [QUICK_START.md](docs/QUICK_START.md) | Guia passo a passo | Setup inicial |
| [CODIGO_UNICO.md](docs/CODIGO_UNICO.md) | Estrutura do código | Entender organização |
| [RESUMO_VISUAL.md](docs/RESUMO_VISUAL.md) | Mapa visual | Navegação rápida |
| [HARDWARE.md](docs/HARDWARE.md) | Especificações HW | Montagem física |
| [SOFTWARE.md](docs/SOFTWARE.md) | Arquitetura SW | Desenvolvimento |
| [BOM.md](docs/BOM.md) | Lista de materiais | Compras |
| [TROUBLESHOOTING.md](docs/TROUBLESHOOTING.md) | Solução de problemas | Quando algo falha |

---

**Desenvolvido com ❤️ para odontologia - Código limpo em arquivo único! 🚀**
