#include "engine.hxx"

Engine::Engine()
{
  int tilesize = 32;
  
  _renderer = Resources::getRenderer();
  _window = Resources::getWindow();
  
  _floorTilesMatrix = vectorMatrix(_width, _height);
  _gridTilesMatrix = vectorMatrix(_width, _height);
  _buildingsTilesMatrix = vectorMatrix(_width, _height);
  _selectedTilesMatrix = vectorMatrix(_width, _height);
  
  _floorCellMatrix = vectorMatrix(_width, _height);

  _zoomLevel = Resources::getZoomLevel();



  // Default: Floor and Buildings are drawn
  _activeLayers = LAYER_FLOOR | LAYER_BUILDINGS;


  SDL_GetWindowSize(_window, &_screen_width, &_screen_height);
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

      
      tile = new Sprite("resources/images/floor/floor.png", Point(x, y));
      grid = new Sprite("resources/images/selection/grid.png", Point(x, y));
      
      if ( (x == 1) && (y == 1) )
        building = new Sprite("resources/images/buildings/house.png", Point(x, y));
      
      // Cells
      Cell* mapCell = new Cell(Point(x, y), tile);
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

  _cameraOffset = Resources::getCameraOffset();
  _zoomLevel = Resources::getZoomLevel();

  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
    {
      // Layer 0 - floor
      if ( _activeLayers & LAYER_FLOOR )
      {
        //if ( _floorTilesMatrix.getSprite(x, y) != nullptr )
        //  _floorTilesMatrix.getSprite(x, y)->render(_cameraOffset, _zoom);
        // TODO: Use cell class for everything instead of direct sprite rendering!
        _floorCellMatrix.getCell(x, y)->renderCell();
      }
      // Layer 1 - grid
      if ( _activeLayers & LAYER_GRID )
      {
        if ( _gridTilesMatrix.getSprite(x, y) != nullptr )
          _gridTilesMatrix.getSprite(x, y)->render();
      }
      // Layer 2 - Buildings
      if ( _activeLayers & LAYER_BUILDINGS )
      {
        if ( _buildingsTilesMatrix.getSprite(x, y) != nullptr )
          _buildingsTilesMatrix.getSprite(x, y)->render();
      }
      // Layer 3 - Selection
      if ( _activeLayers & LAYER_SELECTION )
      {
        if ( _selectedTilesMatrix.getSprite(x, y) != nullptr )
          _selectedTilesMatrix.getSprite(x, y)->render();
      }
    }
  }
}

void Engine::centerScreenOnMap()
{
  Point screenCoordinates = Resources::convertIsoToScreenCoordinates(Point(_width / 2, _height /2), true);
  int x, y;
  _zoomLevel = Resources::getZoomLevel();


  x = (screenCoordinates.getX() + (TILE_SIZE*_zoomLevel)*0.5) - _screen_width * 0.5;
  y = (screenCoordinates.getY() + (TILE_SIZE*_zoomLevel)*0.75) - _screen_height * 0.5;

  Resources::setCameraOffset(Point(x, y));
}

void Engine::centerScreenOnPoint(Point isoCoordinates)
{
  Point screenCoordinates = Resources::convertIsoToScreenCoordinates(isoCoordinates, true);
  int x, y;
  _zoomLevel = Resources::getZoomLevel();

  x = (screenCoordinates.getX() + (TILE_SIZE*_zoomLevel)*0.5) - _screen_width * 0.5;
  y = (screenCoordinates.getY() + (TILE_SIZE*_zoomLevel)*0.75) - _screen_height * 0.5;
  
  Resources::setCameraOffset(Point(x, y));
}

bool Engine::checkBoundaries(Point isoCoordinates)
{
  if ( (isoCoordinates.getX() >= 0 && isoCoordinates.getX() <= _width) && (isoCoordinates.getY() >= 0 && isoCoordinates.getY() <= _height) )
    return true;
  else
    return false;
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

void Engine::increaseHeight(Point isoCoordinates)
{
  _floorCellMatrix.getCell(isoCoordinates.getX(), isoCoordinates.getY())->increaseHeight(1);
}