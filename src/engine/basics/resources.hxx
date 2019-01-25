#ifndef RESOURCES_HXX_
#define RESOURCES_HXX_

#include <unordered_map>

#include <SDL.h>

#include "point.hxx"

// Namespace from json library - https://github.com/nlohmann/json

//forward declare Engine class (prevent cross includes)
class Engine;

class Resources
{
public:
  /** \brief get Zoom Level
  * Gets the Zoom Level, which is used to scale the tiles.
  * \return float variable zoomLevel
  */
  static float getZoomLevel() { return _zoomLevel; };

  /** \brief set Zoom Level
  * Sets the Zoom Level, which is used to scale the tiles.
  * @param zoomLeve float variable zoomLevel.
  */
  static void setZoomLevel(float zoomLevel) { _zoomLevel = zoomLevel; };

  /** \brief get camera offset 
    * The pixel offset that is taken into account when positoning the tiles.
    * \return Point() object containing x and y screen space coordinates
    */
  const static Point &getCameraOffset() { return _cameraOffset; };

  /** \brief set camera offset
  * The pixel offset that is taken into account when positoning the tiles.
  * @param cameraOffset Point() object containing x and y screen space coordinates
  */
  static void setCameraOffset(Point cameraOffset) { _cameraOffset = cameraOffset; };

  /** \brief gets the size of a tile
    * The size of a tile is always 32 by 32 pixel for now.
    * \return int tileSize
    */
  static int getTileSize() { return _tileSize; };

private:
  Resources() = default;
  ~Resources() = default;

  static SDL_Renderer *_renderer;
  static SDL_Window *_window;

  static float _zoomLevel;
  static Point _cameraOffset;

  static const int _tileSize;
};

#endif
