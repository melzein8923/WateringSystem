CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = $(wildcard src/*.c)
TARGET = irrigation_controller
all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)
run: all
	./$(TARGET)
clean:
	rm -f $(TARGET)
