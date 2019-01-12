#include "resources.hxx"

#include "../engine.hxx"

// Instantiate static variables
SDL_Renderer *Resources::_renderer = nullptr;
SDL_Window *Resources::_window = nullptr;
float Resources::_zoomLevel = 1.0;
Point Resources::_cameraOffset;
const int Resources::_tileSize = 32;

Point Resources::convertScreenToIsoCoordinates(const Point &screenCoordinates)
{
  return Engine::instance().findNodeInMap(screenCoordinates);
}

Point Resources::convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.height;
  int heightOffset = 18;

  if (calcWithoutOffset)
  {
    x = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.5) + (_tileSize * _zoomLevel * isoCoordinates.y * 0.5));
    y = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.25) - (_tileSize * _zoomLevel * isoCoordinates.y * 0.25));
  }
  else
  {
    x = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.5) + (_tileSize * _zoomLevel * isoCoordinates.y * 0.5) -
                         _cameraOffset.x);
    y = static_cast<int>((_tileSize * _zoomLevel * isoCoordinates.x * 0.25) - (_tileSize * _zoomLevel * isoCoordinates.y * 0.25) -
                         _cameraOffset.y);
  }

  if (height > 0)
  {
    y = static_cast<int>(y - ((_tileSize - heightOffset) * height * _zoomLevel));
  }

  return {x, y, 0, 0};
}
