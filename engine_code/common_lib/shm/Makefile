CC 		= gcc
INCLUDES 	= -I./ -I../include
CFLAGS = -O0 -g  -static -W -Wall -Werror -finline-functions -Wno-unused-parameter -fsigned-char


.PHONY: all clean

all: libshm.a

libshm.a: shm.o com_memconf.o
	ar cru $@ $^ 
	ranlib $@


%.o: %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a
