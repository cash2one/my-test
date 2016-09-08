all: _install 
	

_install:
	mkdir -p ../gms/updata/upgrade/conf/
	mkdir -p ../gms/conf
	gzexe upgrade_system.sh
	mv upgrade_system.sh upgrade_system.sh.tmp
	mv upgrade_system.sh~ upgrade_system.sh
	cp upgrade_system.sh.tmp ../gms/updata/upgrade_system.sh
#cp upgrade_log.sh ../gms/updata/
	cp ./up/gmsupgrade/Upgrade.jar ../gms/updata/upgrade/
	cp ./up/gmsupgrade/conf/conf.xml ../gms/updata/upgrade/conf/
	cp ./up/gmsupgrade/conf/*.csv ../gms/updata/upgrade/conf/
	cp ./up/gmsupgrade/version.xml ../gms/conf/


clean:
	rm -f upgrade_system.sh.tmp


distclean:clean
	
