
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/proxy/
	mkdir -p ../../../gms/gapi/modules/proxy/cloud
	mkdir -p ../../../gms/gapi/modules/proxy/cloud/conf
	mkdir -p ../../../gms/gapi/modules/proxy/cloud/oss
	python -m compileall cloud
	#python -m compileall cloud/oss
	chmod +x cloud/*.pyc cloud/oss/*.pyc
	cp -f cloud/*.pyc ../../../gms/gapi/modules/proxy/cloud/
	cp -f cloud/*.sh ../../../gms/gapi/modules/proxy/cloud/
	cp -f cloud/conf/*.* ../../../gms/gapi/modules/proxy/cloud/conf/
	cp -f cloud/oss/*.pyc ../../../gms/gapi/modules/proxy/cloud/oss/
	
	/usr/bin/gzexe ../../../gms/gapi/modules/proxy/cloud/*.sh
	chmod a+x ../../../gms/gapi/modules/proxy/cloud/*.sh
	
	mkdir -p  ../../../gms/bin
	cp -f cloud/change_bucket_path.py   ../../../gms/bin


clean:
	rm -rf clould/*.pyc cloud/oss/*pyc
distclean:clean
	
