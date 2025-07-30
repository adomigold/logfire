CC = gcc
CFLAGS = -Iinclude -Isrc
SRC = src/main.c src/logfire.c src/parser.c src/query.c src/formatter.c src/cli.c
OUT = logfire

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)
