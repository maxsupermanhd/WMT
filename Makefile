

all: log.o png.o zip.o wmt.o
	g++ wmt.o log.o png.o zip.o main.c -o WMT -g -O2 -Wall
log.o:
	g++ log.c -o log.o -c -DLOG_USE_COLOR -O2 -Wall -g
png.o:
	g++ TinyPngOut.cpp -o png.o -c -O2 -Wall -g
zip.o:
	g++ zip.c -o zip.o -c -O2 -Wall -g
wmt.o:
	g++ wmt.c -o wmt.o -c -O2 -Wall -g
clean:
	rm *.o WMT
