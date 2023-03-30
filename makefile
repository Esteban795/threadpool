SRCDIR = src
HEADDIR = include
LIBDIR = ./src

CFLAGS = -W -Wall -Wextra -Wvla -fsanitize=address

build:
	gcc $(SRCDIR)/threadpool.c -o ./bin/threadpool $(CFLAGS) 

run:
	./bin/threadpool

clean:
	rm ./bin/threadpool

all:
	make build
	make run
