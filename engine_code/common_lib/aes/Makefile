CC 		= gcc
INCLUDES 	= -I./ -I../include
CFLAGS = -O0 -g  -static -W -Wall -Werror -finline-functions -Wno-unused-parameter -fsigned-char


.PHONY: all clean

all: libaes.a

libaes.a: aes.o
	ar cru $@ $^ 
	ranlib $@


%.o: %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a
