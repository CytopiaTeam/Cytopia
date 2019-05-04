#ifndef MAP_LAYERS_HXX_
#define MAP_LAYERS_HXX_

#include "../common/enums.hxx"

#include <vector>

class MapLayers
{
public:
  static std::vector<Layer> getActiveLayers();

  /** \brief Enable Drawing Layer
* Enable Drawing Layer
* @param Layer enum
*/
  static void enableLayer(Layer layer);

  /** \brief Disable Drawing Layer
* Disable Drawing Layer
* @param Layer enum
*/
  static void disableLayer(Layer layer);

  /** \brief Toggle Drawing Layer
* Toggle Drawing Layer
* @param Layer enum
*/
  static void toggleLayer(Layer layer);

  static bool isLayerActive(Layer layer);

private:
  MapLayers() = default;
  ~MapLayers() = default;

  static std::vector<Layer> m_activeLayers;
};

#endif