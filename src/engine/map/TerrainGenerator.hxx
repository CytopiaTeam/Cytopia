#ifndef TERRAIN_GEN_HXX_
#define TERRAIN_GEN_HXX_

#include "../GameObjects/MapNode.hxx"

#include <map>
#include <vector>

struct BiomeData
{
  std::vector<std::string> terrain;         // Terrain IDs
  std::vector<std::string> water;           // Water IDs
  std::vector<std::string> waterDecoration; // WaterDecoration IDs
  std::vector<std::string> terrainRocks;    // TerrainRocks IDs
  std::vector<std::string> terrainFlowers;  // terrainFlowers IDs
  std::vector<std::string> treesDense;      // terrainFlowers IDs
  std::vector<std::string> treesSmall;      // terrainFlowers IDs
  std::vector<std::string> treesNormal;     // terrainFlowers IDs
};

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

  void loadTerrainDataFromJSON();

private:
  TerrainSettings terrainSettings;

  std::map<std::string, BiomeData> biomeInformation; // key: biome
};

#endif