all: WMT

WMT: log.o png.o zip.o wmt.o main.c
	g++ wmt.o log.o png.o zip.o main.c -o WMT -g -O2 -Wall
log.o: log.c log.h
	g++ log.c -o log.o -c -DLOG_USE_COLOR -O2 -Wall -g
png.o: TinyPngOut.cpp TinyPngOut.hpp
	g++ TinyPngOut.cpp -o png.o -c -O2 -Wall -g
zip.o: zip.c zip.h
	g++ zip.c -o zip.o -c -O2 -Wall -g
wmt.o: wmt.c wmt.h
	g++ wmt.c -o wmt.o -c -O2 -Wall -g
clean:
	rm *.o WMT
