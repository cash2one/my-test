#********************************************************************************
#格式要求
#       每行不要超过80个字符;
#       #       每两次build之间用星号线("***")分割, 模板中的星号线刚好为80个字符;
#       标点符号不要用半角, 用ASCII符号后跟一个空格;
#       #       最新的记录在最上面;
#       注意用tab开控制字体的颜色;
#       #       每组记录与前后记录的间距为两行
#       文件修改标记: 
#       #               (M): 修改 modify
#               (A): 增加 add
#               #				(RA):增加 add directory 
#               (D): 删除 delete
#               #********************************************************************************
#2013-10-24 08:42:30 liudanbing <liudanbing@chanct.com>
#[Cause]
#	创建ChangLog.common文件
#[Treatment]
#	* [A]engine_code/common_lib/ChangeLog.common
#
#
#2014-12-15      lichuanhai <ph@phsec.com>
#修改build.sh为Makefile
#make 编译整个本目录下的整个工程
#make _install  和本模块之外通信的唯一选项
#make clean     清除make过程中产生的临时文件
#make distclean 清除二进制文件
#


TOP_DIR:=`pwd`
SRC_DIR:=$(TOP_DIR)/src

LIB:=$(TOP_DIR)/lib
SO:=$(TOP_DIR)/so
INCLUDE:=$(TOP_DIR)/include

#list=adt aes auth common config inner_func log setsignal shm timer trace
listmk=adt aes auth common config inner_func log setsignal shm timer trace
	
	
all:clean build_dir copy_file 
	
	

_install:


build_dir:
	for i in $(listmk);\
	do \
		echo $$i;\
		make -C $$i;\
	done
		
	#list="common adt config shm trace timer vcrd software_update"
	#for dirname in $list
	#find . -maxdepth 1 -mindepth 1 -type d | grep -v svn | sort | while read dirname
#	for i in $(list);\
#	do \
#		echo $$i;\
#		cd src/$$i;\
#		echo "in $$i";\
#		./vbuild.sh -m;\
#		echo "------------------$$i-----------------------";\
#		echo "|	 MAKE SUCCESS                         |";\
#		echo "--------------------------------------------";\
#		cd ../../;\
#	done
#	cd src/auth;./vbuild.sh;cd ..

copy_file:
	rm -rf lib/* so/*
	find $(TOP_DIR) -maxdepth 2 -mindepth 2 -path "lib*" -prune -o -name "lib*\.a" -type f -exec cp -f {} $(LIB) \;
	find $(TOP_DIR) -maxdepth 2 -mindepth 2 -path "so*"  -prune -o -name "lib*\.so" -type f -exec cp -f {} $(SO) \;


clean:
	rm -rf lib/* so/*
	
	for i in $(listmk);\
	do \
		echo $$i;\
		make -C $$i clean;\
	done

distclean:clean
	
