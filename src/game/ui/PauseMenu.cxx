#include "PauseMenu.hxx"

#include "SettingsMenu.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "SignalMediator.hxx"

namespace ui = ImGui;

void PauseMenu::draw() const
{
  ImVec2 windowSize(300, 400);
  ImVec2 screenSize = ui::GetIO().DisplaySize;

  // dont remove yet, need for tuning
  //bool show = true;
  //ui::ShowDemoWindow(&show);

  auto &uiManager = UIManager::instance();

  const auto &layout = uiManager.getLayouts()["PauseMenuButtons"];
  ui::SetNextWindowPos(ImVec2((screenSize.x - windowSize.x) / 2, (screenSize.y - windowSize.y) / 2));
  ui::SetNextWindowSize(windowSize);

  const ImVec2 buttonSize(200, 40);
  const ImVec2 buttonOffset((windowSize.x - buttonSize.x) / 2, buttonSize.y / 2);

  ui::PushFont(layout.font);
  ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
  ui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(buttonOffset.x, 0));
  ui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, buttonOffset.y));

  bool open = true;
  ui::BeginCt("PauseMenu", &open,
              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                  ImGuiWindowFlags_NoScrollWithMouse);

  ui::Dummy({0.f, (windowSize.y - (buttonSize.y + buttonOffset.y) * 5) / 2});
  if (ui::ButtonCt("Settings", buttonSize))
  {
    Settings::instance().writeFile();
    uiManager.openMenu<SettingsMenu>();
  }

  if (ui::ButtonCt("New Game", buttonSize))
  {
    SignalMediator::instance().signalNewGame.emit(true);
  }

  if (ui::ButtonCt("Save Game", buttonSize))
  {
    SignalMediator::instance().signalSaveGame.emit("save.cts");
  }

  if (ui::ButtonCt("Load Game", buttonSize))
  {
    SignalMediator::instance().signalLoadGame.emit("save.cts");
  }

  if (ui::ButtonCt("Quit Game", buttonSize))
  {
    SignalMediator::instance().signalQuitGame.emit();
  }

  ui::PopFont();
  ui::PopStyleVar(4);

  ui::End();
}