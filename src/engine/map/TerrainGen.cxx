#include "TerrainGenerator.hxx"

void TerrainGenerator::generateTerrain()
{

  // nodes need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x < terrainSettings.mapSize; x++)
  {
    for (int y = Settings::instance().settings.mapSize - 1; y >= 0; y--)
    {
      mapNodes[x * m_columns + y] = std::make_unique<MapNode>(Point{x, y, z++, 0});
      mapNodesInDrawingOrder.push_back(mapNodes[x * m_columns + y].get());
    }
  }
}