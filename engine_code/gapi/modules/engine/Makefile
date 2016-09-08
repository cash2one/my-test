
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/engine/
	python -m compileall .
	chmod +x *.pyc
	cp -f *.pyc ../../../gms/gapi/modules/engine/
	cp -f *.json ../../../gms/gapi/modules/engine/
clean:
	rm -rf *.pyc
distclean:clean
	
