CC := gcc
CFLAGS := -Wall -Wextra -O2 `pkg-config --cflags glib-2.0 gio-2.0 libxml-2.0`
LDFLAGS := `pkg-config --libs glib-2.0 gio-2.0 libxml-2.0`
PLANTUML_CMD := plantuml

SRC_DIR := tests
BIN_DIR := bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_FILES))
ELF_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRC_FILES))
UML_FILES := $(wildcard $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.uml, $(SRC_FILES)))
PNG_FILES := $(patsubst $(SRC_DIR)/%.uml, $(BIN_DIR)/%.png, $(UML_FILES))

all: check_plantuml $(ELF_FILES) $(PNG_FILES)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%: $(BIN_DIR)/%.o
	$(CC) $< -o $@ $(LDFLAGS)

$(BIN_DIR)/%.png: $(SRC_DIR)/%.uml | $(BIN_DIR)
	$(PLANTUML_CMD) -tpng $< -o $(BIN_DIR)

.PHONY: clean
clean:
	rm -rf $(OBJ_FILES) $(ELF_FILES) $(PNG_FILES)

.PHONY: check_plantuml
check_plantuml:
	@command -v $(PLANTUML_CMD) >/dev/null 2>&1 || { echo >&2 "Warning: PlantUML is not installed. Please install it to generate PNG files."; exit 1; }
