list_modules=flow_proc merge_proc flow_def
all:_install
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
