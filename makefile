.ONESHELL:
.PHONY: mcu install

mcu: setup
	cd mcu
	make mcu

install: mcu
	cd mcu
	make install

setup:
	mkdir -p build/bin

