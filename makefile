# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

# Source and object files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/shell.c $(SRC_DIR)/execute.c
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/shell.o $(OBJ_DIR)/execute.o

# Compiler and flags
CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -g
LDFLAGS = -lreadline   # Link with GNU Readline

# Target binary
TARGET = $(BIN_DIR)/myshell

# Default target
all: $(TARGET)

# Link object files to create binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary directories if they don't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

# Run the shell (builds first if needed)
run: $(TARGET)
	@$(TARGET)
.PHONY: all clean run

