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

cross: airwindows-lv2-$(VERSION)-amd64-macos.tar.gz airwindows-lv2-$(VERSION)-aarch64-macos.tar.gz airwindows-lv2-$(VERSION)-amd64-windows.zip airwindows-lv2-$(VERSION)-i686-windows.zip

airwindows-lv2-$(VERSION)-amd64-macos.tar.gz: $(SOURCES)
	rm -rf build-amd64-macos
	meson setup build-amd64-macos
	meson compile -C build-amd64-macos
	meson install --destdir . -C build-amd64-macos
	test -d build-amd64-macos/usr/local/lib/lv2/Airwindows.lv2
	mkdir airwindows-lv2-$(VERSION)-amd64-macos
	cp -r build-amd64-macos/usr/local/lib/lv2/Airwindows.lv2 airwindows-lv2-$(VERSION)-amd64-macos
	rm -rf build-amd64-macos
	cp LICENSE airwindows-lv2-$(VERSION)-amd64-macos
	tar -cvzf airwindows-lv2-$(VERSION)-amd64-macos.tar.gz airwindows-lv2-$(VERSION)-amd64-macos
	rm -r airwindows-lv2-$(VERSION)-amd64-macos

airwindows-lv2-$(VERSION)-aarch64-macos.tar.gz: $(SOURCES) cross/macos-aarch64.txt
	rm -rf build-aarch64-macos
	meson setup --cross-file cross/macos-aarch64.txt build-aarch64-macos
	meson compile -C build-aarch64-macos
	meson install --destdir . -C build-aarch64-macos
	test -d build-aarch64-macos/usr/local/lib/lv2/Airwindows.lv2
	mkdir airwindows-lv2-$(VERSION)-aarch64-macos
	cp -r build-aarch64-macos/usr/local/lib/lv2/Airwindows.lv2 airwindows-lv2-$(VERSION)-aarch64-macos
	rm -rf build-aarch64-macos
	cp LICENSE airwindows-lv2-$(VERSION)-aarch64-macos
	tar -cvzf airwindows-lv2-$(VERSION)-aarch64-macos.tar.gz airwindows-lv2-$(VERSION)-aarch64-macos
	rm -r airwindows-lv2-$(VERSION)-aarch64-macos

airwindows-lv2-$(VERSION)-amd64-windows.zip: $(SOURCES) cross/windows-amd64.txt
	rm -rf build-amd64-windows
	meson setup --cross-file cross/windows-amd64.txt build-amd64-windows
	meson compile -C build-amd64-windows
	meson install --destdir . -C build-amd64-windows
	test -d build-amd64-windows/usr/local/lib/lv2/Airwindows.lv2
	mkdir airwindows-lv2-$(VERSION)-amd64-windows
	cp -r build-amd64-windows/usr/local/lib/lv2/Airwindows.lv2 airwindows-lv2-$(VERSION)-amd64-windows
	rm -rf build-amd64-windows
	cp LICENSE airwindows-lv2-$(VERSION)-amd64-windows
	zip -9 -v -r airwindows-lv2-$(VERSION)-amd64-windows.zip airwindows-lv2-$(VERSION)-amd64-windows
	rm -r airwindows-lv2-$(VERSION)-amd64-windows

airwindows-lv2-$(VERSION)-i686-windows.zip: $(SOURCES) cross/windows-i686.txt
	rm -rf build-i686-windows
	meson setup --cross-file cross/windows-i686.txt build-i686-windows
	meson compile -C build-i686-windows
	meson install --destdir . -C build-i686-windows
	test -d build-i686-windows/usr/local/lib/lv2/Airwindows.lv2
	mkdir airwindows-lv2-$(VERSION)-i686-windows
	cp -r build-i686-windows/usr/local/lib/lv2/Airwindows.lv2 airwindows-lv2-$(VERSION)-i686-windows
	rm -rf build-i686-windows
	cp LICENSE airwindows-lv2-$(VERSION)-i686-windows
	zip -9 -v -r airwindows-lv2-$(VERSION)-i686-windows.zip airwindows-lv2-$(VERSION)-i686-windows
	rm -r airwindows-lv2-$(VERSION)-i686-windows

.PHONY: all clean install
