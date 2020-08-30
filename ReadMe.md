# FragView
<!-- [![Actions Build Status](https://github.com/voldien/fragview/workflows/FragEngine/badge.svg?branch=master)](https://github.com/voldien/fragview/actions) -->
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![GitHub release](https://img.shields.io/github/release/voldien/fragview.svg)](https://github.com/voldien/fragview/releases)
[![TODOs](https://badgen.net/https/api.tickgit.com/badgen/github.com/voldien/fragview)](https://www.tickgit.com/browse?repo=github.com/voldien/fragview)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/voldien/fragview.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/voldien/fragview/context:cpp)
<!-- TODO, LGMT, version  -->

# Work in Progress

## Motivation
Based on the [glslview](https://github.com/voldien/glslview) program, which now has been labeld deprecated. That support OpenGL sandbox with realtime
modification of fragment shader.

## Installation
The software can be easily installed with invoking the following command.
```bash
mkdir build && cd build
cmake ..
cmake --build .
make install
```

## Examples

```c
./fragview --renderer-opengl -f samples/fractal.frag
```

## Dependencies ##
In order to compile the program, the following Debian packages is required. 

The following command will display current version of gcc.
```bash
gcc --version
```

```bash
apt install libyaml-dev libjson-c-dev libxml2-dev libsdl2-dev libglew-dev libvulkan-dev libgl1-mesa-dev opencl-headers libzip-dev libfswatch-dev libfreeimage-dev libavcodec-dev libavfilter-dev libavformat-dev  libassimp-dev libfreetype6-dev 
  sudo apt-get install -y libegl1-mesa-dev libgles2-mesa-dev
  sudo apt-get install -y libglew-dev libgtest-dev libsdl2-dev libsubunit-dev
  sudo apt-get install -y libsdl2-dev libglew-dev libvulkan-dev libgl1-mesa-dev opencl-headers libzip-dev libfswatch-dev libfreeimage-dev libfswatch-dev libxml2-dev
```

## License
This project is licensed under the GPL+3 License - see the [LICENSE](LICENSE) file for details
