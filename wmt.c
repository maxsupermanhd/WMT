/*
 * Copyright (C) Maxim Zhuchkov - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Maxim Zhuchkov <q3.max.2011@ya.ru>, May 2019
 */

#include "wmt.h"

const char *TerrainTypesStrings[] = {"sand", "sandy brush", "baked earth", "green mud", "red brush", "pink rock", "road", "water", "cliffface", "rubble", "sheetice", "slush", "max"};




bool equalstr(char* trg, const char* chk) {
	if(strlen(trg)!=strlen(chk))
		return false;
	for(unsigned int c=0; c<strlen(trg); c++)
		if(trg[c]!=chk[c])
			return false;
	return true;
}

int str_cut(char *str, int begin, int len) {
    int l = strlen(str);
    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);
    return len;
}

bool str_match(char* str, char* sub) {
	log_trace("String match between \"%s\" and \"%s\"", str, sub);
	int i, j=0, k;
	for(i=0; str[i]; i++) {
		if(str[i] == sub[j]) {
			for(k=i, j=0; str[k] && sub[j]; j++, k++)
				if(str[k]!=sub[j]) {
					log_trace("string constr fail in");
					log_trace("str1 %s", str);
					log_trace("     %*c^", k, ' ');
					log_trace("str2 %s", sub);
					log_trace("     %*c^", j, ' ');
					log_trace("on %d %d bc %d != %d", k, j, str[k], sub[j]);
					break;
				}
			if(!sub[j]) {
				return true;
			}
		}
	}
	if(strstr(str, sub)) {
		log_info("strstr worked!");
		return true;
	}
	return false;
}


int SearchFilename(char** arr, unsigned short sizearr, char* name, short urgent = 0) {
	log_trace("Serching filename \"%s\" in array of filenames...", name);
	int foundindex=-1;
	for(int index=0; index<sizearr; index++)
		if(strstr(arr[index], name))
			foundindex=index;
	if(foundindex==-1) {
		switch(urgent) {
			case 2:
				log_fatal("Failed to find \"%s\".", name);
				exit(0);
			break;
			case 1:
				log_warn("Failed to find \"%s\".", name);
				return -1;
			break;
			default:
				log_trace("Failed to find \"%s\".", name);
				return -1;
			break;
		}
	} else {
		log_trace("Serching filename \"%s\" in array of filenames... %d (%s)", name, foundindex, arr[foundindex]);
		return foundindex;
	}
}
