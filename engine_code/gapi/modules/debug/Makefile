
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/debug/
	python -m py_compile common.py  disable_ssh.py  enable_ssh.py  init_ssh.py
	chmod +x *.pyc
	cp -f *.pyc ../../../gms/gapi/modules/debug/
clean:
	rm -rf *.pyc
distclean:clean
	
