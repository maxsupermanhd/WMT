all: log png zip
	g++ -Izip/src/ -Lzip/build log.o TinyPngOut.o zip.o main.c -o WMT -g -DLOG_USE_COLOR -O2 -Wall
log:
	g++ log.c -o log.o -c -DLOG_USE_COLOR -O2 -Wall
png:
	g++ TinyPngOut.cpp -o TinyPngOut.o -c -O2 -Wall
zip:
	g++ zip.c -o zip.o -c -O2 -Wall
clean:
	rm *.o WMT
