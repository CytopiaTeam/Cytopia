#include "tile.hxx"

#include "basics/log.hxx"
#include "textureManager.hxx"

SDL_Texture *Tile::getTexture(TileType type, TileOrientation orientation)
{

  return TextureManager::Instance().getTileTexture(type, orientation);
}

TileOrientation Tile::caluclateOrientation(unsigned char bitMaskElevation)
{
  TileOrientation orientation;
  std::bitset<8> elevationMask = bitMaskElevation;

  // check for all combinations
  if (elevationMask.none())
  { // NONE
    orientation = TileOrientation::DEFAULT;
  }

  // special cases 
  else if (elevationMask.test(3) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::S_AND_W;
  }
  else if (elevationMask.test(2) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::N_AND_E;
  }
  else if (elevationMask.test(3) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::N_AND_W;
  }
  else if (elevationMask.test(2) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::S_AND_E;
  }

  else if (elevationMask.test(0) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::N_AND_E;
  }
  else if (elevationMask.test(1) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::S_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::N_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::S_AND_E;
  }

  // diagonal combinations
  else if (elevationMask.test(0) && elevationMask.test(3))
  { // TOP && RIGHT
    orientation = TileOrientation::N_AND_W;
  }
  else if (elevationMask.test(0) && elevationMask.test(2))
  { // TOP && LEFT
    orientation = TileOrientation::N_AND_E;
  }
  else if (elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM && RIGHT
    orientation = TileOrientation::S_AND_W;
  }
  else if (elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM && LEFT
    orientation = TileOrientation::S_AND_E;
  }

  // default directions
  else if (elevationMask.test(0))
  { // TOP
    orientation = TileOrientation::N;
  }
  else if (elevationMask.test(1))
  { // BOTTOM
    orientation = TileOrientation::S;
  }
  else if (elevationMask.test(2))
  { // LEFT
    orientation = TileOrientation::E;
  }
  else if (elevationMask.test(3))
  { // RIGHT
    orientation = TileOrientation::W;
  }
  else if ((elevationMask.test(4) && elevationMask.test(7)) || (elevationMask.test(5) && elevationMask.test(6)))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::CENTER;
  }
  else if (elevationMask.test(5))
  { // TOP_LEFT
    orientation = TileOrientation::NW;
  }
  else if (elevationMask.test(4))
  { // TOP_RIGHT
    orientation = TileOrientation::NE;
  }
  else if (elevationMask.test(7))
  { // BOTTOM_LEFT
    orientation = TileOrientation::SW;
  }
  else if (elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = TileOrientation::SE;
  }


  else
  {
    LOG(LOG_ERROR) << "No Combination for bitmask " << elevationMask.to_string() << " found! This should not have happened";
  }
  return orientation;
}
// 0 = 2^0 = 1   = TOP
// 1 = 2^1 = 2   = BOTTOM
// 2 = 2^2 = 4   = LEFT
// 3 = 2^3 = 8   = RIGHT
// 4 = 2^4 = 16  = TOP LEFT
// 5 = 2^5 = 32  = TOP RIGHT
// 6 = 2^6 = 64  = BOTTOM LEFT
// 7 = 2^7 = 128 = BOTTOM RIGHT