# Troubleshooting Guide - Cadeira Odontológica

## 🔍 Problemas de Compilação

### ❌ Erro: "sdcc: command not found"

**Sintoma:**
```
'sdcc' is not recognized as an internal or external command
```

**Causa:** SDCC não instalado ou não está no PATH

**Solução:**
1. Baixe o SDCC de http://sdcc.sourceforge.net/
2. Instale o SDCC
3. Adicione ao PATH do Windows:
   ```
   Painel de Controle → Sistema → Configurações Avançadas
   → Variáveis de Ambiente → Path
   → Adicionar: C:\Program Files\SDCC\bin
   ```
4. Reinicie o terminal/VS Code
5. Teste: `sdcc --version`

---

### ❌ Erro: "stm8s.h: No such file or directory"

**Sintoma:**
```
fatal error: stm8s.h: No such file or directory
```

**Causa:** STM8 Standard Peripheral Library não encontrada

**Solução:**
1. Baixe a SPL: https://www.st.com/en/embedded-software/stsw-stm8069.html
2. Extraia os arquivos
3. Copie as pastas:
   ```
   STM8S_StdPeriph_Driver/inc → lib/STM8S_StdPeriph_Driver/inc
   STM8S_StdPeriph_Driver/src → lib/STM8S_StdPeriph_Driver/src
   ```
4. Verifique se existe: `lib/STM8S_StdPeriph_Driver/inc/stm8s.h`

---

### ❌ Erro de Linkagem

**Sintoma:**
```
undefined reference to 'GPIO_Init'
```

**Causa:** Arquivos da biblioteca SPL não estão sendo compilados

**Solução:**
1. Verifique o Makefile
2. Confirme que a linha existe:
   ```makefile
   SOURCES += $(wildcard $(LIB_DIR)/STM8S_StdPeriph_Driver/src/*.c)
   ```
3. Execute: `make clean && make all`

---

## 🔌 Problemas de Programação

### ❌ Erro: "Error: cannot open connection"

**Sintoma:**
```
Error: cannot open connection to ST-LINK
```

**Causas Possíveis:**

**1. ST-LINK não conectado**
- Verifique cabo USB
- Teste em outra porta USB
- LED do ST-LINK deve estar aceso

**2. Drivers não instalados (Windows)**
- Baixe ST-LINK Utility
- Instale os drivers incluídos
- Ou use Zadig para instalar drivers WinUSB

**3. Conexões incorretas**
Verifique pinos:
```
ST-LINK → STM8
VDD     → VDD (pinos 8/32)
GND     → VSS (pinos 7/31)
SWIM    → SWIM
NRST    → NRST (pino 9)
```

**4. Alimentação**
- STM8 deve estar alimentado com 3.3V ou 5V
- Verifique com multímetro: VDD = 3.3V ou 5V
- LED de power deve estar aceso (se houver)

---

### ❌ Erro: "Error: device not found"

**Sintoma:**
```
Error: device not found (0x0000)
```

**Soluções:**

1. **Verifique o modelo no comando:**
   ```bash
   # Correto para STM8S105K4:
   stm8flash -c stlinkv2 -p stm8s105k4 -w file.ihx
   
   # NOT: stm8s105k6 ou stm8s105c6
   ```

2. **Teste conexão:**
   ```bash
   stm8flash -c stlinkv2 -p stm8s105k4
   ```

3. **Reset manual:**
   - Conecte NRST ao GND por 1 segundo
   - Solte e tente programar imediatamente

---

### ❌ MCU não responde após programação

**Sintoma:**
```
Programação OK, mas o MCU não executa o código
```

**Soluções:**

1. **Verifique arquivo .ihx:**
   ```bash
   # Arquivo deve existir e ter tamanho > 0
   dir build\cadeira_odonto.ihx
   ```

2. **Opção de proteção (Option Bytes):**
   ```bash
   # Desbloqueie com ST Visual Programmer:
   # Option Bytes → Read Out Protection → Disabled
   ```

3. **Reset após programação:**
   - Desconecte e reconecte alimentação
   - Ou: pressione botão RESET

4. **Clock incorreto:**
   - Verifique oscilador externo (se usado)
   - Ou use HSI interno (padrão no código)

---

## ⚡ Problemas Elétricos

### ❌ LED de status não acende

**Diagnóstico:**

1. **Teste continuidade:**
   ```
   LED → Resistor → GPIO (PD0)
   ```

2. **Verifique polaridade do LED:**
   - Perna longa (+) → Resistor → VDD
   - Perna curta (-) → GPIO

3. **Teste o GPIO manualmente:**
   - Conecte LED diretamente entre VDD e GND (com resistor)
   - Deve acender

4. **Valor do resistor:**
   - LED vermelho: 470Ω @ 5V
   - Verifique com código de cores

---

### ❌ Motores não funcionam

**Diagnóstico por etapas:**

**1. Verifique alimentação dos drivers:**
```
Multímetro:
- Entrada driver: 24V DC
- Saída 5V do driver: 5V DC (para lógica)
```

**2. Teste drivers isoladamente:**
```
- Desconecte STM8
- Aplique 5V manualmente nos pinos de controle
- Motor deve girar
```

**3. Verifique sinais PWM:**
```
- Use osciloscópio ou LED de teste
- Pino PA1 deve ter PWM quando ativado
- Frequência: ~1kHz
```

**4. Corrente do motor:**
```
- Meça corrente sem carga: < 1A
- Meça corrente com carga: 3-5A
- Se > 5A: motor travado ou problema mecânico
```

**5. Conexões L298N:**
```
STM8        L298N       Motor
────        ─────       ─────
PA1 PWM  →  ENA         
PA2      →  IN1         
GND      →  IN2         
                        OUT1 → M+
                        OUT2 → M-
24V      →  12V/24V     
GND      →  GND         
```

---

### ❌ Buzzer não emite som

**Diagnóstico:**

1. **Tipo de buzzer:**
   - Buzzer ATIVO: 2 fios, 5V direto faz soar
   - Buzzer PASSIVO: precisa de PWM

2. **Teste direto:**
   ```
   Buzzer+ → 5V
   Buzzer- → GND
   Deve soar (se ativo)
   ```

3. **Polaridade:**
   - Verifique marcação +/-
   - Inverta se não funcionar

4. **GPIO:**
   - PE0 configurado como output?
   - Teste com LED no lugar

---

## 🛡️ Problemas de Segurança

### ❌ Sistema trava em EMERGENCY_STOP

**Sintoma:**
```
LED vermelho aceso permanentemente
Motores não respondem
```

**Solução:**

1. **Verifique botão de emergência:**
   ```
   - Deve ser tipo NA+NF (contatos duplos)
   - Teste continuidade
   - Desconectar pino deve liberar
   ```

2. **Reset do sistema:**
   ```
   - Pressione BTN_RESET (PB7)
   - LED vermelho deve apagar
   - Sistema volta ao IDLE
   ```

3. **Lógica invertida:**
   ```c
   // Código usa pull-up interno
   // Botão PRESSIONADO = GND = 0
   // Botão SOLTO = Pull-up = 1
   ```

---

### ❌ Fim de curso não detectado

**Sintoma:**
```
Motor continua girando após limite
Sistema não para
```

**Diagnóstico:**

1. **Teste sensor:**
   ```
   Multímetro em continuidade:
   - Sensor não acionado: aberto
   - Sensor acionado: fechado (0Ω)
   ```

2. **Leitura do GPIO:**
   ```c
   // Adicione no código para debug:
   uint8_t limit = GPIO_ReadInputPin(LIMIT_PORT, LIMIT_UP_PIN);
   // Via UART: imprima o valor
   ```

3. **Pull-up habilitado:**
   ```c
   GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_NO_IT);
   //                              ^^^^^ Importante!
   ```

4. **Posicionamento mecânico:**
   - Sensor muito longe: não aciona
   - Sensor muito perto: sempre acionado
   - Ajuste distância: 1-2mm ideal

---

## 📡 Problemas de Comunicação UART

### ❌ Não recebe dados na serial

**Diagnóstico:**

1. **Configuração terminal:**
   ```
   Baud rate: 9600
   Data bits: 8
   Stop bits: 1
   Parity: None
   Flow control: None
   ```

2. **Conexões:**
   ```
   STM8 TX (PD5) → RX do conversor USB
   STM8 RX (PD6) → TX do conversor USB
   GND           → GND
   
   Atenção: TX conecta em RX e vice-versa!
   ```

3. **Nível de tensão:**
   - STM8 usa 5V ou 3.3V
   - Conversor USB compatível?
   - Use conversor 3.3V-5V se necessário

4. **Teste loopback:**
   ```
   Conecte TX e RX do STM8
   Envie caractere → deve receber de volta
   ```

---

## 🔧 Problemas de Performance

### ❌ Sistema lento/travando

**Causas:**

1. **Loop principal bloqueado:**
   ```c
   // Evite delays longos no loop:
   while(1) {
       System_Process();
       // delay_ms(10); OK
       // delay_ms(1000); RUIM!
   }
   ```

2. **Watchdog ativo:**
   ```c
   // Se usar watchdog, realize reset periódico:
   IWDG_ReloadCounter();
   ```

3. **Interrupções desabilitadas:**
   ```c
   // Certifique-se de habilitar no final da init:
   enableInterrupts();
   ```

---

### ❌ Corrente muito alta

**Sintoma:**
```
ADC lê > 5A constantemente
Sistema entra em erro
```

**Diagnóstico:**

1. **Sem carga:**
   - Desconecte motores
   - Leitura deve ser ~0A

2. **Calibração do sensor:**
   ```c
   // ACS712-20A:
   // 0A = 2.5V = 512 (ADC 10-bit)
   // ±1A = ±0.1V = ±20 counts
   ```

3. **Motor travado:**
   - Verifique parte mecânica
   - Motor deve girar livremente (sem carga)

4. **Short no driver:**
   - Desligue alimentação
   - Teste resistência OUT1-OUT2
   - Deve ser alta (>10kΩ)

---

## 📋 Checklist de Depuração

### Antes de ligar:
- [ ] Alimentação correta (5V para MCU, 24V para motores)
- [ ] Todas as conexões conferidas
- [ ] Sem curtos-circuitos (teste com multímetro)
- [ ] Polaridade correta em todos os componentes
- [ ] Fusíveis instalados

### Primeira Inicialização:
- [ ] LED de alimentação aceso
- [ ] LED de status pisca 3x (inicialização)
- [ ] 2 beeps curtos (inicialização)
- [ ] LED de status permanece aceso (idle)

### Teste de Botões:
- [ ] Cada botão causa resposta (LED pisca)
- [ ] Botão de emergência trava sistema (LED vermelho)
- [ ] Reset libera emergência

### Teste de Motores (SEM CARGA):
- [ ] Motor 1 gira nos dois sentidos
- [ ] Motor 2 gira nos dois sentidos
- [ ] Motor 3 gira nos dois sentidos
- [ ] Velocidade controlável via PWM

### Teste de Segurança:
- [ ] Fim de curso para motor
- [ ] Emergência para todos os motores
- [ ] Sobrecorrente detectada (teste simulado)
- [ ] Timeout funciona (após 30s)

---

## 🆘 Quando Pedir Ajuda

Se após seguir este guia o problema persiste:

1. **Reúna informações:**
   - Versão do firmware
   - Hardware utilizado (drivers, motores, etc.)
   - Mensagens de erro exatas
   - O que foi tentado

2. **Documentação:**
   - Tire fotos das conexões
   - Screenshots do erro
   - Logs da serial (se disponível)

3. **Medições:**
   - Tensões nos pontos principais
   - Correntes medidas
   - Sinais PWM (se tiver osciloscópio)

---

**Última atualização:** Fevereiro 2026  
**Versão:** 1.0
