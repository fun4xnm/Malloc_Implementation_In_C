.PHONY : all clean

CFLAGS=-g -Wall -std=c99 -O0
LDFLAGS=-L . -lyamalloc

all : test

test : test.c
	gcc $^ -g $(LDFLAGS) -o $@

clean :
	rm -f test
