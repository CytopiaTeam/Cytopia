#ifndef CELL_HXX_
#define CELL_HXX_

#include <vector>
#include <map>
#include <cmath>

#include "SDL2/SDL.h"


#include "sprite.hxx"
#include "basics/point.hxx"
#include "basics/resources.hxx"

/** @brief Class that holds map cells
 * Each tile is represented by the map cell class.   
 */
class Cell
{
public:
  Cell();
  explicit Cell(Point isoCoordinates);
  ~Cell();

  /** @brief get Sprite
    * get the Sprite* object for this cell
    * @see Sprite
    */
  Sprite* getSprite();

  /// get iso coordinates of this cell
  Point getCoordinates();

  /// add the cell to the renderer
  void renderCell();

  /// Sets the neighbors of this cell for fast access
  void setNeighbors(std::vector<Cell*> neighbors);

  /** @brief Increase Height
    * Increases the height of this map cell and checks which
    * tileID must be drawn for each neighbor
    */
  void increaseHeight();
  /** @brief Decrease Height 
    * Decreases the height of this map cell and checks which 
    * tileID must be drawn for each neighbor
    */
  void decreaseHeight();

  /** @brief set Tile ID 
    * Change the texture of the map cell to a specific tile id
    * @see Resources#readTileListFile
    * @param tileID The tileID that should be rendered for this map cell
    */
  void setTileID(int tileID);
  /** @brief get Tile ID
    * Retrieves the current Tile ID of this map cell
    * @return Returns the current Tile ID as Integer
    */
  int getTileID();

private:
  Point _isoCoordinates;
  Sprite* _sprite;

  std::vector<Cell*> _neighbors;
  SDL_Renderer* _renderer;
  SDL_Window* _window;

  int _heightOffset = 20; // Offset for Y Coordinate between two height levels
  int _tileID;
  int _maxCellHeight = 32;

  /** \brief: determine which tile ID should be drawn for this cell
    * Checks all the neighbors and determines the tile ID of this mapcell according to it's
    * elevated / lowered neighbors.
    */
  void determineTile();

  /** \brief set tileID for each neighbor
    * After a cell is raised / lowered, each neighbor must check which tileID it should have
    * @see Cell#drawSurroundingTiles
  */
  void drawSurroundingTiles(Point isoCoordinates);
  
  /** Enum (bitmask) for mapping neighbor tile positions
    * [ T B L R TL TR BL BR ]
    * [ 0 0 0 0  0  0  0  0 ]

    * Our neighbor tiles look like this
    * 2 5 8  
    * 1 X 7
    * 0 3 6
    */
  
  enum elevatedTilePosition : unsigned int{
       NO_NEIGHBORS = 0x0,
       ELEVATED_TOP = 0x1,
       ELEVATED_BOTTOM = 0x2,
       ELEVATED_LEFT = 0x4,
       ELEVATED_RIGHT = 0x8,
       ELEVATED_TOP_LEFT = 0x10,
       ELEVATED_TOP_RIGHT = 0x20,
       ELEVATED_BOTTOM_LEFT = 0x40,
       ELEVATED_BOTTOM_RIGHT = 0x80,
  };

  /** UInt to elevate store tile position in */
  unsigned int _elevatedTilePosition;


  /** Map neighbor tile combinations to tile ID for terrain keytiles. */
  std::map<unsigned int, int> keyTileMap =
  {
    { NO_NEIGHBORS, 14 },
    { ELEVATED_TOP, 3 },
    { ELEVATED_BOTTOM, 5 },
    { ELEVATED_RIGHT, 1 },
    { ELEVATED_LEFT, 7 },
    { ELEVATED_BOTTOM_RIGHT, 2 },
    { ELEVATED_BOTTOM_LEFT, 8 },
    { ELEVATED_TOP_RIGHT, 0 },
    { ELEVATED_TOP_LEFT, 6 },

    { ELEVATED_TOP | ELEVATED_TOP_LEFT, 3 },
    { ELEVATED_TOP | ELEVATED_TOP_RIGHT, 3 },
    { ELEVATED_TOP | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT, 3 },
    { ELEVATED_TOP_LEFT | ELEVATED_TOP_RIGHT, 3 },

    { ELEVATED_BOTTOM | ELEVATED_BOTTOM_LEFT, 5 },
    { ELEVATED_BOTTOM | ELEVATED_BOTTOM_RIGHT, 5 },
    { ELEVATED_BOTTOM | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT, 5 },
    { ELEVATED_BOTTOM_LEFT | ELEVATED_BOTTOM_RIGHT, 5 },
  
    { ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 7},
    { ELEVATED_LEFT | ELEVATED_TOP_LEFT, 7},
    { ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 7},
    { ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 7},

    { ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 1 },
    { ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 1 },
    { ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_BOTTOM_RIGHT, 1 },
    { ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_RIGHT, 1 },

    { ELEVATED_TOP | ELEVATED_LEFT, 9 },
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 9 }, 
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_RIGHT, 9 },
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 9 },
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT, 9 },
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_TOP_RIGHT, 9 },
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT, 9 },
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 9 },
    { ELEVATED_TOP | ELEVATED_LEFT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_RIGHT, 9 },

    { ELEVATED_TOP | ELEVATED_RIGHT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_LEFT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_LEFT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_BOTTOM_RIGHT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 10 },
    { ELEVATED_TOP | ELEVATED_RIGHT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 10 },

    { ELEVATED_BOTTOM | ELEVATED_RIGHT, 13 },
    { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_TOP_RIGHT, 13 },
    { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT, 13 },
    { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT, 13 },
    { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT , 13 },
    { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_RIGHT , 13 },
    { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_RIGHT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT , 13 },
    { ELEVATED_BOTTOM | ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 13 }, 
    { ELEVATED_BOTTOM | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 13 }, 

    { ELEVATED_BOTTOM | ELEVATED_LEFT, 11 },
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_TOP_LEFT, 11 },
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT, 11 },
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT, 11 },
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_LEFT, 11 },
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_BOTTOM_RIGHT, 11 },
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 11 },
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 11},
    { ELEVATED_BOTTOM | ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT | ELEVATED_TOP_LEFT, 11 },

    //diagonal tiles - insert a block
    { ELEVATED_TOP_LEFT | ELEVATED_BOTTOM_RIGHT, 4},
    { ELEVATED_BOTTOM_LEFT | ELEVATED_TOP_RIGHT, 4},


    // special cases - elevate the tile
    { ELEVATED_RIGHT | ELEVATED_BOTTOM_LEFT, -1},
    { ELEVATED_RIGHT | ELEVATED_TOP_LEFT, -1},
    { ELEVATED_LEFT | ELEVATED_BOTTOM_RIGHT, -1},
    { ELEVATED_LEFT | ELEVATED_TOP_RIGHT, -1},
    { ELEVATED_TOP | ELEVATED_BOTTOM_LEFT, -1},
    { ELEVATED_TOP | ELEVATED_BOTTOM_RIGHT, -1},
    { ELEVATED_BOTTOM | ELEVATED_TOP_LEFT, -1},
    { ELEVATED_BOTTOM | ELEVATED_TOP_RIGHT, -1}

  };
};


#endif