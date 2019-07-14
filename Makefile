CCFLAGS=-O2

all: WMT

debug: CCFLAGS += -DDEBUG -g -Wall
debug: WMT

pkg: WMT
	cp WMT pkg/usr/bin
	fakeroot dpkg-deb --build pkg WMT.deb

check-pkg: pkg
	lintian WMT.deb

WMT: log.o png.o zip.o wmt.o main.c
	g++ wmt.o log.o png.o zip.o main.c -o WMT $(CCFLAGS)
log.o: log.c log.h
	g++ log.c -o log.o -c -DLOG_USE_COLOR $(CCFLAGS)
png.o: TinyPngOut.cpp TinyPngOut.hpp
	g++ TinyPngOut.cpp -o png.o -c $(CCFLAGS)
zip.o: zip.c zip.h
	g++ zip.c -o zip.o -c $(CCFLAGS)
wmt.o: wmt.c wmt.h
	g++ wmt.c -o wmt.o -c -std=c++17 $(CCFLAGS)
clean:
	rm *.o WMT WMT.deb -f
