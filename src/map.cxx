#include "map.hxx"





Map::Map(SDL_Renderer* renderer, SDL_Window *window)
{






  int tilesize = 32;


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

void Map::render(int cameraoffset_x, int cameraoffset_y, float zoom)
{
  int i = 0;
  int y = 0;
  int x = 0;
  for (std::vector<Sprite*>::iterator it = _tiles.begin(); it != _tiles.end(); ++it)
  {
    (*it)->render(cameraoffset_x, cameraoffset_y, zoom, 0);
    if (_drawGrid)
    {
      _grid[i]->render(cameraoffset_x, cameraoffset_y, zoom, 0);
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

Point Map::getMaxScreenCoords(float zoom)
{
  Point MaxPixelCoords = _tiles[_maxXTile]->getTileScreenCoordinates(0, 0, zoom);
  return MaxPixelCoords;
}

Point Map::getMinScreenCoords(float zoom)
{
  Point MaxPixelCoords = _tiles[_minXTile]->getTileScreenCoordinates(0, 0, zoom);
  return MaxPixelCoords;
}

/// convert screen point to Iso Coordinates
Point Map::getIsoCoords(Point mouseCoords, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  Point isoCoords;
  isoCoords.x = (mouseCoords.x + cameraoffset_x + 2.0*(mouseCoords.y + cameraoffset_y)) / (TILE_SIZE*zoom) - 1.5;
  isoCoords.y = (mouseCoords.x + cameraoffset_x - 2.0*(mouseCoords.y + cameraoffset_y)) / (TILE_SIZE*zoom) + 1.5;
  return isoCoords;
}

Point Map::getScreenCoords(Point isoCoords, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  Point screenCoords;
  screenCoords.x = (TILE_SIZE*zoom * isoCoords.x * 0.5) + (TILE_SIZE*zoom * isoCoords.y * 0.5) - cameraoffset_x;
  screenCoords.y = ((TILE_SIZE*zoom * isoCoords.x * 0.25) - (TILE_SIZE*zoom * isoCoords.y * 0.25)) - cameraoffset_y;

  return screenCoords;
}

