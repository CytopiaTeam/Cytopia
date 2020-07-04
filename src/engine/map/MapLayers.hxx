#ifndef MAP_LAYERS_HXX_
#define MAP_LAYERS_HXX_

#include "../common/enums.hxx"
#include <vector>

class MapLayers
{
public:
  /** \brief Enable drawing layers.
* @param layers Layers to be enabled.
* @see Layer
*/
  static void enableLayers(std::vector<unsigned int> &&layers);

  /** \brief Disable Drawing Layer
* Disable Drawing Layer ( Turned off by using bitwise AND with inversed pattern)
* @param bitmapped uint32_t from enum "Layer"
* @see Layer
*/
  static inline void disableLayer(unsigned int layer) { m_activeLayers &= ~(1U << layer); };

  /** \brief Toggle Drawing Layer
* Toggle Drawing Layer (use bitwise XOR to toggle layer)
* @param bitmapped uint32_t from enum "Layer"
* @see Layer
*/
  static inline void toggleLayer(unsigned int layer) { m_activeLayers ^= (1U << layer); };

  /** \brief Check if given Layer is being drawn
* @param bitmapped uint32_t from enum "Layer"
* @see Layer
*/
  static inline bool isLayerActive(unsigned int layer) { return (m_activeLayers & (1U << layer)); };

  /** \brief Toggle Drawing Layer
* Toggle Drawing Layer (use bitwise XOR to toggle layer)
* @param bitmapped uint32_t from enum "Layer"
* @see Layer
*/
  static void setLayerEditMode(LayerEditMode layerEditMode);

private:
  MapLayers() = default;
  ~MapLayers() = default;

  static unsigned int m_activeLayers;

  /** \brief Toggle Drawing Layer
* Toggle Drawing Layer (use bitwise XOR to toggle layer)
* @param bitmapped uint32_t from enum "Layer"
* @see Layer
*/
  static inline void deactivateAllLayers()
  {
    for (auto layer : allLayersOrdered)
    {
      disableLayer(layer);
    }
  };
};

#endif
