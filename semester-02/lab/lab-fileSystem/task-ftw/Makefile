CC := gcc
CFLAGS := -fPIC -Wall -Wextra -Werror
HEADERS_DIR := lib/
VPATH = lib/

SRC := $(wildcard lib/*.c)
OBJ := $(patsubst lib/%.c,lib/%.o,$(SRC))

app.out: main.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

lib/%.o: lib/%.c
	$(CC) -c -I $(HEADERS_DIR) $< -o $@ $(CFLAGS)

main.o: main.c
	$(CC) -c -I $(HEADERS_DIR) $< -o $@ $(CFLAGS)

clean:
	rm -f *.o lib/*.o app.out

run: app.out
	./app.out ~/studying/sem6/

test: app.out
	./app.out ~/studying/
