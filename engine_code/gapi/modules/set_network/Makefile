all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/set_network/ 
	python -m py_compile set_network_main.py
	chmod +x *.pyc
	cp -af set_network_main.pyc ../../../gms/gapi/modules/set_network/
clean:
	rm -rf *.pyc
distclean:clean
	
