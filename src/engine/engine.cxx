#include "engine.hxx"

#include "basics/mapEdit.hxx"
#include "basics/resources.hxx"
#include "basics/settings.hxx"
#include "GameObjects/mapNode.hxx"
#include "textureManager.hxx"

Engine::Engine()
{
  _windowManager = new WindowManager("Cytopia");
  _tileSize = Resources::getTileSize();

  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _map_size = Settings::instance().settings.mapSize;

  _map = Map(_map_size, _map_size);
  _zoomLevel = Resources::getZoomLevel();

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
  if (isPointWithinBoundaries(isoCoordinates))
  {
    _centerIsoCoordinates = isoCoordinates;
    Point screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates, true);
    int x, y;
    _zoomLevel = Resources::getZoomLevel();

    x = static_cast<int>((screenCoordinates.x + (_tileSize * _zoomLevel) * 0.5) - _screen_width * 0.5);
    y = static_cast<int>((screenCoordinates.y + (_tileSize * _zoomLevel) * 0.75) - _screen_height * 0.5);

    Resources::setCameraOffset(Point{x, y, 0, 0});
    _map.refresh();
  }
}

bool Engine::isPointWithinBoundaries(const Point &isoCoordinates)
{
  return (isoCoordinates.x >= 0 && isoCoordinates.x <= _map_size) && (isoCoordinates.y >= 0 && isoCoordinates.y <= _map_size);
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
  _zoomLevel = Resources::getZoomLevel();

  if (_zoomLevel < 4.0f)
  {
    Resources::setZoomLevel(_zoomLevel + 0.25f);
    centerScreenOnPoint(_centerIsoCoordinates);
    _map.refresh();
  }
}

void Engine::decreaseZoomLevel()
{
  _zoomLevel = Resources::getZoomLevel();

  if (_zoomLevel > 0.5f)
  {
    Resources::setZoomLevel(_zoomLevel - 0.25f);
    centerScreenOnPoint(_centerIsoCoordinates);
    _map.refresh();
  }
}

Point Engine::findNodeInMap(const Point &screenCoordinates)
{
  return _map.findNodeInMap(screenCoordinates);
}

bool Engine::isGameRunning() { return _windowManager->isRunning(); };

void Engine::quitGame() { _windowManager->close(); };

void Engine::toggleFullScreen() { _windowManager->toggleFullScreen(); };

void Engine::setTileIDOfNode(const Point &isoCoordinates, const std::string &tileID)
{
  _map.setTileIDOfNode(isoCoordinates, tileID);
}

void Engine::demolishNode(const Point &isoCoordinates) { _map.demolishNode(isoCoordinates); }
