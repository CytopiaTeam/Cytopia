#include "TerrainGenerator.hxx"
#include <noise.h>

void TerrainGenerator::generateTerrain(MapNodeUniquePtrVector &mapNodes, MapNodeVector &mapNodesInDrawingOrder)
{
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

  //noise::module::Const terrainHeightBlendConstant;
  //terrainHeightBlendConstant.SetConstValue(0.5);

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
        mapNodes[x * terrainSettings.mapSize + y] = std::make_unique<MapNode>(Point{x, y, z++, height});
      }
      mapNodesInDrawingOrder.push_back(mapNodes[x * terrainSettings.mapSize + y].get());
    }
  }
}
