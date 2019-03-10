#!/bin/bash

# Build SDL2 manually to have the latest version
curl -L https://www.libsdl.org/release/SDL2-2.0.9.tar.gz | tar xz
pushd SDL2-* && ./configure && make && sudo make install && popd
curl -L https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.0.14.tar.gz | tar xz
pushd SDL2_ttf-* && ./configure && make && sudo make install && popd
curl -L https://www.libsdl.org/projects/SDL_image/release/SDL2_image-2.0.4.tar.gz | tar xz
pushd SDL2_image-* && ./configure && make && sudo make install && popd
curl -L https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.4.tar.gz | tar xz
pushd SDL2_mixer-* && ./configure && make && sudo make install && popd