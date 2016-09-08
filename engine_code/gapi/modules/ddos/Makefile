
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/ddos/
	python -m compileall . 
	chmod +x *.pyc
	cp -f *.pyc ../../../gms/gapi/modules/ddos/
clean:
	rm -rf *.pyc
distclean:clean
	
