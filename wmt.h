/*
 * Copyright (C) Maxim Zhuchkov - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential 
 * Written by Maxim Zhuchkov <q3.max.2011@ya.ru>, May 2019
 */

#ifndef WMT_H_INCLUDED
#define WMT_H_INCLUDED

#ifndef WMT_VERSION
#define WMT_VERSION 0
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <fstream>
#include <iostream>

#include "zip.h"
#include "log.h"
#include "TinyPngOut.hpp"
#include "jfes.h"

#define MAX_PATH_LEN 2048

enum WMT_TerrainTypes { ttsand, ttsandybrush, ttbakedearth, ttgreenmud, ttredbrush, ttpinkrock, ttroad, ttwater, ttclifface, ttrubble, ttsheetice, ttslush, ttmax};
extern const char* WMT_TerrainTypesStrings[];
extern unsigned short WMT_maptileoffset;

bool WMT_equalstr(char* trg, const char* chk);
int WMT_str_cut(char *str, int begin, int len);
bool WMT_str_match(char* str, char* sub);
int WMT_SearchFilename(char** arr, unsigned short sizearr, char* name, short urgent);

struct WZmap WMT_ReadMap(char* filename);
char* WMT_WriteImage(struct WZmap map, bool CustomPath, char* CustomOutputPath, int picturezoom);

struct WZmap {
	char* path;
	char* mapname;
	bool valid = true;
	
	char **filenames = NULL;
	struct zip_t *zip;
	int totalentries = -1;

	void *ttpcontents = NULL;
	char ttphead[5] = { '0', '0', '0', '0', '\0'};
	unsigned int ttypver = -1;
	unsigned int ttypnum = -1;
	unsigned short ttyptt[1200];

	void *mapcontents = NULL;
	char maphead[5] = { '0', '0', '0', '0', '\0'};
	unsigned int maparrsize = 0;
	unsigned int mapver = -1;
	unsigned int maptotalx = -1;
	unsigned int maptotaly = -1;
	bool mapwater[90000];
	bool mapcliff[90000];
	unsigned short *mapheight;
	
	~WZmap() {
		free(ttpcontents);
		free(mapcontents);
		free(mapheight);
		free(filenames);
		for(int i=0; i<totalentries; i++)
			free(filenames[i]);
		zip_entry_close(zip);
		zip_close(zip);
	}
};

struct PngImage {
	int w=0, h=0;
	long totalpixels;
	char filename[MAX_PATH_LEN];
	uint8_t* pixels;
	
	PngImage(unsigned int nw, unsigned int nh) {
		w=nw;
		h=nh;
		totalpixels = nw*nh*3;
		pixels = (uint8_t*) malloc(totalpixels*sizeof(uint8_t));
	}
	~PngImage() {
		free(pixels);
	}
	bool PutPixel(short x, short y, uint8_t r, uint8_t g, uint8_t b) {
		if(x<0 || x>w) {
			log_error("Putting pixel out of bound! (putting to %d %d)", x, y);
			return false;
		}
		if(y<0 || y>h) {
			log_error("Putting pixel out of bound! (putting to %d %d)", x, y);
			return false;
		}
		//log_debug("Putting pixel to %d %d in %d", x, y, y*w*3+x*3);
		pixels[y*w*3+x*3+0] = r;
		pixels[y*w*3+x*3+1] = g;
		pixels[y*w*3+x*3+2] = b;
		return true;
	}
	void WriteImage(char filename[MAX_PATH_LEN]) {
		try {
			std::ofstream out(filename, std::ios::binary);
			TinyPngOut pngout(static_cast<uint32_t>(w), static_cast<uint32_t>(h), out);
			pngout.write(pixels, static_cast<size_t>(w*h));
		} catch (const std::exception& ex) {
			log_error("%s", ex.what());
		}
	}
};

#endif /* WMT_H_INCLUDED */















