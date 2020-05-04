-include Makefile.config

# External commands
CMAKE ?= cmake
CLANG_FORMAT ?= disney-clang-format
FIND ?= find
MKDIR ?= mkdir -p
PYTHON ?= python
RM_R ?= rm -fr
XARGS ?= xargs

## Path and build flags
FLAVOR ?= optimize
#prefix ?= /usr/local
#libdir ?= lib

## Temporary staging directory
# DESTDIR =
## Specified by `git make-pkg` when building .pkg files
# mac_pkg =

ifdef prefix
    CMAKE_ARGS += -DCMAKE_INSTALL_PREFIX=$(prefix)
endif
ifdef libdir
    CMAKE_ARGS += -DCMAKE_INSTALL_LIBDIR=$(libdir)
endif
ifdef FLAVOR
    CMAKE_ARGS += -DFLAVOR=$(FLAVOR)
endif

export CXX
export DESTDIR
export prefix

all:
	$(MKDIR) build/$(FLAVOR)
	cd build/$(FLAVOR) && $(CMAKE) $(CMAKE_ARGS) $(EXTRA_CMAKE_ARGS) ../..
	$(MAKE) -C build/$(FLAVOR) all
.PHONY: all

clean:
	$(RM_R) build/$(FLAVOR) Linux-*
.PHONY: clean

install: all
	$(MAKE) -C build/$(FLAVOR) install
.PHONY: install

checkDirty:
	git diff --exit-code > /dev/null
.PHONY: checkDirty

format:
	$(FIND) $(CURDIR)/src -name '*.cpp' | $(XARGS) $(CLANG_FORMAT) -i
	$(FIND) $(CURDIR)/src -name '*.h' | $(XARGS) $(CLANG_FORMAT) -i
.PHONY: format

test: install
	$(MAKE) -C build/$(FLAVOR) test
.PHONY: test

# TODO: run this via cmake
imagetest: install
	$(PYTHON) src/tests/imageTestsReportNew.py runall
.PHONY: imagetest

precommit:
	$(MAKE) format
	$(MAKE) checkDirty
	$(MAKE) test
.PHONY: precommit
