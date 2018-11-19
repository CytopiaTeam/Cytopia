#ifndef RESOURCES_HXX_
#define RESOURCES_HXX_

#include <unordered_map>

#include "SDL2/SDL.h"
#include "point.hxx"

// Namespace from json library - https://github.com/nlohmann/json

//forward declare Engine class (prevent cross includes)
class Engine;

class Resources
{
public:
  /** \brief get SDL Renderer
    * Returns a pointer to the SDL Renderer
    * \return Pointer to the SDL_Renderer
    */
  static SDL_Renderer *getRenderer() { return _renderer; };

  /** \brief set SDL Renderer
    * Sets the SDL Renderer. Is only used once when the SDL_Renderer is created.
    * @param renderer SDL_Renderer* to store inside the resources class.
    */
  static void setRenderer(SDL_Renderer *renderer) { _renderer = renderer; };

  /** \brief get SDL Window
    * Returns a pointer to the SDL_Window
    * \return Pointer to the SDL_Window
    */
  static SDL_Window *getWindow() { return _window; };

  /** \brief set SDL Window
  * Sets the SDL Window. Is only used once when the SDL_Window is created.
  * @param renderer SDL_Window* to store inside the resources class.
  */
  static void setWindow(SDL_Window *window) { _window = window; };

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

  /** \brief converts screen space coordinates to isometric space coordinates.
    * To convert screen coordinates in to isometric coordinates, all the textures inside the nodes are checked, if the click is inside the bounding box of a texture
    * and if so, another check ensures, that the click is not on a transparent pixel. The isometric coordinates of the map node containing the texture with
    * the highest Z-Level (drawing order) is returned.
    * Camera Offset and current zoomLevel is taken into account
    * \returns Point() - object containing the isometric coordinates of the tile that matches the screen coordinates
    * @param Point() screenCoordinates - object containing screen space coordinates
    */
  static Point convertScreenToIsoCoordinates(const Point &screenCoordinates);

  /** \brief converts coordinates from isometric to screen space
    * The given isometric coordinates (which contain height information) are converted to the screen coordinates. The coordinates represent the x, y position of the
    * tile where it is drawn (if tile height / width is added, the whole bounding box could be calculated)
    * Camera Offset and current zoomLevel is taken into account
    *  \returns Point() object containing screen space coordinates
    * @param Point() isoCoordinates - object containing isometric coordinates 
    * @param bool calcWithoutOffset - optional parameter to calculate screenspace coordinates without zoomLevel and cameraOffset taken into account 
    */
  static Point convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset = false);

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
