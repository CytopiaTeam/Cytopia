#!/bin/bash -x

pushd contrib

wget https://www.libsdl.org/release/SDL2-2.0.9.tar.gz
rm -rf SDL2
tar xf SDL2-2.0.9.tar.gz
mv SDL2-2.0.9 SDL2
rm SDL2-2.0.9.tar.gz

wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.5.tar.gz
rm -rf SDL2_image
tar xf SDL2_image-2.0.5.tar.gz
mv SDL2_image-2.0.5 SDL2_image
rm SDL2_image-2.0.5.tar.gz

wget https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz
rm -rf SDL2_mixer
tar xf SDL2_mixer-2.0.4.tar.gz
mv SDL2_mixer-2.0.4 SDL2_mixer
rm SDL2_mixer-2.0.4.tar.gz

wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.15.tar.gz
rm -rf SDL2_ttf
tar xf SDL2_ttf-2.0.15.tar.gz
mv SDL2_ttf-2.0.15 SDL2_ttf
rm SDL2_ttf-2.0.15.tar.gz

popd

mkdir -p externLibs
pushd externLibs

rm -rf libnoise
git clone https://github.com/qknight/libnoise.git

rm -rf zlib
wget https://www.zlib.net/zlib-1.2.11.tar.gz
tar xf zlib-1.2.11.tar.gz
mv zlib-1.2.11 zlib
popd
