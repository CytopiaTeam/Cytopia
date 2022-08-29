#include "LoadMenu.hxx"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include "Settings.hxx"
#include "MapFunctions.hxx"
#include "Constants.hxx"

#include <filesystem>

#ifdef USE_AUDIO
#include "../services/AudioMixer.hxx"
#endif

namespace fs = std::filesystem;
namespace ui = ImGui;

LoadMenu::LoadMenu()
{
  // still do nothing
}

void LoadMenu::draw() const
{
  m_result = e_none;
  m_filename.clear();

  ImVec2 windowSize(500, 500);
  ImVec2 screenSize = ui::GetIO().DisplaySize;

  auto &uiManager = UIManager::instance();

  const auto &layout = uiManager.getLayouts()["LoadDialogButtons"];
  ui::SetNextWindowPos(ImVec2((screenSize.x - windowSize.x) / 2, (screenSize.y - windowSize.y) / 2));
  ui::SetNextWindowSize(windowSize);

  constexpr float btnSide = 40;
  const ImVec2 buttonSize(windowSize.x - btnSide * 3, btnSide);
  const ImVec2 buttonOffset((windowSize.x - buttonSize.x) / 2, buttonSize.y / 2);
  const ImVec2 widgetSize((windowSize.x / 2) - 8, 20);

  ui::PushFont(layout.font);
  ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
  ui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, buttonOffset.y));

  bool open = true;
  ui::BeginCt("LoadDialog", &open,
              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar |
                  ImGuiWindowFlags_NoScrollWithMouse);

  // empty header
  ui::PushItemWidth(widgetSize.x);

  // header label
  auto textWidth = ImGui::CalcTextSize("Load city").x;
  ImGui::SetCursorPosX((windowSize.x - textWidth) * 0.5f);
  ui::LabelText("##loadcitylbl", "Load city");

  ImGui::BeginChild("##loadbuttons", {0.f, windowSize.y * 0.6f}, false, ImGuiWindowFlags_NoTitleBar);

  std::string path = CYTOPIA_DATA_DIR + "/saves";
  for (const auto &entry : fs::directory_iterator(path))
  {
    if (!fs::is_regular_file(entry) || entry.path().extension() != ".cts")
      continue;

    ui::Dummy({btnSide / 2.f, 0.f});
    ui::SameLine();
    if (ui::ButtonCt(entry.path().filename().string().c_str(), buttonSize))
    {
      m_filename = entry.path().filename().string();
      m_result = e_load_file;
    }

    ui::SameLine();
    ui::Dummy({btnSide / 2.f, 0.f});
    ui::SameLine();
    if (ui::ButtonCt("X", {btnSide, btnSide}))
    {
      m_filename = entry.path().filename().string();
      m_result = e_delete_file;
    }
  }

  ui::EndChild();
  ui::PopItemWidth();

  ImVec2 btnSize(windowSize.x / 4, btnSide);

  ui::SetCursorPosY(windowSize.y - btnSize.y * 2);
  ui::Dummy({(windowSize.x - btnSize.x) / 2.f, 0.f});
  ui::SameLine();
  if (ui::ButtonCt("OK", btnSize))
  {
    m_result = e_close;
  }

  ui::PopFont();
  ui::PopStyleVar(3);

  ui::End();
}

LoadMenu::~LoadMenu()
{
  // still do nothing
}