#include "TerrainGenerator.hxx"

#include "../common/Constants.hxx"
#include "../basics/log.hxx"

#include "../../ThirdParty/json.hxx"
#include <noise.h>

using json = nlohmann::json;

void TerrainGenerator::generateTerrain(MapNodeUniquePtrVector &mapNodes, MapNodeVector &mapNodesInDrawingOrder)
{
  loadTerrainDataFromJSON();

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
  terrainHeightFractalScaled.SetScale(0.5);
  terrainHeightFractalScaled.SetBias(0.5);

  noise::module::Perlin terrainHeightBlendPerlin;
  terrainHeightBlendPerlin.SetSeed(terrainSettings.seed + 1);
  terrainHeightBlendPerlin.SetFrequency(0.005 / 32);
  noise::module::ScaleBias terrainHeightBlendScale;
  terrainHeightBlendScale.SetSourceModule(0, terrainHeightBlendPerlin);
  terrainHeightBlendScale.SetScale(2.0);
  terrainHeightBlendScale.SetBias(-0.5);
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

  size_t vectorSize = static_cast<size_t>(terrainSettings.mapSize * terrainSettings.mapSize);
  mapNodes.resize(vectorSize);
  int z = 0;
  int height = 0;
  // nodes need to be created at the correct vector "coordinates", or else the Z-Order will be broken
  for (int x = 0; x < terrainSettings.mapSize; x++)
  {
    for (int y = terrainSettings.mapSize - 1; y >= 0; y--)
    {
      height = static_cast<int>(terrainHeight.GetValue(x * 32, y * 32, 0.5));

      if (height < terrainSettings.seaLevel)
      {
        height = terrainSettings.seaLevel;
        mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(Point{x, y, z++, height}, "water");
      }
      else
      {
        double foliageDensity = foliageDensityPerlin.GetValue(x * 32, y * 32, height / 32.0);
        //std::cout << foliageDensity << "\n";
        if (foliageDensity < 0.0)
        {
          mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(Point{x, y, z++, height}, "terrain");
        }
        else
        {
          int tileIndex = (int)abs(round(highFrequencyNoise.GetValue(x * 32, y * 32, height / 32.0) * 100.0));

          if (foliageDensity < 0.1)
          {
            while (tileIndex >= static_cast<int>(biomeInformation["GrassLands"].treesSmall.size()))
            {
              tileIndex -= (int)biomeInformation["GrassLands"].treesSmall.size();
            }
            mapNodes[x * terrainSettings.mapSize + y] =
                std::make_unique<MapNode>(Point{x, y, z++, height}, biomeInformation["GrassLands"].treesSmall[tileIndex]);
          }
          else if (foliageDensity < 0.25)
          {
            while (tileIndex >= static_cast < int >( biomeInformation["GrassLands"].treesNormal.size()))
            {
              tileIndex -= (int)biomeInformation["GrassLands"].treesNormal.size();
            }
            mapNodes[x * terrainSettings.mapSize + y] =
                std::make_unique<MapNode>(Point{x, y, z++, height}, biomeInformation["GrassLands"].treesNormal[tileIndex]);
          }
          else
          {
            while (tileIndex >= static_cast < int >( biomeInformation["GrassLands"].treesDense.size()))
            {
              tileIndex -= (int)biomeInformation["GrassLands"].treesDense.size();
            }
            //std::cout << tileIndex << "\t";
            mapNodes[x * terrainSettings.mapSize + y] =
                std::make_unique<MapNode>(Point{x, y, z++, height}, biomeInformation["GrassLands"].treesDense[tileIndex]);
          }
        }
      }
      mapNodesInDrawingOrder.push_back(mapNodes[x * terrainSettings.mapSize + y].get());
    }
  }
}

void TerrainGenerator::loadTerrainDataFromJSON()
{
  json _biomeDataJSONObject;

  std::string terrainGenDataFileName = SDL_GetBasePath();
  terrainGenDataFileName.append(TERRAINGEN_DATA_FILE_NAME);
  std::ifstream i(terrainGenDataFileName);

  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << TERRAINGEN_DATA_FILE_NAME << " does not exist! Cannot load settings from INI File!";
    return;
  }

  // check if json file can be parsed
  _biomeDataJSONObject = json::parse(i, nullptr, false);
  if (_biomeDataJSONObject.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << TERRAINGEN_DATA_FILE_NAME;
  }
  // parse biome objects
  for (const auto &it : _biomeDataJSONObject.items())
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