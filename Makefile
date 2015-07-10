FLAVOR ?= optimize
prefix ?= $(shell pf-makevar --absolute root)
libdir ?= $(shell pf-makevar lib)

# Don't set CXX when native GCC version is 4.8.
SETCXX := $(shell expr `gcc -dumpversion` \< 4.8)
ifeq "$(SETCXX)" "1"
    CXX=/opt/rh/devtoolset-2/root/usr/bin/g++
endif


## Temporary staging directory
# DESTDIR =

# Specified by `git make-pkg` when building .pkg files
# mac_pkg =

export prefix DESTDIR

all:
	mkdir -p build
	export CXX=${CXX}
	cd build &&  CXX=${CXX} cmake -DCMAKE_INSTALL_PREFIX=$(prefix) -DCMAKE_INSTALL_LIBDIR=$(libdir) ../
	#cd build && cmake -DCMAKE_INSTALL_PREFIX=$(prefix) -DCMAKE_INSTALL_LIBDIR=$(libdir) -DENABLE_LLVM_BACKEND=ON -DLLVM_ROOT="/home/fahome/yuanfanc/Install" ../
	cd build && make -j 32 # && make doc
clean:
	rm -rf build

install: all
	cd build; make install
	pkgconfig-gen --name seexpr2 --desc 'SeExpr v2 Library' \
		--generate --destdir '$(DESTDIR)' --prefix $(prefix) --libdir $(libdir)
