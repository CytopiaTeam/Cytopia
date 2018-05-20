#include "engine.hxx"

Engine::Engine()
{
   TILE_SIZE = Resources::getTileSize();
  
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  _map_size = Resources::settings.mapSize;

  _floorCellMatrix = vectorMatrix(_map_size, _map_size);
  _zoomLevel = Resources::getZoomLevel();

  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;

  _screen_height = Resources::settings.screenHeight;
  _screen_width = Resources::settings.screenWidth;

  // set camera to map center
  _centerIsoCoordinates = Point(_map_size / 2, _map_size / 2);
  centerScreenOnPoint(_centerIsoCoordinates);

  int z = 0;

  // initialize cell Matrix
  for (int x = 0; x <= _map_size; x++)
  {
    for (int y = _map_size; y >= 0; y--)
    {
      z++;
      Cell* mapCell = new Cell(Point(x, y, z));
      _floorCellMatrix.addCell(x, y, mapCell);
    }
  }
  _floorCellMatrix.initCells();
}

void Engine::parseMapFile()
{
  // TODO: load Map from FIle. Coordinates, Height, Tile
}

void Engine::render()
{
  for (int x = 0; x <= _map_size; x++)
  {
    for (int y = _map_size; y >= 0; y--)
    {
      // Layer 0 - floor
      if ( _activeLayers & LAYER_FLOOR )
      {
        _floorCellMatrix.getCell(x, y)->renderCell();
      }
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
  if ( (isoCoordinates.getX() >= 0 && isoCoordinates.getX() <= _map_size) && (isoCoordinates.getY() >= 0 && isoCoordinates.getY() <= _map_size) )
    return true;
  else
    return false;
}

void Engine::enableLayer(unsigned int layer)
{
  // Turn on by using bitwise OR
  _activeLayers |= layer;
}

void Engine::disableLayer(unsigned int layer)
{
  // Turn off by using bitwise AND with inversed pattern
  _activeLayers &= ~layer;
}

void Engine::toggleLayer(unsigned int layer)
{
  // Toggle bitmask by using bitmask XOR
  _activeLayers ^= layer;
}

void Engine::increaseHeight(Point isoCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_RAISE);
  _floorCellMatrix.getCell(isoCoordinates.getX(), isoCoordinates.getY())->increaseHeight();
}

void Engine::decreaseHeight(Point isoCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_LOWER);
  _floorCellMatrix.getCell(isoCoordinates.getX(), isoCoordinates.getY())->decreaseHeight();
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
      Cell* currentCell = _floorCellMatrix.getCell(x, y);

      SDL_Rect spriteRect = currentCell->getSprite()->textureInformation();

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