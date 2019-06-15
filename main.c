/*
 * Copyright (C) Maxim Zhuchkov - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Maxim Zhuchkov <q3.max.2011@ya.ru>, May 2019
 */

#include "wmt.h"

int DebugPrintLevel = 0;
char* wzmappath;
bool IgnoreFree = false;
bool CustomOutputPathFlag = false;
char CustomOutputPath[MAX_PATH_LEN];
bool EnableFilterWater = false;
bool EnableFilterCliffs = false;
bool EnableFilterTextrue = false;
bool picturezoomenabled = false;
unsigned short picturezoom = 1;
bool OpenWithFeh=false;

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
unsigned short maptileoffset = 0x01ff;


int ArgParse(int argc, char **argv) {
	for(int argcounter=2; argcounter<argc; argcounter++) {
		if(DebugPrintLevel > 2)
			printf("Scanning arg %d %s\n", argcounter, argv[argcounter]);
		if(equalstr(argv[argcounter], "-v")) {
			printf("Verbose log level 1.\n");
			log_set_level(LOG_ERROR);
		} else if(equalstr(argv[argcounter], "-vv")) {
			printf("Verbose log level 2!\n");
			log_set_level(LOG_WARN);
		} else if(equalstr(argv[argcounter], "-vvv")) {
			printf("Verbose log level 3!!!\n");
			log_set_level(LOG_INFO);
		} else if(equalstr(argv[argcounter], "-vvvv")) {
			log_warn("Verbose log level 4. (╯°□°）╯︵ ┻━┻");
			log_warn("Warning! Wall of info incomming!");
			log_set_level(LOG_DEBUG);
		} else if(equalstr(argv[argcounter], "-v999")) {
			printf("You better stop reading logs...\n");
			log_set_level(LOG_TRACE);
		} else if(equalstr(argv[argcounter], "--version")) {
			printf("Version %d\nLog version %s\nUsing miniz.h version 9.1.15\n", WMT_VERSION, LOG_VERSION);
		} else if(equalstr(argv[argcounter], "--ignore-free")) {
			IgnoreFree = true;
		} else if(equalstr(argv[argcounter], "-feh")) {
			OpenWithFeh = true;
		} else if(equalstr(argv[argcounter], "-z")) {
			picturezoomenabled = true;
			picturezoom = atoi(argv[argcounter+1]);
			argcounter++;
		} else if(equalstr(argv[argcounter], "-o")) {
			CustomOutputPathFlag = true;
			for(int i=0; i<MAX_PATH_LEN; i++)
				CustomOutputPath[i] = 0;
			snprintf(CustomOutputPath, MAX_PATH_LEN, "%s", argv[argcounter+1]);
			argcounter++;
		} else if(equalstr(argv[argcounter], "--help")||equalstr(argv[argcounter], "-h")) {
			printf("\n	Usage: %s <map-path> [args]\n", argv[0]);
			printf("	\n");
			printf("	Available args:\n");
			printf("	-h    [--help]  Shows this page.\n");
			printf("	-o <path>       Override files placing in ./output/<mapname>/...\n");
			printf("	-v              Enables verbose logging level 1. (Usefull info)\n");
			printf("	-vv             Enables verbose logging level 2. (Maaaany of info)\n");
			printf("	-vvv            Enables verbose logging level 3. (Just spam)\n");
			printf("	-vvvv           Enables verbose logging level 4. (Dont use this)\n");
			printf("	-v999           Enables vrbos- ripping your terminal history with spam.\n");
			printf("	--version       Show version and exit.\n");
			printf("	--ignore-free   Lazy checks of freeing memory. (default false)\n");
			printf("	                 Dont use this in big scripts with multirunning.\n");
			printf("	-z <level>      Overrides zoom level of image. (ex. zoom=1 pixels:tiles 1:1 \n");
			printf("                     zoom=4 pixels:tiles 4:1)                                     ");
			printf("    -feh            Open output image with feh                                    ");
			//printf("	-q [--quiet]    No stdout output.\n");
			printf("\n");
			exit(0);
		}
		else
			if(DebugPrintLevel > 2)
				printf("Idk what that key mean...\n");
	}
	return 0;
}

void cleanexit(void) {
	log_info("Cleaning up...");
	free(ttpcontents);
	free(mapcontents);
	//free(mapwater);
	//free(mapcliff);
	free(mapheight);
	free(filenames);
	for(int i=0; i<totalentries; i++)
		free(filenames[i]);
	zip_entry_close(zip);
	zip_close(zip);
	log_info("Cleaned up!");
}

int main(int argc, char** argv)
{
	printf("Warzone 2100 Maps Tool\n");
	log_set_level(LOG_FATAL);
	ArgParse(argc, argv);
	if(argc <= 1) {
		log_fatal("No enough args... Plz read help. (--help)");
		exit(0);
	}
	
	log_info("Initializing exit freeing memory function... ");
	if(!atexit(cleanexit))
		log_warn("Initializing exit freeing memory function fail!");
	else
		log_info("Initializing exit freeing memory function... DONE!");
	
	
	log_info("Opening file...");
	zip = zip_open(argv[1], 0, 'r');
	if(zip == NULL) {
		log_fatal("Error opening/reading warzone map file! Exiting.");
		exit(0);
	}
	log_info("Opening file... DONE!");
	
	
	
	
	log_info("Dealing with stupid magic of c strings...");
	char* fullpath=argv[1];
	int fullpathlen=strlen(fullpath);
	int lastindex=-1;
	for(int c=0; c<fullpathlen; c++) {
		if(fullpath[c] == '/')
			lastindex=c;
	}
	if(lastindex!=-1)
		str_cut(fullpath, 0, lastindex+1);
	if(strlen(fullpath)>67)
		str_cut(fullpath, strlen(fullpath)-68, strlen(fullpath)-3);
	else
		str_cut(fullpath, strlen(fullpath)-3, -1);
	char* mapname = fullpath;
	log_info("Dealing with stupid magic of c strings... DONE!\n\n\nMap name: \"%s\"\n\n", mapname);
	
	
	log_info("Getting total entries count... ");
	totalentries = zip_total_entries(zip);
	log_info("Getting total entries count... (%d)", totalentries);
	
	
	log_info("Allocating filenames array... ");
	filenames = (char**) calloc(totalentries, sizeof(char*));
	for(int i=0; i<totalentries; i++)
		filenames[i] = (char*) calloc(1024, sizeof(char));
	log_info("Allocating filenames array... DONE!");
	log_info("Checking allocated memory... ");
	if(!filenames) {
		log_fatal("Filenames allocation fail! Exiting.");
		exit(0);
	}
	else
		log_info("Checking allocated memory... DONE!");
	for(int i=0; i<totalentries; i++) {
		if(!filenames[i]) {
			log_fatal("Filename %d allocation fail! Exiting.");
			exit(0);
		}
	}
	
	
	log_info("Listing all shit up (%d) ...", totalentries);
	for(int index=0; index<totalentries; index++) {
		log_trace("Trying index %d", index);
		int ret = zip_entry_openbyindex(zip, index);
		log_trace("Ret %d", ret);
		if(ret>=0) {
			snprintf(filenames[index], 1024, "%s", (char*)zip_entry_name(zip));
			log_trace("%d.\t%s", index, filenames[index]);
			zip_entry_close(zip);
		}
		else
			log_error("%d.\t Error!\n", index);
	}
	log_info("Listing all shit up... DONE!");
	
	
	int indexttypes = SearchFilename(filenames, totalentries, (char*)"ttypes.ttp", 2);
	log_info("Opening file by index... ");
	int openstatus = zip_entry_openbyindex(zip, indexttypes);
	if(openstatus<0) {
		log_fatal("Opening file by index... FAIL!");
	} else {
		log_info("Opening file by index... Wow that worked)");
		log_info("Reading ttypes.ttp ... ");
		size_t ttpfilesize = zip_entry_size(zip);
		size_t readlen;
		ssize_t readed = zip_entry_read(zip, &ttpcontents, &readlen);
		log_info("Reading ttypes.ttp ... (%ld/%ld)", readed, ttpfilesize);
		
		
		
		log_info("Opening as file descriptor... ");
		FILE* ttpf = fmemopen(ttpcontents, readlen, "r");
		if(ttpf==NULL)
		{
			log_fatal("Opening as file descriptor... ERROR!\n");
			exit(0);
		}
		else
		{
			log_info("Opening as file descriptor... DONE!");
			log_info("Making shure that found file is a ttp file... ");
			if(fread(&ttphead, sizeof(char), 4, ttpf) != sizeof(char)*4)
			{
				log_fatal("Making shure that found file is a ttp file... ERROR!");
				exit(0);
			}
			else
			{
				log_info("Making shure that found file is a ttp file... \"%s\"", ttphead);
				if(ttphead[0] != 't' ||
				   ttphead[1] != 't' ||
				   ttphead[2] != 'y' ||
				   ttphead[3] != 'p')
					log_warn("Warning! File header not ttyp!!! (ignore thats not working properly!)");
				log_info("Reading ttp version... ");
				ssize_t ttypverreadret = fread(&ttypver, sizeof(unsigned int), 1, ttpf);
				if(ttypverreadret != 1) {
					log_trace("Fread return count (%d) not (%d)!", ttypverreadret, 1);
					if(ferror(ttpf)) {
						if(ftell(ttpf) == 8) {
							log_info("Reading ttp version... (%d)", ttypver);
							log_error("Return status of reading version bad!");
						} else {
							log_fatal("Fread read only %ld instead of %lu. Cursor not in needed position! (%ld) Readed: %u", ttypverreadret, sizeof(unsigned int), ftell(ttpf), ttypver);
							exit(0);
						}
					} else {
						log_info("Reading ttp version... (%d) (?!?)", ttypver);
						if(ftell(ttpf) != 8)
							log_error("Cursor not in needed position!");
					}
				}
				else
					log_info("Reading ttp version... (%d)", ttypver);
				log_info("Reading landscape types... ");
				ssize_t ttypnumreadret = fread(&ttypnum, sizeof(unsigned int), 1, ttpf);
				if(ttypnumreadret != 1) {
					log_trace("Fread return count (%d) not (%d)!", ttypnumreadret, 1);
					if(ferror(ttpf)) {
						if(ftell(ttpf) == 12) {
							log_info("Reading landscape types... (%d)", ttypnum);
							log_warn("Return status of reading num bad!");
						} else {
							log_fatal("Fread read only %ld instead of %lu. Cursor not in needed position! (%ld) Readed: %u", ttypnumreadret, sizeof(unsigned int), ftell(ttpf), ttypnum);
							exit(0);
						}
					} else {
						log_info("Reading landscape types... (%d) (?!?)", ttypnum);
						if(ftell(ttpf) != 12)
							log_error("Cursor not in needed position!");
					}
				}
				else
					log_info("Reading ttp version... (%d)", ttypnum);
				//printf("Allocating memory for landscape types structures... ");
				//ttyptt = (unsigned short*)calloc(ttypnum, sizeof(unsigned short));
				//printf("DONE!\n");
				log_info("Reading landscape types structure... ");
				ssize_t ttypttreadret = fread(&ttyptt, sizeof(unsigned short), ttypnum, ttpf);
				log_info("Reading landscape types structure... (%lu)", ttypttreadret);
				/*printf("Checking that we hitted EOF... ");
				if(ttpfilesize != ftell(ttpf)) {
					printf("NO WE DONT.\nFilesize: %lu\nReaded:   %lu\n", ttpfilesize, ftell(ttpf));
				} else {
					printf("DONE!\n");
				}*/
				printf("\nResults of readyng ttypes.ttp:\n");
				printf("Header: \"%s\"\n", ttphead);
				printf("Version: %d\n", ttypver);
				printf("Number:  %d\n", ttypnum);	
			}
		}
		fclose(ttpf);
		free(ttpcontents);
		ttpcontents = NULL;
		zip_entry_close(zip);
	}
	
	log_info("Searching for game.map file... ");
	int indexgamemap = SearchFilename(filenames, totalentries, (char*)"game.map", 1);
	if(indexgamemap==-1)
	{
		log_error("Searching for game.map file... FAIL!");
		log_info("Trying another method of finding files...");
		char mapnewpath[MAX_PATH_LEN];
		for(int i=0; i<MAX_PATH_LEN; i++)
			mapnewpath[i]='0';
		log_trace("Constructing path...");
		snprintf(mapnewpath, MAX_PATH_LEN, "multiplay/maps/%s/game.map", mapname);
		//strcat(mapnewpath, "multiplay/maps/");
		//strcat(mapnewpath, mapname);
		//strcat(mapnewpath, "/game.map");
		log_trace("New open path: \"%s\"", mapnewpath);
		log_info("Opening file by path...");
		openstatus = zip_entry_open(zip, mapnewpath);
		if(openstatus<0)
			log_error("New path not valid!");
	}
	else
	{
		log_info("Searching for game.map file... %d", indexgamemap);
		log_info("Found: %s", filenames[indexgamemap]);
		log_info("Opening file by index... ");
		openstatus = zip_entry_openbyindex(zip, indexgamemap);
		if(openstatus<0)
			log_error("File not opened!");
	}
	
	if(openstatus<0) {
		log_fatal("Opening file... FAIL!");
		exit(0);
	} else {
		log_info("Opening file... DONE!");
		log_info("Reading game.map ... ");
		size_t mapfilesize = zip_entry_size(zip);
		void *mapcontents;
		size_t readlen;
		ssize_t readed = zip_entry_read(zip, &mapcontents, &readlen);
		log_info("Reading game.map ... (%ld/%ld)", readed, mapfilesize);
		log_info("Opening as file descriptor... ");
		FILE* mapf = fmemopen(mapcontents, readlen, "r");
		if(mapf==NULL) {
			log_fatal("Opening as file descriptor... ERROR!\n");
			exit(0);
		} else {
			log_info("Opening as file descriptor... DONE!");
			log_info("Making shure that found file is a map file... ");                   //FIXME only read
			if(fread(&maphead, sizeof(char), 4, mapf) != sizeof(char)*4) {
				log_fatal("ERROR!\n");
				exit(0);
			} else {
				log_info("Making shure that found file is a map file... \"%s\"", maphead);
				log_info("Reading map version... ");
				ssize_t mapverreadret = fread(&mapver, sizeof(unsigned int), 1, mapf);
				if(mapverreadret != 1) {
					log_trace("Fread return count (%d) not (%d)!", mapverreadret, 1);
					if(ferror(mapf)) {
						if(ftell(mapf) == 8) {
							log_info("Reading map version... (%d)", mapver);
							log_warn("Return status of reading version bad!");
						} else {
							log_fatal("Fread read only %ld instead of %lu. Cursor not in needed position! (%ld) Readed: %u", mapverreadret, sizeof(unsigned int), ftell(mapf), mapver);
							exit(0);
						}
					} else {
						log_info("Reading map version... (%d) (?!?)", mapver);
						if(ftell(mapf) != 8)
							log_error("Cursor not in needed position!");
					}
				}
				else
					log_info("Reading map version... (%d)", mapver);
				log_info("Reading width... ");
				ssize_t mapwreadret = fread(&maptotalx, sizeof(unsigned int), 1, mapf);
				if(mapwreadret != 1) {
					log_trace("Fread return count (%d) not (%d)!", mapverreadret, 1);
					if(ferror(mapf)) {
						if(ftell(mapf) == 12) {
							log_info("Reading width... (%d)", maptotalx);
							log_warn("Warning return status of reading width bad!");
						} else {
							log_fatal("ERROR!\nFread read only %ld instead of %lu. Cursor not in needed position! (%ld) Readed: %u", mapwreadret, sizeof(unsigned int), ftell(mapf), maptotalx);
							exit(0);
						}
					} else {
						log_info("Reading width... (%d) (?!?)", maptotalx);
						if(ftell(mapf) != 12)
							log_error("Cursor not in needed position!");
					}
				}
				else
					log_info("Reading width... (%d)", maptotalx);
				log_info("Reading height... ");
				ssize_t maphreadret = fread(&maptotaly, sizeof(unsigned int), 1, mapf);
				if(maphreadret != 1) {
					log_trace("Fread return count (%d) not (%d)!", mapverreadret, 1);
					if(ferror(mapf)) {
						if(ftell(mapf) == 16) {
							log_info("Reading height... (%d)", maptotaly);
							log_warn("Return status of reading height bad!");
						}
						else {
							log_fatal("Fread read only %ld instead of %lu. Cursor not in needed position! (%ld) Readed: %u", maphreadret, sizeof(unsigned int), ftell(mapf), maptotaly);
							exit(0);
						}
					} else {
						log_info("Reading height... (%d) (?!?)", maptotaly);
						if(ftell(mapf) != 16)
							log_error("Cursor not in needed position!");
					}
				}
				else
					log_info("Reading height... (%d)", maptotaly);
				
				printf("\nResults of reading game.map\n");
				printf("Version: %d\n", mapver);
				printf("Width:   %d\n", maptotaly);
				printf("Height:  %d\n", maptotalx);
				
				log_info("Starting reading map data...");
				int maparraysize = maptotaly*maptotalx;
				log_trace("Allocating memory for height...");
				mapheight = (unsigned short*) calloc(maparraysize, sizeof(unsigned short));
				if(mapheight==NULL)
					log_error("Height array allocation failed!");
				log_trace("Allocating memory for height... DONE!");
				unsigned short maptileinfo = 0;
				ssize_t mapreadret = -1;
				short maptiletexture = -1;
				TerrainTypes maptileterrain = ttsand;
				int nowmapy=-1, nowmapx=-1;
				maparrsize = maparraysize;
				for(int counter=0; counter<maparraysize; counter++)
				{
					nowmapy = counter/maptotaly;
					nowmapx = counter-(nowmapy*maptotalx);
					log_trace("Reading tile info (%d) (%d)", nowmapy, nowmapx);
					mapreadret = fread(&maptileinfo, 2, 1, mapf);
					if(mapreadret != 1)
						log_warn("Fread scanned %d elements instead of %d (tileinfo)", mapreadret, 1);
					log_trace("Tile info %d", maptileinfo);
					maptiletexture = (maptileinfo & maptileoffset);
					log_trace("maptiletexture %d", maptiletexture);
					/*if(maptiletexture > 12 || maptiletexture < 0) {
						log_error("Map tile texture not in range! (%d)", maptiletexture);
						log_trace("Forcing maptiletexture to 0!");
						maptiletexture = ttsand;
					}*/
					maptileterrain = (TerrainTypes)ttyptt[maptiletexture];
					log_trace("maptileterrain: %s", TerrainTypesStrings[maptileterrain]);
					if(maptileterrain == ttwater) {
						mapwater[counter]=true;
						log_trace("Water");
					}
					if(maptileterrain == ttclifface) {
						mapcliff[counter]=true;
						log_trace("Cliff");
					}
					log_trace("Reading height");
					mapreadret = fread(&mapheight[counter], 1, 1, mapf);
					if(mapreadret != 1)
						log_warn("Fread scanned %d elements instead of %d (height)", mapreadret, 1);
					log_trace("Height %d", mapheight[counter]);
				}
				log_info("End of reading map data.");
			}
		}
		free(mapcontents);
		mapcontents = NULL;
		zip_entry_close(zip);
	}
	
	log_info("Trying to write image...");
	log_info("Generating filename...");
	char pngfilename[MAX_PATH_LEN];
	if(CustomOutputPathFlag) {
		log_debug("Using override... (\"%s\")", CustomOutputPath);
		snprintf(pngfilename, MAX_PATH_LEN, "%s", CustomOutputPath);
	} else {
		snprintf(pngfilename, MAX_PATH_LEN, "output/%s.png", mapname);
	}
	log_info("Filename: %s", pngfilename);
	
	
	
	PngImage OutputImg((unsigned int)maptotalx*picturezoom+picturezoom, (unsigned int)maptotaly*picturezoom+picturezoom);
	for(unsigned short counterx=0; counterx<maptotalx; counterx++) {
		for(unsigned short countery=0; countery<maptotaly; countery++) {
			for(unsigned short zoomcounterx=counterx*picturezoom; zoomcounterx<counterx*picturezoom+picturezoom; zoomcounterx++) {
				for(unsigned short zoomcountery=countery*picturezoom; zoomcountery<countery*picturezoom+picturezoom; zoomcountery++) {
					int nowposinarray = countery*maptotalx+counterx;
					if(mapwater[nowposinarray]) {
						OutputImg.PutPixel(zoomcounterx, zoomcountery, mapheight[nowposinarray]/4, mapheight[nowposinarray]/4, mapheight[nowposinarray]);
					}
					else if(mapcliff[nowposinarray]) {
						OutputImg.PutPixel(zoomcounterx, zoomcountery, mapheight[nowposinarray], mapheight[nowposinarray]/4, mapheight[nowposinarray]/4);   //FIXME not the best way
					} else {
						OutputImg.PutPixel(zoomcounterx, zoomcountery, mapheight[nowposinarray], mapheight[nowposinarray], mapheight[nowposinarray]);
					}
				}
			}
		}
	}
	OutputImg.WriteImage(pngfilename);
	printf("\nHeightmap written to %s\n", pngfilename);
	
	
	log_info("Checking that struct.json does not exist...");                                  //FIXME parse json
	int indexjsonstructs = SearchFilename(filenames, totalentries, (char*)"struct.json", 0);  
	if(indexjsonstructs != -1) {
		log_warn("struct.json exists!!! (%d)", indexjsonstructs);
		log_warn("Sorry, but this tool still too dumb to read structs...");
	}
	int indexstructs = SearchFilename(filenames, totalentries, (char*)"struct.ini", 2);       //FIXME parse INI
	if(indexstructs != -1) {                                                                  //FIXME parse old maps
		log_warn("struct.ini exists!!! (%d)", indexstructs);
		log_warn("Sorry, but this tool still too dumb to read structs...");
	}
	
	if(OpenWithFeh) {
		log_info("Opening output with feh");
		char fehcmd[MAX_PATH_LEN];
		snprintf(fehcmd, MAX_PATH_LEN, "feh %s", pngfilename);
		int retval = system(fehcmd);
		log_debug("system call returned %d", retval);
	}
	
	exit(0);
}




























