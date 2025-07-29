CC = gcc
CFLAGS = -Iinclude -Isrc
SRC = src/main.c src/logfire.c src/parser.c src/cli.c src/formatter.c
OUT = logfire

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
