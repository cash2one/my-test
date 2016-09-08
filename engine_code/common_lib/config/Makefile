CC 		= gcc
INCLUDES 	= -I./ -I../include -I../../../include -I../../common/include
CFLAGS = -O2 -static -W -Wall -Werror -finline-functions -Wno-unused-parameter -fsigned-char


.PHONY: all clean

all: libconfig.a

libconfig.a: rule_table_n.o config.o misc.o ca_string.o modify_config.o
	ar cru $@ $^ 
	ranlib $@


%.o: %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a
