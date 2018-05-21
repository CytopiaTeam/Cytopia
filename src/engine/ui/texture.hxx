#include <string>
#include "SDL2/SDL.h"
#include "../basics/point.hxx"
#include "../basics/resources.hxx"
#include "../engine/textureManager.hxx"
#include "../log.hxx"

class Texture
{
public:
  //Initializes variables
  Texture();

  //Deallocates memory
  ~Texture() { };

  void loadFile(std::string &filePath);

  //Renders texture at given point
  //void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
  void render();
  void changeTexture(int tileID);

  void renderTexture(int w, int h);

  void renderTexture();

private:
  SDL_Texture * _texture;
  SDL_Renderer* _renderer;
  SDL_Window* _window;
  SDL_Rect _destRect;

  Point _isoCoordinates;
  Point _screenCoordinates;
  int _uiID;

};