#ifndef MAP_LAYERS_HXX_
#define MAP_LAYERS_HXX_

#include "../common/enums.hxx"

class MapLayers
{
public:
  /** \brief Enable Drawing Layer
* Enable Drawing Layer (use bitwise OR to add layer)
* @param bitmapped uint32_t from enum "Layer"
* @see Layer
*/
  static inline void enableLayer(unsigned int layer) { m_activeLayers |= (1U << layer); };

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

private:
  MapLayers() = default;
  ~MapLayers() = default;

  static uint32_t m_activeLayers;
};

#endif