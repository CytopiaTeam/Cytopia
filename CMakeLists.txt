set (PROJECT_NAME "Isometric Engine")
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

if ( UNIX )
  find_package(SDL2 REQUIRED)
  find_package(SDL2_image REQUIRED)

  if ( SDL2_FOUND AND SDL2_IMAGE_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
  endif ()
else()
  set (SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdl)

  set (SDL2_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x64/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x64/SDL2main.lib")
  set (SDL2_IMAGE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x64/SDL2_image.lib")

  set (SDL2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include")
endif()



file (GLOB_RECURSE ISOENGINE_SOURCES "src/*.cxx")
file (GLOB_RECURSE ISOENGINE_INCLUDES "src/*.hxx")


add_executable (${PROJECT_NAME} ${ISOENGINE_SOURCES})
include_directories (${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${ISOENGINE_INCLUDES} )
target_link_libraries (${PROJECT_NAME} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} )
