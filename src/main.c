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
bool AnalyzeMap = false;
bool DryRun = false;
bool Rebuild = false;


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
			printf("Version %s\nLog version %s\nUsing miniz.h version 9.1.15\n", WMT_VERSION, LOG_VERSION);
			exit(0);
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
		} else if(WMT_equalstr(argv[argcounter], "--analyze")||WMT_equalstr(argv[argcounter], "-a")) {
			AnalyzeMap = true;
		} else if(WMT_equalstr(argv[argcounter], "-q")) {
			log_set_quiet(1);
		} else if(WMT_equalstr(argv[argcounter], "--quiet")) {
			log_set_quiet(1);
		} else if(WMT_equalstr(argv[argcounter], "--dry")) {
			DryRun = true;
		} else if(WMT_equalstr(argv[argcounter], "--print")||WMT_equalstr(argv[argcounter], "-p")) {
			PrintInfo = true;
		} else if(WMT_equalstr(argv[argcounter], "--rebuild")||WMT_equalstr(argv[argcounter], "-r")) {
			Rebuild = true;
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
			printf("   -a [--analyze]  Analyzes map.\n");
			printf("   -r [--rebuild]  Try to recompile map with json format.\n");
			printf("   \n");
			printf("   == image options ==\n");
			printf("   --nowater       Forcing not to draw water. Drawing heghtmap instead.\n");
			printf("   --nobuildings   Forcing not to draw buildings\n");
			printf("   --nooil         Forcing not to draw oil rigs\n");
			printf("   --nocliff       Forcing not to draw cliff tiles as red\n");
			printf("   --singlecolorwater Forcing to draw water as always blue\n");
			printf("   -z <level>      Overrides zoom level of image.\n");
			printf("   --dry           Don\'t draw any images.\n");
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
		if(Rebuild)
			WMT_WriteMap(&buildmap);
		if(buildmap.valid == false) {
			log_fatal("Error building info for file %s!", wzmappath);
			exit(-1);
		}
		if(!DryRun) {
			char *outname = NULL;
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
			free(outname);
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
			printf("scroll min (x/y) %d x %d\n", buildmap.scrollminx, buildmap.scrollminy);
			printf("scroll max (x/y) %d x %d\n", buildmap.scrollmaxx, buildmap.scrollmaxy);
			printf("=Level info=\n");
			if(buildmap.haveadditioninfo)
			{
				printf("%s\n", buildmap.createdon);
				printf("%s\n", buildmap.createddate);
				printf("%s\n", buildmap.createdauthor);
				printf("%s\n", buildmap.createdlicense);
			}
			printf("= Levels = (%d)\n", buildmap.levelsfound);
			for(unsigned int i=0; i<buildmap.levelsfound; i++) {
				printf("Level %s\n", buildmap.levels[i].name);
				printf("Players %d\n", buildmap.levels[i].players);
				printf("Type %d\n", buildmap.levels[i].type);
				printf("Dataset %s\n", buildmap.levels[i].dataset);
			}
			int scavcount = 0;
			for(unsigned int i=0; i<buildmap.numStructures; i++)
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
			for(unsigned int i=0; i<buildmap.featuresCount; i++)
				if(WMT_equalstr(buildmap.features[i].name, (char*)"OilResource"))
					oilcount++;
			printf("Oil rigs         %d\n", oilcount);
			int barrelscount = 0;
			for(unsigned int i=0; i<buildmap.featuresCount; i++)
				if(WMT_equalstr(buildmap.features[i].name, (char*)"OilDrum"))
					barrelscount++;
			if(barrelscount > 0)
				printf("Oil drums        %d\n", barrelscount);
		}
		if(AnalyzeMap) {
			printf("Analyzing map %s\n", buildmap.mapname);
			bool DetectedPlayers[10];
			int PlayerBuildings[10];
			int FactoriesPerPlayer[10];
			int CyborgFactoriesPerPlayer[10];
			int VtolFactoriesPerPlayer[10];
			int FactoryModulesPerPlayer[10];
			int LabsPerPlayer[10];
			int LabModulesPerPlayer[10];
			int GeneratorsPerPlayer[10];
			int GeneratorsModulesPerPlayer[10];
			bool MapBalanced = true;
			int ScavPlayer = -1;
			bool ScavDetected = false;
			for(int i=0; i<10; i++) {
				DetectedPlayers[i] = false;
				PlayerBuildings[i] = 0;
				FactoriesPerPlayer[i] = 0;
				CyborgFactoriesPerPlayer[i] = 0;
				VtolFactoriesPerPlayer[i] = 0;
				FactoryModulesPerPlayer[i] = 0;
				LabsPerPlayer[i] = 0;
				LabModulesPerPlayer[i] = 0;
				GeneratorsPerPlayer[i] = 0;
				GeneratorsModulesPerPlayer[i] = 0;
			}
			for(unsigned int i=0; i<buildmap.numStructures; i++) {
				DetectedPlayers[buildmap.structs[i].player] = true;
				PlayerBuildings[buildmap.structs[i].player]++;
				if(WMT_equalstr(buildmap.structs[i].name, "A0LightFactory"))
					FactoriesPerPlayer[buildmap.structs[i].player]++;
				else if(WMT_equalstr(buildmap.structs[i].name, "A0CyborgFactory"))
					CyborgFactoriesPerPlayer[buildmap.structs[i].player]++;
				else if(WMT_equalstr(buildmap.structs[i].name, "A0VTolFactory1"))
					VtolFactoriesPerPlayer[buildmap.structs[i].player]++;
				else if(WMT_equalstr(buildmap.structs[i].name, "A0FacMod1"))
					FactoryModulesPerPlayer[buildmap.structs[i].player]++;
				else if(WMT_equalstr(buildmap.structs[i].name, "A0ResearchFacility"))
					LabsPerPlayer[buildmap.structs[i].player]++;
				else if(WMT_equalstr(buildmap.structs[i].name, "A0ResearchModule1"))
					LabModulesPerPlayer[buildmap.structs[i].player]++;
				else if(WMT_equalstr(buildmap.structs[i].name, "A0PowerGenerator"))
					GeneratorsPerPlayer[buildmap.structs[i].player]++;
				else if(WMT_equalstr(buildmap.structs[i].name, "A0PowMod1"))
					GeneratorsModulesPerPlayer[buildmap.structs[i].player]++;
			}
			int DetectedPlayersCount = 0;
			for(int i=0; i<10; i++)
				if(DetectedPlayers[i])
					DetectedPlayersCount++;
			bool LastPlayerDetected = true;
			for(int i=0; i<10; i++)
				if(!DetectedPlayers[i] && LastPlayerDetected) {
					LastPlayerDetected = false;
					//printf("Players slots ended on %d\n", i-1);
				} else if(DetectedPlayers[i] && !LastPlayerDetected) {
					if(ScavPlayer != -1)
						printf("WARNING! More scav slots detected!\n");
					printf("Scav detected on slot %d\n", i);
					ScavPlayer = i;
					ScavDetected = true;
				}
			printf("Detected players: 0 1 2 3 4 5 6 7 8 9 \n");
			printf("                  ");
			for(int i=0; i<10; i++) {
				if(DetectedPlayers[i])
					if(i == ScavPlayer && ScavDetected)
						printf("S ");
					else
						printf("Y ");
				else
					printf("N ");
			}
			printf("\n");
			printf("Detected players count: %d\n", ScavDetected ? DetectedPlayersCount-1 : DetectedPlayersCount);
			bool BuildingsCountNotEqual = false;
			for(int i=0; i<9; i++) {
				for(int b=i+1; b<10; b++) {
					if(i == ScavPlayer || b == ScavPlayer)
						continue;
					if((PlayerBuildings[i] != PlayerBuildings[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d buildings, but player %d have %d!\n", i, PlayerBuildings[i], b, PlayerBuildings[b]);
						MapBalanced = false;
						BuildingsCountNotEqual = true;
					} if((FactoriesPerPlayer[i] != FactoriesPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d factories, but player %d have %d!\n", i, FactoriesPerPlayer[i], b, FactoriesPerPlayer[b]);
						MapBalanced = false;
					} if((CyborgFactoriesPerPlayer[i] != CyborgFactoriesPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d cyborg factories, but player %d have %d!\n", i, CyborgFactoriesPerPlayer[i], b, CyborgFactoriesPerPlayer[b]);
						MapBalanced = false;
					} if((VtolFactoriesPerPlayer[i] != VtolFactoriesPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d vtol factories, but player %d have %d!\n", i, VtolFactoriesPerPlayer[i], b, VtolFactoriesPerPlayer[b]);
						MapBalanced = false;
					} if((FactoryModulesPerPlayer[i] != FactoryModulesPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d factory modules, but player %d have %d!\n", i, FactoryModulesPerPlayer[i], b, FactoryModulesPerPlayer[b]);
						MapBalanced = false;
					} if((LabsPerPlayer[i] != LabsPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d labs, but player %d have %d!\n", i, LabsPerPlayer[i], b, LabsPerPlayer[b]);
						MapBalanced = false;
					} if((LabModulesPerPlayer[i] != LabModulesPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d lab modules, but player %d have %d!\n", i, LabModulesPerPlayer[i], b, LabModulesPerPlayer[b]);
						MapBalanced = false;
					} if((GeneratorsPerPlayer[i] != GeneratorsPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d generators, but player %d have %d!\n", i, GeneratorsPerPlayer[i], b, GeneratorsPerPlayer[b]);
						MapBalanced = false;
					} if((GeneratorsModulesPerPlayer[i] != GeneratorsModulesPerPlayer[b]) && DetectedPlayers[i] && DetectedPlayers[b]) {
						printf("WARNING! Player %d have %d generator modules, but player %d have %d!\n", i, GeneratorsModulesPerPlayer[i], b, GeneratorsModulesPerPlayer[b]);
						MapBalanced = false;
					}
				}
			}
			if(!MapBalanced)
				printf("WARNING! Map not balanced! Someone have odds!\n");
			if(!BuildingsCountNotEqual)
				printf("Buildings per player: %d\n", PlayerBuildings[0]);
			else {
				int BuildingsAverage = 0;
				for(int i=0; i<DetectedPlayersCount; i++)
					BuildingsAverage+=PlayerBuildings[i];
				BuildingsAverage = BuildingsAverage/DetectedPlayersCount;
				printf("Average buildings count: %d\n", BuildingsAverage);
			}
		}
		WMT_FreeMap(&buildmap);
		exit(buildmap.errorcode);
	} else {
		printf("Usage: %s <map path> [args]\n", argv[0]);
	}
	exit(-1);
}




























