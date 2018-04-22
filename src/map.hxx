#ifndef MAP_HPP_
#define MAP_HPP_


#include <string>
#include <cmath>
#include <vector>

//#include "yaml-cpp/yaml.h"
#include "SDL.h"

#include "Sprite.hxx"


class Map
{
public:
  Map(SDL_Renderer *renderer, SDL_Window *window);
  virtual ~Map();

  void parseMapFile();

  void render(int cameraoffset_x, int cameraoffset_y, float zoom = 1);

  float getMaxPixelY(float zoom = 1.0);
  float getMinPixelY(float zoom = 1.0);
  float getMaxPixelX(float zoom = 1.0);
  float getMinPixelX(float zoom = 1.0);

  float getIsoX(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);
  float getIsoY(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);

  int getIsoCoordinateX(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);
  int getIsoCoordinateY(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom = 1.0);

  float getPixelX(float x, float y, int cameraoffset_x = 0, int cameraoffset_y = 0, float zoom = 1.0);
  float getPixelY(float x, float y, int cameraoffset_x = 0, int cameraoffset_y = 0, float zoom = 1.0);

  bool getDrawGrid() { return _drawGrid; }
  void toggleGrid() { _drawGrid = !_drawGrid; }

  std::vector<Sprite*> _tiles;
  std::vector<Sprite*> _grid;

  int _width;
  int _height;
  int mmap[16][16];

private:
  bool _drawGrid = false;

  int _maxXTile;
  int _minXTile;
  int _maxYTile;
  int _minYTile;
};


#endif /* MAP_HPP_ */
