#ifndef GAME_HXX_
#define GAME_HXX_

#pragma once

#ifndef VERSION
#define VERSION 0
#endif

#include "SDL2/SDL.h"

#include "engine/engine.hxx"
#include "engine/eventManager.hxx"
#include "engine/uiManager.hxx"
#include "engine/audioMixer.hxx"
#include "engine/basics/point.hxx"
#include "engine/basics/resources.hxx"
#include "engine/basics/log.hxx"

SDL_Renderer *_renderer;
SDL_Window *_window;

#endif