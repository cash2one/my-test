CC 		= gcc
INCLUDES 	= -I./ -I../include -I../../../include -I../../common/include
CFLAGS = -O0 -g  -static -W -Wall -Werror -finline-functions -Wno-unused-parameter -fsigned-char


.PHONY: all clean

all: libsignal.a

libsignal.a: setsignal.o send_warmboot_signal.o record_coredump.o trace_stack.o
	ar cru $@ $^ 
	ranlib $@


%.o: %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a
