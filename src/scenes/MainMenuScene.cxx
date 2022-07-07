#include "MainMenuScene.hxx"
#ifdef USE_AUDIO
#include "../services/AudioMixer.hxx"
#endif
#include <Settings.hxx>
#include <SignalMediator.hxx>
#include <SDL.h>
#include <UIManager.hxx>
#include <Image.hxx>
#include "OSystem.hxx"
#include "MapScene.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

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

void SceneMainMenu::onBeforeStart()
{
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

  m_logoTex = ResourcesManager::instance().getUITexture("Cytopia_Logo");
  m_discordTex = ResourcesManager::instance().getUITexture("Discord_icon");
  m_githubTex = ResourcesManager::instance().getUITexture("Github_icon");

  SDL_QueryTexture(m_logoTex, nullptr, nullptr, &m_logoTexSize.x, &m_logoTexSize.y);
}

void SceneMainMenu::onEvent(const SDL_Event &event)
{
  ImGui_ImplSDL2_ProcessEvent(&event);

  if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_KEYDOWN)
    m_opacity = 254;

  if (event.type == SDL_QUIT) {
    m_isActive = false;
  }
}

void SceneMainMenu::onRender(float dt) {
  int screenWidth = Settings::instance().screenWidth;
  int screenHeight = Settings::instance().screenHeight;

  ui::SetNextWindowBgAlpha(0);
  ui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ui::SetNextWindowSize(ui::GetIO().DisplaySize);

  bool open = true;
  ui::Begin("MainWnd", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  // fade in Logo
  const ImVec2 logoImgPos(screenWidth / 2 - m_logoTexSize.x / 2, screenHeight / 4);
  if (m_opacity < 254)
    m_opacity++;

  ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ui::SetCursorPos(logoImgPos);
  float op = m_opacity / 255.f;
  ui::Image(m_logoTex, ImVec2(m_logoTexSize.x, m_logoTexSize.y), ImVec2(0, 0), ImVec2(1, 1), ImVec4{op, op, op, op});
  ui::PopStyleVar(1);

  const auto &buttonFont = UIManager::instance().getAllLayoutGroups()["MainMenuButtons"].layout.font;
  const bool drawButtons = m_opacity > 250;

  // main logic render widgets in main menu
  {
    ui::PushFont(buttonFont);
    ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

    constexpr ImVec2 buttonSize(200, 40);
    constexpr int buttonInterval = 20;
    ImVec2 buttonPos(screenWidth / 2.f - buttonSize.x / 2.f, screenHeight / 2.f - buttonSize.y);
    ui::SetCursorPos(buttonPos);
    if (drawButtons && ui::ButtonCt("New Game", buttonSize)) {
#ifdef USE_AUDIO
      playAudioMajorSelection();
#endif //  USE_AUDIO 
      m_isActive = false;
      auto loadingScreen = new SceneLoadingNew;
      nextScene(loadingScreen);
    }

    buttonPos.y += buttonSize.y + buttonInterval;
    ui::SetCursorPos(buttonPos);
    if (drawButtons && ui::ButtonCt("Load Game", buttonSize)) {
#ifdef USE_AUDIO
      playAudioMajorSelection();
#endif //  USE_AUDIO 
      auto loadingScreen = new SceneLoadingExist;
      nextScene(loadingScreen);
    }

    buttonPos.y += buttonSize.y + buttonInterval;
    ui::SetCursorPos(buttonPos);
    if (drawButtons && ui::ButtonCt("Quit Game", buttonSize)) {
      m_isActive = false;
    }

    constexpr int xOffset = 5;
    constexpr int btnSize = 32;
    ImVec2 leftBottom(xOffset, screenHeight - btnSize - xOffset * 2);
    ui::SetCursorPos(leftBottom);
    if (drawButtons && ui::ImageButton(m_discordTex, ImVec2(btnSize, btnSize))) {
      OSystem::openDir("https://discord.gg/MG3tgYV6ce");
    }

    leftBottom.x += xOffset * 2 + btnSize; // xOffset * 2 because, need interval between buttons
    ui::SetCursorPos(leftBottom);
    if (drawButtons && ui::ImageButton(m_githubTex, ImVec2(btnSize, btnSize))) {
      OSystem::openDir("https://github.com/CytopiaTeam/Cytopia/issues/new");
    }

    ui::PopFont();
    ui::PopStyleVar(1);
  }

  {
    constexpr ImVec2 fpsTextPos(5, 5);
    ui::SetCursorPos(fpsTextPos);
    ui::Text("[%.1f FPS]", ui::GetIO().Framerate);

    ImVec2 textSize = ImGui::CalcTextSize(VERSION);
    const ImVec2 versionPos(screenWidth - textSize.x - 10.f, screenHeight - textSize.y - 10.f);
    ui::SetCursorPos(versionPos);
    ui::Text(VERSION);
  }

  ui::End();
}

void SceneMainMenu::onDestroy() {
  delete this;
}