.ONESHELL:
.PHONY: mcu install test setup

mcu: setup
	cd mcu
	make mcu

install: setup
	cd mcu
	make install

test: install
	cd test
	make test

setup:
	mkdir -p build/bin

