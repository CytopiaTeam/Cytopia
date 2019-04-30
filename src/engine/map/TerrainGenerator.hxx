#include "../GameObjects/mapNode.hxx"

struct TerrainSettings
{
  int mapSize = 128;
  int seed = 1234;
  int seaLevel = 5;
};

class TerrainGenerator
{
public:
  TerrainGenerator() = default;
  ~TerrainGenerator() = default;

  typedef std::vector<MapNode *> MapNodeVector;
  typedef std::vector<std::unique_ptr<MapNode>> MapNodeUniquePtrVector;

  void generateTerrain(MapNodeUniquePtrVector &mapNodes, MapNodeVector &mapNodesInDrawingOrder);

private:
  TerrainSettings terrainSettings;
};