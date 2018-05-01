#include "engine.hxx"

Engine::Engine(SDL_Renderer* renderer, SDL_Window *window)
{
  int tilesize = 32;
  
  _renderer = renderer;
  _window = window;
  
  _floorTilesMatrix = vectorMatrix(_width, _height);
  _gridTilesMatrix = vectorMatrix(_width, _height);
  _buildingsTilesMatrix = vectorMatrix(_width, _height);
  _selectedTilesMatrix = vectorMatrix(_width, _height);
  
  _floorCellMatrix = vectorMatrix(_width, _height);



  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;


  SDL_GetWindowSize(window, &_screen_width, &_screen_height);
  // 16 x 16 height for further tests

  const int height = 16;
  const int width = 16;

  int mapp[height][width] = {
    { 1,1,0,2,2,2,2,2,1,1,2,2,2,2,2,1 },
  { 1,1,1,1,2,1,1,2,1,1,2,2,2,2,2,1 },
  { 2,1,1,0,2,2,2,2,1,1,2,2,2,2,2,1 },
  { 2,1,1,2,2,1,1,2,1,1,2,2,2,2,2,1 },
  { 2,1,1,4,4,4,1,2,1,1,2,2,2,2,4,1 },
  { 2,1,1,4,4,4,1,2,1,1,2,2,2,2,2,1 },
  { 2,1,1,4,4,4,1,2,1,1,2,2,4,2,2,1 },
  { 2,2,2,4,4,4,2,1,2,3,3,3,4,2,2,1 },
  { 1,1,2,2,2,2,2,3,4,3,3,3,4,2,2,2 },
  { 1,1,1,1,2,1,1,2,1,3,3,3,2,2,2,3 },
  { 2,1,1,1,2,2,2,2,1,1,2,2,2,2,2,1 },
  { 2,1,1,2,2,1,1,2,1,1,3,2,2,2,4,4 },
  { 2,1,1,4,2,1,1,2,1,1,3,2,2,2,2,4 },
  { 2,1,1,1,2,1,1,2,1,1,3,3,3,3,3,4 },
  { 2,1,1,1,1,1,1,2,1,1,2,2,2,2,4,4 },
  { 2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,1 }
  };






  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
    {
      Sprite *tile = nullptr;
      Sprite *grid = nullptr;
      Sprite *building = nullptr;

      
      tile = new Sprite("resources/images/floor/floor.png", Point(x, y), renderer, window);
      grid = new Sprite("resources/images/selection/grid.png", Point(x, y), renderer, window);
      
      if ( (x == 1) && (y == 1) )
        building = new Sprite("resources/images/buildings/house.png", Point(x, y), renderer, window);
      
      // Cells
      Cell* mapCell = new Cell(Point(x, y), tile, _renderer, _window);
      _floorCellMatrix.addCell(x, y, mapCell);


      _floorTilesMatrix.addSprite(x, y, tile);
      _gridTilesMatrix.addSprite(x, y, grid);
      _buildingsTilesMatrix.addSprite(x, y, building);
    }
  }

  _floorCellMatrix.initCells();

}



Engine::~Engine()
{
  // TODO Auto-generated destructor stub
}


void Engine::parseMapFile()
{
  // TODO: load Map from FIle. Coordinates, Height, Tile
}

void Engine::render()
{
  int i = 0;
  int y = 0;
  int x = 0;

  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
    {
      // Layer 0 - floor
      if ( _activeLayers & LAYER_FLOOR )
      {
        //if ( _floorTilesMatrix.getSprite(x, y) != nullptr )
        //  _floorTilesMatrix.getSprite(x, y)->render(_cameraOffset, _zoom);

        _floorCellMatrix.getCell(x, y)->renderCell(_cameraOffset, _zoom);
      }
      // Layer 1 - grid
      if ( _activeLayers & LAYER_GRID )
      {
        if ( _gridTilesMatrix.getSprite(x, y) != nullptr )
          _gridTilesMatrix.getSprite(x, y)->render(_cameraOffset, _zoom);
      }
      // Layer 2 - Buildings
      if ( _activeLayers & LAYER_BUILDINGS )
      {
        if ( _buildingsTilesMatrix.getSprite(x, y) != nullptr )
          _buildingsTilesMatrix.getSprite(x, y)->render(_cameraOffset, _zoom);
      }
      // Layer 3 - Selection
      if (_activeLayers & LAYER_SELECTION)
      {
        if (_selectedTilesMatrix.getSprite(x, y) != nullptr)
          _selectedTilesMatrix.getSprite(x, y)->render(_cameraOffset, _zoom);
      }
    }
  }
}


/// convert Screen Coordinates to Iso Coordinates
Point Engine::getIsoCoords(Point screenCoordinates, bool calcWithoutOffset)
{
  Point isoCoordinates;
  int x, y;

  if ( calcWithoutOffset )
  {
    x = (screenCoordinates.getX() + 2.0*(screenCoordinates.getY())) / (TILE_SIZE*_zoom) - 1.5;
    y = (screenCoordinates.getX() - 2.0*(screenCoordinates.getY())) / (TILE_SIZE*_zoom) + 1.5;
  }
  else
  {
    x = (screenCoordinates.getX() + _cameraOffset.getX() + 2.0*(screenCoordinates.getY() + _cameraOffset.getY())) / (TILE_SIZE*_zoom) - 1.5;
    y = (screenCoordinates.getX() + _cameraOffset.getX() - 2.0*(screenCoordinates.getY() + _cameraOffset.getY())) / (TILE_SIZE*_zoom) + 1.5;
  }
  isoCoordinates.setCoords(x, y);
  return isoCoordinates;
}

/// convert Iso Coordinates to Screen Coordinates
Point Engine::getScreenCoords(Point isoCoordinates, bool calcWithoutOffset)
{
  Point screenCoordinates;
  int x, y;

  if ( calcWithoutOffset )
  {
    x = (TILE_SIZE*_zoom * isoCoordinates.getX() * 0.5) + (TILE_SIZE*_zoom * isoCoordinates.getY() * 0.5);
    y = (TILE_SIZE*_zoom * isoCoordinates.getX() * 0.25) - (TILE_SIZE*_zoom * isoCoordinates.getY() * 0.25);
  }
  else
  {
    x = (TILE_SIZE*_zoom * isoCoordinates.getX() * 0.5) + (TILE_SIZE*_zoom * isoCoordinates.getY() * 0.5) - _cameraOffset.getX();
    y = ((TILE_SIZE*_zoom * isoCoordinates.getX() * 0.25) - (TILE_SIZE*_zoom * isoCoordinates.getY() * 0.25)) - _cameraOffset.getY();
  }
  screenCoordinates.setCoords(x, y);
  return screenCoordinates;
}

void Engine::centerScreenOnPoint(Point isoCoordinates)
{
  Point screenCoordinates = getScreenCoords(isoCoordinates, true);
  int x, y;

  x = (screenCoordinates.getX() + (TILE_SIZE*_zoom)*0.5) - _screen_width * 0.5;
  y = (screenCoordinates.getY() + (TILE_SIZE*_zoom)*0.75) - _screen_height * 0.5;
  
  _cameraOffset.setCoords(x, y);
}

Point Engine::getCameraOffset()
{ 
  return _cameraOffset; 
}

// Check if Point is with map boundaries

bool Engine::checkBoundaries(Point isoCoordinates)
{
  if ( (isoCoordinates.getX() >= 0 && isoCoordinates.getX() <= _width) && (isoCoordinates.getY() >= 0 && isoCoordinates.getY() <= _height) )
    return true;
  else
    return false;
}

float Engine::getZoomLevel()
{ 
  return _zoom; 
}

void Engine::setZoomLevel(float zoomLevel)
{ 
  _zoom = zoomLevel; 
}

void Engine::setCameraOffset(Point offset)
{ 
  _cameraOffset = offset; 
}

std::vector<Sprite*> Engine::findNeighbors(Point isoCoords)
{
  return _floorTilesMatrix.findNeighbors(isoCoords.getX(), isoCoords.getY());
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

void Engine::increaseOLDHeight(Point isoCoordinates)
{
  _selectedTilesMatrix.clearMatrix();
  Sprite *selection = nullptr;
  Point coords;

  int tileHeight = _floorTilesMatrix.getSprite(isoCoordinates.getX(), isoCoordinates.getY())->getTileIsoCoordinates().getHeight();

  selection = new Sprite("resources/images/floor/floor.png", coords, _renderer, _window);
  _floorTilesMatrix.addSprite(isoCoordinates.getX(), isoCoordinates.getY(), selection);



  _floorTilesMatrix.getSprite(isoCoordinates.getX(), isoCoordinates.getY())->setHeight(tileHeight + 1);

  std::vector<Sprite*> neighbors = findNeighbors(isoCoordinates);

  // TODO: floor surroundings must be drawn in the floor tilematrix, not in the selection
  for (int i=0; i < neighbors.size(); i++)
  {
    if ( neighbors[i] != nullptr )
    {
      coords = neighbors[i]->getTileIsoCoordinates();
      if (coords.getHeight() != tileHeight + 1)
      {
        selection = new Sprite("resources/images/floor/floor_"+std::to_string(i)+".png", coords, _renderer, _window);
        _floorTilesMatrix.addSprite(coords.getX(), coords.getY(), selection);
      }
      else
      {

      selection = new Sprite("resources/images/floor/floor.png", coords, _renderer, _window);
      _floorTilesMatrix.addSprite(coords.getX(), coords.getY(), selection);
      }

    }
  }
}

void Engine::increaseHeight(Point isoCoordinates)
{
  _floorCellMatrix.getCell(isoCoordinates.getX(), isoCoordinates.getY())->increaseHeight(1);
}