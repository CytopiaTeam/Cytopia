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
      enableLayer(Layer::BLUEPRINT);
      enableLayer(Layer::UNDERGROUND);
      break;
    case LayerEditMode::TERRAIN:
      deactivateAllLayers();
      enableLayer(Layer::TERRAIN);
      enableLayer(Layer::WATER);
      enableLayer(Layer::ROAD);
      enableLayer(Layer::ZONE);
      enableLayer(Layer::BUILDINGS);
      break;
    }
  };
}