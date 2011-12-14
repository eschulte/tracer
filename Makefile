CC=gcc

all: tracer hello

hello: hello.c
	$(CC) -o hello hello.c

tracer: tracer.c
	$(CC) -static -o tracer tracer.c

clean:
	rm hello tracer

check: hello tracer
	./tracer ./hello |sort -n|uniq|less
