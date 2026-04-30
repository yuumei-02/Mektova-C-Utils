.ONESHELL:
.PHONY: mcu install

mcu: setup
	cd mcu
	make mcu

install: setup
	cd mcu
	make install

setup:
	mkdir -p build/bin

