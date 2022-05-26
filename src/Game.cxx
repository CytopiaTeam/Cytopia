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
#include <Map.hxx>
#include <MapFunctions.hxx>

#include <SDL.h>
#include <SDL_ttf.h>

#ifdef USE_ANGELSCRIPT
#include "Scripting/ScriptEngine.hxx"
#endif

#ifdef USE_MOFILEREADER
#include "moFileReader.h"
#endif

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif

namespace Cytopia
{
Game::Game() { LOG(LOG_DEBUG) << "Created Game Object"; }

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
}

bool Game::initialize(const char *videoDriver)
{
  if (SDL_Init(0) != 0)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL";
    LOG(LOG_ERROR) << "SDL Error: " << SDL_GetError();
    return false;
  }

  if (SDL_VideoInit(videoDriver) != 0)
  {
    LOG(LOG_ERROR) << "Unknown video driver " << videoDriver;
    int nbDriver = SDL_GetNumRenderDrivers();
    for (int i = 0; i < nbDriver; i++)
    {
      SDL_RendererInfo info;
      SDL_GetRenderDriverInfo(i, &info);
      LOG(LOG_ERROR) << "Found driver " << i << ": " << (info.name ? info.name : "Invalid driver")
                     << " with flags=" << info.flags;
    }
    return false;
  }

  if (TTF_Init() == -1)
  {
    LOG(LOG_ERROR) << "Failed to Init SDL_TTF";
    LOG(LOG_ERROR) << "SDL Error: " << TTF_GetError();
    return false;
  }

  // initialize window manager
  WindowManager::instance().setWindowTitle(VERSION);

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

  LOG(LOG_DEBUG) << "Initialized Game Object";
  return true;
}

#ifdef USE_AUDIO
static void playAudioMajorSelection()
{
  auto &m_AudioMixer = AudioMixer::instance();
  m_AudioMixer.stopAll();
  if (!Settings::instance().audio3DStatus)
    m_AudioMixer.play(SoundtrackID{"MajorSelection"});
  else
    m_AudioMixer.play(SoundtrackID{"MajorSelection"}, Coordinate3D{0, 0, -4});
}
#endif // USE_AUDIO

bool Game::mainMenu()
{
  SDL_Event event;

  int screenWidth = Settings::instance().screenWidth;
  int screenHeight = Settings::instance().screenHeight;
  bool mainMenuLoop = true;
  bool quitGame = false;

#ifdef USE_AUDIO
  auto &m_AudioMixer = AudioMixer::instance();
  /* Trigger MainMenu music */
  m_AudioMixer.setMusicVolume(Settings::instance().musicVolume);
  m_AudioMixer.setSoundEffectVolume(Settings::instance().soundEffectsVolume); // does nothing right now
  if (!Settings::instance().audio3DStatus)
    m_AudioMixer.play(AudioTrigger::MainMenu);
  else
    m_AudioMixer.play(AudioTrigger::MainMenu, Coordinate3D{0, 3, 0.5});
#endif // USE_AUDIO

  Image logo;
  logo.setTextureID("Cytopia_Logo");
  logo.setVisibility(true);
  logo.setPosition(screenWidth / 2 - logo.getUiElementRect().w / 2, screenHeight / 4 - logo.getUiElementRect().h / 2);

  Text versionText;
  versionText.setText(VERSION);
  versionText.setPosition(screenWidth - versionText.getUiElementRect().w, screenHeight - versionText.getUiElementRect().h);

  Button newGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20, 200, 40});
  newGameButton.setText("New Game");
  newGameButton.setUIElementID("newgame");
  newGameButton.registerCallbackFunction(
      []()
      {
#ifdef USE_AUDIO
        playAudioMajorSelection();
#endif //  USE_AUDIO                                                                                                             \
    // TODO: Game.run??
      });

  Button loadGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20 + newGameButton.getUiElementRect().h * 2, 200, 40});
  loadGameButton.setText("Load Game");
  loadGameButton.registerCallbackFunction(
      []()
      {
#ifdef USE_AUDIO
        playAudioMajorSelection();
#endif // USE_AUDIO

        //TODO: This will need more refactoring to work. Split main menu into a seperate class. ##945
      });

  Button quitGameButton({screenWidth / 2 - 100, screenHeight / 2 - 20 + loadGameButton.getUiElementRect().h * 4, 200, 40});
  quitGameButton.setText("Quit Game");
  quitGameButton.registerCallbackFunction(
      [this, &quitGame]()
      {
        quit();
        quitGame = true;
      });

  // store elements in vector
  std::vector<UIElement *> uiElements;
  uiElements.push_back(&newGameButton);
  uiElements.push_back(&loadGameButton);
  uiElements.push_back(&quitGameButton);
  uiElements.push_back(&logo);
  uiElements.push_back(&versionText);

  UIElement *m_lastHoveredElement = nullptr;

  // fade in Logo
  for (Uint8 opacity = 0; opacity < 255; opacity++)
  {
    // break the loop if an event occurs
    if (SDL_PollEvent(&event) && (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN))
    {
      logo.setOpacity(SDL_ALPHA_OPAQUE);
      break;
    }
    SDL_RenderClear(WindowManager::instance().getRenderer());
    logo.setOpacity(opacity);

    for (const auto &element : uiElements)
    {
      element->draw();
    }

    // reset renderer color back to black
    SDL_SetRenderDrawColor(WindowManager::instance().getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(WindowManager::instance().getRenderer());
    SDL_Delay(5);
  }

  while (mainMenuLoop)
  {
    SDL_RenderClear(WindowManager::instance().getRenderer());

    while (SDL_PollEvent(&event) != 0)
    {
      for (const auto &it : uiElements)
      {
        switch (event.type)
        {
        case SDL_QUIT:
          quit();
          return true;
        case SDL_MOUSEBUTTONDOWN:
          it->onMouseButtonDown(event);
          break;
        case SDL_MOUSEBUTTONUP:

          if (it->onMouseButtonUp(event))
          {
            mainMenuLoop = false;
          }
          break;
        case SDL_MOUSEMOTION:
          it->onMouseMove(event);

          // if the mouse cursor left an element, we're not hovering any more and we need to reset the pointer to the last hovered
          if (m_lastHoveredElement && !m_lastHoveredElement->isMouseOverHoverableArea(event.button.x, event.button.y))
          {
            m_lastHoveredElement->onMouseLeave(event);
            m_lastHoveredElement = nullptr;
          }

          // if the element we're hovering over is not the same as the stored "lastHoveredElement", update it
          if (it->isMouseOverHoverableArea(event.button.x, event.button.y) && it != m_lastHoveredElement)
          {
            it->onMouseMove(event);

            if (m_lastHoveredElement != nullptr)
            {
              m_lastHoveredElement->onMouseLeave(event);
            }
            m_lastHoveredElement = it;
            it->onMouseEnter(event);
          }
          break;
        default:;
        }
      }
    }

    for (const auto &element : uiElements)
    {
      element->draw();
    }

    // reset renderer color back to black
    SDL_SetRenderDrawColor(WindowManager::instance().getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderPresent(WindowManager::instance().getRenderer());
  }

  return quitGame;
}

void Game::run(bool SkipMenu)
{
  LOG(LOG_INFO) << VERSION;

  // we need to instantiate the MapFunctions object so it's ready for new game
  MapFunctions::instance();
  SignalMediator::instance().signalNewGame.emit(true);

  Camera::instance().centerScreenOnMapCenter();

  SDL_Event event;
  EventManager &evManager = EventManager::instance();

  UIManager &uiManager = UIManager::instance();
  uiManager.init();

  GameClock &gameClock = GameClock::instance();
  GamePlay m_GamePlay;

#ifdef USE_ANGELSCRIPT
  ScriptEngine &scriptEngine = ScriptEngine::instance();
  scriptEngine.init();
#endif

#ifdef USE_AUDIO
  if (!Settings::instance().audio3DStatus)
  {
    gameClock.addRealTimeClockTask(
        []()
        {
          AudioMixer::instance().play(AudioTrigger::MainTheme);
          return false;
        },
        0s, 8min);
    gameClock.addRealTimeClockTask(
        []()
        {
          AudioMixer::instance().play(AudioTrigger::NatureSounds);
          return false;
        },
        0s, 3min);
  }
  else
  {
    gameClock.addRealTimeClockTask(
        []()
        {
          AudioMixer::instance().play(AudioTrigger::MainTheme, Coordinate3D{0, 0.5, 0.1});
          return false;
        },
        0s, 8min);
    gameClock.addRealTimeClockTask(
        []()
        {
          AudioMixer::instance().play(AudioTrigger::NatureSounds, Coordinate3D{0, 0, -2});
          return false;
        },
        0s, 3min);
  }
#endif // USE_AUDIO

  // FPS Counter variables
  const float fpsIntervall = 1.0; // interval the fps counter is refreshed in seconds.
  Uint32 fpsLastTime = SDL_GetTicks();
  Uint32 fpsFrames = 0;

  // GameLoop
  while (!m_shutDown)
  {
#ifdef MICROPROFILE_ENABLED
    MICROPROFILE_SCOPEI("Map", "Gameloop", MP_GREEN);
#endif
    SDL_RenderClear(WindowManager::instance().getRenderer());

    evManager.checkEvents(event);
    gameClock.tick();

    // render the tileMap
    if (MapFunctions::instance().getMap())
    {
      MapFunctions::instance().getMap()->renderMap();
    }

    // render the ui
    // TODO: This is only temporary until the new UI is ready. Remove this afterwards
    if (GameStates::instance().drawUI)
    {
      uiManager.drawUI();
    }

    // preset the game screen
    WindowManager::instance().renderScreen();

    fpsFrames++;

    if (fpsLastTime < SDL_GetTicks() - fpsIntervall * 1000)
    {
      fpsLastTime = SDL_GetTicks();
      uiManager.setFPSCounterText(std::to_string(fpsFrames) + " FPS");
      fpsFrames = 0;
    }

#ifdef MICROPROFILE_ENABLED
    MicroProfileFlip(nullptr);
#endif
  }
}

void Game::shutdown()
{
  LOG(LOG_DEBUG) << "In shutdown";
  TTF_Quit();
  SDL_Quit();
}
} // namespace Cytopia
