# Makefile for wave synthesizer with raylib and libsoundio

CC = clang
TARGET = wave
SOURCES = main.c

CFLAGS = -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib -lsoundio -lraylib -lm

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
	rm -f $(TARGET)
