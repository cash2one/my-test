#Makefile for gcc template
PROJECT = ipserv           #This is the name of project.
CLIJECT = ipinfo
# executable file directory
BUILD_DIR = .

C_GMS_VERSION = y

# set compile command
CC = gcc
CFLAGS = -c -W -Wall -O2 -Wstrict-prototypes -Wall -Wunused -Wcast-align -Wint-to-pointer-cast -Wunused-but-set-variable
LDFLAGS =

# args
RELEASE = 0
BITS32 = -m32
BITS64 = -m64
# [args] 生成模式. 0代表debug模式, 1代表release模式. make RELEASE=1.
ifeq ($(RELEASE),0)
	# debug
	CFLAGS += -g
else
	# release
	CFLAGS += -static -O3 -DNDEBUG
	LFLAGS += -static
endif

ifeq ($(C_GMS_VERSION),y)
    CFLAGS += -D C_GMS
endif

# [args] 程序位数. 32代表32位程序, 64代表64位程序, 其他默认. make BITS=32.
#ifeq ($(BITS),32)
#	CFLAGS += -m32
#	LFLAGS += -m32
#else
#	ifeq ($(BITS),64)
#		CFLAGS += -m64
#		LFLAGS += -m64
#	else
#	endif
#endif

LIBS = -liplib -lthunk
#LIBS = -lthunk

TARGET = ipserv ipinfo 


OBJECTS = socket_ser.o rwlock.o iplib_common.o 
IPJECTS = socket_cli_now.o

.PHONY : all
all: $(TARGET) _install

ipserv: $(OBJECTS)
	$(CC) -o $(BUILD_DIR)/$(PROJECT) $(OBJECTS) -L $(BUILD_DIR) $(LIBS)

ipinfo: $(IPJECTS)
	$(CC) -o $(BUILD_DIR)/$(CLIJECT) $(IPJECTS) -L $(BUILD_DIR) $(LIBS)

socket_cli_now.o: socket_cli_now.c
	$(CC) $(CFLAGS) $< -o $@ 

rwlock.o: rwlock.c
	$(CC) $(CFLAGS) $< -o $@ 
    
socket_ser.o: socket_ser.c
	$(CC) $(CFLAGS) $< -o $@ 

iplib_common.o:iplib_common.c
	$(CC) $(CFLAGS) $< -o $@ 
_install:
	mkdir -p ../gms/addr_query
	chmod a+x *.sh
	cp -p $(TARGET) ../gms/addr_query
	cp -p iplib.dat ../gms/addr_query
	cp -p *.sh ../gms/addr_query
clean:
	rm -f *.o
	#rm -f libiplib.a
	#rm -f libiplib32.a
	rm -rf $(TARGET)
