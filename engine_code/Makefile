include ../Makefile.base
TOP_DIR:=`pwd`
list= database_lib auth storage termctrl guard query_flow 
listmk=common_lib gapi upgrade comm ../bootloader_code/python_helper url_detect addr_query 
store_list=storage tcad
dir_list=	./gms/				\
		./gms/conf/			\
		./gms/mtx/			\
		./gms/mtx/conf/			\
		./gms/vds			\
		./gms/vds/conf/			\
		./gms/apt/			\
		./gms/nmsmw/			\
						\
		./gms/url_detect/		\
		./gms/url_detect/conf/		\
						\
		./gms/storage			\
		./gms/storage/conf/		\
						\
		./gms/query_flow/		\
		./gms/query_flow/conf/		\
						\
		./gms/comm/			\
		./gms/comm/conf/		\
		./gms/comm/oss/			\
		./gms/comm/transform/		\
		./gms/comm/transform/mtx/	\
		./gms/comm/transform/vds/	\
		./gms/comm/transform/apt/	\
		./gms/comm/transform/common/	\
		./gms/comm/cloud/		\
		./gms/comm/cloud/conf		\
		./gms/comm/remote/		\
						\
		./gms/auth			\
		./gms/auth/conf/		\
						\
		./gms/guard			\
		./gms/guard/conf/		\
						\
		./gms/termctrl/			\
		./gms/termctrl/conf/		\
						\
		./gms/updata/			\
		./gms/updata/back/		\
		./gms/updata/pack/		\
		./gms/updata/unpack/		\
		./gms/updata/upgrade/		\
		./gms/updata/upgrade/conf/	\
						\
		./gms/doc/			\
		./gms/doc/tool/			\
		./usr/bin
	
	
all:clean build_dir mk_gms_dir copy_file mtx_apt_vds
	
	find ./gms -name "*.pyc" | xargs -I {} ../bootloader_code/python_helper/ph {}
	find ./gms -name .svn -print0 | xargs -0 rm -r -f
	tar -cvf gms_engine.tar ./gms
	#ln -sf $packet_name gms_engine.tar
	echo "Creat gms_engine.tar ... SUCC"
	rm -rf gms


_install:
	mkdir -p $(install)
	mv gms_engine.tar $(install)/;


build_dir:
	
	for i in $(listmk);\
	do \
		echo $$i;\
		make -C $$i;\
	done
	
	#find . -maxdepth 1 -mindepth 1 -type d | grep -v svn | sort | while read dirname
	for i in $(list);\
	do \
		echo $$i;\
		cd $$i;\
		echo "in $$i";\
		chmod +x ./build.sh;\
		./build.sh -m;\
		echo "------------------$$i-----------------------";\
		echo "|	 MAKE SUCCESS                         |";\
		echo "--------------------------------------------";\
		cd ..;\
	done
	
	for i in $(store_list);\
	do \
		echo $$i;\
		cd $$i; \
		make ;\
		cd -;\
	done


mk_gms_dir:
	######### 建立目录 ###########
	for i in $(dir_list);\
	do \
		mkdir -p $$i;\
		echo "Mkdir $dirname SUCC!";\
	done


copy_file:
	######### 拷贝文件 ############
	cp $(TOP_DIR)/storage/store_proc/conf/database.conf ./gms/conf/
	cp $(TOP_DIR)/storage/store_proc/build/store_proc ./gms/storage/
	cp $(TOP_DIR)/storage/store_proc/conf/*.conf ./gms/storage/conf/
	cp $(TOP_DIR)/storage/store_proc/conf/store_trace.conf ./gms/storage/conf/
	cp $(TOP_DIR)/storage/flow_proc_apt/build/flow_proc_apt ./gms/storage/
	cp $(TOP_DIR)/storage/flow_proc_apt/conf/flow_trace.conf ./gms/storage/conf/
	cp $(TOP_DIR)/storage/flow_proc_apt/conf/flow_time.conf ./gms/storage/conf/
	cp $(TOP_DIR)/storage/flow_proc_apt/conf/custom_flow.conf ./gms/conf/
	
	#auth
	#cp $(TOP_DIR)/auth/genkey/build/genkey ./gms/auth/
	#cp $(TOP_DIR)/auth/logmon/build/logmon ./gms/auth/
	#cp $(TOP_DIR)/auth/conf/auth.conf ./gms/auth/conf/
	cp $(TOP_DIR)/auth/conf/rsa.1024.pub ./gms/auth/conf/
	#cp $(TOP_DIR)/auth/conf/product_id.conf ./gms/conf/
	#gapi
	#make -C $(TOP_DIR)/gapi/ _install
	
	#guard
	cp $(TOP_DIR)/guard/sys_guard/build/sys_guard ./gms/guard/
	cp $(TOP_DIR)/guard/conf/cronjob.conf	./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/service_class.conf ./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/process.conf ./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/guard.conf ./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/drop_cache.sh ./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/process_source.sh ./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/GenStat.sh ./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/last_time ./gms/guard/conf/
	cp $(TOP_DIR)/guard/conf/wrong_time ./gms/guard/conf/
	
	#query_flow
	cp $(TOP_DIR)/query_flow/query_flow/conf/query_info.conf ./gms/query_flow/conf/
	cp $(TOP_DIR)/query_flow/query_flow/build/query_flow ./gms/query_flow/
	
	#comm
	cp $(TOP_DIR)/comm/conf/comm_interface_conf.xml ./gms/conf/
	cp $(TOP_DIR)/comm/conf/comm_pre_conf.json ./gms/comm/conf/
	cp $(TOP_DIR)/comm/conf/comm_interface_conf.xml ./gms/comm/conf/
	#cp $(TOP_DIR)/comm/*.py ./gms/comm/
	#cp $(TOP_DIR)/comm/oss/*.py ./gms/comm/oss/
	#cp $(TOP_DIR)/comm/cloud/*.py ./gms/comm/cloud/
	#cp $(TOP_DIR)/comm/transform/*.py ./gms/comm/transform/
	#cp $(TOP_DIR)/comm/transform/common/*.py ./gms/comm/transform/common/
	#cp $(TOP_DIR)/comm/transform/apt/*.py ./gms/comm/transform/apt/
	#cp $(TOP_DIR)/comm/transform/mtx/*.py ./gms/comm/transform/mtx/
	#cp $(TOP_DIR)/comm/transform/vds/*.py ./gms/comm/transform/vds/
	cp $(TOP_DIR)/comm/*.pyc ./gms/comm/
	cp $(TOP_DIR)/comm/oss/*.pyc ./gms/comm/oss/
	cp $(TOP_DIR)/comm/cloud/*.pyc ./gms/comm/cloud/
	cp $(TOP_DIR)/comm/transform/*.pyc ./gms/comm/transform/
	cp $(TOP_DIR)/comm/transform/common/*.pyc ./gms/comm/transform/common/
	cp $(TOP_DIR)/comm/transform/apt/*.pyc ./gms/comm/transform/apt/
	cp $(TOP_DIR)/comm/transform/mtx/*.pyc ./gms/comm/transform/mtx/
	cp $(TOP_DIR)/comm/transform/vds/*.pyc ./gms/comm/transform/vds/
	
	#remote_trans
	cp $(TOP_DIR)/comm/conf/ftp_server.conf ./gms/conf/
	cp $(TOP_DIR)/comm/remote/remote_trans.pyc ./gms/comm/remote/
		
	
	#termctrl
	cp $(TOP_DIR)/termctrl/term/build/term ./gms/termctrl/
	cp $(TOP_DIR)/termctrl/term/conf/netport.conf ./gms/termctrl/conf/
	cp $(TOP_DIR)/termctrl/term/conf/netsetfile.conf ./gms/termctrl/conf/
	cp $(TOP_DIR)/termctrl/term/conf/pag.conf ./gms/termctrl/conf/
	chmod a+x $(TOP_DIR)/termctrl/term/conf/*.sh 
	cp $(TOP_DIR)/termctrl/term/conf/*.sh ./gms/termctrl/conf/
	
	#upgrade
	#make -C ./upgrade/ _install	
	
mtx_apt_vds:	
	mkdir -p gms/mtx/conf
	mkdir -p gms/tcad
	mkdir -p gms/apt
	mkdir -p gms/vds
	mkdir -p gms/nmsmw
	mkdir -p gms/doc/tool/
	#mtx
	cp $(TOP_DIR)/../public_libs/third_party_engines/mtx/mtx ./gms/mtx/
	cp $(TOP_DIR)/../public_libs/third_party_engines/mtx/cxhash ./gms/mtx/
	cp $(TOP_DIR)/../public_libs/third_party_engines/mtx/conf/* ./gms/mtx/conf/
	cp $(TOP_DIR)/../public_libs/third_party_engines/mtx/pag.conf ./gms/mtx/
	#cp $(TOP_DIR)/../public_libs/third_party_engines/mtx/mtx_5.4 ./gms/mtx/
	
	#apt 
	cp $(TOP_DIR)/../public_libs/third_party_engines/apt/* ./gms/apt/ -ar
	#tcad
	chmod a+x $(TOP_DIR)/tcad/bin/*.sh
	cp $(TOP_DIR)/tcad/bin/* ./gms/tcad/ -ar
	
	#vds
	cp $(TOP_DIR)/../public_libs/third_party_engines/vds/* ./gms/vds/ -r    
	
	#nmsmw
	cp $(TOP_DIR)/../public_libs/third_party_engines/nmsmw/* ./gms/nmsmw/ -r
	
	#sofeware: apt/nm/eth
	cp $(TOP_DIR)/../software/apt.tar.gz ./gms/doc/tool/
	cp $(TOP_DIR)/../software/ethpcap-PAG-is1.09.00.00-rc4-k2.6.32-358.el6.x86_64-x86_64.tar ./gms/doc/tool/
	cp $(TOP_DIR)/../software/nmsmw.tar ./gms/doc/tool/

clean:
	for i in $(listmk);\
	do \
		echo $$i;\
		make -C $$i clean;\
	done
	for i in $(list);\
	do \
		echo $$i;\
		cd $$i;\
		echo "in $$i";\
		chmod +x ./build.sh;\
		./build.sh -c;\
		echo "------------------$$i-----------------------";\
		echo "|	 BUILT CLEAN                             |";\
		echo "--------------------------------------------";\
		cd ..;\
	done
	for i in $(store_list);\
	do \
		echo $$i;\
		make -C $$i clean;\
	done
	rm -rf gms
	rm -fr gms_engine.tar
	rm -rf auth/Makefile
	rm -rf auth/genkey/build
	rm -rf auth/genkey/configure
	rm -rf auth/genkey/Makefile.in
	rm -rf auth/genkey/autom4te.cache
	rm -rf auth/genkey/depcomp
	rm -rf auth/genkey/missing
	rm -rf auth/genkey/aclocal.m4
	rm -rf auth/genkey/install-sh
	rm -rf auth/logmon/build
	rm -rf auth/logmon/configure
	rm -rf auth/logmon/Makefile.in
	rm -rf auth/logmon/autom4te.cache
	rm -rf auth/logmon/depcomp
	rm -rf auth/logmon/missing
	rm -rf auth/logmon/aclocal.m4
	rm -rf auth/logmon/install-sh
	rm -rf guard/sys_guard/build
	rm -rf guard/sys_guard/configure
	rm -rf guard/sys_guard/Makefile.in
	rm -rf guard/sys_guard/autom4te.cache
	rm -rf guard/sys_guard/depcomp
	rm -rf guard/sys_guard/missing
	rm -rf guard/sys_guard/aclocal.m4
	rm -rf guard/sys_guard/install-sh
	rm -rf storage/store_proc/configure
	rm -rf storage/store_proc/Makefile.in
	rm -rf storage/store_proc/depcomp
	rm -rf storage/store_proc/build
	rm -rf storage/store_proc/autom4te.cache
	rm -rf storage/store_proc/missing
	rm -rf storage/store_proc/aclocal.m4
	rm -rf storage/store_proc/install-sh
	rm -rf storage/merge_proc/configure
	rm -rf storage/merge_proc/Makefile.in
	rm -rf storage/merge_proc/depcomp
	rm -rf storage/merge_proc/build
	rm -rf storage/merge_proc/autom4te.cache
	rm -rf storage/merge_proc/missing
	rm -rf storage/merge_proc/aclocal.m4
	rm -rf storage/merge_proc/install-sh
	rm -rf storage/flow_proc/configure
	rm -rf storage/flow_proc/Makefile.in
	rm -rf storage/flow_proc/depcomp
	rm -rf storage/flow_proc/build
	rm -rf storage/flow_proc/autom4te.cache
	rm -rf storage/flow_proc/missing
	rm -rf storage/flow_proc/aclocal.m4
	rm -rf storage/flow_proc/install-sh
	rm -rf database_lib/so
	rm -rf database_lib/include
	rm -rf database_lib/lib
	rm -rf database_lib/src/postgres/configure
	rm -rf database_lib/src/postgres/Makefile.in
	rm -rf database_lib/src/postgres/depcomp
	rm -rf database_lib/src/postgres/build
	rm -rf database_lib/src/postgres/autom4te.cache
	rm -rf database_lib/src/postgres/missing
	rm -rf database_lib/src/postgres/aclocal.m4
	rm -rf database_lib/src/postgres/install-sh
	rm -rf database_lib/src/redis/configure
	rm -rf database_lib/src/redis/Makefile.in
	rm -rf database_lib/src/redis/depcomp
	rm -rf database_lib/src/redis/build
	rm -rf database_lib/src/redis/autom4te.cache
	rm -rf database_lib/src/redis/missing
	rm -rf database_lib/src/redis/aclocal.m4
	rm -rf database_lib/src/redis/install-sh
	rm -rf query_flow/query_flow/configure
	rm -rf query_flow/query_flow/Makefile.in
	rm -rf query_flow/query_flow/depcomp
	rm -rf query_flow/query_flow/build
	rm -rf query_flow/query_flow/autom4te.cache
	rm -rf query_flow/query_flow/missing
	rm -rf query_flow/query_flow/aclocal.m4
	rm -rf query_flow/query_flow/install-sh
	rm -rf termctrl/term/configure
	rm -rf termctrl/term/Makefile.in
	rm -rf termctrl/term/depcomp
	rm -rf termctrl/term/build
	rm -rf termctrl/term/autom4te.cache
	rm -rf termctrl/term/missing
	rm -rf termctrl/term/aclocal.m4
	rm -rf termctrl/term/install-sh

distclean:clean
	
