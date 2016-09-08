CC 		= gcc
INCLUDES 	= -I./ -I../include
CFLAGS = -O0 -g  -W -Wall -Werror -finline-functions -Wunused -Wno-unused-parameter -Bsymbolic -fPIC -g -O2
SHAREDFLAGS = -O0 -g -W -Wall -Werror -finline-functions -Wunused -Wno-unused-parameter -Bsymbolic -fPIC -g -O2 -fPIC -shared

.PHONY: all clean

all: libinnerfunc.so
	

libinnerfunc.so: var_func.o
	$(CC) $(SHAREDFLAGS) -o $@ $^
	

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a *.so
