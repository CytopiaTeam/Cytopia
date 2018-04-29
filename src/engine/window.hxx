#pragma once

#include <string>
#include <iostream>

#include "SDL2/SDL.h"

#include "sprite.hxx"



class Window {
  public:
    Window(const std::string &title, int width, int height);
    ~Window();
    
    inline bool isClosed() const { return _closed; }
    void close() { _closed = true; }


    SDL_Renderer* Window::getSDLRenderer() { return _renderer; }
    SDL_Window* Window::getSDLWindow() { return _window; }


  private:
    bool init();
      
  private:
    std::string _title;
    int _width = 800;
    int _height = 600;

    
    bool _closed = false;

    SDL_Window *_window = nullptr;
	  SDL_Renderer *_renderer = nullptr;
  
};


