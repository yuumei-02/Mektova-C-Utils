cflags := -Wall -Wextra -pedantic -std=c23
cflags-debug = $(cflags) -Og -ggdb
cflags-release = $(cflags) -O2 -DNDEBUG
compiler = gcc

.ONESHELL:
.PHONY: mcu mcu-debug mcu-release install

mcu-debug:
	$(compiler) $(cflags-debug) -c *.c
	ar rcs ../build/bin/libmcu-debug.a *.o
	rm *.o

mcu-release:
	$(compiler) $(cflags-release) -c *.c
	ar rcs ../build/bin/libmcu-release.a *.o
	rm *.o

mcu: mcu-debug mcu-release
	echo "Compiled both release and debug version"

install: mcu
	sudo mkdir -p /usr/local/include/mcu
	for header in *.h; do \
	     sudo cp "$$header" /usr/local/include/mcu/; \
	done
	sudo cp ../build/bin/libmcu-debug.a /usr/local/lib/libmcu-debug.a
	sudo cp ../build/bin/libmcu-release.a /usr/local/lib/libmcu-release.a

