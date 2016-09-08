
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/wtl/
	python -m compileall .
	chmod +x *.pyc
	cp -f parse.pyc ../../../gms/gapi/modules/wtl/
	cp -f wtl_log.pyc ../../../gms/gapi/modules/wtl/
	cp -f wtl.pyc ../../../gms/gapi/modules/wtl/
clean:
	rm -rf *.pyc
distclean:clean
	
