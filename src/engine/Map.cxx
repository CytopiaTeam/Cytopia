#include "Map.hxx"

#include "basics/Camera.hxx"
#include "basics/isoMath.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "LOG.hxx"
#include "common/Constants.hxx"
#include "map/MapLayers.hxx"
#include "common/JsonSerialization.hxx"
#include "common/Constants.hxx"
#include "Filesystem.hxx"
#include "../services/Randomizer.hxx"

#include "json.hxx"

#include <sstream>
#include <string>
#include <set>
#include <queue>

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif

using json = nlohmann::json;

NeighborNodesPosition operator++(NeighborNodesPosition &nn, int)
{
  NeighborNodesPosition res = nn;

  switch (nn)
  {
  case NeighborNodesPosition::BOTTOM_LEFT:
    nn = NeighborNodesPosition::LEFT;
    break;
  case NeighborNodesPosition::LEFT:
    nn = NeighborNodesPosition::TOP_LEFT;
    break;
  case NeighborNodesPosition::TOP_LEFT:
    nn = NeighborNodesPosition::BOTTOM;
    break;
  case NeighborNodesPosition::BOTTOM:
    nn = NeighborNodesPosition::CENTER;
    break;
  case NeighborNodesPosition::CENTER:
    nn = NeighborNodesPosition::TOP;
    break;
  case NeighborNodesPosition::TOP:
    nn = NeighborNodesPosition::BOTTOM_RIGHT;
    break;
  case NeighborNodesPosition::BOTTOM_RIGHT:
    nn = NeighborNodesPosition::RIGHT;
    break;
  case NeighborNodesPosition::RIGHT:
    nn = NeighborNodesPosition::TOP_RIGHT;
    break;
  case NeighborNodesPosition::TOP_RIGHT:
    nn = NeighborNodesPosition::BOTTOM_LEFT;
    break;
  default:
    assert(false);
    break;
  }

  return res;
}

Map::Map(int columns, int rows) : pMapNodesVisible(new Sprite *[columns * rows]), m_columns(columns), m_rows(rows)
{
  // TODO move Random Engine out of map
  randomEngine.seed();
  MapLayers::enableLayers({TERRAIN, BUILDINGS, WATER, GROUND_DECORATION, ZONE, ROAD, POWERLINES, FLORA});
}

Map::Map(int columns, int rows, const bool generateTerrain)
    : pMapNodesVisible(new Sprite *[columns * rows]), m_columns(columns), m_rows(rows)
{
  // TODO move Random Engine out of map
  randomEngine.seed();
  MapLayers::enableLayers({TERRAIN, BUILDINGS, WATER, GROUND_DECORATION, ZONE, ROAD, POWERLINES, FLORA});

  if (generateTerrain)
  {
    m_terrainGen.generateTerrain(mapNodes, mapNodesInDrawingOrder);
    m_terrainGen.generateRiver(mapNodes);
  }
  else
  {
    for (int x = 0; x < Settings::instance().mapSize; x++)
    {
      for (int y = 0; y < Settings::instance().mapSize; y++)
      {
        mapNodes.emplace_back(MapNode{Point{x, y, 0, 1}, DEFAULT_TERRAIN});
      }
    }
  }
}

Map::~Map() { delete[] pMapNodesVisible; }

void Map::renderMap() const
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("Map", "Render Map", MP_YELLOW);
#endif

  for (int i = 0; i < m_visibleNodesCount; ++i)
  {
    pMapNodesVisible[i]->render();
  }
}

void Map::refresh()
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("Map", "Refresh Map", MP_YELLOW);
#endif

  calculateVisibleMap();

  for (int i = 0; i < m_visibleNodesCount; ++i)
  {
    pMapNodesVisible[i]->refresh();
  }
}

void Map::calculateVisibleMap(void)
{
  const Point topLeft = calculateIsoCoordinates({0, 0});
  const Point bottomRight = calculateIsoCoordinates({Settings::instance().screenWidth, Settings::instance().screenHeight});

  // Screen edges
  const int left = topLeft.x + topLeft.y - 2;
  const int right = bottomRight.x + bottomRight.y + 1;
  const int top = topLeft.y - topLeft.x + 1;
  // Lower the bottom because of high terrain nodes under the screen which will be pushed into the view
  const int bottom = bottomRight.y - bottomRight.x - 1 - MapNode::maxHeight;

  m_visibleNodesCount = 0;

  // ZOrder starts from topmost node to the right. (0,127) =1,(1,127) =2, ...
  for (int y = m_columns - 1; y >= 0; y--)
  {
    for (int x = 0; x < m_rows; x++)
    {
      const int xVal = x + y;
      const int yVal = y - x;

      if ((xVal >= left) && (xVal <= right) && (yVal <= top) && (yVal >= bottom))
      {
        pMapNodesVisible[m_visibleNodesCount++] = getMapNode({x, y}).getSprite();
      }
    }
  }
}
