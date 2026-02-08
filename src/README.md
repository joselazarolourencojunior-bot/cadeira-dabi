# Código Fonte

## 📄 Arquivo Único: main.c

Todo o sistema de controle da cadeira odontológica está implementado em **`main.c`** (~1000 linhas).

### 🎯 Estrutura Interna

O arquivo está organizado em 10 seções bem definidas:

1. **Includes** - Bibliotecas necessárias
2. **Configurações** - Todos os #defines do hardware
3. **Tipos** - Enumerações e estruturas
4. **Variáveis Globais** - Estado do sistema
5. **Protótipos** - Declaração de todas as funções
6. **main()** - Função principal
7. **Sistema** - Inicialização e máquina de estados
8. **Motores** - Controle PWM e movimentação
9. **Interface** - Botões, LEDs e buzzer
10. **Segurança** - Sensores e proteções

### 📖 Leia Mais

Veja [docs/CODIGO_UNICO.md](../docs/CODIGO_UNICO.md) para detalhes completos sobre:
- Mapa do código
- Como navegar
- Como modificar
- Boas práticas

### ⚡ Compilação

```bash
make clean
make all
```

**Resultado:** Um único arquivo objeto + bibliotecas STM8 SPL

### 🔍 Navegação Rápida

**VS Code:**
- `Ctrl + Shift + O` → Lista todas as funções
- `Ctrl + F` → Buscar função
- `Ctrl + G` → Ir para linha

**Buscar por seção:**
- `/SEÇÃO 1` → Includes
- `/SEÇÃO 2` → Configurações
- `/SEÇÃO 3` → Tipos
- `/SEÇÃO 6` → main()
- `/SEÇÃO 7` → Sistema
- `/SEÇÃO 8` → Motores
- `/SEÇÃO 9` → Interface
- `/SEÇÃO 10` → Segurança

---

**Simplicidade é a chave! 🚀**
