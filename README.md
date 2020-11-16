# Warzone Map Tool

[![Build status](https://ci.appveyor.com/api/projects/status/d08cc775etwm94d0?svg=true)](https://ci.appveyor.com/project/maxsupermanhd/wmt)
[![Version](https://badge.fury.io/gh/maxsupermanhd%2FWMT.svg)](https://github.com/maxsupermanhd/WMT)

Tool for viewing and generating previews of maps for Warzone2100.

- [x] Read old formats of Warzone2100 maps.
- [x] Read FlaME comments in file headers.
- [ ] Read https://wz2100.euphobos.ru map database comments.
- [x] Make preview images.
- [x] Tweak colors of cliffs, terrain, water tiles.
- [x] Analyze map by structure count.
- [x] Print useful info to console.
- [x] Provide easy wrapper to Warzone 2100 map format.
- [x] Windows support. (see below)
- [x] CMake build system.
- [x] Read 3.3+ map format. (JSON)

# How to build

#### With `make`
1. Install deps.
 That can be done with `$ sudo apt-get install g++ libstdc++-6-dev lib32gcc-7-dev libc6-dev make git`
2. Clone this repo `$ git clone https://github.com/maxsupermanhd/WMT.git`
3. Go to directory `$ cd WMT`
4. Make `$ make`
5. To build shared library consider rebuilding completely with `make clean; make lib`

#### With `CMake`
1. Install CMake(3.10+). Also get dependencies from `make` method.
2. Create build directory. ex: `mkdir build`
3. Navigate to build directory. ex: `cd build`
4. Run CMake over project directory with any flags you want. ex: `cmake -G"Ninja" -DCMAKE_BUILD_TYPE=MinSizeRel ..`
5. Build with CMake. ex: `cmake --build .`
6. Grab shared library from `./lib/`
7. Grab demo preview generator from `./bin/`

# How to use

1. Find a Warzone 2100 map.
2. Run WMT with supplied map file. (including path)
3. Open the generated image with any png viewer/editor.
4. For more info/tweaks see `--help` argument.

# How to run on Windows based OS

Just use .exe file from releases. (use cmd to run)
If you want to build this from source, you better stop right here and go to the releases page.

# Issues recommended template

- OS (Linux/Windows) (include kernel/lib versions).
- Type: crash(segfault/abort), infinite loop, opening or saving error. (or any other error)
- Logfile generated with piping WMT output with -v999 to file.
- The map file that was used.

**Warning!**
*I will not respond to any windows-only issues.*
*I don't have a windows PC. If you want help with your problem on your Windows based system you should ask someone else.*

###### Explanation
*Any zip file that WMT opens is stored in dynamically allocated memory and opens it with the `fmemopen` function.*
*In Windows there is no true alternative for this, so I decided to make a temp-files crutch solution to Windows builds.*
*If you want to help in making Windows releases please contact me.*

# License
### GNU GPL-2.0
For more details see LICENSE file.

# Contact me

You can contact me and ask any interesting questions:
 - VK https://vk.com/1dontknow2
 - Mail q3.max.2011@ya.ru
 - Discord MaX#6717 (https://discord.gg/mJ5qzck)
 - TeamSpeak3 server on https://warzone2100.pro
PR welcome. Open issues if you have any problems.
