# Top level makefile, the real shit is at src/Makefile

default: all

.DEFAULT:
	cd src && $(MAKE) $@

wasm:
	cd src && $(MAKE) -f Makefile_wasm

install:
	cd src && $(MAKE) $@

.PHONY: install
