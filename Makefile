# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude -g
LDFLAGS = -lm -lraylib -lsoundio -lpthread

# Directories and target
SRC_DIR   = src
BUILD_DIR = build
TARGET    = wave

# List all .c files in src, then compute object files in build
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule: compile each .c file from src into a .o file in build.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist.
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
