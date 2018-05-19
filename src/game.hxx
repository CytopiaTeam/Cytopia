#ifndef GAME_HXX_
#define GAME_HXX_

#pragma once

#include <iostream>

#include "SDL2/SDL.h"

#include "engine/window.hxx"
#include "engine/sprite.hxx"
#include "engine/engine.hxx"
#include "engine/basics/point.hxx"
#include "engine/basics/resources.hxx"
//#include "engine/basics/log.hxx"


bool editMode;
SDL_Renderer* _renderer;
SDL_Window* _window;

#endif