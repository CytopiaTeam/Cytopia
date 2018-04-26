#ifndef MAP_HXX_
#define MAP_HXX_


#include <string>
#include <cmath>
#include <vector>

//#include "yaml-cpp/yaml.h"
#include "SDL.h"

#include "sprite.hxx"
#include "point.hxx"

class Map
{
public:

  int _width = 16;
  int _height = 16;

  Map(SDL_Renderer *renderer, SDL_Window *window);
  virtual ~Map();

  void parseMapFile();

  void render();

  void Map::centerScreenOnPoint(Point isoCoordinates);

  Point getMaxScreenCoords();
  Point getMinScreenCoords();

  Point getIsoCoords(Point mouseCoords, bool calcWithoutOffset = false);
  Point getScreenCoords(Point isoCoords, bool calcWithoutOffset = false);

  Point getCameraOffset();
  void Map::setCameraOffset(Point offset) { _cameraOffset = offset; };

  bool Map::checkBoundaries(Point isoCoordinates);

  float Map::getZoomLevel() { return _zoom; };
  void Map::setZoomLevel(float zoomLevel) { _zoom = zoomLevel; };

  bool getDrawGrid() { return _drawGrid; }
  void toggleGrid() { _drawGrid = !_drawGrid; }

  std::vector<Sprite*> _tiles;
  std::vector<Sprite*> _grid;

  int mmap[16][16];

private:
  bool _drawGrid = false;

  int _maxXTile = 0;
  int _minXTile = 0;
  int _maxYTile = 0;
  int _minYTile = 0;

  int _screen_width;
  int _screen_height;


  float _zoom = 1.0;
  Point _cameraOffset;
  Point _centerScreenCoords;
};


#endif 