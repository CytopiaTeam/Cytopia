#include "MapLayers.hxx"

#include <algorithm>

// static variables
std::vector<Layer> MapLayers::m_activeLayers;

std::vector<Layer> MapLayers::getActiveLayers() { return m_activeLayers; }

bool MapLayers::isLayerActive(Layer layer)
{
  return std::find(m_activeLayers.begin(), m_activeLayers.end(), layer) != m_activeLayers.end();
}

void MapLayers::enableLayer(Layer layer)
{
  if (std::find(m_activeLayers.begin(), m_activeLayers.end(), layer) == m_activeLayers.end())
  {
    m_activeLayers.push_back(layer);
  }
}

void MapLayers::disableLayer(Layer layer)
{
  if (std::find(m_activeLayers.begin(), m_activeLayers.end(), layer) != m_activeLayers.end())
  {
    m_activeLayers.erase(std::remove(m_activeLayers.begin(), m_activeLayers.end(), layer), m_activeLayers.end());
  }
}

void MapLayers::toggleLayer(Layer layer)
{
  if (std::find(m_activeLayers.begin(), m_activeLayers.end(), layer) != m_activeLayers.end())
  {
    m_activeLayers.erase(std::remove(m_activeLayers.begin(), m_activeLayers.end(), layer), m_activeLayers.end());
  }
  else
  {
    m_activeLayers.push_back(layer);
  }
}