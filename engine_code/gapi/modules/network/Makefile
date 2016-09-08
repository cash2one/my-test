
all: _install
_install:
	mkdir -p ../../../gms/gapi/modules/network/reg/
	python -m py_compile xml_att.py check_ip.py text_xml.py init_ddos.py
	python -m compileall reg/
	chmod +x *.pyc
	cp -f xml_att.pyc ../../../gms/gapi/modules/network/
	cp -f init_ddos.pyc ../../../gms/gapi/modules/network/
	cp -f text_xml.pyc ../../../gms/gapi/modules/network/
	cp -f check_ip.pyc ../../../gms/gapi/modules/network/
	cp -f network_conf.json ../../../gms/gapi/modules/network/
	cp -f reg/*.pyc ../../../gms/gapi/modules/network/reg/
clean:
	rm -rf *.pyc reg/*.pyc
distclean:clean
	
