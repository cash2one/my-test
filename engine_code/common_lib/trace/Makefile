CC 		= gcc
INCLUDES 	= -I./ -I../include -I../../../include -I../../common/include
CFLAGS = -O0 -g  -static -W -Wall -Werror -finline-functions -Wno-unused-parameter -fsigned-char


.PHONY: all clean

all: libtrace.a

libtrace.a: trace.o output_action.o output_file.o output_stdout.o output_syslog.o output_udp.o udp_socket.o ../config/libconfig.a  ../common/libcommon.a
	ar cru $@ $^ 
	ranlib $@


%.o: %.c 
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.a
