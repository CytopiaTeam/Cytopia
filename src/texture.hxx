#pragma once

#ifndef TEXTURE_HXX_
#define TEXTURE_HXX_


#include <iostream>
#include <string>

#include "SDL.h"
#include "SDL_image.h"

#include "point.hxx"

SDL_Texture *loadTexture(std::string file, SDL_Renderer *renderer, bool colorkey = true);

void renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, Point tileScreenCoords, int w, int h);
void renderTexture(SDL_Texture *texture, SDL_Renderer *renderer, Point tileScreenCoords);


#endif 
