#include "resources.hxx"

#include "../engine.hxx"

// Instantiate static variables
SDL_Renderer *Resources::_renderer = nullptr;
SDL_Window *Resources::_window = nullptr;
float Resources::_zoomLevel = 1.0;
Point Resources::_cameraOffset;
const int Resources::_tileSize = 32;
