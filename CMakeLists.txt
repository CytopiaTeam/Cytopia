set (PROJECT_NAME "IsometricEngine")
set (TARGET "IsometricEngine")
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

find_package(Doxygen)

# own target for Doxygen builds
if ( DOXYGEN_FOUND )
  set (DoxyFile_src ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
  set (DoxyFile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file(${DoxyFile_src} ${DoxyFile} @ONLY)

  
  message("Doxygen build started.")
  add_custom_target(
    doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${DoxyFile}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
  )
endif()

cmake_minimum_required(VERSION 3.9)

if ( UNIX )
  find_package(SDL2 REQUIRED)
  find_package(SDL2_image REQUIRED)
  find_package(SDL2_ttf REQUIRED)

  if ( SDL2_FOUND AND SDL2_IMAGE_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
    include_directories(${SDL2_IMAGE_INCLUDE_DIR})
    include_directories(${SDL2_TTF_INCLUDE_DIR})
  endif ()
  
else()
  set (SDL2_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x64/SDL2.lib;${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x64/SDL2main.lib")
  set (SDL2_IMAGE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x64/SDL2_image.lib")
  set (SDL2_TTF_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x64/SDL2_ttf.lib")

  set (SDL2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdl/include")
endif()


file (GLOB_RECURSE ISOENGINE_SOURCES "src/*.cxx")
file (GLOB_RECURSE ISOENGINE_INCLUDES "src/*.hxx")


add_executable (${TARGET} ${ISOENGINE_SOURCES})
include_directories (${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIR} ${ISOENGINE_INCLUDES} )
target_link_libraries (${TARGET} ${SDL2_LIBRARY} ${SDL2_IMAGE_LIBRARY} )


add_custom_command(TARGET ${TARGET} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${TARGET}>/resources)

if ( WIN32 )
  # set startup project for Visual Studio Builds
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  # set Visual Studio working directory to the Configuration directory
  set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)")
endif ()
