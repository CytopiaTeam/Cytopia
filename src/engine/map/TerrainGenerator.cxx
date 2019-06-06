#include "TerrainGenerator.hxx"

#include "../common/Constants.hxx"
#include "../basics/LOG.hxx"

#include "../../ThirdParty/json.hxx"
#include <noise.h>

using json = nlohmann::json;

void TerrainGenerator::generateTerrain(MapNodeUniquePtrVector &mapNodes, MapNodeVector &mapNodesInDrawingOrder)
{
  loadTerrainDataFromJSON();

  if (terrainSettings.seed == 0)
  {
    srand(static_cast<unsigned int>(time(0)));
    terrainSettings.seed = rand();
    //std::cout << "Seed is now " << terrainSettings.seed << "\n";
  }

  noise::module::Perlin terrainHeightPerlin;
  terrainHeightPerlin.SetSeed(terrainSettings.seed);
  terrainHeightPerlin.SetFrequency(0.003 / 32);
  terrainHeightPerlin.SetLacunarity(1.5);
  terrainHeightPerlin.SetOctaveCount(16);
  noise::module::ScaleBias terrainHeightPerlinScaled;
  terrainHeightPerlinScaled.SetSourceModule(0, terrainHeightPerlin);
  terrainHeightPerlinScaled.SetScale(0.25);
  terrainHeightPerlinScaled.SetBias(-0.5);

  noise::module::RidgedMulti terrainHeightFractal;
  terrainHeightFractal.SetSeed(terrainSettings.seed);
  terrainHeightFractal.SetFrequency(0.005 / 32);
  terrainHeightFractal.SetLacunarity(2);
  noise::module::ScaleBias terrainHeightFractalScaled;
  terrainHeightFractalScaled.SetSourceModule(0, terrainHeightFractal);
  //terrainHeightFractalScaled.SetScale(0.5);
  terrainHeightFractalScaled.SetScale(terrainSettings.mountainAmplitude * 0.025);
  terrainHeightFractalScaled.SetBias(0.5);

  noise::module::Perlin terrainHeightBlendPerlin;
  terrainHeightBlendPerlin.SetSeed(terrainSettings.seed + 1);
  terrainHeightBlendPerlin.SetFrequency(0.005 / 32);
  noise::module::ScaleBias terrainHeightBlendScale;
  terrainHeightBlendScale.SetSourceModule(0, terrainHeightBlendPerlin);
  terrainHeightBlendScale.SetScale(2.0);
  terrainHeightBlendScale.SetBias(-0.1 * terrainSettings.mountainAmplitude);
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
  foliageDensityPerlin.SetSeed(terrainSettings.seed + 1234);
  foliageDensityPerlin.SetFrequency(0.05 / 32);

  // Arbitrary Noise
  noise::module::Perlin highFrequencyNoise;
  highFrequencyNoise.SetSeed(terrainSettings.seed + 42);
  highFrequencyNoise.SetFrequency(1);

  const size_t vectorSize = static_cast<size_t>(terrainSettings.mapSize * terrainSettings.mapSize);
  mapNodes.resize(vectorSize);
  int z = 0;

  std::string currentBiome = "GrassLands";

  // nodes need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x < terrainSettings.mapSize; x++)
  {
    for (int y = terrainSettings.mapSize - 1; y >= 0; y--)
    {
      double rawHeight = terrainHeight.GetValue(x * 32, y * 32, 0.5);
      int height = static_cast<int>(rawHeight);

      if (height < terrainSettings.seaLevel)
      {
        height = terrainSettings.seaLevel;
        mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(Point{x, y, z++, height}, "water");
      }
      else
      {
        const double foliageDensity = foliageDensityPerlin.GetValue(x * 32, y * 32, height / 32.0);
        bool placed = false;

        if (foliageDensity > 0.0 && height > terrainSettings.seaLevel)
        {
          int tileIndex = static_cast<int>(std::abs(round(highFrequencyNoise.GetValue(x * 32, y * 32, height / 32.0) * 200.0)));

          if (foliageDensity < 0.1)
          {
            if (tileIndex < 20)
            {
              tileIndex = tileIndex % static_cast<int>(biomeInformation[currentBiome].treesSmall.size());
              mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(
                  Point{x, y, z++, height}, "terrain", biomeInformation[currentBiome].treesSmall[tileIndex]);
              placed = true;
            }
          }
          else if (foliageDensity < 0.25)
          {
            if (tileIndex < 50)
            {
              tileIndex = tileIndex % static_cast<int>(biomeInformation[currentBiome].treesNormal.size());
              mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(
                  Point{x, y, z++, height}, "terrain", biomeInformation[currentBiome].treesNormal[tileIndex]);
              placed = true;
            }
          }
          else if (foliageDensity < 1.0)
          {
            if (tileIndex < 95)
            {
              tileIndex = tileIndex % static_cast<int>(biomeInformation[currentBiome].treesDense.size());

              mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(
                  Point{x, y, z++, height}, "terrain", biomeInformation[currentBiome].treesDense[tileIndex]);
              placed = true;
            }
          }
        }
        if (placed == false)
        {
          mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(Point{x, y, z++, height}, "terrain");
        }
      }
      mapNodesInDrawingOrder.push_back(mapNodes[x * terrainSettings.mapSize + y].get());
    }
  }
}

void TerrainGenerator::loadTerrainDataFromJSON()
{
  std::string terrainGenDataFileName = SDL_GetBasePath();
  terrainGenDataFileName.append(TERRAINGEN_DATA_FILE_NAME);
  std::ifstream i(terrainGenDataFileName);

  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << TERRAINGEN_DATA_FILE_NAME << " does not exist! Cannot load settings from INI File!";
    return;
  }

  // check if json file can be parsed
  json biomeDataJsonObject = json::parse(i, nullptr, false);
  if (biomeDataJsonObject.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << TERRAINGEN_DATA_FILE_NAME;
  }
  // parse biome objects
  for (const auto &it : biomeDataJsonObject.items())
  {
    biomeInformation[it.key()] = it.value();
  }
}

// JSON deserializer for Terrain Gen Data
void from_json(const json &j, BiomeData &b)
{
  if (j.find("trees") != j.end())
  {
    for (const auto &it : j["trees"].items())
    {
      if (it.key() == "small")
      {
        std::vector<std::string> temp = it.value();
        b.treesSmall = temp;
      }
      if (it.key() == "normal")
      {
        std::vector<std::string> temp = it.value();
        b.treesNormal = temp;
      }
      if (it.key() == "dense")
      {
        std::vector<std::string> temp = it.value();
        b.treesDense = temp;
      }
    }
  }

  if (j.find("terrain") != j.end())
  {
    std::vector<std::string> temp = j["terrain"];
    b.terrain = temp;
  }
  if (j.find("water") != j.end())
  {
    std::vector<std::string> temp = j["water"];
    b.water = temp;
  }
  if (j.find("waterdecoration") != j.end())
  {
    std::vector<std::string> temp = j["waterdecoration"];
    b.waterDecoration = temp;
  }
  if (j.find("terrainRocks") != j.end())
  {
    std::vector<std::string> temp = j["terrainRocks"];
    b.terrainRocks = temp;
  }
  if (j.find("terrainFlowers") != j.end())
  {
    std::vector<std::string> temp = j["terrainFlowers"];
    b.terrainFlowers = temp;
  }
}