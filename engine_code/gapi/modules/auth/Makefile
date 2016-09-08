
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/auth/
	mkdir -p ../../../gms/gapi/modules/auth/conf
	gzexe gms_logmon.sh
	mv gms_logmon.sh gms_logmon.sh.tmp
	mv gms_logmon.sh~ gms_logmon.sh
	chmod +x gms_logmon.sh.tmp
	mv gms_logmon.sh.tmp ../../../gms/gapi/modules/auth/gms_logmon.sh
	rm -rf *.pyc
	python -m compileall .
	chmod +x sys_auth.pyc sys_logmon.pyc auth_log.pyc genkey.pyc com_to_conf.pyc gms_auth.pyc init_xml.pyc
	cp -f sys_auth.pyc ../../../gms/gapi/modules/auth/
	cp -f gms_auth.pyc ../../../gms/gapi/modules/auth/
	cp -f sys_logmon.pyc ../../../gms/gapi/modules/auth/
	cp -f auth_log.pyc ../../../gms/gapi/modules/auth/
	cp -f genkey.pyc ../../../gms/gapi/modules/auth/
	cp -f com_to_conf.pyc ../../../gms/gapi/modules/auth/
	cp -f init_xml.pyc ../../../gms/gapi/modules/auth/
	cp -f conf/auth_path.json ../../../gms/gapi/modules/auth/conf/
	make -C ./os _install
clean:
	rm -rf *.pyc
	make -C os clean
distclean:clean
	
