#ifndef MAP_HXX_
#define MAP_HXX_


#include <string>
#include <cmath>
#include <vector>

//#include "yaml-cpp/yaml.h"
#include "SDL2/SDL.h"

#include "sprite.hxx"
#include "cell.hxx"
#include "basics/point.hxx"
#include "basics/vectorMatrix.hxx"


class Engine
{
public:

  int _width = 16;
  int _height = 16;
  int mmap[16][16];

  Engine(SDL_Renderer *renderer, SDL_Window *window);
  virtual ~Engine();

  void parseMapFile();

  void render();

  void centerScreenOnPoint(Point isoCoordinates);

  Point getIsoCoords(Point screenCoordinates, bool calcWithoutOffset = false);
  Point getScreenCoords(Point isoCoordinates, bool calcWithoutOffset = false);
  std::vector<Sprite*> findNeighbors(Point isoCoords);

  Point getCameraOffset();
  void setCameraOffset(Point offset);

  bool checkBoundaries(Point isoCoordinates);

  float getZoomLevel();
  void setZoomLevel(float zoomLevel);

  void enableLayer(unsigned int layer);
  void disableLayer(unsigned int layer);
  void toggleLayer(unsigned int layer);
  void increaseHeight(Point isoCoordinates);
  void decreaseHeight(Point isoCoordinates);

private:
  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _screen_width;
  int _screen_height;

  float _zoom = 1.0;
  Point _cameraOffset;
  Point _centerScreenCoords;

  vectorMatrix _floorTilesMatrix;
  vectorMatrix _gridTilesMatrix;
  vectorMatrix _buildingsTilesMatrix;
  vectorMatrix _selectedTilesMatrix;

  // Cells
  vectorMatrix _floorCellMatrix;


public:
  enum Layers : unsigned int
  {
    LAYER_FLOOR       = 1u<<0,
    LAYER_BUILDINGS   = 1u<<1,
    LAYER_GRID        = 1u<<2,
    LAYER_SELECTION   = 1u<<3
  };

private:
  unsigned int _activeLayers;

};


#endif 