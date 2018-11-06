#include "tile.hxx"

#include "basics/log.hxx"
#include "textureManager.hxx"

SDL_Texture *Tile::getTexture(const std::string &type, const std::string &orientation)
{

  return TextureManager::Instance().getTileTexture(type, orientation);
}

std::string Tile::caluclateOrientation(unsigned char bitMaskElevation)
{
  std::string orientation;
  std::bitset<8> elevationMask(bitMaskElevation);

  // check for all combinations
  if (elevationMask.none())
  { // NONE
    orientation = "default";
  }

  // special cases
  else if (elevationMask.test(3) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = "s_and_w";
  }
  else if (elevationMask.test(2) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = "n_and_e";
  }
  else if (elevationMask.test(3) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = "n_and_w";
  }
  else if (elevationMask.test(2) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = "s_and_e";
  }

  else if (elevationMask.test(0) && elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = "n_and_e";
  }
  else if (elevationMask.test(1) && elevationMask.test(5))
  { // BOTTOM_RIGHT
    orientation = "s_and_w";
  }
  else if (elevationMask.test(0) && elevationMask.test(7))
  { // BOTTOM_RIGHT
    orientation = "n_and_w";
  }
  else if (elevationMask.test(1) && elevationMask.test(4))
  { // BOTTOM_RIGHT
    orientation = "s_and_e";
  }

  // diagonal combinations
  else if (elevationMask.test(0) && elevationMask.test(3))
  { // TOP && RIGHT
    orientation = "n_and_w";
  }
  else if (elevationMask.test(0) && elevationMask.test(2))
  { // TOP && LEFT
    orientation = "n_and_e";
  }
  else if (elevationMask.test(1) && elevationMask.test(3))
  { // BOTTOM && RIGHT
    orientation = "s_and_w";
  }
  else if (elevationMask.test(1) && elevationMask.test(2))
  { // BOTTOM && LEFT
    orientation = "s_and_e";
  }

  // default directions
  else if (elevationMask.test(0))
  { // TOP
    orientation = "n";
  }
  else if (elevationMask.test(1))
  { // BOTTOM
    orientation = "s";
  }
  else if (elevationMask.test(2))
  { // LEFT
    orientation = "e";
  }
  else if (elevationMask.test(3))
  { // RIGHT
    orientation = "w";
  }
  else if ((elevationMask.test(4) && elevationMask.test(7)) || (elevationMask.test(5) && elevationMask.test(6)))
  { // BOTTOM_RIGHT
    orientation = "center";
  }
  else if (elevationMask.test(5))
  { // TOP_LEFT
    orientation = "nw";
  }
  else if (elevationMask.test(4))
  { // TOP_RIGHT
    orientation = "ne";
  }
  else if (elevationMask.test(7))
  { // BOTTOM_LEFT
    orientation = "sw";
  }
  else if (elevationMask.test(6))
  { // BOTTOM_RIGHT
    orientation = "se";
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