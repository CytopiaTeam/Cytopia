#include "Game.hxx"
#include "engine/EventManager.hxx"
#include "engine/UIManager.hxx"
#include "engine/WindowManager.hxx"
#include "engine/basics/Camera.hxx"
#include "LOG.hxx"
#include "engine/ui/widgets/Image.hxx"
#include "engine/basics/Settings.hxx"
#include "engine/basics/GameStates.hxx"
#include "Filesystem.hxx"
#include "OSystem.hxx"
#include <Map.hxx>
#include <MapFunctions.hxx>
#include "util/ThreadWorkers.hxx"
#include "scenes/Scenes.hxx"
#include "game/ThreadSceneUpdate.hxx"
#include "../game/ui/BuildMenu.hxx"

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef USE_ANGELSCRIPT
#include "scripting/ScriptEngine.hxx"
#endif

#ifdef USE_MOFILEREADER
#include "moFileReader.h"
#endif

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif

BaseSceneSelector *BaseScene::m_sceneSelector = nullptr;
Cytopia::Game *g_game = nullptr;

namespace Cytopia
{

Game::Game()
{
  g_game = this;
  LOG(LOG_DEBUG) << "Created Game Object";
  initialize();
}

Game::~Game()
{
  /* do nothing yet */
}

void Game::quit()
{
#ifdef USE_AUDIO
  auto &m_AudioMixer = AudioMixer::instance();
  m_AudioMixer.stopAll();
  if (!Settings::instance().audio3DStatus)
  {
    m_AudioMixer.play(SoundtrackID{"NegativeSelect"});
  }
  else
  {
    m_AudioMixer.play(SoundtrackID{"NegativeSelect"}, Coordinate3D{0, 0, -4});
  }
#endif // USE_AUDIO

  g_game = nullptr;
}

void Game::initialize()
{
  m_mt.initialize(/*num_workers=*/4);
  ThreadSceneUpdate::instance().initialize();

#ifdef USE_MOFILEREADER
  std::string moFilePath = fs::getBasePath();
  moFilePath = moFilePath + "languages/" + Settings::instance().gameLanguage + "/Cytopia.mo";

  if (moFileLib::moFileReaderSingleton::GetInstance().ReadFile(moFilePath.c_str()) == moFileLib::moFileReader::EC_SUCCESS)
  {
    LOG(LOG_INFO) << "Loaded MO file " << moFilePath;
  }
  else
  {
    LOG(LOG_ERROR) << "Failed to load MO file " << moFilePath;
  }
#endif

  // Register Callbacks
  SignalMediator::instance().registerCbQuitGame([this]() { m_shutDown = true; });
  SignalMediator::instance().registerCbNewGame(Signal::slot(this, &Game::newGame));
  SignalMediator::instance().registerCbLoadGame(Signal::slot(this, &Game::loadGame));

  // we need to instantiate the Map object to be able to send signals for new / load game
  MapFunctions::instance();

  LOG(LOG_DEBUG) << "Initialized Game Object";
}

void Game::run()
{
  Camera::instance().centerScreenOnMapCenter();

  struct GameSceneSelector : public BaseSceneSelector
  {
    Game &game;
    explicit GameSceneSelector(Game &g) : game(g) { BaseScene::m_sceneSelector = this; }
    void nextScene(BaseScene *scene) override { game.nextScene(scene); }
  } gameSceneSelector(*this);

  Uint32 tickLastTime = SDL_GetTicks();

  UIManager &uiManager = UIManager::instance();
  uiManager.init();

  GameClock &gameClock = GameClock::instance();
  ResourcesManager &textures = ResourcesManager::instance();
  ThreadSceneUpdate &threadScene = ThreadSceneUpdate::instance();

#ifdef USE_ANGELSCRIPT
  ScriptEngine &scriptEngine = ScriptEngine::instance();
  scriptEngine.init();
  scriptEngine.loadScript(fs::getBasePath() + "/resources/test.as", ScriptCategory::BUILD_IN);
#endif

  uiManager.addPersistentMenu<BuildMenu>();

  // GameLoop
  while (!m_shutDown)
  {
    BaseScene *scene = nullptr;
    {
      std::unique_lock lock(this->m_scenes_access);

      if (this->m_shutDown || this->m_scenes.empty())
        break;

      scene = this->m_scenes.front();
      this->m_scenes.pop();
    }

    scene->onBeforeStart();
    threadScene.setupScene(scene);
    while (scene->isActive())
    {
      if (m_shutDown)
        break;

      threadScene.nextFrame();

      SDL_Event event;
      while (SDL_PollEvent(&event) != 0) {
        scene->onEvent(event);
      }
      SDL_RenderClear(WindowManager::instance().getRenderer());

      WindowManager::instance().newImGuiFrame();

      Uint32 tickCurrentTime = SDL_GetTicks();
      scene->onRender((float)(tickCurrentTime - tickLastTime)/1000.f);

#ifdef MICROPROFILE_ENABLED
      MICROPROFILE_SCOPEI("Map", "Gameloop", MP_GREEN);
#endif
      gameClock.tick();

      // we need to instantiate the MapFunctions object so it's ready for new game
      WindowManager::instance().renderScreen();

      m_fpsFrames++;
      tickLastTime = tickCurrentTime;

      // need here, because SDL resolve textures in main thread only
      textures.performDelayedActions();
      if (!threadScene.isReady()) {
        LOG(LOG_INFO) << "Scene thread not ready yet";
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
      }
#ifdef MICROPROFILE_ENABLED
      MicroProfileFlip(nullptr);
#endif
    }
    threadScene.setupScene(nullptr);
    scene->onBeforeStop();
    scene->onDestroy();
  }
}

void Game::shutdown()
{
  ThreadSceneUpdate::instance().shutdown();
  m_mt.shutdown();

  LOG(LOG_DEBUG) << "In shutdown";
  TTF_Quit();
  SDL_Quit();
}

void Game::newGame(bool generateTerrain)
{
  MapFunctions::instance().newMap(generateTerrain);
  m_GamePlay.resetManagers();
}

void Game::loadGame(const std::string &fileName)
{
  MapFunctions::instance().loadMapFromFile(fileName);
  m_GamePlay.resetManagers();
}

} // namespace Cytopia
