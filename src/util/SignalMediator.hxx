#ifndef SIGNALMEDIATOR_HXX_
#define SIGNALMEDIATOR_HXX_

#include <Singleton.hxx>
#include <Signal.hxx>
#include <MapNode.hxx>
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

  // GameSignals
  Signal::Signal<void(bool)> signalNewGame;
  Signal::Signal<void()> signalQuitGame;
  Signal::Signal<void(const std::string &)> signalLoadGame;
  Signal::Signal<void(const std::string &)> signalSaveGame;

  // Signals Map
  Signal::Signal<void(const MapNode &)> signalSetTileID;
  Signal::Signal<void(MapNode *)> signalDemolish;

  // Signals Gameplay Managers
  Signal::Signal<void(const std::vector<PowerGrid> &)> signalUpdatePower;

  // Callback functions Game
  void registerCbNewGame(std::function<void(bool)> const &cb) { signalNewGame.connect(cb); }
  void registerCbQuitGame(std::function<void()> const &cb) { signalQuitGame.connect(cb); }
  void registerCbLoadGame(std::function<void(const std::string &)> const &cb) { signalLoadGame.connect(cb); }
  void registerCbSaveGame(std::function<void(const std::string &)> const &cb) { signalSaveGame.connect(cb); }

  // Callback functions Map
  void registerCbSetTileID(std::function<void(const MapNode &)> const &cb) { signalSetTileID.connect(cb); }
  void registerCbDemolish(std::function<void(MapNode *)> const &cb) { signalDemolish.connect(cb); }

  // Callback functions Gameplay Managers to interact with each other
  void registerCbUpdatePower(std::function<void(const std::vector<PowerGrid> &)> const &cb) { signalUpdatePower.connect(cb); }
};

#endif