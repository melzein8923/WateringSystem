CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Ihardware
LDFLAGS = -lm -lgpiod
SRC = $(wildcard src/*.c) $(wildcard hardware/*.c)
TARGET = irrigation_controller

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
