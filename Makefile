ARCH=$(shell getconf LONG_BIT)
ifeq ($(ARCH),64)
PC_REG:=rip
endif
ifeq ($(ARCH),32)
PC_REG:=eip
endif
CC=gcc
CFLAGS=-static -Ofast -D PC_REG=$(PC_REG)

all: tracer

tracer: tracer.c libelf.c
	$(CC) -o $@ $(CFLAGS) $^
	strip -s $@

clean:
	rm -f hello tracer

check: hello tracer
	./tracer ./hello
