# IsometricEngine  
[![Build Status](https://travis-ci.org/JimmySnails/IsometricEngine.svg?branch=master)](https://travis-ci.org/JimmySnails/IsometricEngine)


An SDL2 based, isometric rendering engine.
The projects goal is to create a citybuilding game with similiar style to SimCity 2000

For code documentation see the projects [Doxygen Documentation](https://jimmysnails.github.io/IsometricEngine/build/html/index.html).

If you have questions or if you want to join the project, visit the [Projects Discord Server](https://discord.gg/qwa2H3G) 

#### Key Features:
  - Different zoom levels
  - Relocate camera via rightclick
  - Read settings from json files ( Filename to tile ID  / Engine specific Settings )
  - Terrain editing (raise / lower terrain)
  


#### Supported Platforms:
  Linux (clang / g++-5 or higher)
  Windows
  
#### Prerequisites

 SDL 1.2.5 or later
  http://www.libsdl.org
  http://www.libsdl.org/projects/SDL_image/
 
 CMake 3.9 or later
  https://cmake.org/

#### Build instructions:
 - cmake <PATH_TO_REPOSITORY>
 - cmake --build .

![alt text](https://raw.githubusercontent.com/JimmySnails/IsometricEngine/master/images/Screenshot1.png)
