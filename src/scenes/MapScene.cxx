#include "MapScene.hxx"
#ifdef USE_AUDIO
#include "../services/AudioMixer.hxx"
#endif
#include <Settings.hxx>
#include <SignalMediator.hxx>
#include <SDL.h>
#include "../engine/UIManager.hxx"
#include <Image.hxx>
#include <Settings.hxx>
#include <OSystem.hxx>
#include "../Game.hxx"
#include "basics/Camera.hxx"
#include "map/MapFunctions.hxx"
#include "basics/GameStates.hxx"
#include <ThreadWorkers.hxx>
#include "EventManager.hxx"

void SceneMap::onBeforeStart()
{
  Camera::instance().centerScreenOnMapCenter();

  UIManager &uiManager = UIManager::instance();
  uiManager.init();

  GameClock &gameClock = GameClock::instance();

#ifdef USE_AUDIO
  if (!Settings::instance().audio3DStatus)
  {
    gameClock.addRealTimeClockTask(
      [] ()
      {
        AudioMixer::instance().play(AudioTrigger::MainTheme);
        return false;
  },
      0s, 8min);
    gameClock.addRealTimeClockTask(
      [] ()
      {
        AudioMixer::instance().play(AudioTrigger::NatureSounds);
        return false;
      },
      0s, 3min);
}
  else
  {
    gameClock.addRealTimeClockTask(
      [] ()
      {
        AudioMixer::instance().play(AudioTrigger::MainTheme, Coordinate3D{0, 0.5, 0.1});
        return false;
      },
      0s, 8min);
    gameClock.addRealTimeClockTask(
      [] ()
      {
        AudioMixer::instance().play(AudioTrigger::NatureSounds, Coordinate3D{0, 0, -2});
        return false;
      },
      0s, 3min);
  }
#endif // USE_AUDIO

  Camera::instance().centerScreenOnMapCenter();
}

void SceneMap::onRender(float dt)
{
#ifdef USE_ANGELSCRIPT
  ScriptEngine &scriptEngine = ScriptEngine::instance();
  scriptEngine.init();
#endif

  if (MapFunctions::instance().getMap())
  {
    MapFunctions::instance().getMap()->renderMap();
  }

  if (GameStates::instance().drawUI)
  {
    UIManager::instance().drawUI();
  }
}

void SceneMap::onEvent(const SDL_Event &event) {
  EventManager &evManager = EventManager::instance();
  evManager.handleEvent(event);
}

void SceneMap::onUpdate() {
  if (MapFunctions::instance().getMap())
  {
    MapFunctions::instance().getMap()->updateMap();
  }
}

void SceneLoading::onBeforeStart()
{
  int screenWidth = Settings::instance().screenWidth;
  int screenHeight = Settings::instance().screenHeight;

  loadingText = new Text;
  loadingText->setText("");
  loadingText->setPosition((screenWidth - loadingText->getUiElementRect().w) / 2, (screenHeight - loadingText->getUiElementRect().h) / 2);
}

void SceneLoading::onRender(float dt) {
  int screenWidth = Settings::instance().screenWidth;
  int screenHeight = Settings::instance().screenHeight;

#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("Map", "Wait", MP_GREEN);
#endif
  ++frames;
  if (frames % 30 == 0)
  {
    std::string buffer_text = "Loading: "+ std::to_string(frames);
    loadingText->setText(buffer_text);
    loadingText->setPosition((screenWidth - loadingText->getUiElementRect().w) / 2, (screenHeight - loadingText->getUiElementRect().h) / 2);
  }
  loadingText->draw();
  //std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void SceneLoading::onEvent(const SDL_Event &event) {

}

void SceneLoading::onBeforeStop() {
  delete loadingText;
  auto *mapLevel = new SceneMap;
  nextScene(mapLevel);
}

void SceneLoadingNew::onBeforeStart()
{
  waiting4load = true;;
  g_game->mt().execute([this] {
    SignalMediator::instance().signalNewGame.emit(true);
    waiting4load = false;
  });

  SceneLoading::onBeforeStart();
}

void SceneLoadingExist::onBeforeStart()
{
  waiting4load = true;;
  g_game->mt().execute([this] {
    SignalMediator::instance().signalLoadGame.emit("save.cts");
    waiting4load = false;
  });

  SceneLoading::onBeforeStart();
}
