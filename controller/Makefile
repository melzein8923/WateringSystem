CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Ihardware -lm
SRC = $(wildcard src/*.c) $(wildcard hardware/*.c)
TARGET = irrigation_controller

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
