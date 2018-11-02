#include "tile.hxx"

#include "basics/log.hxx"
#include "textureManager.hxx"

SDL_Texture *Tile::getTexture(unsigned int bitMaskElevation)
{

  //LOG() << "****** begin Bitmask: " << (int)bitMaskElevation;
  std::string direction;

  elevationMask = bitMaskElevation;
  //LOG() << "real bitmask: " << elevationMask.to_string();

  // check for all combinations
  if (elevationMask.none())
  { // NONE
    direction = "none";
  }
  else if (elevationMask.test(0) && elevationMask.test(3))
  { // TOP && RIGHT
    direction = "n_and_w";
  }
  else if (elevationMask.test(0) && elevationMask.test(2))
  { // TOP && LEFT
    direction = "n_and_e";
  }
  else if (elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM && RIGHT
    direction = "s_and_w";
  }
  else if (elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM && LEFT
    direction = "s_and_e";
  }
  else if (elevationMask.test(0))
  { // TOP
    direction = "n";
  }
  else if (elevationMask.test(1))
  { // BOTTOM
    direction = "s";
  }
  else if (elevationMask.test(2))
  { // LEFT
    direction = "e";
  }
  else if (elevationMask.test(3))
  { // RIGHT
    direction = "w";
  }
  else if (elevationMask.test(5))
  { // TOP_LEFT
    direction = "nw";
  }
  else if (elevationMask.test(4))
  { // TOP_RIGHT
    direction = "ne";
  }
  else if (elevationMask.test(7))
  { // BOTTOM_LEFT
    direction = "sw";
  }
  else if (elevationMask.test(6))
  { // BOTTOM_RIGHT
    direction = "se";
  }

  else
  {
    LOG(LOG_ERROR) << "No Combination for bitmask " << elevationMask.to_string() << " found! This should not have happened";
  }

  return TextureManager::Instance().getTileTextureNew("floor", direction);
}

// 0 = 2^0 = 1   = TOP
// 1 = 2^1 = 2   = BOTTOM
// 2 = 2^2 = 4   = LEFT
// 3 = 2^3 = 8   = RIGHT
// 4 = 2^4 = 16  = TOP LEFT
// 5 = 2^5 = 32  = TOP RIGHT
// 6 = 2^6 = 64  = BOTTOM LEFT
// 7 = 2^7 = 128 = BOTTOM RIGHT