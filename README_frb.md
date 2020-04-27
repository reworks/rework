# frb
[![Build Status](https://ci.appveyor.com/api/projects/status/de4x20pcop04ylhm?svg=true)](https://ci.appveyor.com/project/reworks/frb)
[![Documentation Status](https://travis-ci.org/DomRe/frb.svg?branch=master)](https://domre.github.io/frb/)
[![Apache 2.0 Licensed](https://img.shields.io/badge/license-apache-blue.svg)](./LICENSE.txt)

Modern C++ OpenAL audio library.

## Basic Usage
```cpp
frb::Context context;
context.initialize();

frb::Audible music;
music.loadFromFile("bin/test.ogg");
music.play();
	
std::cin.get();
music.stop();
```

### Documentation:  
See [documentation](https://domre.github.io/frb/) for more.  

### Credits
* [doxygen-dark-theme](https://github.com/MaJerle/doxygen-dark-theme).
* [OpenAL](https://www.openal.org/).
* [openal-soft](https://github.com/kcat/openal-soft).
* [stb_vorbis-header](https://github.com/BareRose/stb/blob/master/stb_vorbis.h).