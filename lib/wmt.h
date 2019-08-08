/*
 * Copyright (C) Maxim Zhuchkov - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential 
 * Written by Maxim Zhuchkov <q3.max.2011@ya.ru>, May 2019
 */

#ifndef WMT_H_INCLUDED
#define WMT_H_INCLUDED

#ifndef WMT_VERSION
#define WMT_VERSION "1.5"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <errno.h>

#include "zip.h"
#include "log.h"
#include "TinyPngOut.hpp"
#include "jfes.h"

#define MAX_PATH_LEN 2048

/*#ifndef typename(x)
#define typename(x) _Generic((x),                                                 \
        _Bool: "_Bool",                  unsigned char: "unsigned char",          \
         char: "char",                     signed char: "signed char",            \
    short int: "short int",         unsigned short int: "unsigned short int",     \
          int: "int",                     unsigned int: "unsigned int",           \
     long int: "long int",           unsigned long int: "unsigned long int",      \
long long int: "long long int", unsigned long long int: "unsigned long long int", \
        float: "float",                         double: "double",                 \
  long double: "long double",                   char *: "pointer to char",        \
       void *: "pointer to void",                int *: "pointer to int",         \
      default: "other")
#endif */

enum WMT_TerrainTypes { ttsand, ttsandybrush, ttbakedearth, ttgreenmud, ttredbrush, ttpinkrock, ttroad, ttwater, ttclifface, ttrubble, ttsheetice, ttslush, ttmax};
extern const char* WMT_TerrainTypesStrings[];
extern unsigned short WMT_maptileoffset;

bool WMT_equalstr(char* trg, const char* chk);
int WMT_str_cut(char *str, int begin, int len);
bool WMT_str_match(char* str, char* sub);
int WMT_SearchFilename(char** arr, unsigned short sizearr, char* name, short urgent);
void WMT_PrintInfoAboutMap(struct WZmap map);

void WMT_ReadMap(char* filename, struct WZmap *map);
char* WMT_WriteImage(struct WZmap *map, bool CustomPath, char* CustomOutputPath, struct ImageOptions);

enum WZtileset { tileset_arizona, tileset_urban, tileset_rockies };

struct WZobject {
	char name[128];
	uint32_t id;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t direction;
	uint32_t player;
	bool infire;
	uint32_t burnStart;
	uint32_t burnDamage;
	uint8_t structPadding;
	uint8_t structPadding1;
	uint8_t structPadding2;
	uint8_t structPadding3;
	int32_t buildPoints;
	uint32_t body;
	uint32_t armour;
	uint32_t resistance;
	uint32_t dummy;
	uint32_t subjectInc;
	uint32_t timeStarted;
	uint32_t output;
	uint32_t capacity;
	uint32_t quantity;
	
	uint32_t factoryInc;
	uint8_t loopsPerformed;
	uint8_t structPadding4;
	uint8_t structPadding5;
	uint8_t structPadding6;
	uint32_t powerAccrued;
	uint32_t dummy2;
	uint32_t droidTimeStarted;
	uint32_t timeToBuild;
	uint32_t timeStartHold;
	
	uint8_t visibility[8];
	
	char researchName[60];
	//int type;
	//char script[32];
};

struct WZfeature {
	char name[128];
	uint32_t id;
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t direction;
	uint32_t player;
	uint32_t inFire;
	uint32_t burnStart;
	uint32_t burnDamage;
};

struct WZmap {
	char* path;
	char* mapname;
	
	bool valid = true;
	int errorcode = 0;
	
	char **filenames = NULL;
	zip_t *zip;
	int totalentries = -1;
	
	char createdon[128];
	char createddate[128];
	char createdauthor[128];
	char createdlicense[128];
	bool haveadditioninfo = false;

	unsigned int ttypver = -1;
	unsigned int ttypnum = -1;
	unsigned short ttyptt[1200];
	WZtileset tileset = tileset_arizona;

	unsigned int maparrsize = 0;
	unsigned int mapver = -1;
	unsigned int maptotalx = -1;
	unsigned int maptotaly = -1;
	bool mapwater[90000];
	bool mapcliff[90000];
	unsigned short *mapheight;
	
	uint32_t structVersion;
	uint32_t numStructures;
	WZobject *structs = NULL;
	
	uint32_t featureVersion;
	uint32_t featuresCount;
	WZfeature *features = NULL;
	
	bool fields_clean = false;
	
	~WZmap() {
		if(fields_clean) {
			log_info("Prevented double clean!\n");
			return;
		}
		free(mapheight);
		free(filenames);
		free(structs);
		free(features);
		for(int i=0; i<totalentries; i++)
			free(filenames[i]);
		zip_entry_close(zip);
		zip_close(zip);
		fields_clean=true;
	}
};

struct ImageOptions {
	bool DrawWater = true;
	bool DrawCliffsAsRed = true;
	bool DrawBuildings = true;
	bool DrawOilRigs = true;
	bool SinglecolorWater = false;
	bool ZoomEnabled = false;
	int ZoomLevel = 1;
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
		log_debug("Putting pixel to %d %d in %d", x, y, y*w*3+x*3);
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















