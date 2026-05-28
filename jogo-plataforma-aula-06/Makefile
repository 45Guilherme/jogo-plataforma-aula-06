# Makefile forçando GCC no Windows
CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-g++
CFLAGS = -Isrc/ -Isrc/include/ -MMD -MP -O1 -Wall -Wextra -Wno-unused-parameter -pedantic-errors -std=c99 -Wno-missing-braces
LDFLAGS = -Llib/ -L"C:/Strawberry/c/x86_64-w64-mingw32/lib" -lraylib -lmingwex -lmingw32 -lopengl32 -lgdi32 -lwinmm -lm

BUILD_DIR = build
SRC_DIRS = src
SRCS = $(wildcard $(SRC_DIRS)/*.c)
OBJS = $(SRCS:$(SRC_DIRS)/%.c=$(BUILD_DIR)/%.o)
TARGET = sonic.exe

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "✅ Compilado! Execute: $(BUILD_DIR)/$(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(BUILD_DIR)
	@rm -f $(TARGET)

run: $(BUILD_DIR)/$(TARGET)
	./$(BUILD_DIR)/$(TARGET)

.PHONY: all clean run