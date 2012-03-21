FLAVOR ?= optimize
prefix ?= $(CURDIR)/$(shell uname)-$(shell fa.arch -r)-$(shell uname -m)-$(FLAVOR)
# DESTDIR =
# SKIP_RELEASE_FILE =

export prefix DESTDIR

all:
	mkdir -p build
	cd build; cmake -DCMAKE_INSTALL_PREFIX=$(prefix) ../
	cd build; make; make doc
clean:
	rm -rf build

install: all
	cd build; make install
	if test -z "$(SKIP_RELEASE_FILE)"; then \
		echo "lib64" > $(DESTDIR)$(prefix)/.release.SeExpr && \
		echo "share" >> $(DESTDIR)$(prefix)/.release.SeExpr && \
		echo "include" >> $(DESTDIR)$(prefix)/.release.SeExpr; \
	fi
