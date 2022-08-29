#ifndef GAMESTATES_HXX_
#define GAMESTATES_HXX_

#include <string>

#include "../../util/Singleton.hxx"
#include "../common/enums.hxx"

/**
 * @brief Data struct for GameStates class
 **/
struct GameStatesData
{
  bool drawUI = true;            ///< Draw UI. This is a temporary variable until the new UI has been implemented.
  bool rectangularRoads = false; ///< place rectangular road tiles instead of diagonal tiles
  LayerEditMode layerEditMode =
      LayerEditMode::TERRAIN; ///< Specifies the Layer Editmode. Editing Terrain or Blueprint (water pipes, subway,..)
  PlacementMode placementMode = PlacementMode::LINE; ///< Specifies the placement mode when holding down the mouse
  DemolishMode demolishMode = DemolishMode::DEFAULT;
};

class GameStates : public GameStatesData, public Singleton<GameStates>
{
public:
  friend Singleton<GameStates>;

private:
  GameStates() = default;
  ~GameStates() = default;
};

#endif
