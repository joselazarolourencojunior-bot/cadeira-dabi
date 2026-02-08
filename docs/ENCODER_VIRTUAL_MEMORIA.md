# 📊 Sistema de Encoder Virtual e Memória

## 🎯 Visão Geral

Sistema de **encoder virtual baseado em tempo** que memoriza a posição atual do assento e encosto na EEPROM interna do STM8S105K4T6C.

---

## ⚙️ Funcionamento

### **Encoder Virtual por Tempo**

O sistema não utiliza encoders físicos. A posição é calculada baseada no **tempo de acionamento dos relés**:

```
Posição (ms) = Tempo Total de Movimento
```

- **Curso Total**: 30.000ms (30 segundos) para assento e encosto
- **Posição Mínima**: 0ms (limite inferior)
- **Posição Máxima**: 30.000ms (limite superior)

### **Atualização em Tempo Real**

```c
// A cada ciclo (10ms), o encoder é atualizado:

Se Sobe Assento ativo:
    posicaoAssento += deltaTime
    
Se Desce Assento ativo:
    posicaoAssento -= deltaTime
    
Se Sobe Encosto ativo:
    posicaoEncosto += deltaTime
    
Se Desce Encosto ativo:
    posicaoEncosto -= deltaTime
```

---

## 💾 Sistema de Memória EEPROM

### **Endereços de Memória**

| Dado | Endereço | Tamanho | Descrição |
|------|----------|---------|-----------|
| **Posição Assento** | 0x4000 | 4 bytes | Posição em ms (0-30000) |
| **Posição Encosto** | 0x4004 | 4 bytes | Posição em ms (0-30000) |
| **Flag Calibrado** | 0x4008 | 1 byte | 0xAB = calibrado, 0x00 = não calibrado |

### **Salvamento Automático**

- **Intervalo**: A cada **5 segundos** durante movimento
- **Boot**: Carrega posições salvas da EEPROM
- **Validação**: Número mágico `0xAB` confirma dados válidos

---

## 🔧 Calibração do Sistema

### **Primeira Inicialização**

```
1. Sistema liga → EEPROM vazia → calibrado = 0
2. Obrigatório executar VZ (Volta a Zero)
3. VZ completa → Encoder calibra (posição = 0)
4. Salva na EEPROM → calibrado = 1
```

### **Após Calibração**

```
Boot → Carrega EEPROM → Recupera última posição
```

### **Recalibração**

Executar ciclo VZ completo:
- Zera encoder (posição = 0)
- Salva na EEPROM
- PT fica liberado

---

## 📊 Estrutura de Dados

```c
typedef struct {
    uint32_t posicaoAssento;      /* 0-30000 ms */
    uint32_t posicaoEncosto;      /* 0-30000 ms */
    uint32_t lastUpdateTime;      /* Timestamp última atualização */
    uint32_t lastSaveTime;        /* Timestamp último save EEPROM */
    uint8_t calibrado;            /* 0=não, 1=sim */
    uint8_t movimentoAtivo;       /* 0=parado, 1=movimento */
} EncoderVirtual_t;
```

---

## 🛠️ Funções Disponíveis

### **Inicialização**
```c
void Encoder_Init(void);
```
- Desbloqueia EEPROM
- Carrega dados salvos
- Inicializa em zero se não calibrado

### **Atualização**
```c
void Encoder_Update(void);
```
- **Chamada**: A cada 10ms no loop principal
- **Função**: Atualiza posições baseado no movimento
- **Auto-save**: Salva EEPROM a cada 5s

### **Calibração**
```c
void Encoder_Calibrar(void);
```
- **Quando**: Fim do ciclo VZ
- **Ação**: Zera posições e marca calibrado

### **Consulta de Posição**

```c
uint32_t Encoder_GetPosicaoAssento(void);    // Retorna 0-30000ms
uint32_t Encoder_GetPosicaoEncosto(void);    // Retorna 0-30000ms

uint8_t Encoder_GetPercentualAssento(void);  // Retorna 0-100%
uint8_t Encoder_GetPercentualEncosto(void);  // Retorna 0-100%
```

### **Persistência**

```c
void Encoder_SaveToEEPROM(void);   // Salva manualmente
void Encoder_LoadFromEEPROM(void); // Carrega manualmente
```

---

## 🔄 Fluxo de Operação

### **Primeira Vez (Sem Calibração)**

```
┌──────────────┐
│ BOOT SISTEMA │  calibrado = 0, posição = 0
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Executar VZ  │  Obrigatório para calibrar
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ VZ Completo  │  Encoder_Calibrar() → posição = 0
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Salva EEPROM │  calibrado = 1, posições salvas
└──────────────┘
```

### **Próximas Inicializações (Já Calibrado)**

```
┌──────────────┐
│ BOOT SISTEMA │  calibrado = 1
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Carrega EEPROM│ Recupera: posição assento e encosto
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Sistema Pronto│ Posições restauradas
└──────────────┘
```

### **Durante Uso**

```
┌─────────────────┐
│ Movimento Manual│  Atualiza encoder em tempo real
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ A cada 5 segundos│ Auto-save na EEPROM
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ Sistema Desliga │ Última posição salva
└─────────────────┘
```

---

## 📈 Exemplo de Uso

### **Consultar Posição Atual**

```c
uint32_t posAssento = Encoder_GetPosicaoAssento();
uint8_t percentAssento = Encoder_GetPercentualAssento();

// Exemplo: posAssento = 15000ms = 50% do curso
```

### **Verificar Calibração**

```c
if (encoder.calibrado) {
    // Sistema calibrado - posições confiáveis
} else {
    // Requer calibração (executar VZ)
}
```

### **Forçar Salvamento**

```c
// Salva imediatamente (exemplo: antes de desligar)
Encoder_SaveToEEPROM();
```

---

## ⚠️ Pontos Importantes

### **Precisão**

- ✅ Baseado em tempo, não em pulsos
- ✅ Precisão: ±10ms (resolução de 1 tick)
- ⚠️ Sofre desvio se motores variarem velocidade
- ⚠️ Calibração periódica recomendada (VZ)

### **Durabilidade EEPROM**

- **Ciclos de escrita**: 100.000 garantidos
- **Intervalo de save**: 5 segundos
- **Vida útil estimada**: 
  ```
  100.000 ciclos ÷ (3600s/5s) = 138 horas contínuas
  Com uso intermitente: muitos anos
  ```

### **Limites Físicos**

- Sensores de limite têm prioridade
- Se sensor ativar antes de 30s, encoder ajusta automaticamente
- VZ recalibra o zero

---

## 🎯 Vantagens do Sistema

✅ **Sem hardware adicional** - Usa apenas tempo  
✅ **Memória persistente** - Sobrevive a quedas de energia  
✅ **Auto-calibração** - VZ reseta a referência  
✅ **Tempo real** - Atualização contínua  
✅ **Baixo custo** - CPU + EEPROM interna  

---

## 📝 Resumo Técnico

| Característica | Valor |
|----------------|-------|
| **Método** | Encoder virtual por tempo |
| **Resolução** | 10ms (1 tick) |
| **Curso Total** | 30.000ms (30s) |
| **Memória** | EEPROM interna (1KB disponível) |
| **Endereços usados** | 0x4000 - 0x4008 (9 bytes) |
| **Intervalo Save** | 5 segundos |
| **Calibração** | Automática via VZ |
| **Persistência** | 100.000 ciclos de escrita |

---

**Desenvolvido por:** Lazaro  
**Data:** 08/Fevereiro/2026  
**Hardware:** STM8S105K4T6C (1KB EEPROM)
