
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/ethinfo/
	python -m compileall .
	chmod +x *.pyc
	cp -f *.pyc ../../../gms/gapi/modules/ethinfo/
	cp -f *.json ../../../gms/gapi/modules/ethinfo/
clean:
	rm -rf *.pyc
distclean:clean
	
