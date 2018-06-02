# definice konstant a cest
CC = gcc
CFLAGS = -Wall -pedantic -W -std=c99 -lm -O3
TARGET = ifj2016

BIN = ./
OBJ = ./
SRC = ./
DOC = doc
HEADERS = ./
COMPOSE = bin/compose/

# Získání seznamu souborů v jednotlivých složkách
SOURCES   := $(wildcard $(SRC)/*.c)
INCLUDES  := $(wildcard $(HEADERS)/*.h)
# Vygenerování jmén .o souborů
OBJECTS   := $(SOURCES:$(SRC)/%.c=$(OBJ)/%.o)

# Příkazy pro vytvoření složek
MKDIR_OBJ := mkdir -p $(OBJ)
MKDIR_BIN := mkdir -p $(BIN)
MKDIR_COM := mkdir -p $(COMPOSE)

# Make ALL, linkuje, pokud jsou soubory změněny
$(BIN): $(OBJECTS)
	@$(MKDIR_BIN)
	@$(CC) $(CFLAGS) -o $(BIN)/$(TARGET)  $(OBJECTS)
	@echo "Linking complete!"
# Make ALL, kompipuje změněné soubory a připravuje .o pro linkování
$(OBJECTS): $(OBJ)/%.o : $(SRC)/%.c
	@$(MKDIR_OBJ)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"


