#include "MapLayers.hxx"

// static variables
unsigned int MapLayers::m_activeLayers = 0;

void MapLayers::setLayerEditMode(LayerEditMode layerEditMode)
{
  {
    switch (layerEditMode)
    {
    case LayerEditMode::BLUEPRINT:
      deactivateAllLayers();
      enableLayer({BLUEPRINT, UNDERGROUND});
      break;
    case LayerEditMode::TERRAIN:
      deactivateAllLayers();
      enableLayer({TERRAIN, WATER, ROAD, ZONE, BUILDINGS});
      break;
    }
  };
}

void MapLayers::enableLayer(std::vector<unsigned int> &&layers)
{
  unsigned int enLayers = 0U;

  for (auto layer : layers)
  {
    enLayers |= 1U << layer;
  }

  m_activeLayers |= enLayers;
}
