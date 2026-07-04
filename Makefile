# Makefile pour α-Redis en C
# Compilateur et flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS = 

# Répertoires
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# Fichiers sources
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/engine.c \
          $(SRC_DIR)/hash_table.c \
          $(SRC_DIR)/linked_list.c

# Fichiers objets
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Exécutable
TARGET = $(BIN_DIR)/aredis

# Règle par défaut
all: $(TARGET)

# Créer les répertoires
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compiler les fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Éditer les liens
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "✓ Build successful! Run with: ./$(TARGET)"

# Tests
test: $(TARGET)
	@echo "Running unit tests..."
	$(CC) $(CFLAGS) tests/test_aredis.c $(OBJECTS) -o $(BIN_DIR)/test_aredis
	@echo ""
	@$(BIN_DIR)/test_aredis

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "✓ Clean complete"

# Nettoyage complet (avec l'ancien code Python)
distclean: clean
	rm -f aredis/*.pyc
	rm -rf __pycache__
	rm -rf aredis/__pycache__
	@echo "✓ Distclean complete"

# Installation (copier dans /usr/local/bin)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	@echo "✓ Installed to /usr/local/bin/aredis"

# Désinstallation
uninstall:
	rm -f /usr/local/bin/aredis
	@echo "✓ Uninstalled"

# Aide
help:
	@echo "α-Redis Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build the project (default)"
	@echo "  test      - Run basic tests"
	@echo "  clean     - Remove build files"
	@echo "  distclean - Remove all generated files"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  make              # Build"
	@echo "  make test         # Test"
	@echo "  ./bin/aredis      # Run"

# Phony targets
.PHONY: all clean distclean install uninstall test help