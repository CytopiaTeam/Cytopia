#include "engine.hxx"

Engine::Engine()
{
  _tileSize = Resources::getTileSize();

  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _map_size = Settings::Instance().settings.mapSize;

  _mapCellMatrix = vectorMatrix(_map_size, _map_size);
  _zoomLevel = Resources::getZoomLevel();

  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;

  _screen_height = Settings::Instance().settings.screenHeight;
  _screen_width = Settings::Instance().settings.screenWidth;

  // set camera to map center
  _centerIsoCoordinates = Point{_map_size / 2, _map_size / 2, 0, 0};
  centerScreenOnPoint(_centerIsoCoordinates);
}

void Engine::render()
{
  int mapSize = Settings::Instance().settings.mapSize;

  for (int x = 0; x <= mapSize; x++)
  {
    for (int y = mapSize; y >= 0; y--)
    {
      _mapCellMatrix.getCell(x, y)->getSprite()->render();
    }
  }
}

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
  }
}

bool Engine::isPointWithinBoundaries(const Point &isoCoordinates)
{
  return (isoCoordinates.x >= 0 && isoCoordinates.x <= _map_size) && (isoCoordinates.y >= 0 && isoCoordinates.y <= _map_size);
}

void Engine::increaseHeightOfCell(const Point &isoCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_RAISE);
  _mapCellMatrix.increaseHeightOfCell(isoCoordinates);
}

void Engine::decreaseHeightOfCell(const Point &isoCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_LOWER);
  _mapCellMatrix.decreaseHeightOfCell(isoCoordinates);
}

void Engine::increaseZoomLevel()
{
  _zoomLevel = Resources::getZoomLevel();

  if (_zoomLevel < 2.0f)
  {
    Resources::setZoomLevel(_zoomLevel + 0.25f);
    centerScreenOnPoint(_centerIsoCoordinates);
  }
}

void Engine::decreaseZoomLevel()
{
  _zoomLevel = Resources::getZoomLevel();

  if (_zoomLevel > 0.5f)
  {
    Resources::setZoomLevel(_zoomLevel - 0.25f);
    centerScreenOnPoint(_centerIsoCoordinates);
  }
}

Point Engine::findCellAt(const Point &screenCoordinates)
{
  Point foundCoordinates{-1, -1, 0, 0};

  // check all cells of the map to find the clicked point
  for (int x = 0; x <= _map_size; x++)
  {
    for (int y = _map_size; y >= 0; y--)
    {
      std::shared_ptr<Cell> currentCell = _mapCellMatrix.getCell(x, y);

      SDL_Rect spriteRect = currentCell->getSprite()->getTextureInformation();

      int clickedX = screenCoordinates.x;
      int clickedY = screenCoordinates.y;

      int spriteX = spriteRect.x;
      int spriteY = spriteRect.y;

      if (clickedX >= spriteX && clickedX < spriteX + spriteRect.w && clickedY >= spriteY && clickedY < spriteY + spriteRect.h)
      {
        // Calculate the position of the clicked pixel within the surface
        int pixelX = (clickedX - spriteX);
        int pixelY = (clickedY - spriteY);
        // "un-zoom" the positon to match the un-adjusted surface
        pixelX = static_cast<int>(pixelX / _zoomLevel);
        pixelY = static_cast<int>(pixelY / _zoomLevel);

        // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
        if (TextureManager::Instance().getPixelColor(currentCell->getTileID(), pixelX, pixelY).a != SDL_ALPHA_TRANSPARENT)
        {
          if (foundCoordinates.z < currentCell->getCoordinates().z)
          {
            foundCoordinates = currentCell->getCoordinates();
          }
        }
      }
    }
  }
  return foundCoordinates;
}

void Engine::rotateMap()
{
  _mapCellMatrix.rotateMatrix();
}