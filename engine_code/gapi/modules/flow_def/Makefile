all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/flow_def/ ../../../gms/gapi/modules/flow_def/conf/
	python -m py_compile flow_def_main.py
	chmod +x *.pyc
	cp -af flow_def_main.pyc ../../../gms/gapi/modules/flow_def/
	cp -f conf/*.json ../../../gms/gapi/modules/flow_def/conf/
clean:
	rm -rf *.pyc
distclean:clean
	
