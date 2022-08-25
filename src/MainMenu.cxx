#include "MainMenu.hxx"
#ifdef USE_AUDIO
#include "services/AudioMixer.hxx"
#endif
#include <Settings.hxx>
#include <SignalMediator.hxx>
#include <SDL.h>
#include "engine/UIManager.hxx"
#include "engine/basics/Settings.hxx"
#include "../engine/ResourcesManager.hxx"
#include "WindowManager.hxx"
#include "OSystem.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "game/ui/LoadMenu.hxx"

namespace ui = ImGui;

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

bool mainMenu()
{
  SDL_Event event;

  int screenWidth = Settings::instance().screenWidth;
  int screenHeight = Settings::instance().screenHeight;
  bool mainMenuLoop = true;
  bool startGame = true;
  bool showLoadDialog = false;

  UIManager::instance().init();

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

  auto logoTex = ResourcesManager::instance().getUITexture("Cytopia_Logo");
  auto discordTex = ResourcesManager::instance().getUITexture("Discord_icon");
  auto githubTex = ResourcesManager::instance().getUITexture("Github_icon");
  int logoTexW, logoTexH;
  SDL_QueryTexture(logoTex, nullptr, nullptr, &logoTexW, &logoTexH);

  auto beginFrame = []
  {
    SDL_RenderClear(WindowManager::instance().getRenderer());

    WindowManager::instance().newImGuiFrame();

    ui::SetNextWindowBgAlpha(0);
    ui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ui::SetNextWindowSize(ui::GetIO().DisplaySize);

    bool open = true;
    ui::Begin("MainWnd", &open,
              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                  ImGuiWindowFlags_NoScrollWithMouse);
  };

  auto renderFrame = []
  {
    ui::End();

    WindowManager::instance().renderScreen();
    SDL_Delay(5);
  };

  // fade in Logo
  const ImVec2 logoImgPos(screenWidth / 2 - logoTexW / 2, screenHeight / 4);
  for (Uint8 opacity = 0; opacity < 255; opacity++)
  {
    beginFrame();

    // break the loop if an event occurs
    const bool has_event = SDL_PollEvent(&event) != 0;
    if (has_event && event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN)
      opacity = 254;

    ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ui::SetCursorPos(logoImgPos);
    float op = opacity / 255.f;
    ui::Image(logoTex, ImVec2(logoTexW, logoTexH), ImVec2(0, 0), ImVec2(1, 1), ImVec4{op, op, op, op});
    ui::PopStyleVar(1);

    renderFrame();
  }

  LoadMenu loadMenu;

  const auto &buttonFont = UIManager::instance().getLayouts()["MainMenuButtons"].font;
  while (mainMenuLoop)
  {
    beginFrame();

    while (SDL_PollEvent(&event) != 0)
    { // while there is a pending event
      ImGui_ImplSDL2_ProcessEvent(&event);

      if (event.type == SDL_QUIT)
      {
        startGame = false;
        mainMenuLoop = false;
      }
    }

    if (showLoadDialog)
    {
      loadMenu.draw();
      switch (loadMenu.result())
      {
      case LoadMenu::e_close:
        showLoadDialog = false;
        break;

      case LoadMenu::e_load_file:
#ifdef USE_AUDIO
        playAudioMajorSelection();
#endif //  USE_AUDIO
        SignalMediator::instance().signalLoadGame.emit(loadMenu.filename());
        mainMenuLoop = false;
        break;
      default:
        break;
      }
    }
    else
    {
      ui::PushFont(buttonFont);
      ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

      ui::SetCursorPos(logoImgPos);
      ui::Image(logoTex, ImVec2(logoTexW, logoTexH));

      constexpr ImVec2 buttonSize(200, 40);
      constexpr int buttonInterval = 20;
      ImVec2 buttonPos(screenWidth / 2 - buttonSize.x / 2, screenHeight / 2 - buttonSize.y);
      ui::SetCursorPos(buttonPos);
      if (ui::ButtonCt("New Game", buttonSize))
      {
#ifdef USE_AUDIO
        playAudioMajorSelection();
#endif //  USE_AUDIO
        mainMenuLoop = false;
        SignalMediator::instance().signalNewGame.emit(true);
      }

      buttonPos.y += buttonSize.y + buttonInterval;
      ui::SetCursorPos(buttonPos);
      if (ui::ButtonCt("Load Game", buttonSize))
      {
        showLoadDialog = true;
      }

      buttonPos.y += buttonSize.y + buttonInterval;
      ui::SetCursorPos(buttonPos);
      if (ui::ButtonCt("Quit Game", buttonSize))
      {
        startGame = false;
        mainMenuLoop = false;
      }

      constexpr int xOffset = 5, btnSize = 32;
      ImVec2 leftBottom(xOffset, screenHeight - btnSize - xOffset * 2);
      ui::SetCursorPos(leftBottom);
      if (ui::ImageButton(discordTex, ImVec2(btnSize, btnSize)))
      {
        OSystem::openDir("https://discord.gg/MG3tgYV6ce");
      }

      leftBottom.x += xOffset * 2 + btnSize; // xOffset * 2 because, need interval between buttons
      ui::SetCursorPos(leftBottom);
      if (ui::ImageButton(githubTex, ImVec2(btnSize, btnSize)))
      {
        OSystem::openDir("https://github.com/CytopiaTeam/Cytopia/issues/new");
      }

      ui::PopFont();
      ui::PopStyleVar(1);
    }

    /* FPS counter.*/
    /** @TODO remove or alter to use existing fps counter from debug menu */
    constexpr ImVec2 fpsTextPos(5, 5);
    ui::SetCursorPos(fpsTextPos);
    ui::Text("[%.1f FPS]", ui::GetIO().Framerate);

    ImVec2 textSize = ImGui::CalcTextSize(VERSION);
    const ImVec2 versionPos(screenWidth - textSize.x - 10, screenHeight - textSize.y - 10);
    ui::SetCursorPos(versionPos);
    ui::Text(VERSION);

    renderFrame();
  }
  return startGame;
}