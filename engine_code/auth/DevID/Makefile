CC = gcc
CFLAGS:= -O3

.PHONY: all clean

all: DevID


DevID: DevID.c vis24.c
	$(CC) $(CFLAGS) $^ -o genkey

_install:clean DevID
	mkdir -p ../../gms/auth/
	cp genkey ../../gms/auth/

distclean: clean

clean:
	rm -rf genkey *.o

