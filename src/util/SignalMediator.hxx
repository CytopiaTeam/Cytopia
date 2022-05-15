#ifndef SIGNALMEDIATOR_HXX_
#define SIGNALMEDIATOR_HXX_

#include <Singleton.hxx>
#include <Signal.hxx>
#include "../game/PowerGrid.hxx"

/**
 * @class SignalMediator
 * @brief Create signals and register callbacks here
 */

class SignalMediator : public Singleton<SignalMediator>
{
public:
  SignalMediator(){};
  ~SignalMediator(){};

  // Signals Map
  Signal::Signal<void(const MapNode &)> signalSetTileID;
  Signal::Signal<void(MapNode *)> signalDemolish;

  // Signals Gameplay Managers
  Signal::Signal<void(const std::vector<PowerGrid> &)> signalUpdatePower;

  // Callback functions Map
  void registerCbSetTileID(std::function<void(const MapNode &)> const &cb) { signalSetTileID.connect(cb); }
  void registerCbDemolish(std::function<void(MapNode *)> const &cb) { signalDemolish.connect(cb); }

  // Callback functions Gameplay Managers to interact with each other
  void registerCbUpdatePower(std::function<void(const std::vector<PowerGrid> &)> const &cb) { signalUpdatePower.connect(cb); }

};

#endif