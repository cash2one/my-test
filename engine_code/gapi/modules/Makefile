list_modules=auth debug network time warn ethinfo proxy engine flow_def pro_restore bug device shell wtl set_network est ddos
all: _install
_install:
	for i in $(list_modules);\
	do\
		echo $$i;\
		make -C $$i _install;\
	done
clean:
	for i in $(list_modules);\
	do\
		echo $$i;\
		make -C $$i clean;\
	done
distclean:clean
	
