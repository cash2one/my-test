include ../../Makefile.base
sqlchop_ver=0.6.1
_install_path=../gms/url_detect/
.PHONY: clean


all: sqlchop _install 

sqlchop:sqlchop-$(sqlchop_ver).tar.gz
	tar zxvf sqlchop-$(sqlchop_ver).tar.gz
	cd sqlchop-$(sqlchop_ver);make sqlchop_python;cd -
	python -m py_compile url_detect.py white_list.py 
	
_install:
	mkdir -p $(_install_path)/conf
	#####install sqlchop
	chmod 777 sqlchop-$(sqlchop_ver)/libsqlchop.so *.sh
	cp -p sqlchop-$(sqlchop_ver)/libsqlchop.so $(_install_path)
	cp -p sqlchop-$(sqlchop_ver)/sqlchop.py $(_install_path)
	cp -p sqlchop-$(sqlchop_ver)/sqlchopio_pb2.py $(_install_path)
	cp -p *.pyc $(_install_path)
	cp -p *.sh  $(_install_path)
	cp -p conf/url_detect.conf  conf/url_detect.rule  $(_install_path)conf
	

clean:
	rm -rf sqlchop-$(sqlchop_ver).tar.gz  sqlchop-$(sqlchop_ver) *.pyc

distclean:
