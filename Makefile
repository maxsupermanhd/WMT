all: WMT

WMT: log.o png.o zip.o wmt.o main.c
	g++ wmt.o log.o png.o zip.o main.c -o WMT -g -Wall -std=c++17
log.o: log.c log.h
	g++ log.c -o log.o -c -DLOG_USE_COLOR -Wall -g
png.o: TinyPngOut.cpp TinyPngOut.hpp
	g++ TinyPngOut.cpp -o png.o -c -Wall -g
zip.o: zip.c zip.h
	g++ zip.c -o zip.o -c -Wall -g
wmt.o: wmt.c wmt.h
	g++ wmt.c -o wmt.o -c -std=c++17 -Wall -g
clean:
	rm *.o WMT
