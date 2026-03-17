CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
LDFLAGS = -lX11

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

TARGET = gravity-sim

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

run: all
	./$(TARGET)