#include "engine.hxx"

Engine::Engine()
{
   TILE_SIZE = Resources::getTileSize();
  
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
    
  _floorCellMatrix = vectorMatrix(_width, _height);
  _zoomLevel = Resources::getZoomLevel();

  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;


  SDL_GetWindowSize(_window, &_screen_width, &_screen_height);

  // set camera to map center
  _centerIsoCoordinates = Point(_width / 2, _height / 2);
  centerScreenOnPoint(_centerIsoCoordinates);

  // initialize cell Matrix
  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
    {
      Cell* mapCell = new Cell(Point(x, y));
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
  int y = 0;
  int x = 0;

  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
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

    x = (screenCoordinates.getX() + (TILE_SIZE*_zoomLevel)*0.5) - _screen_width * 0.5;
    y = (screenCoordinates.getY() + (TILE_SIZE*_zoomLevel)*0.75) - _screen_height * 0.5;
  
    Resources::setCameraOffset(Point(x, y));
  }
}

bool Engine::checkBoundaries(Point isoCoordinates)
{
  if ( (isoCoordinates.getX() >= 0 && isoCoordinates.getX() <= _width) && (isoCoordinates.getY() >= 0 && isoCoordinates.getY() <= _height) )
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

void Engine::increaseHeight(Point mouseClickCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_RAISE);
  //_floorCellMatrix.getCell(isoCoordinates.getX(), isoCoordinates.getY())->increaseHeight();
}

void Engine::decreaseHeight(Point mouseClickCoordinates)
{
  Resources::setTerrainEditMode(Resources::TERRAIN_LOWER);
  //_floorCellMatrix.getCell(isoCoordinates.getX(), isoCoordinates.getY())->decreaseHeight();
}

void Engine::increaseZoomLevel()
{
  _zoomLevel = Resources::getZoomLevel();

  if (_zoomLevel < 2.0)
  {
    Resources::setZoomLevel(_zoomLevel + 0.25);
    centerScreenOnPoint(_centerIsoCoordinates);
  }
}

void Engine::decreaseZoomLevel()
{
  _zoomLevel = Resources::getZoomLevel();

  if (_zoomLevel > 0.5)
  {
    Resources::setZoomLevel(_zoomLevel - 0.25);
    centerScreenOnPoint(_centerIsoCoordinates);
  }
}

Point Engine::findCellAtCoords(Point screenCoordinates)
{
  int foundCells = 0;
  Point isoCoordinates;


  // THIS DOES NOT WORK !!!!!!!!
  // isoCoordinates = _floorCellMatrix.findCellAtCoords(screenCoordinates);
  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
    {
      if (_floorCellMatrix.getCell(x, y)->isInCell(screenCoordinates))
      { 
        foundCells++;
        isoCoordinates = Point (x, y);
        std::cout << "TOLLER POINT " << x << y << std::endl;
      }
    }
  }

  if (foundCells > 1)
  {
    std::cerr << "ERROR! More than one cells were found! This should never happen! check code at Engine::findCellAtCoords ";
  }

  return isoCoordinates;
}