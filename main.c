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
bool picturezoomenabled = false;
unsigned short picturezoom = 1;
bool OpenWithFeh=false;
struct ImageOptions options;


int ArgParse(int argc, char **argv) {
	for(int argcounter=1; argcounter<argc; argcounter++) {
		if(DebugPrintLevel > 2)
			printf("Scanning arg %d %s\n", argcounter, argv[argcounter]);
		if(WMT_equalstr(argv[argcounter], "-v")) {
			printf("Verbose log level 1.\n");
			log_set_level(LOG_ERROR);
		} else if(WMT_equalstr(argv[argcounter], "-vv")) {
			printf("Verbose log level 2!\n");
			log_set_level(LOG_WARN);
		} else if(WMT_equalstr(argv[argcounter], "-vvv")) {
			printf("Verbose log level 3!!!\n");
			log_set_level(LOG_INFO);
		} else if(WMT_equalstr(argv[argcounter], "-vvvv")) {
			log_warn("Verbose log level 4. (╯°□°）╯︵ ┻━┻");
			log_warn("Warning! Wall of info incomming!");
			log_set_level(LOG_DEBUG);
		} else if(WMT_equalstr(argv[argcounter], "-v999")) {
			printf("You better stop reading logs...\n");
			log_set_level(LOG_TRACE);
		} else if(WMT_equalstr(argv[argcounter], "--version")) {
			printf("Version %d\nLog version %s\nUsing miniz.h version 9.1.15\n", WMT_VERSION, LOG_VERSION);
		} else if(WMT_equalstr(argv[argcounter], "--ignore-free")) {
			IgnoreFree = true;
		} else if(WMT_equalstr(argv[argcounter], "-feh")) {
			OpenWithFeh = true;
		} else if(WMT_equalstr(argv[argcounter], "-z")) {
			options.ZoomEnabled = true;
			options.ZoomLevel = atoi(argv[argcounter+1]);
			argcounter++;
		} else if(WMT_equalstr(argv[argcounter], "-o")) {
			CustomOutputPathFlag = true;
			for(int i=0; i<MAX_PATH_LEN; i++)
				CustomOutputPath[i] = 0;
			snprintf(CustomOutputPath, MAX_PATH_LEN, "%s", argv[argcounter+1]);
			argcounter++;
		} else if(WMT_equalstr(argv[argcounter], "--nowater")) {
			options.DrawWater=false;
		} else if(WMT_equalstr(argv[argcounter], "--singlecolorwater")) {
			options.SinglecolorWater=true;
		} else if(WMT_equalstr(argv[argcounter], "--nobuildings")) {
			options.DrawBuildings=false;
		} else if(WMT_equalstr(argv[argcounter], "--nooil")) {
			options.DrawOilRigs=false;
		} else if(WMT_equalstr(argv[argcounter], "--nocliff")) {
			options.DrawCliffsAsRed=false;
		} else if(WMT_equalstr(argv[argcounter], "--help")||WMT_equalstr(argv[argcounter], "-h")) {
			printf("\n	Usage: %s [map-path] [args]\n", argv[0]);
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
			printf("	-z <level>      Overrides zoom level of image. (ex. zoom=1 pixels:tiles 1:1 \n");
			printf("                     zoom=4 pixels:tiles 4:1)                                     \n");
			printf("    -feh            Open output image with feh                                    \n");
			printf("    --nowater       Forcing not to draw water. Drawing heghtmap instead.\n");
			printf("    --singlecolorwater \n");
			printf("                    Forcing to draw water as always blue (not by height division)\n");
			printf("    --nobuildings   Forcing not to draw buildings\n");
			printf("    --nooil         Forcing not to draw oil rigs\n");
			printf("    --nocliff       Forcing not to draw cliff tiles as red\n");
			//printf("	-q [--quiet]    No stdout output.\n");
			printf("\n");
			exit(0);
		}
		else
			if(argcounter == 1)
				wzmappath = argv[1];
	}
	return 0;
}




int main(int argc, char** argv)
{
	printf("Warzone 2100 Map Tool\n");
	log_set_level(LOG_WARN);
	ArgParse(argc, argv);
	
	if(argc >= 1) {
		struct WZmap buildmap;
		WMT_ReadMap(wzmappath, &buildmap);
		if(buildmap.valid == false) {
			log_fatal("Error building info for file %s!", argv[1]);
		}
		char *outname;
		outname = WMT_WriteImage(&buildmap, CustomOutputPathFlag, CustomOutputPath, options);
		printf("Output: %s\n", outname);
		if(OpenWithFeh) {
			if(outname == NULL)
				exit(0);
			log_info("Opening output with feh");
			char fehcmd[MAX_PATH_LEN];
			snprintf(fehcmd, MAX_PATH_LEN, "feh %s", outname);
			int retval = system(fehcmd);
			log_debug("System call returned %d", retval);
		}
		free(outname);
		exit(0);
	} else {
		printf("Usage: %s <map path> [args]\n", argv[0]);
	}
	exit(0);
}




























