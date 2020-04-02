#ifndef GAMESTATES_HXX_
#define GAMESTATES_HXX_

#include <string>

#include "../../util/Singleton.hxx"
#include "../common/enums.hxx"

class GameStates : public Singleton<GameStates>
{
public:
  friend Singleton<GameStates>;

  /**
   * @brief Draw UI. This is a temporary variable until the new UI has been implemented.
   * TODO Remove this function afterwards
   */
  bool drawUI = true;
  bool rectangularRoads = false;
  LayerEditMode layerEditMode = LayerEditMode::TERRAIN;
  PlacementMode placementMode = PlacementMode::LINE;

private:
  GameStates() = default;
  ~GameStates() = default;
};

#endif