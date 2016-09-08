
all: _install

_install:
	mkdir -p ../gms/gapi/ ../gms/conf
	mkdir -p ../gms/gapi/init
	mkdir -p ../gms/gapi/common/
	mkdir -p ../gms/termctrl/conf/
	python -m compileall init
	python -m compileall common
	python -m py_compile gapi.py
	chmod +x gapi.pyc init/cloud_start.pyc common/*.pyc
	cp gapi.pyc ../gms/gapi/
	#cp init.sh ../gms/gapi/
	cp init/cloud_start.pyc ../gms/gapi/init
	cp *.xml ../gms/conf
	cp *.pdf ../gms/conf
	cp gmsconfig.xml ../gms/termctrl/conf -f
	cp -rf common/*.pyc ../gms/gapi/common/
	make -C ./modules _install

clean:
	rm -rf *.pyc init/*pyc
	make -C ./modules clean
distclean:clean
	
