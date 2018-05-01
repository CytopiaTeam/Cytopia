#include "cell.hxx"

Cell::Cell()
{

}

Cell::Cell(Point isoCoordinates, Sprite* sprite, SDL_Renderer* renderer, SDL_Window* window)
{
  _sprite = sprite;
  _coordinates = isoCoordinates;

  _renderer = renderer;
  _window = window;
  // if it's a floor cell, draw sprite based on neighbor tile height



}

Cell::~Cell()
{

}

void Cell::setSprite(Sprite* sprite)
{
  _sprite = sprite;
}

Sprite* Cell::getSprite()
{
  return _sprite;
}

void Cell::setNeighbors(std::vector<Cell*> neighbors)
{
  _neighbors = neighbors;
}

void Cell::renderCell(Point cameraOffset, float zoom)
{

  _sprite->render(cameraOffset, zoom);
}


void Cell::drawSurroundingTiles(Point isoCoordinates)
{

  // Check for surrounding tiles everytime we render
  Point coords;
  Sprite* tile;

  int numElevatedNeighbors = 0;

  int tileHeight = getSprite()->getTileIsoCoordinates().getHeight();

  printf("Count neighbors: %d", _neighbors.size());


  for (int i = 0; i < _neighbors.size(); i++)
  {
    if (_neighbors[i] != nullptr)
    {
      coords = _neighbors[i]->getSprite()->getTileIsoCoordinates();
      if (coords.getHeight() == tileHeight )
      {
        numElevatedNeighbors++;
        tile = new Sprite("resources/images/floor/floor.png", coords, _renderer, _window);
        printf("Found elevated tile at %d,%d\n", coords.getX(), coords.getY());
      }
      else
      {
        tile = new Sprite("resources/images/floor/floor_" + std::to_string(i) + ".png", coords, _renderer, _window);

      }
      //printf("Setting tile %d to coord %d,%d\n", i, coords.getX(), coords.getY());
      _neighbors[i]->setSprite(tile);

    }
  }
  if (numElevatedNeighbors >= 1)
  {
    _sprite = new Sprite("resources/images/floor/floor.png", _sprite->getTileIsoCoordinates(), _renderer, _window);
  }

}

void Cell::increaseHeight(int height)
{
  int tileHeight = _sprite->getTileIsoCoordinates().getHeight();
  _sprite->setHeight(tileHeight + 1);
  drawSurroundingTiles(_sprite->getTileIsoCoordinates());

}