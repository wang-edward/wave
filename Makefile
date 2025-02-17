# Compiler and flags
CC      = gcc
# Add Raylib include path along with your own include directory.
CFLAGS  = -Wall -Wextra -Wno-error -std=c99 -Iinclude -isystem external/raylib/src -g
# Remove -lraylib since we'll compile it from source.
LDFLAGS = -lm -lsoundio -lpthread

# Directories and target
SRC_DIR   = src
BUILD_DIR = build
TARGET    = wave

# Project sources
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Raylib sources (assumed to be in external/raylib/src)
RAYLIB_DIR   = external/raylib
RAYLIB_SRC   = $(RAYLIB_DIR)/src
RAYLIB_SRCS  = $(wildcard $(RAYLIB_SRC)/*.c)
# To avoid filename collisions, prefix Raylib object files with "raylib_"
RAYLIB_OBJS  = $(patsubst $(RAYLIB_SRC)/%.c,$(BUILD_DIR)/raylib_%.o,$(RAYLIB_SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) $(RAYLIB_OBJS)
	$(CC) $(OBJS) $(RAYLIB_OBJS) -o $(TARGET) $(LDFLAGS)

# Compile project source files.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile Raylib source files.
$(BUILD_DIR)/raylib_%.o: $(RAYLIB_SRC)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist.
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all clean
