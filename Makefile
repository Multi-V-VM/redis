# Top level makefile, the real shit is at src/Makefile

default: all

.DEFAULT:
	cd src && $(MAKE) $@

wasm:
	cd src && $(MAKE) -f Makefile_wasm && mv redis.wasm ../
	#~/.cargo/bin/fce embed -i redis.wasm -o redis.wasm -w redis.wit

install:
	cd src && $(MAKE) $@

.PHONY: install
