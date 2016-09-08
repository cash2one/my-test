all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/device/ ../../../gms/gapi/modules/device/conf/
	python -m py_compile set_sw.py
	chmod +x *.pyc
	chmod +x conf/*.sh
	cp -af set_sw.pyc ../../../gms/gapi/modules/device/
	cp -f conf/*.json ../../../gms/gapi/modules/device/conf/
	cp -af conf/*.sh ../../../gms/gapi/modules/device/conf/
clean:
	rm -rf *.pyc
distclean:clean
	
