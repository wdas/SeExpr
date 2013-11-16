FLAVOR ?= optimize
prefix ?= $(shell pf-makevar --absolute root)
libdir ?= $(shell pf-makevar lib)

## Temporary staging directory
# DESTDIR =

# Specified by `git make-pkg` when building .pkg files
# mac_pkg =

export prefix DESTDIR

all:
	mkdir -p build
	cd build && cmake -DCMAKE_INSTALL_PREFIX=$(prefix) -DCMAKE_INSTALL_LIBDIR=$(libdir) ../
	cd build && make && make doc
clean:
	rm -rf build

install: all
	cd build; make install
	pkgconfig-gen --name SeExpr --desc 'SeExpr Library' \
		--generate --destdir '$(DESTDIR)' --prefix $(prefix) --libdir $(libdir)
