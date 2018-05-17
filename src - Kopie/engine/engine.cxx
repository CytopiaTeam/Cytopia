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

  int z = 0;

  // initialize cell Matrix
  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
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

void Engine::clickCell(Point clickedCoords)
{
  Point foundCoordinates;
  for (int x = 0; x <= _width; x++)
  {
    for (int y = _height; y >= 0; y--)
    {
        //_floorCellMatrix.getCell(x, y)->isInCell(clickedCoords);
        //_floorCellMatrix.getCell(x, y)->isInRect(clickedCoords);

        // get all found cells and use the coordinates of the one with the highest z-order !



        // NEW
      Cell* currentCell = _floorCellMatrix.getCell(x, y);

      SDL_Rect spriteRect = currentCell->getSprite()->textureInformation();
      float zoomLevel = Resources::getZoomLevel();
        //int x = clickedCoords.getX();
        //int y = clickedCoords.getY();



        if (clickedCoords.getX() >= spriteRect.x && clickedCoords.getX() < (spriteRect.x + spriteRect.w))
        {

          /* Check Y coordinate is within rectangle range */
          if (clickedCoords.getY() >= spriteRect.y && clickedCoords.getY() < (spriteRect.y + spriteRect.h))
          {

            spriteRect.x *= zoomLevel;
            spriteRect.y *= zoomLevel;


            int pixelX = clickedCoords.getX() - spriteRect.x;
            int pixelY = clickedCoords.getY() - spriteRect.y;

            std::cout << "pixelXY " << pixelX << pixelY << std::endl;
 //           std::cout << "PixelXY !!! " << pixelX << " , " << pixelY << std::endl;
            //std::cout << "CLICKED !!! " << clickedCoords.getX() << " , " << clickedCoords.getY() << std::endl;
            //std::cout << "RECT !!! " << myRect.x << " , " << myRect.y << std::endl;

            //std::cout << "My TileID = " << _tileID << std::endl;
            //std::cout << "TEST: " << (int)TextureManager::Instance().GetPixelColor(_tileID, pixelX, pixelY).a << std::endl;

            SDL_Surface* surface = TextureManager::Instance().getSurface(currentCell->getTileID());

            if (surface == nullptr)
              std::cerr << "No Surface Found! This should not happen!" << std::endl;

            // ------- Surface alpha


            //SDL_PixelFormat* pixelFormat = surface->format;
            int bpp = surface->format->BytesPerPixel;
            Uint8* p = (Uint8*)surface->pixels + pixelX * surface->pitch + pixelX * bpp;
            // ! here the game crashes
            //Uint32 pixelColor = { 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE };
            //Uint32 pixelColor = *p;
            Uint8*	pPixel = (Uint8*)surface->pixels + pixelY * surface->pitch + pixelY * bpp;
            Uint32	pixelColor = (Uint32)pPixel;



            // get the RGBA values

            SDL_Color Color = { 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE };
            Uint8 red, green, blue, alpha;

            // this function fails, sometimes the game crashes here
            SDL_GetRGBA(pixelColor, surface->format, &Color.r, &Color.g, &Color.b, &Color.a);

            // ---------------


            //std::cout << "BEFORE ALPHA CHECK!!! " << _isoCoordinates.getX() << " , " << _isoCoordinates.getY() << std::endl;
            //std::cout << "Pixel Color Red detected = " << (int)Color.r << std::endl;  
            //std::cout << "Pixel Alpha detected = " << (int)Color.a << std::endl;
            if (TextureManager::Instance().GetPixelColor(currentCell->getTileID(), pixelX, pixelY).a != SDL_ALPHA_TRANSPARENT)
              //if (! TextureManager::Instance().isPixelTransparent(_tileID, x, y))
            {


              /* X and Y is inside the rectangle */
              //std::cout << "my tile height = " << myRect.h << std::endl;

              // Find the tile that's rendered in the front (top Z Order)
              if (foundCoordinates.getZ() < currentCell->getCoordinates().getZ())
              {
                foundCoordinates = currentCell->getCoordinates();
              }
            }
          }

        }

        // NEW 
    }
  }
  std::cout << "The Cell, that was clicked is " << foundCoordinates.getX() << " , " << foundCoordinates.getY() << std::endl;
}