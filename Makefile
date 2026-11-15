CC      := clang
CFLAGS  := -std=c17 -Wall -Wextra -g -Iinclude -MMD -MP
LDFLAGS :=
LDLIBS  := -lm

SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=obj/%.o)
BIN := bin/bmpf

$(BIN): $(OBJ) | bin
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

bin obj:
	mkdir -p $@

run: $(BIN)
	./$(BIN)

clean:
	rm -rf obj/* bin/*

.PHONY: run clean
-include $(OBJ:.o=.d)
