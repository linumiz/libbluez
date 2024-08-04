CC := gcc
CFLAGS := -Wall -Wextra -O2 `pkg-config --cflags glib-2.0 gio-2.0`
LDFLAGS := `pkg-config --libs glib-2.0 gio-2.0`

SRC_DIR := tests
BIN_DIR := bin

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_FILES))
ELF_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRC_FILES))

all: $(ELF_FILES)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/%: $(BIN_DIR)/%.o
	$(CC) $< -o $@ $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(BIN_DIR)/*.o $(ELF_FILES)
