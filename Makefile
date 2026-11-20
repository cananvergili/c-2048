CC      ?= gcc
CFLAGS  ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
TARGET  := 2048
TEST_BIN := test_game

SRCS := src/main.c src/game.c src/render.c src/input.c src/storage.c
TEST_SRCS := tests/test_game.c src/game.c src/storage.c

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): $(SRCS) src/game.h src/render.h src/input.h src/storage.h
	$(CC) $(CFLAGS) -o $@ $(SRCS)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_SRCS) src/game.h src/storage.h
	$(CC) $(CFLAGS) -o $(TEST_BIN) $(TEST_SRCS)
	./$(TEST_BIN)

clean:
	rm -f $(TARGET) $(TARGET).exe $(TEST_BIN) $(TEST_BIN).exe *.obj
