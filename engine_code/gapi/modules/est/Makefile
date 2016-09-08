all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/est/
	python -m compileall .
	chmod +x *.pyc
#chmod +x *.sh
	cp -f *.pyc ../../../gms/gapi/modules/est/
#	cp  *.sh ../../../gms/gapi/modules/est/
	make -C log _install;
clean:
	rm -rf *pyc
	make -C log clean;
distclean:clean
