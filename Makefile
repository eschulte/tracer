CC=gcc
CFLAGS=-static -Ofast
ARCH=$(shell getconf LONG_BIT)

all: tracer hello

hello: hello.c
	$(CC) -o hello hello.c

tracer: tracer-$(ARCH).c libelf.c
	$(CC) -o $@ $(CFLAGS) tracer-$(ARCH).c libelf.c
	strip -s $@

clean:
	rm -f hello tracer

check: hello tracer
	time ./tracer ./hello
