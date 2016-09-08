
all: genkey descrypt

genkey:
	gcc -o genkey.so -shared -fPIC DevID.c vis24.c
	gcc -o genkey DevID.c vis24.c

descrypt:
	gcc -o mydes.so -shared -fPIC cap_des.c

_install:genkey descrypt
	mkdir -p ../../../../gms/bin
	mkdir -p ../../../../gms/gapi/modules/auth/os/
	cp ./*.so ../../../../gms/gapi/modules/auth/os/
	cp genkey ../../../../gms/bin


clean:
	rm -rf *.so genkey

distclean:clean
	
