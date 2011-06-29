all:
	mkdir -p build
	cd build; cmake ../
	cd build; make -j 32 doc all
	cd build; make install
