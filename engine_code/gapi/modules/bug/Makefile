
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/bug/
	python -m py_compile getgmsinfo.py 
	chmod +x *.pyc
	cp -f getgmsinfo.pyc ../../../gms/gapi/modules/bug/
clean:
	rm -rf *.pyc
distclean:clean
	
