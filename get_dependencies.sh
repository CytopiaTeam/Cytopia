#!/bin/bash -x

cd external/android/SDL

wget https://www.libsdl.org/release/SDL2-2.0.9.tar.gz
rm -rf SDL2
tar xf SDL2-2.0.9.tar.gz
rm SDL2-2.0.9.tar.gz
mv SDL2-2.0.9 SDL2
rm SDL2-2.0.9.tar.gz

wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz
rm -rf SDL2_image
tar xf SDL2_image-2.0.5.tar.gz
rm SDL2_image-2.0.5.tar.gz
mv SDL2_image-2.0.5 SDL2_image
rm SDL2_image-2.0.5.tar.gz

wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz
rm -rf SDL2_ttf
tar xf SDL2_ttf-2.0.15.tar.gz
rm SDL2_ttf-2.0.15.tar.gz
mv SDL2_ttf-2.0.15 SDL2_ttf
rm SDL2_ttf-2.0.15.tar.gz

cd ..

mkdir -p externLibs
cd other

rm -rf libnoise
git clone https://github.com/qknight/libnoise.git

rm -rf zlib
wget https://www.zlib.net/zlib-1.2.11.tar.gz
tar xf zlib-1.2.11.tar.gz
rm zlib-1.2.11.tar.gz
mv zlib-1.2.11 zlib

rm -rf openal-soft
wget https://github.com/kcat/openal-soft/archive/openal-soft-1.20.1.tar.gz
tar xf openal-soft-1.20.1.tar.gz
rm openal-soft-1.20.1.tar.gz
mv openal-soft-openal-soft-1.20.1 openal-soft

rm -rf ogg
git clone https://github.com/xiph/ogg.git

rm -rf vorbis
git clone https://github.com/xiph/vorbis.git

#adjust vorbis cmakelists to make it buildable on android
sed -i '10istring(REPLACE "libvorbis" "libogg" OGG_BINARY_DIR ${CMAKE_BINARY_DIR})' vorbis/CMakeLists.txt
sed -i '11istring(REPLACE "libvorbis" "libogg" OGG_LIBRARY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})' vorbis/CMakeLists.txt
sed -i '12istring(REPLACE "vorbis" "ogg" OGG_SOURCE_DIR ${CMAKE_SOURCE_DIR})' vorbis/CMakeLists.txt
sed -i '13ifile(COPY ${OGG_BINARY_DIR}/include/ogg/config_types.h DESTINATION ${OGG_SOURCE_DIR}/include/ogg)' vorbis/CMakeLists.txt
sed -i '14iset (OGG_INCLUDE_DIR "${OGG_SOURCE_DIR}/include")' vorbis/CMakeLists.txt
sed -i '15iset (OGG_LIBRARY ${OGG_LIBRARY}/libogg.so)' vorbis/CMakeLists.txt

cd ../../..
