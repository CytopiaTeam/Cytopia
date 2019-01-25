#include "engine.hxx"

#include "basics/camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/settings.hxx"
#include "GameObjects/mapNode.hxx"
#include "textureManager.hxx"

Engine::Engine()
{
  // get renderer and initialize windowManager singleton class
  _renderer = WindowManager::instance().getRenderer();
  _window = WindowManager::instance().getWindow();

  _map_size = Settings::instance().settings.mapSize;

  _map = Map(_map_size, _map_size);

  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;

  _screen_height = Settings::instance().settings.screenHeight;
  _screen_width = Settings::instance().settings.screenWidth;

  // set camera to map center
  _centerIsoCoordinates = Point{_map_size / 2, _map_size / 2, 0, 0};
  centerScreenOnPoint(_centerIsoCoordinates);
}

void Engine::render() { _map.renderMap(); }

void Engine::centerScreenOnPoint(const Point &isoCoordinates)
{
  if (isPointWithinMapBoundaries(isoCoordinates))
  {
    _centerIsoCoordinates = isoCoordinates;
    Point screenCoordinates = convertIsoToScreenCoordinates(isoCoordinates, true);
    int x, y;

    x = static_cast<int>((screenCoordinates.x + (_tileSize * Camera::zoomLevel) * 0.5) - _screen_width * 0.5);
    y = static_cast<int>((screenCoordinates.y + (_tileSize * Camera::zoomLevel) * 0.75) - _screen_height * 0.5);

    Camera::cameraOffset = { x, y };
    _map.refresh();
  }
}

void Engine::increaseHeight(const Point &isoCoordinates)
{
  terrainEditMode = TerrainEdit::RAISE;
  _map.increaseHeight(isoCoordinates);
}

void Engine::decreaseHeight(const Point &isoCoordinates)
{
  terrainEditMode = TerrainEdit::LOWER;
  _map.decreaseHeight(isoCoordinates);
}

void Engine::increaseZoomLevel()
{
  if (Camera::zoomLevel < 4.0f)
  {
    Camera::zoomLevel += 0.25f;
    centerScreenOnPoint(_centerIsoCoordinates);
    _map.refresh();
  }
}

void Engine::decreaseZoomLevel()
{
  if (Camera::zoomLevel > 0.5f)
  {
    Camera::zoomLevel -= 0.25f;
    centerScreenOnPoint(_centerIsoCoordinates);
    _map.refresh();
  }
}

Point Engine::findNodeInMap(const Point &screenCoordinates) { return _map.findNodeInMap(screenCoordinates); }

void Engine::toggleFullScreen() { WindowManager::instance().toggleFullScreen(); };

void Engine::setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID)
{
  _map.setTileIDOfNode(isoCoordinates, tileID);
}

void Engine::demolishNode(const Point &isoCoordinates) { _map.demolishNode(isoCoordinates); }
