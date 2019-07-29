/*
 * Copyright (C) Maxim Zhuchkov - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 * Written by Maxim Zhuchkov <q3.max.2011@ya.ru>, May 2019
 */

#include "../lib/wmt.h"

int DebugPrintLevel = 0;
char* wzmappath;
bool IgnoreFree = false;
bool CustomOutputPathFlag = false;
char CustomOutputPath[MAX_PATH_LEN];
bool picturezoomenabled = false;
unsigned short picturezoom = 1;
bool OpenWithFeh=false;
struct ImageOptions options;
bool PrintInfo = false;


int ArgParse(int argc, char **argv) {
	wzmappath = argv[1];
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
		} else if(WMT_equalstr(argv[argcounter], "-q")) {
			log_set_quiet(1);
		} else if(WMT_equalstr(argv[argcounter], "--quiet")) {
			log_set_quiet(1);
		} else if(WMT_equalstr(argv[argcounter], "--print")||WMT_equalstr(argv[argcounter], "-p")) {
			PrintInfo = true;
		} else if(WMT_equalstr(argv[argcounter], "--help")||WMT_equalstr(argv[argcounter], "-h")) {
			printf("   Usage: %s <map-path> [args]\n", argv[0]);
			printf("   Available args:\n");
			printf("   \n");
			printf("   == general ==\n");
			printf("   -h    [--help]  Shows this page.\n");
			printf("   -o <path>       Override output filename.\n");
			printf("   --version       Show version and exit.\n");
			printf("   -feh            Open output image with feh. (need feh to make this work)\n");
			printf("   -q [--quiet]    No stdout output.\n");
			printf("   -p [--print]    Print info to stdout.\n");
			printf("   \n");
			printf("   == image options ==\n");
			printf("   --nowater       Forcing not to draw water. Drawing heghtmap instead.\n");
			printf("   --nobuildings   Forcing not to draw buildings\n");
			printf("   --nooil         Forcing not to draw oil rigs\n");
			printf("   --nocliff       Forcing not to draw cliff tiles as red\n");
			printf("   --singlecolorwater Forcing to draw water as always blue\n");
			printf("   -z <level>      Overrides zoom level of image.\n");
			printf("   \n");
			printf("   == debug options ==\n");
			printf("   -v              Enables verbose logging level 1. (Usefull info)\n");
			printf("   -vv             Enables verbose logging level 2. (Maaaany of info)\n");
			printf("   -vvv            Enables verbose logging level 3. (Just spam)\n");
			printf("   -vvvv           Enables verbose logging level 4. (Dont use this)\n");
			printf("   -v999           Enables vrbos- ripping your terminal history with spam.\n");
			printf("\n");
			exit(0);
		}
	}
	return 0;
}

int main(int argc, char** argv)
{
	log_set_level(LOG_WARN);
	ArgParse(argc, argv);
	if(argc > 1) {
		struct WZmap buildmap;
		WMT_ReadMap(wzmappath, &buildmap);
		if(buildmap.valid == false) {
			log_fatal("Error building info for file %s!", wzmappath);
			exit(-1);
		}
		char *outname;
		outname = WMT_WriteImage(&buildmap, CustomOutputPathFlag, CustomOutputPath, options);
		printf("Image written to: %s\n", outname);
		if(OpenWithFeh) {
			if(outname == NULL)
				log_fatal("Null filename! (%d)", buildmap.errorcode);
			else {
				int dpkg_ret = system("dpkg-query -s 'feh' > /dev/null 2>&1\n");
				if(dpkg_ret == 256) {
					log_fatal("No feh installed!");
				} else {
					log_info("Opening output with feh");
					char fehcmd[MAX_PATH_LEN];
					snprintf(fehcmd, MAX_PATH_LEN, "feh %s", outname);
					int retval = system(fehcmd);
					log_debug("System call returned %d", retval);
				}
			}
		}
		if(PrintInfo) {
			printf("==== Report for: %s ====\n", buildmap.mapname);
			printf("Path             %s\n", buildmap.path);
			printf("ttypes   version %d\n", buildmap.ttypver);
			printf("ttypes   count   %d\n", buildmap.ttypnum);
			printf("structs  version %d\n", buildmap.structVersion);
			printf("structs  count   %d\n", buildmap.numStructures);
			printf("map      version %d\n", buildmap.mapver);
			printf("map      size    %d x %d\n", buildmap.maptotalx, buildmap.maptotaly);
			printf("features version %d\n", buildmap.featureVersion);
			printf("features count   %d\n", buildmap.featuresCount);
			if(buildmap.haveadditioninfo)
			{
				printf("=Archive info=\n");
				printf("%s", buildmap.createdon);
				printf("%s", buildmap.createddate);
				printf("%s", buildmap.createdauthor);
				printf("%s", buildmap.createdlicense);
			}
			int scavcount = 0;
			for(uint i=0; i<buildmap.numStructures; i++)
				if(buildmap.structs[i].player == 10)
					scavcount++;
			printf("Scavengers: ");
			if(scavcount < 10)
				printf("no (%d)\n", scavcount);
			if(scavcount >= 10 && scavcount < 50)
				printf("few (%d)\n", scavcount);
			if(scavcount >= 50)
				printf("a lot (%d)\n", scavcount);
			int oilcount = 0;
			for(uint i=0; i<buildmap.featuresCount; i++)
				if(WMT_equalstr(buildmap.features[i].name, (char*)"OilResource"))
					oilcount++;
			printf("Oil rigs         %d\n", oilcount);
			int barrelscount = 0;
			for(uint i=0; i<buildmap.featuresCount; i++)
				if(WMT_equalstr(buildmap.features[i].name, (char*)"OilDrum"))
					barrelscount++;
			if(barrelscount > 0)
				printf("Oil drums        %d\n", barrelscount);
		}
		free(outname);
		exit(buildmap.errorcode);
	} else {
		printf("Usage: %s <map path> [args]\n", argv[0]);
	}
	exit(-1);
}




























