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

enum TerrainTypes { ttsand, ttsandybrush, ttbakedearth, ttgreenmud, ttredbrush, ttpinkrock, ttroad, ttwater, ttclifface, ttrubble, ttsheetice, ttslush, ttmax};
extern const char* TerrainTypesStrings[];

bool equalstr(char* trg, const char* chk);
int str_cut(char *str, int begin, int len);
bool str_match(char* str, char* sub);
int SearchFilename(char** arr, unsigned short sizearr, char* name, short urgent);

struct PngImage {
	int w=0, h=0;
	long totalpixels;
	char filename[MAX_PATH_LEN];
	uint8_t pixels[187500];
	
	PngImage();
	
	PngImage(unsigned int nw, unsigned int nh)
	{
		w=nw;
		h=nh;
		totalpixels = nw*nh*3;
	}
	
	bool PutPixel(short x, short y, uint8_t r, uint8_t g, uint8_t b)
	{
		if(x<0 || x>w) {
			log_error("Putting pixel out of bound! (putting to %d %d)", x, y);
			return false;
		}
		if(y<0 || y>h) {
			log_error("Putting pixel out of bound! (putting to %d %d)", x, y);
			return false;
		}
		pixels[y*h*3+x*3+0] = r;
		pixels[y*h*3+x*3+1] = g;
		pixels[y*h*3+x*3+2] = b;
		return true;
	}
	
	void WriteImage(char filename[MAX_PATH_LEN])
	{
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















