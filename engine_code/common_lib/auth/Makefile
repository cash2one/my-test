CC 		= gcc
INCLUDES 	= -I./ -I../include -I../../../include -I../../common/include
CFLAGS = -O2 -static -W -Wall -Werror -finline-functions -Wno-unused-parameter -fsigned-char


.PHONY: all clean

all: libauth.a

libauth.a: get_cpu.o get_board_no.o product_conf.o md5.o make_key.o

	ar cru $@ $^ 
	ranlib $@


%.o: %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a
