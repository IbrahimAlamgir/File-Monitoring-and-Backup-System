```makefile
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN = monitor_system

SRC = $(SRC_DIR)/main.c $(SRC_DIR)/scanner.c $(SRC_DIR)/backup.c
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: directories $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

directories:
	@mkdir -p $(OBJ_DIR) backup logs

clean:
	rm -rf $(OBJ_DIR) $(BIN)
	rm -rf logs/*.txt

.PHONY: all clean directories
