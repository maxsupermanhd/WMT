# Warzone map tool

[![Build status](https://ci.appveyor.com/api/projects/status/d08cc775etwm94d0?svg=true)](https://ci.appveyor.com/project/maxsupermanhd/wmt)
[![Version](https://badge.fury.io/gh/maxsupermanhd%2FWMT.svg)](https://github.com/maxsupermanhd/WMT)

Tool for viewing and generating previews of Warzone2100 maps.
Works only with old map format generated with FlaME.

- [x] Read old formats of Warzone2100 maps.
- [x] Read FlaME comments in file headers.
- [x] Make preview images.
- [x] Tweak colors of cliffs, terrain, water tiles.
- [x] Analyze map by structures count.
- [x] Print useful info to console.
- [x] Provide easy wrapper to Warzone 2100 map format.
- [x] Windows support. (see below)
- [x] CMake build system.
- [ ] Read new map format. (JSON) (will be added soon)
- [ ] Create map renders. (like from in game)
- [ ] Edit maps. (Will be added in a separate app)
- [ ] GUI for Windows users.

# How to build

#### With `make`
1. Install deps.
 That can be done with `$ sudo apt-get install g++ libstdc++-6-dev lib32gcc-7-dev libc6-dev make git`
2. Clone this repo `$ git clone https://github.com/maxsupermanhd/WMT.git`
3. Go to directory `$ cd WMT`
4. Make `$ make`
5. To build shared library consider rebuilding completely with `make clean; make lib`

#### With `CMake`
1. Install CMake. Do it somehow. Also get dependencies from `make` method.
2. Create build directory. ex: `mkdir build`
3. Navigate to build directory. ex: `cd build`
4. Run CMake over project directory with any flags you want. ex: `cmake -G"Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel ..`
5. Build this with CMake. ex: `cmake --build .`
6. Grab shared library from `./lib/`
7. Grab demo preview generator form `./bin/`

# How to use

1. Find some Warzone map
2. Run WMT with supplied map file (includes path)
3. Open with any png viewer/editor generated image.
4. For more info/tweaks see `--help` argument

# How to run on Windows based OS

Just use .exe file from releases. (use cmd to run)
If you want to build that from source, you better stop right here and go to releases page.

# Issues recommended template

- OS (Linux/Windows) (include kernel/libs versions).
- Type: crash(segfault/abort), infinitie loop, opening error or saving error. (or write something other)
- Logfile generated with piping WMT output with -v999 to file.
- Map file to reproduce error.

**Warning!**
*I will not respond to any windows-only issues.*
*I don't have any windows PC. If you want help with your problem on Windows based system you should ask someone else.*

###### Explanation
*Any zip file that WMT opens stores in dynamicly allocated memory and opens it with `fmemopen` function.*
*In Windows there is no true alternative for this kind of operations, so I decided to make temp-files crutch solution to Windows builds.*
*If you want to help making Windows releases please contact me.*

# License
### GNU GPL-2.0
For more details see LICENSE file.

# Contact me

You can write me in VK https://vk.com/1dontknow2 or mail q3.max.2011@ya.ru
PR welcome. Open issues if you have any problems.
