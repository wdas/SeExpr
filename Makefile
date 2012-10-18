FLAVOR ?= optimize
prefix ?= $(shell pf-makevar --absolute root)
libdir ?= $(shell pf-makevar lib)

## Temporary staging directory
# DESTDIR =

## Avoid creating .release.SeExpr
# SKIP_RELEASE_FILE =

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
	if test -z "$(SKIP_RELEASE_FILE)" && test -z "$(mac_pkg)"; then \
		echo "lib64" > $(DESTDIR)$(prefix)/.release.SeExpr && \
		echo "share" >> $(DESTDIR)$(prefix)/.release.SeExpr && \
		echo "include" >> $(DESTDIR)$(prefix)/.release.SeExpr; \
	fi
	pkgconfig-gen --name SeExpr --desc 'SeExpr Library' \
		--generate --destdir '$(DESTDIR)' --prefix $(prefix) --libdir $(libdir)
