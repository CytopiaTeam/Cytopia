#include "../GameObjects/mapNode.hxx"

struct TerrainSettings
{
  int mapSize = 128;
};

class TerrainGenerator
{
public:
  TerrainGenerator() = default;
  ~TerrainGenerator() = default;

  std::vector<std::unique_ptr<MapNode>> &generateTerrain();

private:
  TerrainSettings terrainSettings;

  std::vector<std::unique_ptr<MapNode>> mapNodes;
  std::vector<MapNode *> mapNodesInDrawingOrder;
};