/*
 * Copyright (C) Maxim Zhuchkov - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Maxim Zhuchkov <q3.max.2011@ya.ru>, May 2019
 */

#include "wmt.h"

const char *WMT_TerrainTypesStrings[] = {"sand", "sandy brush", "baked earth", "green mud", "red brush", "pink rock", "road", "water", "cliffface", "rubble", "sheetice", "slush", "max"};
unsigned short WMT_maptileoffset = 0x01ff;




bool WMT_equalstr(char* trg, const char* chk) {
	if(strlen(trg)!=strlen(chk))
		return false;
	for(unsigned int c=0; c<strlen(trg); c++)
		if(trg[c]!=chk[c])
			return false;
	return true;
}

int WMT_str_cut(char *str, int begin, int len) {
    int l = strlen(str);
    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);
    return len;
}

bool WMT_str_match(char* str, char* sub) {
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

void WMT_PrintObject(WZobject obj) {
	printf("WZobject dump:\n");
	printf("Name:           %s\n", obj.name);
	printf("Id:             %d\n", obj.id);
	printf("X:              %d\n", obj.x);
	printf("Y:              %d\n", obj.y);
	printf("Z:              %d\n", obj.z);
	printf("Direction:      %d\n", obj.direction);
	printf("Player:         %d\n", obj.player);
	if(obj.infire!=0) {
		printf("In fire:        %d\n", obj.infire);
		printf("Burn start:     %d\n", obj.burnStart);
		printf("Burn damage:    %d\n", obj.burnDamage);
	}
	if(obj.structPadding!=1)
		printf("Struct padding: %d\n", obj.structPadding);
	if(obj.buildPoints!=0)
		printf("Build points:   %d\n", obj.buildPoints);
	if(obj.body!=0)
		printf("Body:           %d\n", obj.body);
	if(obj.armour!=0)
		printf("Armour:         %d\n", obj.armour);
	if(obj.resistance!=0)
		printf("Resistance:     %d\n", obj.resistance);
	if(obj.subjectInc!=0)
		printf("Subject inc.:   %d\n", obj.subjectInc);
	if(obj.timeStarted!=0)
		printf("Time started:   %d\n", obj.timeStarted);
	if(obj.output!=0)
		printf("Output:         %d\n", obj.output);
	if(obj.capacity!=0)
		printf("Capacity:       %d\n", obj.capacity);
	if(obj.quantity!=0)
		printf("Quantity:       %d\n", obj.quantity);
	//printf("visibility [0]: %d\n", obj.visibility[0]);
	//printf("visibility [1]: %d\n", obj.visibility[1]);
	//printf("visibility [2]: %d\n", obj.visibility[2]);
	//printf("visibility [3]: %d\n", obj.visibility[3]);
	//printf("visibility [4]: %d\n", obj.visibility[4]);
	//printf("visibility [5]: %d\n", obj.visibility[5]);
	//printf("visibility [6]: %d\n", obj.visibility[6]);
	//printf("visibility [7]: %d\n", obj.visibility[7]);
}

void WMT_PrintObjectShort(WZobject obj) {
	printf("WZobject   %s\n", obj.name);
	printf("Id:        %d\n", obj.id);
	printf("X:         %d\n", obj.x);
	printf("Y:         %d\n", obj.y);
	printf("Z:         %d\n", obj.z);
	//printf("Direction: %d\n", obj.direction);
	printf("Player:    %d\n", obj.player);
}

void WMT_PrintFeatureShort(WZfeature feat) {
	printf("WZfeature: %s\n", feat.name);
	printf("Id:        %d\n", feat.id);
	printf("X:         %d\n", feat.x);
	printf("Y:         %d\n", feat.y);
	printf("Z:         %d\n", feat.z);
	printf("Player:    %d\n", feat.player);
}

int WMT_SearchFilename(char** arr, unsigned short sizearr, char* name, short urgent = 0) {
	//log_trace("Serching filename \"%s\" in array of filenames...", name);
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
		//log_trace("Serching filename \"%s\" in array of filenames... %d (%s)", name, foundindex, arr[foundindex]);
		return foundindex;
	}
}

char* WMT_GetMapNameFromFilename(char* filename) {
	char* fullpath=filename;
	int fullpathlen=strlen(fullpath);
	int lastindex=-1;
	for(int c=0; c<fullpathlen; c++) {
		if(fullpath[c] == '/')
			lastindex=c;
	}
	if(lastindex!=-1)
		WMT_str_cut(fullpath, 0, lastindex+1);
	if(strlen(fullpath)>67)
		WMT_str_cut(fullpath, strlen(fullpath)-68, strlen(fullpath)-3);
	else
		WMT_str_cut(fullpath, strlen(fullpath)-3, -1);
	return fullpath;
}

bool WMT_ListFiles(WZmap *map) { //pass pointer to struct to make changes
	map->totalentries = zip_total_entries(map->zip);
	map->filenames = (char**) calloc(map->totalentries, sizeof(char*));
	for(int i=0; i<map->totalentries; i++)
		map->filenames[i] = (char*) calloc(1024, sizeof(char));
	if(!map->filenames) {
		log_fatal("Filenames array allocation fail!");
		map->errorcode = -2;
		return false;
	}
	for(int i=0; i<map->totalentries; i++) {
		if(!map->filenames[i]) {
			log_fatal("Filename %d allocation fail!");
			map->errorcode = -2;
			return false;
		}
	}
	//log_trace("Listing all shit up (%d) ...", map->totalentries);
	for(int index=0; index<map->totalentries; index++) {
		//log_trace("Trying index %d", index);
		int ret = zip_entry_openbyindex(map->zip, index);
		if(ret>=0) {
			snprintf(map->filenames[index], 1024, "%s", (char*)zip_entry_name(map->zip));
			//log_trace("%d.\t%s", index, map->filenames[index]);
			zip_entry_close(map->zip);
		}
		else {
			log_error("Error openig file %d! Status %d", index, ret);
		}
	}
	return true;
}

char* WMT_PrintTilesetName(WZtileset t) {
	switch(t) {
	case tileset_arizona:
	return (char*)"Arizona";
	break;
	case tileset_urban:
	return (char*)"Urban";
	break;
	case tileset_rockies:
	return (char*)"Rockies";
	break;
	default:
	return (char*)"Unknown";
	break;
	}
}

void WMT_PrintInfoAboutMap(struct WZmap map) {
	if(!map.valid) {
		printf("Not valid map!\n");
		return;
	}
	printf("Map: %s\n", map.mapname);
	printf("Tileset: %s\n", WMT_PrintTilesetName(map.tileset));
	printf("Size: %dx%d\n", map.maptotalx, map.maptotaly);
}

bool WMT_ReadFromFile(FILE *fp, size_t s, size_t v, void *var) {
	size_t r = fread(var, s, v, fp);
	//log_debug("Fread return %d need %d", r, v);
	if(r != v) {
		log_warn("Fread return count (%d) not (%d)!", r, v);
	}
	if(ferror(fp)) {
			log_fatal("Fread read only %ld instead of %lu. Cursor: %ld Readed: %u", r, s, ftell(fp), var);
			log_fatal("Fread error: %s", strerror(errno));
			return false;
	}
	return true;
}

bool WMT_ReadTTypesFile(WZmap *map) {
	bool success = true;
	int indexttypes = WMT_SearchFilename(map->filenames, map->totalentries, (char*)"ttypes.ttp", 2);
	int openstatus = zip_entry_openbyindex(map->zip, indexttypes);
	if(openstatus<0) {
		log_fatal("Opening file by index error! Status %d.", openstatus);
		map->errorcode = -4;
		success = false;
	} else {
		//size_t ttpfilesize = zip_entry_size(map->zip);
		size_t readlen;
		void *ttpcontents;
		ssize_t readed = zip_entry_read(map->zip, &ttpcontents, &readlen);
		if(readed==-1) {
			log_fatal("Error reading ttypes file!");
		} else {
			FILE* ttpf = fmemopen(ttpcontents, readlen, "r");
			if(ttpf==NULL) {
				log_fatal("Error opening as file descriptor!");
				success = false;
			} else {
				char ttphead[5] = { '0', '0', '0', '0', '\0'};
				if(!WMT_ReadFromFile(ttpf, sizeof(char), 4, &ttphead))
					log_error("Failed to read ttypes header!");
				if(ttphead[0] != 't' ||
				   ttphead[1] != 't' ||
				   ttphead[2] != 'y' ||
				   ttphead[3] != 'p')
					log_warn("Ttypes file header not \'ttyp\'!");
				if(!WMT_ReadFromFile(ttpf, sizeof(unsigned int), 1, &map->ttypver))
					log_error("Failed to read ttypes version!");
				if(!WMT_ReadFromFile(ttpf, sizeof(unsigned int), 1, &map->ttypnum))
					log_error("Failed to read number of terrain types!");
				if(!WMT_ReadFromFile(ttpf, sizeof(unsigned short), map->ttypnum, &map->ttyptt))
					log_error("Failed to read terrain types!");
				
				uint8_t TileSetProbe[3];
				TileSetProbe[0] = (uint8_t)map->ttyptt[0];
				TileSetProbe[1] = (uint8_t)map->ttyptt[1];
				TileSetProbe[2] = (uint8_t)map->ttyptt[2];
				
				if (TileSetProbe[0] == 1 && TileSetProbe[1] == 0 && TileSetProbe[2] == 2)
					map->tileset = tileset_arizona;
				else if (TileSetProbe[0] == 2 && TileSetProbe[1] == 2 && TileSetProbe[2] == 2)
					map->tileset = tileset_urban;
				else if (TileSetProbe[0] == 0 && TileSetProbe[1] == 0 && TileSetProbe[2] == 2)
					map->tileset = tileset_rockies;
				
				//printf("Tileset: %s\n", WMT_PrintTilesetName(map->tileset));
				
				fclose(ttpf);
				//printf("Results of readyng ttypes.ttp:\n");
				//printf("Header: \"%s\"\n", ttphead);
				//printf("Version: %d\n", map->ttypver);
				//printf("Types:  %d\n", map->ttypnum);		
			}
			free(ttpcontents);
			ttpcontents = NULL;
		}
		zip_entry_close(map->zip);
	}
	return success;
}

bool WMT_ReadGameMapFile(WZmap *map) {
	bool success = true;
	int indexgamemap = WMT_SearchFilename(map->filenames, map->totalentries, (char*)"game.map", 1);
	int openstatus;
	if(indexgamemap==-1) {
		log_warn("Failed to search game.map file! Trying second way!");
		char mapnewpath[MAX_PATH_LEN];
		for(int i=0; i<MAX_PATH_LEN; i++)
			mapnewpath[i]='0';
		snprintf(mapnewpath, MAX_PATH_LEN, "multiplay/maps/%s/game.map", map->mapname);
		openstatus = zip_entry_open(map->zip, mapnewpath);
	} else {
		openstatus = zip_entry_openbyindex(map->zip, indexgamemap);
	}
	if(openstatus<0) {
		log_fatal("Failed to open game.map file!");
		map->errorcode = -4;
		success = false;
	} else {
		//size_t mapfilesize = zip_entry_size(map->zip);
		void *mapcontents = NULL;
		size_t readlen;
		ssize_t readed = zip_entry_read(map->zip, &mapcontents, &readlen);
		if(readed==-1) {
			log_warn("Zip file reading error!");
		}
		FILE* mapf = fmemopen(mapcontents, readlen, "r");
		if(mapf==NULL) {
			log_fatal("Error opening file from memory!");
			success = false;
		} else {
			char maphead[5] = { '0', '0', '0', '0', '\0'};
			if(!WMT_ReadFromFile(mapf, sizeof(char), 4, &maphead))
				log_error("Failed to read ttypes header!");
			if(maphead[0] != 'm' ||
			   maphead[1] != 'a' ||
			   maphead[2] != 'p')
				log_warn("Map file header not \'map\'!");
			if(!WMT_ReadFromFile(mapf, sizeof(unsigned int), 1, &map->mapver))
				log_error("Failed to read map file version!");
			if(!WMT_ReadFromFile(mapf, sizeof(unsigned int), 1, &map->maptotalx))
				log_error("Failed to read map bounds (x)");
			if(!WMT_ReadFromFile(mapf, sizeof(unsigned int), 1, &map->maptotaly))
				log_error("Failed to read map bounds (y)");
			//printf("\nResults of reading game.map\n");
			//printf("Version: %d\n", map->mapver);
			//printf("Width:   %d\n", map->maptotaly);
			//printf("Height:  %d\n", map->maptotalx);
			
			int maparraysize = map->maptotaly*map->maptotalx;
			map->mapheight = (unsigned short*) calloc(maparraysize, sizeof(unsigned short));
			if(map->mapheight==NULL) {
				log_fatal("Height array allocation failed!");
			}
			unsigned short maptileinfo = 0;
			ssize_t mapreadret = -1;
			short maptiletexture = -1;
			WMT_TerrainTypes maptileterrain = ttsand;
			for(int counter=0; counter<maparraysize; counter++)
			{
				//nowmapy = counter/map->maptotaly;
				//nowmapx = counter-(nowmapy*map->maptotalx);
				mapreadret = fread(&maptileinfo, 2, 1, mapf);
				if(mapreadret != 1)
					log_error("Fread scanned %d elements instead of %d (tileinfo)", mapreadret, 1);
				maptiletexture = (maptileinfo & WMT_maptileoffset);
				maptileterrain = (WMT_TerrainTypes)map->ttyptt[maptiletexture];
				if(maptileterrain == ttwater) {
					map->mapwater[counter]=true;
				}
				if(maptileterrain == ttclifface) {
					map->mapcliff[counter]=true;
				}
				mapreadret = fread(&map->mapheight[counter], 1, 1, mapf);
				if(mapreadret != 1)
					log_error("Fread scanned %d elements instead of %d (height)", mapreadret, 1);
			}
		}
		free(mapcontents);
		mapcontents = NULL;
		zip_entry_close(map->zip);
	}
	return success;
}

void WMT_ReadAddonLev(WZmap *map) {
	char addonpath[MAX_PATH_LEN];
	for(int i=0; i<MAX_PATH_LEN; i++)
		addonpath[i]='0';
	int opened = 0;
	snprintf(addonpath, MAX_PATH_LEN, "%s.xplayers.lev", map->mapname);
	opened = zip_entry_open(map->zip, addonpath);
	if(opened < 0) {
		for(int i=0; i<MAX_PATH_LEN; i++)
			addonpath[i]='0';
		snprintf(addonpath, MAX_PATH_LEN, "%s.addon.lev", map->mapname);
		opened = zip_entry_open(map->zip, addonpath);
	}
	if(opened<0) {
		log_fatal("Failed to open addon.lev file!");
		map->errorcode = -4;
	} else {
		void *addoncontents;
		size_t readlen;
		ssize_t readed = zip_entry_read(map->zip, &addoncontents, &readlen);
		if(readed==-1) {
			log_warn("Zip file reading error!");
		}
		FILE* addonf = fmemopen(addoncontents, readlen, "r");
		if(addonf==NULL) {
			log_fatal("Error opening file from memory!");
		} else {
			char addonhead[4] = { '0', '0', '0', '\0'};
			if(!WMT_ReadFromFile(addonf, sizeof(char), 3, &addonhead))
				log_error("Failed to read ttypes header!");
			else {
				if(addonhead[0] != '/' || addonhead[1] != '/' || addonhead[2] != ' ')
					log_warn("Addon file does not contain created info!!");
				else {
					char addonbuff = 2;
					char addonwrite[128];
					for(int i=0; i<128; i++)
						addonwrite[i] = 0;
					int addoncounter = 0;
					do {
						if(!WMT_ReadFromFile(addonf, sizeof(char), 1, &addonbuff)) {
							log_error("Failed to read from memory!");
							break;
						} else {
							addonwrite[addoncounter] = addonbuff;
							addoncounter++;
						}
					} while (addonbuff != '\n');
					for(int i=0; i<128; i++)
						map->createdon[i] = addonwrite[i];
				}
			}
			if(!WMT_ReadFromFile(addonf, sizeof(char), 3, &addonhead))
				log_error("Failed to read ttypes header!");
			else {
				if(addonhead[0] != '/' || addonhead[1] != '/' || addonhead[2] != ' ')
					log_warn("Addon file does not contain created info!!");
				else {
					char addonbuff = 2;
					char addonwrite[128];
					for(int i=0; i<128; i++)
						addonwrite[i] = 0;
					int addoncounter = 0;
					do {
						if(!WMT_ReadFromFile(addonf, sizeof(char), 1, &addonbuff)) {
							log_error("Failed to read from memory!");
							break;
						} else {
							addonwrite[addoncounter] = addonbuff;
							addoncounter++;
						}
					} while (addonbuff != '\n');
					for(int i=0; i<128; i++)
						map->createddate[i] = addonwrite[i];
				}
			}
			if(!WMT_ReadFromFile(addonf, sizeof(char), 3, &addonhead))
				log_error("Failed to read ttypes header!");
			else {
				if(addonhead[0] != '/' || addonhead[1] != '/' || addonhead[2] != ' ')
					log_warn("Addon file does not contain created info!!");
				else {
					char addonbuff = 2;
					char addonwrite[128];
					for(int i=0; i<128; i++)
						addonwrite[i] = 0;
					int addoncounter = 0;
					do {
						if(!WMT_ReadFromFile(addonf, sizeof(char), 1, &addonbuff)) {
							log_error("Failed to read from memory!");
							break;
						} else {
							addonwrite[addoncounter] = addonbuff;
							addoncounter++;
						}
					} while (addonbuff != '\n');
					for(int i=0; i<128; i++)
						map->createdauthor[i] = addonwrite[i];
				}
			}
			if(!WMT_ReadFromFile(addonf, sizeof(char), 3, &addonhead))
				log_error("Failed to read ttypes header!");
			else {
				if(addonhead[0] != '/' || addonhead[1] != '/' || addonhead[2] != ' ')
					log_warn("Addon file does not contain created info!!");
				else {
					char addonbuff = 2;
					char addonwrite[128];
					for(int i=0; i<128; i++)
						addonwrite[i] = 0;
					int addoncounter = 0;
					do {
						if(!WMT_ReadFromFile(addonf, sizeof(char), 1, &addonbuff)) {
							log_error("Failed to read from memory!");
							break;
						} else {
							addonwrite[addoncounter] = addonbuff;
							addoncounter++;
						}
					} while (addonbuff != '\n');
					for(int i=0; i<128; i++)
						map->createdlicense[i] = addonwrite[i];
				}
			}
			map->haveadditioninfo = true;
			fclose(addonf);
		}
		free(addoncontents);
		addoncontents = NULL;
		zip_entry_close(map->zip);
	}
	return;
}

bool WMT_ReadStructs(WZmap *map) {
	bool success = true;
	int indexstructs = WMT_SearchFilename(map->filenames, map->totalentries, (char*)"struct.bjo");
	if(indexstructs == -1) {
		log_fatal("Failed to find struct.bjo!");
		return false;
	} 
	int openstatus = zip_entry_openbyindex(map->zip, indexstructs);
	if(openstatus<0) {
		log_fatal("Failed to open struct.bjo!");
		map->errorcode = -4;
		success = false;
	} else {
		void *structcontents;
		size_t readlen;
		ssize_t readed = zip_entry_read(map->zip, &structcontents, &readlen);
		if(readed == -1) {
			log_fatal("Failed to read struct.bjo!");
			success = false;
		} else {
			FILE* structf = fmemopen(structcontents, readlen, "r");
			if(structf==NULL) {
				log_fatal("Error opening struct.bjo from memory!");
				success = false;
			} else {
				char structshead[5] = { '0', '0', '0', '0', '\0'};
				if(!WMT_ReadFromFile(structf, sizeof(char), 4, &structshead))
					log_error("Failed to read struct header!");
				if(structshead[0] != 's' ||
				   structshead[1] != 't' ||
				   structshead[2] != 'r' ||
				   structshead[3] != 'u')
					log_warn("Struct file header not \'stru\'! (%s)", structshead);
				if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structVersion))
					log_error("Failed to read struct file version!");
				if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->numStructures))
					log_error("Failed to read structure count!");
				
				//printf("Struct version: %d\n", map->structVersion);
				//printf("Structs count:  %d\n", map->numStructures);
				
				
				map->structs = (WZobject*)malloc(sizeof(WZobject)*map->numStructures);
				if(map->structs == NULL) {
					log_fatal("Failed to allocate memory for structures!");
					map->errorcode = -2;
					return false;
				}
				int nameLength = 60;
				if(map->structVersion <= 19)
					nameLength = 40;
				
				
				for(unsigned int structnum = 0; structnum<map->numStructures; structnum++) {
					if(!WMT_ReadFromFile(structf, sizeof(char), nameLength, &map->structs[structnum].name))
						log_error("Failed to read struct name!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].id))
						log_error("Failed to read struct id!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].x))
						log_error("Failed to read struct x!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].y))
						log_error("Failed to read struct y!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].z))
						log_error("Failed to read struct z!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].direction))
						log_error("Failed to read struct direction!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].player))
						log_error("Failed to read struct player!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].infire))
						log_error("Failed to read struct in fire!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].burnStart))
						log_error("Failed to read struct start burn!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].burnDamage))
						log_error("Failed to read struct burn damage!");
					if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].structPadding))
						log_error("Failed to read struct padding!");
					if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].structPadding1))
						log_error("Failed to read struct padding (1)!");
					if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].structPadding2))
						log_error("Failed to read struct padding (2)!");
					if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].structPadding3))
						log_error("Failed to read struct padding (3)!");
					if(!WMT_ReadFromFile(structf, sizeof(int32_t), 1, &map->structs[structnum].buildPoints))
						log_error("Failed to read struct build points!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].body))
						log_error("Failed to read struct body!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].armour))
						log_error("Failed to read struct armour!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].resistance))
						log_error("Failed to read struct resistance!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].dummy))
						log_error("Failed to read some dummy var!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].subjectInc))
						log_error("Failed to read struct subject inc!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].timeStarted))
						log_error("Failed to read struct time started!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].output))
						log_error("Failed to read struct output?!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].capacity))
						log_error("Failed to read struct capacity!");
					if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].quantity))
						log_error("Failed to read struct quantity!");
					if(map->structVersion >= 12) {
						if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].factoryInc))
							log_error("Failed to read struct factory inc!");
						if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].loopsPerformed))
							log_error("Failed to read struct loops!");
						if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].structPadding4))
							log_error("Failed to read struct padding (4)!");
						if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].structPadding5))
							log_error("Failed to read struct padding (5)!");
						if(!WMT_ReadFromFile(structf, sizeof(uint8_t), 1, &map->structs[structnum].structPadding6))
							log_error("Failed to read struct padding (6)!");
						if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].powerAccrued))
							log_error("Failed to read struct power!");
						if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].dummy2))
							log_error("Failed to read another dummy var!");
						if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].droidTimeStarted))
							log_error("Failed to read struct 'droid time started'!");
						if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].timeToBuild))
							log_error("Failed to read struct time to build!");
						if(!WMT_ReadFromFile(structf, sizeof(uint32_t), 1, &map->structs[structnum].timeStartHold))
							log_error("Failed to read struct time start hold!");
					}
					if(map->structVersion >= 14) {
						size_t freadret = fread(&map->structs[structnum].visibility, 8, 1, structf);
						if(freadret != 1)
							log_error("Error reading visibility! Code %d.", freadret);
					}
					if(map->structVersion >= 15) {
						size_t freadret = fread(&map->structs[structnum].researchName, sizeof(char), nameLength, structf);
						if(freadret != 1)
							log_error("Error reading research name! Code %d.", freadret);
					}
					if(map->structVersion >= 17) {
						int16_t dummy;
						WMT_ReadFromFile(structf, sizeof(int16_t), 1, &dummy);
						(void)dummy;
					}
					if(map->structVersion >= 15) {
						int16_t dummy;
						WMT_ReadFromFile(structf, sizeof(int16_t), 1, &dummy);
						(void)dummy;
					}
					if(map->structVersion >= 21) {
						uint32_t dummy;
						WMT_ReadFromFile(structf, sizeof(int16_t), 1, &dummy);
						(void)dummy;
					}
					//WMT_PrintObjectShort(map->structs[structnum]);
				}
				fclose(structf);
			}
			free(structcontents);
			structcontents = NULL;
		}
		zip_entry_close(map->zip);
	}
	return success;
}

bool WMT_ReadFeaturesFile(WZmap *map) {
	bool success = true;
	int indexfeat = WMT_SearchFilename(map->filenames, map->totalentries, (char*)"feat.bjo", 2);
	int openstatus = zip_entry_openbyindex(map->zip, indexfeat);
	if(openstatus<0) {
		log_fatal("Opening file by index error! Status %d.", openstatus);
		map->errorcode = -4;
		success = false;
	} else {
		//size_t featfilesize = zip_entry_size(map->zip);
		size_t readlen;
		void *featcontents;
		ssize_t readed = zip_entry_read(map->zip, &featcontents, &readlen);
		if(readed==-1) {
			log_fatal("Error reading features file!");
		} else {
			FILE* featf = fmemopen(featcontents, readlen, "r");
			if(featf==NULL) {
				log_fatal("Error opening as file descriptor!");
				success = false;
			} else {
				char feathead[5] = { '0', '0', '0', '0', '\0'};
				if(!WMT_ReadFromFile(featf, sizeof(char), 4, &feathead))
					log_error("Failed to read features file header!");
				if(feathead[0] != 'f' ||
				   feathead[1] != 'e' ||
				   feathead[2] != 'a' ||
				   feathead[3] != 't')
					log_warn("Features file header not \'feat\'!");
				if(!WMT_ReadFromFile(featf, sizeof(unsigned int), 1, &map->featureVersion))
					log_error("Failed to read features version!");
				if(!WMT_ReadFromFile(featf, sizeof(unsigned int), 1, &map->featuresCount))
					log_error("Failed to read number of features!");
				
				map->features = (WZfeature*)malloc(map->featuresCount * sizeof(WZfeature));
				if(map->features == NULL) {
					log_error("Error allocating memory for features!");
					map->errorcode = -2;
					return false;
				}
				int nameLength = 60;
				if(map->featureVersion <= 19)
					nameLength = 40;
				
				for(uint32_t featnum = 0; featnum<map->featuresCount; featnum++) {
					WMT_ReadFromFile(featf, sizeof(char), nameLength, &map->features[featnum].name);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].id);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].x);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].y);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].z);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].direction);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].player);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].inFire);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].burnStart);
					WMT_ReadFromFile(featf, sizeof(uint32_t), 1, &map->features[featnum].burnDamage);
					//WMT_PrintFeatureShort(map->features[featnum]);
				}
				
				fclose(featf);
				//printf("Features version: %d\n", map->featureVersion);
				//printf("Features count:   %d\n", map->featuresCount);
			}
			free(featcontents);
			featcontents = NULL;
		}
		zip_entry_close(map->zip);
	}
	return success;
}

void WMT_ReadMap(char* filename, WZmap *map) {
	//struct WZmap map = (struct WZmap)malloc(sizeof(struct WZmap));
	map->path=filename;
	map->zip = zip_open(map->path, 0, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 1, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 2, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 3, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 4, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 5, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 6, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 7, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 8, 'r');
	if(map->zip == NULL)
		map->zip = zip_open(map->path, 9, 'r');
	if(map->zip == NULL) {
		log_fatal("Error opening/reading warzone map file! (bad zip file)");
		map->valid=false;
		map->errorcode = -1;
		return;
	}
	map->mapname = WMT_GetMapNameFromFilename(filename);
	//printf("Map name: \"%s\"\n", map->mapname);
	if(!WMT_ListFiles(map)) {
		log_fatal("Error listing map files!");
		map->valid=false;
		return;
	}
	if(!WMT_ReadTTypesFile(map)) {
		log_fatal("Error reading ttypes file!");
		map->valid=false;
		return;
	}
	if(!WMT_ReadGameMapFile(map)) {
		log_fatal("Error reading map file!");
		map->valid=false;
		return;
	}
	if(!WMT_ReadStructs(map)) {
		log_fatal("Error reading struct file!");
		map->valid=false;
		return;
	}
	if(!WMT_ReadFeaturesFile(map)) {
		log_fatal("Error reading features file!");
		map->valid=false;
		return;
	}
	WMT_ReadAddonLev(map);
	log_info("Map reading done!");
	return;
}

void _WMT_PutZoomPixel(PngImage *img, int zoom, unsigned short x, unsigned short y, uint8_t r, uint8_t g, uint8_t b) {
	for(unsigned short zx = x*zoom; zx<x*zoom+zoom; zx++) {
		for(unsigned short zy = y*zoom; zy<y*zoom+zoom; zy++) {
			log_trace("Putting pixel to %d %d", zx, zy);
			img->PutPixel(zx, zy, r, g, b);
		}
	}
}

char* WMT_WriteImage(struct WZmap *map, bool CustomPath, char* CustomOutputPath, struct ImageOptions options) {
	log_info("Drawing preview...");
	char *pngfilename = (char*)malloc(sizeof(char)*MAX_PATH_LEN);
	if(pngfilename == NULL) {
		log_fatal("Error allocation memory for output filename");
		map->errorcode = -2;
		return (char*)"";
	}
	if(CustomPath) {
		snprintf(pngfilename, MAX_PATH_LEN, "%s", CustomOutputPath);
	} else {
		snprintf(pngfilename, MAX_PATH_LEN, "./%s.png", map->mapname);
	}
	PngImage OutputImg((unsigned int)map->maptotalx*options.ZoomLevel, (unsigned int)map->maptotaly*options.ZoomLevel);
	for(unsigned short counterx=0; counterx<map->maptotalx; counterx++) {
		for(unsigned short countery=0; countery<map->maptotaly; countery++) {
			int nowposinarray = countery*map->maptotalx+counterx;
			if(map->mapwater[nowposinarray] && options.DrawWater) {
				if(options.SinglecolorWater)
					_WMT_PutZoomPixel(&OutputImg, 
									  options.ZoomLevel,
									  counterx, 
									  countery, 
									  0, 
									  0, 
									  255);
				else
					_WMT_PutZoomPixel(&OutputImg, 
									  options.ZoomLevel,
									  counterx, 
									  countery, 
									  map->mapheight[nowposinarray]/4, 
									  map->mapheight[nowposinarray]/4, 
									  map->mapheight[nowposinarray]);
			}
			else if(map->mapcliff[nowposinarray] && options.DrawCliffsAsRed) {
				_WMT_PutZoomPixel(&OutputImg, 
								  options.ZoomLevel,
								  counterx, 
								  countery, 
								  map->mapheight[nowposinarray], 
								  map->mapheight[nowposinarray]/4, 
								  map->mapheight[nowposinarray]/4);
			} else {
				_WMT_PutZoomPixel(&OutputImg, 
								  options.ZoomLevel,
								  counterx, 
								  countery, 
								  map->mapheight[nowposinarray], 
								  map->mapheight[nowposinarray], 
								  map->mapheight[nowposinarray]);
			}
		}
	}
	for(uint32_t i = 0; i<map->numStructures; i++) {
		unsigned short strx = map->structs[i].x/128;
		unsigned short stry = map->structs[i].y/128;
		
		//
		//  [+0] [+1] [+2]
		//  [+0] [+0] [+0]
		//
		//  [+0] [+1] [+2]
		//  [+1] [+1] [+1]
		//
		//  [+0] [+1] [+2]
		//  [+2] [+2] [+2]
		//
		
		if(options.DrawOilRigs) {
			if(strcmp(map->structs[i].name, "A0ResourceExtractor") == 0) {
				log_debug("Found extractor at %d %d", strx, stry);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx, stry, 255, 255, 0);
			}
		}
		if(options.DrawBuildings) {
			if(strcmp(map->structs[i].name, "A0CyborgFactory") == 0) {
				log_debug("Found cyborg at %d %d", strx, stry);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx, stry, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx, stry+1, 0, 255, 0);
			}
			if(strcmp(map->structs[i].name, "A0ResearchFacility") == 0 ||
			   strcmp(map->structs[i].name, "A0CommandCentre") == 0 ||
			   strcmp(map->structs[i].name, "A0PowerGenerator") == 0 ||
			   strcmp(map->structs[i].name, "A0Sat-linkCentre") == 0 ||
			   strcmp(map->structs[i].name, "A0LasSatCommand") == 0 ||
			   strcmp(map->structs[i].name, "X-Super-Cannon") == 0 ||
			   strcmp(map->structs[i].name, "X-Super-MassDriver") == 0 ||
			   strcmp(map->structs[i].name, "X-Super-Missile") == 0 ||
			   strcmp(map->structs[i].name, "X-Super-Rocket") == 0 ||
			   strcmp(map->structs[i].name, "A0ComDroidControl") == 0) {
				log_debug("Found 2x2 object at %d %d", strx, stry);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx, stry, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+1, stry, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx, stry+1, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+1, stry+1, 0, 255, 0);
			}
			if(strcmp(map->structs[i].name, "A0LightFactory") == 0 ||
			   strcmp(map->structs[i].name, "A0VTolFactory1") == 0) {
				log_debug("Found factory at %d %d", strx, stry);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx,   stry,   0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+1, stry,   0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+2, stry,   0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx  , stry+1, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+1, stry+1, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+2, stry+1, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx  , stry+2, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+1, stry+2, 0, 255, 0);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, strx+2, stry+2, 0, 255, 0);
			}
		}
	}
	//log_debug("%d", map->featuresCount);
	if(options.DrawOilRigs) {
		for(uint32_t i = 0; i<map->featuresCount; i++) {
			unsigned short featx = map->features[i].x/128;
			unsigned short featy = map->features[i].y/128;
			if(strcmp(map->features[i].name, "OilResource") == 0) {
				log_debug("Found resource at %d %d", featx, featy);
				_WMT_PutZoomPixel(&OutputImg, options.ZoomLevel, featx, featy, 255, 255, 0);
			}
		}
	}
	OutputImg.WriteImage(pngfilename);
	//printf("\nHeightmap written to %s\n", pngfilename);
	return pngfilename;
}

