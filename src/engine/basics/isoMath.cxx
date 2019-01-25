#include "isoMath.hxx"

#include "../engine.hxx"
#include "settings.hxx"
#include "resources.hxx"
#include "point.hxx"

Point convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset)
{
  int x, y;

  int height = isoCoordinates.height;
  int heightOffset = 18;

  int _tileSize = Resources::getTileSize();
  Point _cameraOffset = Resources::getCameraOffset();
  float _zoomLevel = Resources::getZoomLevel();
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

Point convertScreenToIsoCoordinates(const Point &screenCoordinates)
{
  return Engine::instance().findNodeInMap(screenCoordinates);
}
