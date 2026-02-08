# Matriz de Teclado 3x4

## Descrição
Sistema de leitura de matriz de teclado 3x4 (12 teclas) para controle da cadeira odontológica.

## Hardware

### Conexões

#### Linhas (Saídas - GPIOD)
- **ROW1**: PD3 (GPIO_PIN_3)
- **ROW2**: PD4 (GPIO_PIN_4)
- **ROW3**: PD5 (GPIO_PIN_5)

#### Colunas (Entradas com Pull-up - GPIOF)
- **COL1**: PF4 (GPIO_PIN_4)
- **COL2**: PF5 (GPIO_PIN_5)
- **COL3**: PF6 (GPIO_PIN_6)
- **COL4**: PF7 (GPIO_PIN_7)

### Layout Físico da Matriz

```
           COL1    COL2    COL3    COL4
           PF4     PF5     PF6     PF7
            |       |       |       |
ROW1 PD3 ---|---[1]---[2]---[3]---[A]
            |       |       |       |
ROW2 PD4 ---|---[4]---[5]---[6]---[B]
            |       |       |       |
ROW3 PD5 ---|---[7]---[8]---[9]---[C]
            |       |       |       |
```

## Mapeamento de Funções

| Tecla | Função |
|-------|--------|
| **1** | Sobe Assento (pulso 100ms) |
| **2** | Desce Assento (pulso 100ms) |
| **3** | Sobe Encosto (pulso 100ms) |
| **4** | Desce Encosto (pulso 100ms) |
| **5** | Toggle Refletor ON/OFF |
| **6** | Volta a Zero (automático 30s) |
| **7** | Posição de Trabalho (automático 30s) |
| **8** | **Salvar** posição na memória |
| **9** | **Executar** posição memorizada |
| **A** | Função customizável (disponível) |
| **B** | Função customizável (disponível) |
| **C** | Parada de Emergência |

## Funcionamento

### Princípio de Scan

1. **Inicialização**: Todas as linhas em HIGH, colunas com pull-up interno
2. **Scan sequencial**: 
   - Para cada linha (1-3):
     - Coloca linha atual em LOW
     - Mantém outras linhas em HIGH
     - Lê estado das 4 colunas
     - Se coluna em LOW = tecla pressionada
3. **Detecção**: Identifica par [linha, coluna] e mapeia para código da tecla

### Debounce

- Tempo: **50ms**
- Método: Temporal (ignora leituras rápidas)
- Garante única leitura por pressionamento

### Segurança

- Processamento apenas em `SYSTEM_IDLE`
- Verificação de limites antes de acionar motores
- Teclas 1-4: pulsos curtos (100ms) para controle incremental
- Tecla C: parada de emergência imediata

## Uso no Código

### Funções Principais

```c
void Keypad_Init(void);           // Inicializa matriz
uint8_t Keypad_Scan(void);        // Scan de baixo nível
uint8_t Keypad_GetKey(void);      // Obtém tecla com debounce
void Keypad_ProcessKey(uint8_t key); // Processa ação da tecla
```

### Integração no Loop

```c
/* Loop principal (main.c) */
uint8_t key = Keypad_GetKey();
if (key != KEY_NONE)
{
    Keypad_ProcessKey(key);
}
```

## Extensibilidade

### Adicionar Nova Função

Para customizar teclas A ou B:

1. Edite `Keypad_ProcessKey()` em main.c
2. Adicione case para `KEY_A` ou `KEY_B`
3. Implemente a lógica desejada

Exemplo:
```c
case KEY_A:
    /* Tecla A - Exemplo: Liga todos os relés por 2s */
    Rele_SobeAssento(RELE_ON);
    Rele_SobeEncosto(RELE_ON);
    delay_ms(2000);
    Rele_StopAll();
    break;
```

## Diagrama de Conexão

### Teclado Matricial 3x4

```
Teclado              STM8S105K4T6C
-------              --------------
ROW1 -----------------> PD3
ROW2 -----------------> PD4
ROW3 -----------------> PD5

COL1 -----------------> PF4 (pull-up interno)
COL2 -----------------> PF5 (pull-up interno)
COL3 -----------------> PF6 (pull-up interno)
COL4 -----------------> PF7 (pull-up interno)
```

**Nota**: Não são necessários resistores externos, pois o STM8 possui pull-ups internos nas colunas.

## Consumo de Pinos

- **Total**: 7 pinos GPIO
- **Economia**: Controle de 12 teclas usando apenas 7 pinos (ao invés de 12)
- **Disponíveis**: Teclas A e B para futuras expansões

## Performance

- **Taxa de scan**: ~100 Hz (10ms por ciclo completo do loop)
- **Latência**: < 60ms (debounce 50ms + scan 10ms)
- **Múltiplas teclas**: Não suportado (apenas uma tecla por vez)

## Observações

1. **Movimentos incrementais**: Teclas 1-4 dão pulsos curtos para controle preciso
2. **Movimentos automáticos**: Teclas 6 e 7 executam sequências completas
3. **Memória**: Teclas 8 e 9 para salvar/recuperar posições
4. **Segurança**: Tecla C para parada imediata
5. **Refletor**: Tecla 5 para controle ON/OFF da luz

---

**Versão**: 1.0  
**Data**: 08/02/2026  
**Autor**: Lazaro
