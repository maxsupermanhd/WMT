# Warzone map tool

Tool for viewing and generating previews of Warzone2100 maps.
Works only with old map format generated with FlaME.

- [x] Read old formats of Warzone2100 maps.
- [x] Read FlaME comments in file headers.
- [x] Make preview images.
- [x] Tweak colors of cliffs, terrain, water tiles.
- [x] Analyze map by structures count.
- [x] Print usefull info to console.
- [x] Provide easy wrapper to Warzone map format.
- [ ] Windows support. (see below)
- [ ] Read new map format. (json) (will be added soon)
- [ ] Create map renders. (like from ingame)
- [ ] Edit maps. (Will be added in a sperate app)
- [ ] GUI for Windows users.

# How to build

1. Install deps.
 That can be done with `$ sudo apt-get install gcc libstdc++-6-dev lib32gcc-7-dev libc6-dev make`
2. Clone this repo `$ git clone https://github.com/maxsupermanhd/WMT.git`
3. Go to directory `$ cd WMT`
4. Make `$ make`

# How to use

1. Find some Warzone map
2. Run WMT with supplied map file (includes path)
3. Open with any png viewer/editor generated image.
4. For more info/tweaks see `--help` argument

# How to run on Windows based OS

It's possible to make this thing work on Windows.
Main problem now that I (maxsupermanhd) can't find propre way to do that.
Any zip file that WMT opens stores in dynamicly allocated memory and opens it with `fmemopen` function.
In Windows there is no true alternative for this kind of operations, so I decided to leave this until i find true Windows solution.
If you really want to do make this work, you can make a crutch with unzipping files into temp directory. (WMT.c :292 :377 :450 :534 :627 :785 :855)

# Contact me

You can write me in VK https://vk.com/1dontknow2 or mail q3.max.2011@ya.ru
PR welcome. Open issues if you have any problems.

