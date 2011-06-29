
prefix ?= $(CURDIR)/$(shell uname)-$(shell fa.arch -r)-$(shell uname -m)-optimize
DESTDIR =

all:
	mkdir -p build
	cd build; cmake ../
	cd build; make  ; make doc
clean:
	rm -rf build
install: all
	cd build; make install
	echo "lib64" > ${DESTDIR}${prefix}/.release.SeExpr
	echo "share" >> ${DESTDIR}${prefix}/.release.SeExpr
	echo "include" >> ${DESTDIR}${prefix}/.release.SeExpr
