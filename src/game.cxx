#include <SDL.h>

#include "engine/engine.hxx"
#include "engine/eventManager.hxx"
#include "engine/uiManager.hxx"
#include "engine/windowManager.hxx"
#include "engine/audioMixer.hxx"
#include "engine/basics/camera.hxx"
#include "engine/basics/point.hxx"
#include "engine/basics/log.hxx"

void run()
{
  Timer benchmarkTimer;
  LOG() << VERSION;

  WindowManager::instance().setWindowTitle(VERSION);

  benchmarkTimer.start();
  Engine &engine = Engine::instance();
  LOG() << "Map initialized in " << benchmarkTimer.getElapsedTime() << "ms";
  Camera::centerScreenOnMapCenter();

  SDL_Event event;
  EventManager evManager;

  UIManager &uiManager = UIManager::instance();
  uiManager.init();

  AudioMixer audiomixer;
  audiomixer.playMusic();

  // FPS Counter variables
  const float fpsIntervall = 1.0; // interval the fps counter is refreshed in seconds.
  Uint32 fpsLastTime = SDL_GetTicks();
  Uint32 fpsFrames = 0;

  // Fake loading screen.
  unsigned short i = 0;
  srand(time(NULL));
  const unsigned short MAX_FAKE_LOADING_SCREEN_MILLIS = rand() % 500 + 500;
  const unsigned char TTF_INIT_FAILED = 0xFE;
  const unsigned char TTF_OPEN_FONT_FAILED = 0xFF;
  std::vector<std::string> loading_lines = std::vector<std::string>();
  loading_lines.push_back("loading  traffic");
  loading_lines.push_back("calling  aliens");
  loading_lines.push_back("heating  volcanoes");
  const unsigned char chosen_line_index = rand() % loading_lines.size();
  const char * chosen_loading_line =
      loading_lines.at(chosen_line_index).c_str();
  char * pct_as_string = (char *) malloc(10);
  while (engine.isGameRunning() && i < MAX_FAKE_LOADING_SCREEN_MILLIS)
  {
    SDL_RenderClear(WindowManager::instance().getRenderer());

    evManager.checkEvents(event, engine);

    Text loading_line(SDL_Rect{320, 240, 120, 40});
    loading_line.setText(chosen_loading_line);
    loading_line.draw();

    Text pct_Text(SDL_Rect{360, 260, 80, 60});
    const unsigned char pct = i * 100 / MAX_FAKE_LOADING_SCREEN_MILLIS / 10 * 10;
    sprintf(pct_as_string, "%u", pct);
    strcat(pct_as_string, "%");
    pct_Text.setText(pct_as_string);
    pct_Text.draw();

    // Render the Frame
    SDL_RenderPresent(WindowManager::instance().getRenderer());

    SDL_Delay(1);
    i++;
  }
  free(pct_as_string);

  // Gameloop
  while (engine.isGameRunning())
  {
    SDL_RenderClear(WindowManager::instance().getRenderer());

    evManager.checkEvents(event, engine);

    // render the tilemap
    engine.getMap()->renderMap();

    // render the ui
    uiManager.drawUI();

    // reset renderer color back to black
    SDL_SetRenderDrawColor(WindowManager::instance().getRenderer(), 0, 0, 0, SDL_ALPHA_OPAQUE);

    // Render the Frame
    SDL_RenderPresent(WindowManager::instance().getRenderer());

    fpsFrames++;
    if (fpsLastTime < SDL_GetTicks() - fpsIntervall * 1000)
    {
      fpsLastTime = SDL_GetTicks();
      uiManager.setFPSCounterText(std::to_string(fpsFrames) + " FPS");
      fpsFrames = 0;
    }

    //SDL_Delay(1);
  }
}
