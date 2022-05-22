#include "MapFunctions.hxx"
#include <PointFunctions.hxx>

#include <set>
#include <queue>

void MapFunctions::increaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, true); }

void MapFunctions::decreaseHeight(const Point &isoCoordinates) { changeHeight(isoCoordinates, false); }

bool MapFunctions::updateHeight(Point coordinate, const bool elevate)
{
  if (getMapNode(coordinate).changeHeight(elevate))
  {
    for (const auto neighbor : PointFunctions::getNeighbors(coordinate, false))
    {
      if (getMapNode(neighbor).isLayerOccupied(Layer::ZONE))
      {
        getMapNode(neighbor).demolishLayer(Layer::ZONE);
      }
    }

    return true;
  }

  return false;
}

void MapFunctions::changeHeight(const Point &isoCoordinates, const bool elevate)
{
  std::vector<Point> nodesToUpdate{isoCoordinates};
  std::vector<Point> neighorCoordinates = PointFunctions::getNeighbors(isoCoordinates, true);

  if (updateHeight(isoCoordinates, elevate))
  {
    // If lowering node height, than all nodes around should be lowered to be on same height with the central one.
    if (!elevate)
    {
      const int centerHeight = getMapNode(isoCoordinates).getCoordinates().height;

      for (Point &neighborCoord : neighorCoordinates)
      {
        MapNode &neighborNode = getMapNode(neighborCoord);
        if (centerHeight < neighborNode.getCoordinates().height)
        {
          neighborNode.changeHeight(false);
          nodesToUpdate.push_back(neighborNode.getCoordinates());
        }
      }
    }
    m_map->demolishNode(neighorCoordinates);
    m_map->updateNodeNeighbors(nodesToUpdate);
  }
}

void MapFunctions::updateNodeNeighbors(std::vector<Point> nodes)
{
  // those bitmask combinations require the tile to be elevated.
  constexpr unsigned char elevateTileComb[] = {
      NeighborNodesPosition::TOP | NeighborNodesPosition::BOTTOM,
      NeighborNodesPosition::LEFT | NeighborNodesPosition::RIGHT,
      NeighborNodesPosition::TOP_LEFT | NeighborNodesPosition::RIGHT | NeighborNodesPosition::BOTTOM,
      NeighborNodesPosition::TOP_RIGHT | NeighborNodesPosition::LEFT | NeighborNodesPosition::BOTTOM,
      NeighborNodesPosition::BOTTOM_LEFT | NeighborNodesPosition::RIGHT | NeighborNodesPosition::TOP,
      NeighborNodesPosition::BOTTOM_RIGHT | NeighborNodesPosition::LEFT | NeighborNodesPosition::TOP};

  std::vector<Point> nodesToBeUpdated;
  std::map<int, std::vector<Point>> nodeCache;
  std::queue<Point> updatedNodes;
  std::vector<Point> nodesToElevate;
  std::vector<Point> nodesToDemolish;

  for (auto currentNode : nodes)
  {
    updatedNodes.push(currentNode);

    while (!updatedNodes.empty() || !nodesToElevate.empty())
    {
      while (!updatedNodes.empty())
      {
        const Point pHeighChangedNode = getMapNode(updatedNodes.front()).getCoordinates();
        updatedNodes.pop();
        const int tileHeight = pHeighChangedNode.height;

        if (nodeCache.count(pHeighChangedNode.toIndex()) == 0)
        {
          nodeCache[pHeighChangedNode.toIndex()] = PointFunctions::getNeighbors(pHeighChangedNode, false);
        }

        if (std::find(nodesToElevate.begin(), nodesToElevate.end(), pHeighChangedNode) == nodesToElevate.end())
        {
          nodesToElevate.push_back(pHeighChangedNode);
        }

        for (auto neighbor : nodeCache[pHeighChangedNode.toIndex()])
        {
          // get real coords that include height
          const Point neighborCoords = getMapNode(neighbor).getCoordinates();
          const int heightDiff = tileHeight - neighborCoords.height;

          if (nodeCache.count(neighborCoords.toIndex()) == 0)
          {
            nodeCache[neighborCoords.toIndex()] = PointFunctions::getNeighbors(neighborCoords, false);
          }

          if (std::find(nodesToElevate.begin(), nodesToElevate.end(), neighborCoords) == nodesToElevate.end())
          {
            nodesToElevate.push_back(neighborCoords);
          }

          if (std::abs(heightDiff) > 1)
          {
            updatedNodes.push(getMapNode(neighborCoords).getCoordinates());
            updateHeight(neighborCoords, (heightDiff > 1) ? true : false);
          }
        }
      }

      while (updatedNodes.empty() && !nodesToElevate.empty())
      {
        Point nodeToElevate = nodesToElevate.back();
        nodesToBeUpdated.push_back(nodeToElevate);
        nodesToElevate.pop_back();

        if (nodeCache.count(nodeToElevate.toIndex()) == 0)
        {
          nodeCache[nodeToElevate.toIndex()] = PointFunctions::getNeighbors(nodeToElevate, false);
        }
        const unsigned char elevationBitmask = m_map->getElevatedNeighborBitmask(nodeToElevate);

        if (elevationBitmask != getMapNode(nodeToElevate).getElevationBitmask())
        {
          nodesToDemolish.push_back(nodeToElevate);
          getMapNode(nodeToElevate).setElevationBitMask(elevationBitmask);
        }

        for (const auto &elBitMask : elevateTileComb)
        {
          if ((elevationBitmask & elBitMask) == elBitMask)
          {
            updateHeight(nodeToElevate, true);
            updatedNodes.push(getMapNode(nodeToElevate).getCoordinates());
            break;
          }
        }
      }
    }
  }

  if (!nodesToDemolish.empty())
  {
    m_map->demolishNode(nodesToDemolish);
  }

  for (Point node : nodesToBeUpdated)
  {
    getMapNode(node).setAutotileBitMask(m_map->calculateAutotileBitmask(node));
  }

  for (Point node : nodesToBeUpdated)
  {
    getMapNode(node).updateTexture();
  }
}

void MapFunctions::updateAllNodes()
{
  std::vector<Point> allCoords(m_map->mapNodes.size());
  std::transform(m_map->mapNodes.begin(), m_map->mapNodes.end(), allCoords.begin(),
                 [](MapNode &mn) { return mn.getCoordinates(); });
  updateNodeNeighbors(allCoords);
}
