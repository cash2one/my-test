all: _install
_install:
	mkdir -p ../../../../gms/gapi/modules/est/log/
	python -m compileall .
	chmod +x *.pyc
	cp -f *.pyc ../../../../gms/gapi/modules/est/log/
	cp -f logger.conf ../../../../gms/gapi/modules/est/log/logger.conf
clean:
	rm -rf *pyc
distclean:clean
