CCFLAGS=-O2

all: WMT

debug: CCFLAGS += -DDEBUG -g -Wall
debug: WMT

pkg: WMT
	cp WMT pkg/usr/bin
	fakeroot dpkg-deb --build pkg WMT.deb

check-pkg: pkg
	lintian WMT.deb

WMT: build/log.o build/png.o build/zip.o build/wmt.o src/main.c
	g++ build/wmt.o build/log.o build/png.o build/zip.o src/main.c -o WMT $(CCFLAGS)
build/log.o: lib/log.c lib/log.h
	g++ lib/log.c -o build/log.o -c -DLOG_USE_COLOR $(CCFLAGS)
build/png.o: lib/TinyPngOut.cpp lib/TinyPngOut.hpp
	g++ lib/TinyPngOut.cpp -o build/png.o -c $(CCFLAGS)
build/zip.o: lib/zip.c lib/zip.h
	g++ lib/zip.c -o build/zip.o -c $(CCFLAGS)
build/wmt.o: lib/wmt.c lib/wmt.h
	g++ lib/wmt.c -o build/wmt.o -c -std=c++17 $(CCFLAGS)
clean:
	rm build WMT WMT.deb -rf
