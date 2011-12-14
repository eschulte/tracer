CC=gcc
ARCH=$(shell getconf LONG_BIT)

all: tracer hello

hello: hello.c
	$(CC) -o hello hello.c

tracer: tracer-$(ARCH).c
	$(CC) -static -o tracer tracer-$(ARCH).c libelf.c

clean:
	rm -f hello tracer

check: hello tracer
	./tracer ./hello |sort -n|uniq|less
