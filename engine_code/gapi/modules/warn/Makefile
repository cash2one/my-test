
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/warn/
	python -m py_compile common.py run_syslog.py 
	chmod +x *.pyc
	cp -f *.pyc ../../../gms/gapi/modules/warn/
clean:
	rm -rf *.pyc
distclean:clean
	
