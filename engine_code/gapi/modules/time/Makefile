
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/time/
	python -m py_compile xml_time.py
	chmod +x *.pyc
	cp -f *.pyc ../../../gms/gapi/modules/time/
clean:
	rm -rf *.pyc
distclean:clean
	
