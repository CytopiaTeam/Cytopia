#include "TerrainGenerator.hxx"

#include "Constants.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "JsonSerialization.hxx"
#include "Filesystem.hxx"
#include <PointFunctions.hxx>

#include "json.hxx"

#include <noise/noise.h>

#include <random>

using json = nlohmann::json;

void TerrainGenerator::generateTerrain(std::vector<MapNode> &mapNodes, std::vector<MapNode *> &mapNodesInDrawingOrder)
{
  loadTerrainDataFromJSON();

  if (m_terrainSettings.seed == 0)
  {
    srand(static_cast<unsigned int>(time(0)));
    m_terrainSettings.seed = rand();
  }

  noise::module::Perlin terrainHeightPerlin;
  terrainHeightPerlin.SetSeed(m_terrainSettings.seed);
  terrainHeightPerlin.SetFrequency(0.003 / 32);
  terrainHeightPerlin.SetLacunarity(1.5);
  terrainHeightPerlin.SetOctaveCount(16);
  noise::module::ScaleBias terrainHeightPerlinScaled;
  terrainHeightPerlinScaled.SetSourceModule(0, terrainHeightPerlin);
  terrainHeightPerlinScaled.SetScale(0.25);
  terrainHeightPerlinScaled.SetBias(-0.5);

  noise::module::RidgedMulti terrainHeightFractal;
  terrainHeightFractal.SetSeed(m_terrainSettings.seed);
  terrainHeightFractal.SetFrequency(0.005 / 32);
  terrainHeightFractal.SetLacunarity(2);
  noise::module::ScaleBias terrainHeightFractalScaled;
  terrainHeightFractalScaled.SetSourceModule(0, terrainHeightFractal);
  //terrainHeightFractalScaled.SetScale(0.5);
  terrainHeightFractalScaled.SetScale(m_terrainSettings.mountainAmplitude * 0.025);
  terrainHeightFractalScaled.SetBias(0.5);

  noise::module::Perlin terrainHeightBlendPerlin;
  terrainHeightBlendPerlin.SetSeed(m_terrainSettings.seed + 1);
  terrainHeightBlendPerlin.SetFrequency(0.005 / 32);
  noise::module::ScaleBias terrainHeightBlendScale;
  terrainHeightBlendScale.SetSourceModule(0, terrainHeightBlendPerlin);
  terrainHeightBlendScale.SetScale(2.0);
  terrainHeightBlendScale.SetBias(-0.1 * m_terrainSettings.mountainAmplitude);
  noise::module::Clamp terrainHeightBlendControl;
  terrainHeightBlendControl.SetSourceModule(0, terrainHeightBlendScale);
  terrainHeightBlendControl.SetBounds(0, 1);

  noise::module::Blend terrainHeightBlend;
  terrainHeightBlend.SetSourceModule(0, terrainHeightPerlinScaled);
  terrainHeightBlend.SetSourceModule(1, terrainHeightFractalScaled);
  terrainHeightBlend.SetControlModule(terrainHeightBlendControl);

  noise::module::ScaleBias terrainHeightScale;
  terrainHeightScale.SetSourceModule(0, terrainHeightBlend);
  terrainHeightScale.SetScale(20.0);
  terrainHeightScale.SetBias(4.0);

  noise::module::Clamp terrainHeight;
  terrainHeight.SetSourceModule(0, terrainHeightScale);
  terrainHeight.SetBounds(0, 255);

  // Foliage
  noise::module::Perlin foliageDensityPerlin;
  foliageDensityPerlin.SetSeed(m_terrainSettings.seed + 1234);
  foliageDensityPerlin.SetFrequency(0.05 / 32);

  // Arbitrary Noise
  noise::module::Perlin highFrequencyNoise;
  highFrequencyNoise.SetSeed(m_terrainSettings.seed + 42);
  highFrequencyNoise.SetFrequency(1);

  const int mapSize = m_terrainSettings.mapSize;
  const size_t vectorSize = static_cast<size_t>(mapSize * mapSize);
  mapNodes.reserve(vectorSize);

  // River random source
  std:: minstd_rand riverRand;
  riverRand.seed(m_terrainSettings.seed);

  // For now, the biome string is read from settings.json for debugging
  std::string currentBiome = Settings::instance().biome;

  // nodes need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x < mapSize; x++)
  {
    for (int y = 0; y < mapSize; y++)
    {
      const int z = 0; // it's not possible to calculate the correct z-index, so set it later in a for loop
      double rawHeight = terrainHeight.GetValue(x * 32, y * 32, 0.5);
      int height = static_cast<int>(rawHeight);

      if (height < m_terrainSettings.seaLevel)
      {
        height = m_terrainSettings.seaLevel;
        mapNodes.emplace_back(MapNode{Point{x, y, z, height, rawHeight}, m_biomeInformation[currentBiome].water[0]});
      }
      else
      {
        const double foliageDensity = foliageDensityPerlin.GetValue(x * 32, y * 32, height / 32.0);
        bool placed = false;

        if (foliageDensity > 0.0 && height > m_terrainSettings.seaLevel)
        {
          int tileIndex = static_cast<int>(std::abs(round(highFrequencyNoise.GetValue(x * 32, y * 32, height / 32.0) * 200.0)));

          if (foliageDensity < 0.1)
          {
            if (tileIndex < 20)
            {
              tileIndex = tileIndex % static_cast<int>(m_biomeInformation[currentBiome].treesLight.size());
              mapNodes.emplace_back(MapNode{Point{x, y, z, height, rawHeight}, m_biomeInformation[currentBiome].terrain[0],
                                            m_biomeInformation[currentBiome].treesLight[tileIndex]});
              placed = true;
            }
          }
          else if (foliageDensity < 0.25)
          {
            if (tileIndex < 50)
            {
              tileIndex = tileIndex % static_cast<int>(m_biomeInformation[currentBiome].treesMedium.size());
              mapNodes.emplace_back(MapNode{Point{x, y, z, height, rawHeight}, m_biomeInformation[currentBiome].terrain[0],
                                            m_biomeInformation[currentBiome].treesMedium[tileIndex]});
              placed = true;
            }
          }
          else if (foliageDensity < 1.0 && tileIndex < 95)
          {
            tileIndex = tileIndex % static_cast<int>(m_biomeInformation[currentBiome].treesDense.size());

            mapNodes.emplace_back(MapNode{Point{x, y, z, height, rawHeight}, m_biomeInformation[currentBiome].terrain[0],
                                          m_biomeInformation[currentBiome].treesDense[tileIndex]});
            placed = true;
          }
        }
        if (placed == false)
        {
          mapNodes.emplace_back(MapNode{Point{x, y, z, height, rawHeight}, m_biomeInformation[currentBiome].terrain[0]});
        }

        if (height > m_terrainSettings.seaLevel)
        {
          const int riverSourcePossibility = riverRand() % m_terrainSettings.maxHeight;
          if ((riverSourcePossibility >= (m_terrainSettings.maxHeight + m_terrainSettings.seaLevel - height)) && (riverRand() % 50 == 0))
          {
            m_riverSourceNodes.emplace_back(Point{x, y, z, height, rawHeight});
          }
        }
      }
    }
  }

  int z = 0;
  // set the z-Index for the mapNodes. It is not used, but it's better to have the correct z-index set
  for (int y = mapSize - 1; y >= 0; y--)
  {
    for (int x = 0; x < mapSize; x++)
    {
      z++;
      mapNodes[x * mapSize + y].setZIndex(z);
      mapNodesInDrawingOrder.push_back(&mapNodes[x * mapSize + y]);
    }
  }
}

void TerrainGenerator::generateRiver(std::vector<MapNode> &mapNodes)
{
  std::unordered_set<Point> riverNodes;
  std::vector<Point> neighbors;
  Point curPoint, nextPoint;
  double curHeight, nextHeight, neighborHeight;
  int riverNumber = 0;

  for (int i = 0; i < m_riverSourceNodes.size(); i++)
  {
    curPoint = m_riverSourceNodes[i];
    curHeight = mapNodes[curPoint.toIndex()].getCoordinates().rawHeight;

    while (true)
    {
      if (riverNodes.find(curPoint) != riverNodes.end())
        riverNodes.insert(curPoint);

      // Find the lowest node
      nextHeight = 32;
      neighbors = PointFunctions::getNeighbors(curPoint, false);
      for (int j = 0; j < neighbors.size(); ++j)
      {
        if (riverNodes.find(neighbors[j]) != riverNodes.end())
          continue;
        neighborHeight = mapNodes[neighbors[j].toIndex()].getCoordinates().rawHeight;
        if (neighborHeight < nextHeight)
        {
          nextHeight = neighborHeight;
          nextPoint = neighbors[j];
        }
      }

      // Add lowest nodes in the area to riverNodes
      for (int j = 0; j < neighbors.size(); ++j)
      {
        neighborHeight = mapNodes[neighbors[j].toIndex()].getCoordinates().rawHeight;
        if (neighborHeight <= nextHeight + 0.005 && (riverNodes.find(neighbors[j]) == riverNodes.end()))
        {
          riverNodes.insert(neighbors[j]);
        }
      }

      // These nodes can reach the sea, so set nodes to river
      if (curHeight < m_terrainSettings.seaLevel)
      {
        for (Point point: riverNodes)
        {
          MapNode *node = &(mapNodes[point.toIndex()]);
          node->setTileID("water", point);
        }
        riverNumber++;
        riverNodes.clear();
        break;
      }

      // Terrain slight rise is not able to stop the river
      if (nextHeight > curHeight + 0.03)
      {
        riverNodes.clear();
        break;
      }

      curPoint = nextPoint;
      curHeight = nextHeight;
    }
  }

  LOG(LOG_INFO) << __func__ << __LINE__ << "Number of rivers: " << riverNumber;
}

void TerrainGenerator::loadTerrainDataFromJSON()
{
  std::string jsonFileContent = fs::readFileAsString(TERRAINGEN_DATA_FILE_NAME);
  json biomeDataJsonObject = json::parse(jsonFileContent, nullptr, false);

  // check if json file can be parsed
  if (biomeDataJsonObject.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + string{TERRAINGEN_DATA_FILE_NAME});

  // parse biome objects
  for (const auto &it : biomeDataJsonObject.items())
  {
    m_biomeInformation[it.key()] = it.value();
  }
}
