
all: _install
_install:
	mkdir -p ../../gms/comm/est/
	python -m compileall . 
	chmod +x *.pyc
	cp -f *.pyc ../../gms/comm/est/
clean:
	rm -rf *.pyc
distclean:clean
	
