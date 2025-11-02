.POSIX:

SOURCES=src/manifest.ttl.in $(src/*/*) meson.build

VERSION = $(shell awk '/version/ { print substr($$3,2,length($$3)-2) }' meson.build)

all: build $(SOURCES)
	meson compile -C build

build: meson_options.txt
	@if [ `uname` = "Darwin" ]; then\
	    meson setup build -Dlv2dir=$(HOME)/Library/Audio/Plug-Ins/LV2 ;\
	else \
	    meson setup build ;\
	fi

install:
	meson install -C build

clean:
	rm -rf build
	rm -rf zig-out
	rm -rf .zig-cache

cross:
	rm -rf zig-out/cross
	zig build dist -Doptimize=ReleaseFast
	mv zig-out/cross/*.zip .
	mv zig-out/cross/*.tar.gz .

.PHONY: all clean install cross
