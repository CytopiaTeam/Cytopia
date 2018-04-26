#include "map.hxx"





Map::Map(SDL_Renderer* renderer, SDL_Window *window)
{
  int tilesize = 32;

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






  int i = 0;
  for (int x = 0; x<_width; x++)
  {
    for (int y = _height; y >= 0; y--)
    {
      Sprite *tile = nullptr;
      Sprite *grid = nullptr;

      tile = new Sprite("resources/images/city/grass.png", x, y, renderer, window);
      if ((x == 1) && (y == 1))
        tile = new Sprite("resources/images/city/house.png", x, y, renderer, window);
        
      grid = new Sprite("resources/images/city/grid.png", x, y, renderer, window);
      // TODO: Iterate through map file and draw different tiles for each space
      
      _tiles.push_back(tile);
      _grid.push_back(grid);
    
      if ((x == 0) && (y == 0))
      {
        _minXTile = i;
      }

      else if ((x == _width - 1) && (y == _height - 1))
      {
        _maxXTile = i;
      }

      else if ((x == 0) && (y == _height - 1))
      {
        _minYTile = i;
      }

      else if ((x == _width - 1) && (y == 0))
      {
        _maxYTile = i;
      }

      i++;
    }
  }
}

Map::~Map()
{
  // TODO Auto-generated destructor stub
}


void Map::parseMapFile()
{
  // TODO: load Map from FIle. Coordinates, Height, Tile
}

void Map::render()
{
  int i = 0;
  int y = 0;
  int x = 0;
  for (std::vector<Sprite*>::iterator it = _tiles.begin(); it != _tiles.end(); ++it)
  {
    (*it)->render(_cameraOffset, _zoom, 0);
    if (_drawGrid)
    {
      _grid[i]->render(_cameraOffset, _zoom, 0);
      i++;
    }

    x++;
    if (x == _width)
    {
      y++;
      x = 0;
    }

  }
}

Point Map::getMaxScreenCoords()
{
  Point MaxPixelCoords = _tiles[_maxXTile]->getTileScreenCoordinates(_zoom);
  return MaxPixelCoords;
}

Point Map::getMinScreenCoords()
{
  // should be calculated for offset 0 ?
  Point MaxPixelCoords = _tiles[_minXTile]->getTileScreenCoordinates(_zoom);
  return MaxPixelCoords;
}

/// convert Screen Coordinates to Iso Coordinates
Point Map::getIsoCoords(Point screenCoords, bool calcWithoutOffset)
{
  Point isoCoords;

  if (calcWithoutOffset)
  {
    isoCoords.x = (screenCoords.x + 2.0*(screenCoords.y)) / (TILE_SIZE*_zoom) - 1.5;
    isoCoords.y = (screenCoords.x - 2.0*(screenCoords.y)) / (TILE_SIZE*_zoom) + 1.5;
  }
  else
  {
    isoCoords.x = (screenCoords.x + _cameraOffset.x + 2.0*(screenCoords.y + _cameraOffset.y)) / (TILE_SIZE*_zoom) - 1.5;
    isoCoords.y = (screenCoords.x + _cameraOffset.x - 2.0*(screenCoords.y + _cameraOffset.y)) / (TILE_SIZE*_zoom) + 1.5;
  }
  return isoCoords;
}

/// convert Iso Coordinates to Screen Coordinates
Point Map::getScreenCoords(Point isoCoords, bool calcWithoutOffset)
{
  Point screenCoords;

  if (calcWithoutOffset)
  {
    screenCoords.x = (TILE_SIZE*_zoom * isoCoords.x * 0.5) + (TILE_SIZE*_zoom * isoCoords.y * 0.5);
    screenCoords.y = ((TILE_SIZE*_zoom * isoCoords.x * 0.25) - (TILE_SIZE*_zoom * isoCoords.y * 0.25));
  }
  else
  {
    screenCoords.x = (TILE_SIZE*_zoom * isoCoords.x * 0.5) + (TILE_SIZE*_zoom * isoCoords.y * 0.5) - _cameraOffset.x;
    screenCoords.y = ((TILE_SIZE*_zoom * isoCoords.x * 0.25) - (TILE_SIZE*_zoom * isoCoords.y * 0.25)) - _cameraOffset.y;
  }
  return screenCoords;
}

void Map::centerScreenOnPoint(Point isoCoordinates)
{
  Point screenCoordinates = getScreenCoords(isoCoordinates, true);
  _cameraOffset.x = (screenCoordinates.x + (TILE_SIZE*_zoom)*0.5) - _screen_width * 0.5;
  _cameraOffset.y = (screenCoordinates.y + (TILE_SIZE*_zoom)*0.75) - _screen_height * 0.5;
}

Point Map::getCameraOffset() 
{ 
  return _cameraOffset; 
}

// Check if Point is with map boundaries

bool Map::checkBoundaries(Point isoCoordinates)
{
  if ((isoCoordinates.x >= 0 && isoCoordinates.x <= _width) && (isoCoordinates.y >= 0 && isoCoordinates.y <= _height))
    return true;
  else
    return false;
}