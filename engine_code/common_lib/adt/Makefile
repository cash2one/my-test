CC 		= gcc
INCLUDES 	= -I./ -I../include -I../../../include -I../../common/include
CFLAGS = -O0 -g  -static -W -Wall -Werror -finline-functions -Wno-unused-parameter -fsigned-char


.PHONY: all clean

all: libadt.a

libadt.a: hash.o mem_pool.o module_debug.o list.o str_hash.o
	ar cru $@ $^ 
	ranlib $@


%.o: %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a
