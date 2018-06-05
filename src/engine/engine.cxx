#include "engine.hxx"

Engine::Engine()
{
   TILE_SIZE = Resources::getTileSize();
  
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _map_size = Resources::settings.mapSize;

  _mapCellMatrix = vectorMatrix(_map_size, _map_size);
  _zoomLevel = Resources::getZoomLevel();

  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;

  _screen_height = Resources::settings.screenHeight;
  _screen_width = Resources::settings.screenWidth;

  // set camera to map center
  _centerIsoCoordinates = Point(_map_size / 2, _map_size / 2);
  centerScreenOnPoint(_centerIsoCoordinates);
}

void Engine::parseMapFile()
{
  // TODO: load Map from FIle. Coordinates, Height, Tile
}

void Engine::render()
{
  int mapSize = Resources::settings.mapSize;

  for (int x = 0; x <= mapSize; x++)
  {
    for (int y = mapSize; y >= 0; y--)
    {
      _mapCellMatrix.getCell(x, y)->getSprite()->render();
    }
  }
}

void Engine::centerScreenOnPoint(Point isoCoordinates)
{
  if (checkBoundaries(isoCoordinates))
  {
    _centerIsoCoordinates = isoCoordinates;
    Point screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates, true);
    int x, y;
    _zoomLevel = Resources::getZoomLevel();

    x = static_cast<int>((screenCoordinates.getX() + (TILE_SIZE*_zoomLevel)*0.5) - _screen_width * 0.5);
    y = static_cast<int>((screenCoordinates.getY() + (TILE_SIZE*_zoomLevel)*0.75) - _screen_height * 0.5);
  
    Resources::setCameraOffset(Point(x, y));
  }
}

bool Engine::checkBoundaries(Point isoCoordinates)
{
  if (( isoCoordinates.getX() >= 0 && isoCoordinates.getX() <= _map_size ) 
  && (  isoCoordinates.getY() >= 0 && isoCoordinates.getY() <= _map_size ))
    return true;
  else
    return false;
}

void Engine::increaseHeight(Point isoCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_RAISE);
  _mapCellMatrix.increaseHeight(isoCoordinates);
}

void Engine::decreaseHeight(Point isoCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_LOWER);
  _mapCellMatrix.decreaseHeight(isoCoordinates);
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

Point Engine::findCellAt(Point screenCoordinates)
{
  Point foundCoordinates = Point(-1, -1);
 
  // check all cells of the map to find the clicked point
  for (int x = 0; x <= _map_size; x++)
  {
    for (int y = _map_size; y >= 0; y--)
    {
      std::shared_ptr<Cell> currentCell = _mapCellMatrix.getCell(x, y);

      SDL_Rect spriteRect = currentCell->getSprite()->getTextureInformation();

      int clickedX = screenCoordinates.getX() ;
      int clickedY = screenCoordinates.getY() ;

      int spriteX = spriteRect.x;
      int spriteY = spriteRect.y;

      if ( clickedX >= spriteX && clickedX < spriteX + spriteRect.w  
      &&   clickedY >= spriteY && clickedY < spriteY + spriteRect.h )
      {
        // Calculate the position of the clicked pixel within the surface
        int pixelX = (clickedX - spriteX );
        int pixelY = (clickedY - spriteY );
        // "un-zoom" the positon to match the un-adjusted surface
        pixelX = static_cast<int>(pixelX / _zoomLevel);
        pixelY = static_cast<int>(pixelY / _zoomLevel);

        // Check if the clicked Sprite is not transparent (we hit a point within the pixel)
        if (TextureManager::Instance().GetPixelColor(currentCell->getTileID(), pixelX, pixelY).a != SDL_ALPHA_TRANSPARENT)
        {
          if (foundCoordinates.getZ() < currentCell->getCoordinates().getZ())
          {
            foundCoordinates = currentCell->getCoordinates();
          }
        }
      }
    }
  }
  return foundCoordinates;
}