

all: log.o png.o zip.o wmt.o
	g++ wmt.o log.o png.o zip.o main.c -o WMT -g -O0 -Wall
log.o:
	g++ log.c -o log.o -c -DLOG_USE_COLOR -O0 -Wall
png.o:
	g++ TinyPngOut.cpp -o png.o -c -O0 -Wall
zip.o:
	g++ zip.c -o zip.o -c -O0 -Wall
wmt.o:
	g++ wmt.c -o wmt.o -c -O0 -Wall
clean:
	rm *.o WMT
