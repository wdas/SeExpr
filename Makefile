-include Makefile.config

# External commands
CMAKE ?= cmake
CLANG_FORMAT ?= ./src/build/disney-clang-format
FIND ?= find
MKDIR ?= mkdir -p
PYTHON ?= python
RM_R ?= rm -fr
XARGS ?= xargs

## Path and build flags
FLAVOR ?= optimize
BUILD = build/$(FLAVOR)
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

CMAKE_FILES += Makefile
CMAKE_FILES += $(wildcard CMakeLists.txt */*/CMakeLists.txt)

export ARGS
export CXX
export DESTDIR
export prefix

all: $(BUILD)
	$(MAKE) -C $(BUILD) $@

install: all
	$(MAKE) -C $(BUILD) $@
.PHONY: install

checkDirty:
	git diff --exit-code >/dev/null
.PHONY: checkDirty

clean:
	$(RM_R) $(BUILD) Linux-* Darwin-*
.PHONY: clean

$(BUILD): $(CMAKE_FILES)
	mkdir -p $(BUILD)
	cd $(BUILD) && $(CMAKE) $(CMAKE_ARGS) $(EXTRA_CMAKE_ARGS) ../..
	touch $@

format:
	git ls-files '*.cpp' '*.h' | $(XARGS) $(CLANG_FORMAT) -i
.PHONY: format

test: install
	$(MAKE) -C $(BUILD) ARGS="--output-on-failure $(ARGS)" $@
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
