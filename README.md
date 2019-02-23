![alt text](https://raw.githubusercontent.com/JimmySnails/Cytopia/master/resources/images/app_icons/logo_big_textured.png)

[![Build Status](https://travis-ci.org/JimmySnails/Cytopia.svg?branch=master)](https://travis-ci.org/JimmySnails/Cytopia)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/f686b5cf79384e0ab807578cb392b0bc)](https://www.codacy.com/app/JimmySnails/Cytopia?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=JimmySnails/Cytopia&amp;utm_campaign=Badge_Grade)

Cytopia is a free, open source retro pixel-art city building game with a big focus on mods. It’s based on a custom isometric rendering engine based on SDL2. 

Current Key Features:
- Custom UI System, based on JSON. Highly moddable
- SDL2 based rendering engine written in C++
- Camera panning, zooming, relocating
- Terrain manipulation
- Pixel-art graphics made by Kingtut 101 and Pineapples Trix
- A Qt based tile editor for editing TileData JSON files.

Planned features:
- Procedural Terrain Generation
- OpenGL Renderer
- Gameplay mechanics
- In-Game Mod downloading mechanism
- Android / iOS / Mac Support
- Scripting language for mods (like LUA)

For code documentation see the projects [Doxygen Documentation](https://jimmysnails.github.io/Cytopia/build/html/index.html).

If you have questions or if you want to join the project, visit the [Projects Discord Server](https://discord.gg/qwa2H3G) 

#### Key Features
- Different zoom levels
- Relocate camera via rightclick
- Read settings from json files ( Filename to tile ID  / Engine specific Settings )
- Terrain editing (raise / lower terrain)
  
#### Supported Platforms
Linux (clang / g++-5 or higher)
Windows
  
#### Prerequisites

SDL 2
 
<http://www.libsdl.org>
<http://www.libsdl.org/projects/SDL_image>
 
CMake 3.11 or later
 
<https://cmake.org/>
  
QT 5.9 or later (optional, needed for the TileData editor)
 
<http://qt.io>

WiX Toolset v3.11 (optional, needed for packaging on windows)

<http://wixtoolset.org/>

#### Build instructions

To build the TileData editor, you will need a qt installation for your build environment. 

On Windows, download and install the qt installer and select the appropriate environment during installation (e.g. mscv
and set the environment variable Qt5_DIR to your installation directory.
`set Qt5_DIR=D:\Qt\5.12.0\msvc2017_64`

This project uses [CMake](https://cmake.org) to manage the build.
Please make sure you have installed at least version 3.9 or later.
To build the project, create a new build directory, then:

    cmake <PATH_TO_REPOSITORY>

This will configure the build.
If any dependencies are missing, you will receive an error here.

On windows, please use
    git submodule init
in your repository to download the sdl dependencies.

If you are using Windows and have manually installed SDL2, you will need to update your PATH environment variable to point to it.

Note: If you want to change your install directory (per default it's set to ../install) add the definition `-DCMAKE_INSTALL_PREFIX=/foo/bar` to your cmake call.

Once the build is configured, you can then compile it using:

    cmake --build .
    
To install the project, 

    cmake --build . --target install

Create a package (MSI, RPM, DEB, ZIP)

    cmake --build . --target package

#### Coding guidelines

Please use the clang-format (.clang-format file is provided in the root of the repository), if you want to contribute in order to format your code according to our coding guidelines.

If you are on a linux system, you can use:

    find . -regex '.*\.\(hxx\|cxx\)' -not -path '*/ThirdParty/*' -exec clang-format -style=file -i {} \;  

#### Work-in-Progress Screenshot

![alt text](https://raw.githubusercontent.com/JimmySnails/Cytopia/master/images/Screenshot1.png)
![alt text](https://raw.githubusercontent.com/JimmySnails/Cytopia/master/images/Screenshot2.png)

#### Latest build

.deb Package:
<https://jimmysnails.github.io/Cytopia/build/Cytopia-0.1-Linux.deb>
