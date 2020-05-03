#include "TerrainGenerator.hxx"

#include "Constants.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "JsonSerialization.hxx"

#include "json.hxx"
#include <noise.h>

using json = nlohmann::json;

void TerrainGenerator::generateTerrain(MapNodeUniquePtrVector &mapNodes, MapNodeVector &mapNodesInDrawingOrder)
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

  const size_t vectorSize = static_cast<size_t>(m_terrainSettings.mapSize * m_terrainSettings.mapSize);
  mapNodes.resize(vectorSize);
  int z = 0;

  // For now, the biome string is read from settings.json for debugging
  std::string currentBiome = Settings::instance().biome;

  // nodes need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x < m_terrainSettings.mapSize; x++)
  {
    for (int y = m_terrainSettings.mapSize - 1; y >= 0; y--)
    {
      double rawHeight = terrainHeight.GetValue(x * 32, y * 32, 0.5);
      int height = static_cast<int>(rawHeight);

      if (height < m_terrainSettings.seaLevel)
      {
        height = m_terrainSettings.seaLevel;
        mapNodes[x * m_terrainSettings.mapSize + y] =
            std::make_unique<MapNode>(Point{x, y, z++, height}, m_biomeInformation[currentBiome].water[0]);
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
              mapNodes[x * m_terrainSettings.mapSize + y] =
                  std::make_unique<MapNode>(Point{x, y, z++, height}, m_biomeInformation[currentBiome].terrain[0],
                                            m_biomeInformation[currentBiome].treesLight[tileIndex]);
              placed = true;
            }
          }
          else if (foliageDensity < 0.25)
          {
            if (tileIndex < 50)
            {
              tileIndex = tileIndex % static_cast<int>(m_biomeInformation[currentBiome].treesMedium.size());
              mapNodes[x * m_terrainSettings.mapSize + y] =
                  std::make_unique<MapNode>(Point{x, y, z++, height}, m_biomeInformation[currentBiome].terrain[0],
                                            m_biomeInformation[currentBiome].treesMedium[tileIndex]);
              placed = true;
            }
          }
          else if (foliageDensity < 1.0 && tileIndex < 95)
          {
            tileIndex = tileIndex % static_cast<int>(m_biomeInformation[currentBiome].treesDense.size());

            mapNodes[x * m_terrainSettings.mapSize + y] =
                std::make_unique<MapNode>(Point{x, y, z++, height}, m_biomeInformation[currentBiome].terrain[0],
                                          m_biomeInformation[currentBiome].treesDense[tileIndex]);
            placed = true;
          }
        }
        if (placed == false)
        {
          mapNodes[x * m_terrainSettings.mapSize + y] =
              std::make_unique<MapNode>(Point{x, y, z++, height}, m_biomeInformation[currentBiome].terrain[0]);
        }
      }
      mapNodesInDrawingOrder.push_back(mapNodes[x * m_terrainSettings.mapSize + y].get());
    }
  }
}

void TerrainGenerator::loadTerrainDataFromJSON()
{
  std::string terrainGenDataFileName = fs::getBasePath();
  terrainGenDataFileName.append(TERRAINGEN_DATA_FILE_NAME);
  std::ifstream i(terrainGenDataFileName);

  if (!i)
    throw ConfigurationError(TRACE_INFO "Could not open file " + string{TERRAINGEN_DATA_FILE_NAME});

  // check if json file can be parsed
  json biomeDataJsonObject = json::parse(i, nullptr, false);
  if (biomeDataJsonObject.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + string{TERRAINGEN_DATA_FILE_NAME});
  // parse biome objects
  for (const auto &it : biomeDataJsonObject.items())
  {
    m_biomeInformation[it.key()] = it.value();
  }
}
