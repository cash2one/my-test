all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/pro_restore/ ../../../gms/gapi/modules/pro_restore/conf/
	python -m py_compile pro_restore_main.py
	chmod +x *.pyc
	cp -af pro_restore_main.pyc ../../../gms/gapi/modules/pro_restore/
	cp -f conf/*.json ../../../gms/gapi/modules/pro_restore/conf/
clean:
	rm -rf *.pyc
distclean:clean
	
