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
	cd build &&  CXX=${CXX} cmake -DENABLE_LLVM_BACKEND=OFF -DCMAKE_INSTALL_PREFIX=$(prefix) -DCMAKE_INSTALL_LIBDIR=$(libdir) ../
	$(MAKE) -C build all
clean:
	rm -rf build Linux-*

install: all
	$(MAKE) -C build install
	pkgconfig-gen --name seexpr2 --desc 'SeExpr v2 Library' \
	--generate --destdir '$(DESTDIR)' --prefix $(prefix) --libdir $(libdir)
