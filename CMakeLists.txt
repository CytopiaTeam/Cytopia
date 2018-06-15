cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(
  # this will be the ${PROJECT_NAME}
  IsometricEngine
  # this will provide ${PROJECT_VERSION}
  VERSION 0.0.2
  # we are using C++
  LANGUAGES CXX
)

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

# find all required dependences - if these fail the build will not continue
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

file (GLOB_RECURSE ISOENGINE_SOURCES "src/*.cxx")

add_executable (${TARGET} ${ISOENGINE_SOURCES})
target_link_libraries (
  ${TARGET}
  PRIVATE
    SDL::SDL2main
    SDL::SDL2_image
    SDL::SDL2_ttf
)

add_custom_command(TARGET ${TARGET} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${TARGET}>/resources)

if ( MSVC )
  # set startup project for Visual Studio Builds
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
  # set Visual Studio working directory to the Configuration directory
  set_target_properties(${TARGET} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)")
endif ()

add_definitions(-DVERSION="${PROJECT_NAME} ${PROJECT_VERSION} alpha build")