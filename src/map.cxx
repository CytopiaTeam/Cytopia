#include "map.hxx"




Map::Map(SDL_Renderer* renderer, SDL_Window *window)
{

  _maxXTile = 0;
  _minXTile = 0;
  _maxYTile = 0;
  _minYTile = 0;



  _width = 16;
  _height = 16;

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
  for (int y = 0; y<_width; y++)
  {
    for (int x = 0; x<_height; x++)
    {
      Sprite *tile = nullptr;
      Sprite *grid = nullptr;
      tile = new Sprite("resources/images/city/grass.png", x, y, renderer, window);
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









float Map::getMaxPixelX(float zoom)
{
  return _tiles[_maxXTile]->getPixelX(0, 0, zoom);
}

float Map::getMinPixelX(float zoom)
{
  return _tiles[_minXTile]->getPixelX(0, 0, zoom);
}

float Map::getMaxPixelY(float zoom)
{
  return _tiles[_maxYTile]->getPixelY(0, 0, zoom);
}

float Map::getMinPixelY(float zoom)
{
  return _tiles[_minYTile]->getPixelY(0, 0, zoom);
}

float Map::getIsoX(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pos_x = (x + cameraoffset_x + 2.0*(y + cameraoffset_y)) / (TILE_SIZE*zoom) - 1.5;
  return pos_x;
}

float Map::getIsoY(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pos_y = (x + cameraoffset_x - 2.0*(y + cameraoffset_y)) / (TILE_SIZE*zoom) + 1.5;
  return pos_y;
}

int Map::getIsoCoordinateX(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pos_x = getIsoX(x, y, cameraoffset_x, cameraoffset_y, zoom);
  return std::floor(pos_x);
}

int Map::getIsoCoordinateY(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pos_y = getIsoY(x, y, cameraoffset_x, cameraoffset_y, zoom);
  return std::floor(pos_y);
}

float Map::getPixelX(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pixel_x = 0;
  pixel_x = (TILE_SIZE*zoom * x * 0.5) + (TILE_SIZE*zoom * y * 0.5) - cameraoffset_x;

  return pixel_x;
}

float Map::getPixelY(float x, float y, int cameraoffset_x, int cameraoffset_y, float zoom)
{
  float pixel_y = 0;
  pixel_y = ((TILE_SIZE*zoom * x * 0.25) - (TILE_SIZE*zoom * y * 0.25)) - cameraoffset_y;

  return pixel_y;
}
