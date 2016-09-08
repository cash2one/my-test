
all: _install
_install:
	mkdir -p ../gms/comm/transform/apt
	mkdir -p ../gms/comm/transform/vds
	mkdir -p ../gms/comm/transform/mtx
	mkdir -p ../gms/comm/transform/common
	mkdir -p ../gms/comm/query/
	mkdir -p ../gms/comm/oss/
	mkdir -p ../gms/comm/conf/
	mkdir -p ../gms/comm/cloud/conf
	#mkdir -p ../gms/comm/rsa_test/
	python -m compileall ./
	chmod +x *.pyc transform/*.pyc transform/apt/*.pyc transform/mtx/*.pyc transform/vds/*.pyc transform/common/*.pyc oss/*.pyc query/*.pyc cloud/*pyc #rsa_test/*pyc
	cp -f *.pyc ../gms/comm/
	cp -f conf/* ../gms/comm/conf/
	#cp -f rsa_test/*_123 ../gms/comm/rsa_test/
	#cp -f rsa_test/*mydes.so ../gms/comm/rsa_test/
	#cp -f rsa_test/rsa.1024.pub ../gms/comm/rsa_test/
	#cp -f rsa_test/*pyc ../gms/comm/rsa_test/
	cp -f transform/*.pyc ../gms/comm/transform/
	cp -f transform/apt/*.pyc ../gms/comm/transform/apt/
	cp -f transform/mtx/*.pyc ../gms/comm/transform/mtx/
	cp -f transform/vds/*.pyc ../gms/comm/transform/vds/
	cp -f transform/common/*.pyc ../gms/comm/transform/common/
	cp -f oss/*.pyc ../gms/comm/oss/
	cp -f query/*.pyc ../gms/comm/query/
	cp -f cloud/*pyc  ../gms/comm/cloud/
	make -C est 
clean:
	rm -rf *.pyc transform/*.pyc transform/apt/*.pyc transform/mtx/*.pyc transform/vds/*.pyc transform/common/*.pyc oss/*.pyc query/*.pyc cloud/*pyc #rsa_test/*pyc
	make -C est clean
distclean:clean
	
