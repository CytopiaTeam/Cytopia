#ifndef ENGINE_HXX_
#define ENGINE_HXX_

#include "SDL2/SDL.h"

#include "textureManager.hxx"
#include "cell.hxx"
#include "basics/point.hxx"
#include "basics/vectorMatrix.hxx"
#include "basics/resources.hxx"


class Engine
{
public:
  Engine();
  virtual ~Engine() { };

  // Map Size
  int _width = 16;
  int _height = 16;

  void parseMapFile();

  /**Renders each Map Cell
  */
  void render();

  bool checkBoundaries(Point isoCoordinates);
  void centerScreenOnPoint(Point isoCoordinates);
  void enableLayer(unsigned int layer);
  void disableLayer(unsigned int layer);
  void toggleLayer(unsigned int layer);
  void increaseHeight(Point isoCoordinates);
  void decreaseHeight(Point isoCoordinates);
  void increaseZoomLevel();
  void decreaseZoomLevel();

private:
  SDL_Renderer *_renderer;
  SDL_Window *_window;

  int _screen_width;
  int _screen_height;
  int TILE_SIZE;

  float _zoomLevel;
  Point _cameraOffset;
  Point _centerIsoCoordinates;

  // Cells
  vectorMatrix _floorCellMatrix;

  unsigned int _activeLayers;

  // Layer ENUM
public:
  enum Layers : unsigned int
  {
    LAYER_FLOOR       = 1u<<0,
    LAYER_BUILDINGS   = 1u<<1,
    LAYER_GRID        = 1u<<2,
    LAYER_SELECTION   = 1u<<3
  };
};

#endif 