# Makefile para STM8S105K4T6C - Cadeira Odontológica
# Compilador: SDCC (Small Device C Compiler)

# ====================
# Configurações
# ====================
PROJECT_NAME = cadeira_odonto
MCU = stm8s105
F_CPU = 16000000

# Diretórios
SRC_DIR = src
INC_DIR = inc
LIB_DIR = lib
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Ferramentas
CC = sdcc
FLASH = stm8flash
OBJCOPY = objcopy

# Flags do compilador
CFLAGS = -mstm8 
CFLAGS += --opt-code-size
CFLAGS += -D STM8S105
CFLAGS += -D F_CPU=$(F_CPU)UL
CFLAGS += -I$(INC_DIR)
CFLAGS += -I$(LIB_DIR)/STM8S_StdPeriph_Driver/inc
CFLAGS += --out-fmt-ihx
CFLAGS += --stack-auto
CFLAGS += --std-sdcc99

# Flags do linker
LDFLAGS = -mstm8
LDFLAGS += --out-fmt-ihx

# Arquivos fonte (APENAS MAIN.C - TUDO EM UM ARQUIVO)
SOURCES = $(SRC_DIR)/main.c
SOURCES += $(wildcard $(LIB_DIR)/STM8S_StdPeriph_Driver/src/*.c)

# Arquivos objeto
OBJECTS = $(patsubst %.c,$(OBJ_DIR)/%.rel,$(notdir $(SOURCES)))

# Arquivo de saída
OUTPUT = $(BUILD_DIR)/$(PROJECT_NAME)

# ====================
# Regras
# ====================

.PHONY: all clean flash directories

all: directories $(OUTPUT).ihx
	@echo "===================================="
	@echo "Build completo!"
	@echo "Arquivo gerado: $(OUTPUT).ihx"
	@echo "===================================="

directories:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

$(OUTPUT).ihx: $(OBJECTS)
	@echo "Linkando..."
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR)/%.rel: $(SRC_DIR)/%.c
	@echo "Compilando $<..."
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.rel: $(LIB_DIR)/STM8S_StdPeriph_Driver/src/%.c
	@echo "Compilando biblioteca $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Limpando arquivos de build..."
	@if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	@echo "Limpeza concluída!"

flash: all
	@echo "Programando STM8S105..."
	$(FLASH) -c stlinkv2 -p $(MCU) -w $(OUTPUT).ihx
	@echo "Programação concluída!"

size: all
	@echo "Tamanho do programa:"
	@dir "$(OUTPUT).ihx"

help:
	@echo "Comandos disponíveis:"
	@echo "  make all     - Compila o projeto"
	@echo "  make clean   - Remove arquivos de build"
	@echo "  make flash   - Compila e programa o STM8"
	@echo "  make size    - Mostra o tamanho do programa"
	@echo "  make help    - Mostra esta ajuda"
