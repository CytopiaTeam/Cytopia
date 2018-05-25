#include <string>
#include "SDL2/SDL.h"
#include "../basics/point.hxx"
#include "../basics/resources.hxx"
#include "../engine/textureManager.hxx"
#include "../log.hxx"

class UiBasics
{
public:
  //Initializes variables
  UiBasics();
  UiBasics(int x, int y, int uiSpriteID);


  //Deallocates memory
  ~UiBasics() { };

  void loadFile(std::string &filePath);

  //Renders texture at given point
  //void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
  void render();
  void changeTexture(int tileID);

  void renderTexture(int w, int h);

  void renderTexture();

  Point _screenCoordinates;
private:
  SDL_Texture * _texture;
  SDL_Renderer* _renderer;
  SDL_Window* _window;
  SDL_Rect _destRect;

  Point _isoCoordinates;
  int _uiID;

};