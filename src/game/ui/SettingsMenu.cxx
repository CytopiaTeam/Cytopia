#include "SettingsMenu.hxx"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include "imgui_internal.h"

#include "Settings.hxx"
#include "MapFunctions.hxx"
#include "WindowManager.hxx"

#ifdef USE_AUDIO
#include "../services/AudioMixer.hxx"
#endif

namespace ui = ImGui;

SettingsMenu::SettingsMenu() {
}

void SettingsMenu::draw() const {
  const ImVec2 windowSize = ui::ImVec2Scaled(600, 600);
  auto &io = ui::GetIO();
  ImVec2 screenSize = io.DisplaySize;

  // dont remove yet, need for tuning
  //bool show = true;
  //ui::ShowDemoWindow(&show); 

  auto &uiManager = UIManager::instance();
  auto &settings = Settings::instance();

  const auto &layout = uiManager.getLayouts()["PauseMenuButtons"];
  ui::SetNextWindowPos(ImVec2((screenSize.x - windowSize.x)/2, (screenSize.y - windowSize.y)/2));
  ui::SetNextWindowSize(windowSize);

  const float btnh = 20.f * io.FontGlobalScale;
  const ImVec2 buttonSize = ui::ImVec2Scaled(200, 40);
  const ImVec2 buttonOffset = ui::ImVec2Scaled((windowSize.x - buttonSize.x) / 2, buttonSize.y / 2);
  const ImVec2 widgetSize((windowSize.x / 2) - 8, btnh);

  ui::PushFont(layout.font);
  ui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
  //ui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(buttonOffset.x, 0));
  ui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, buttonOffset.y));

  bool open = true;
  ui::BeginCt("PauseMenu", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

  // empty header 
  ui::Dummy({0.f, buttonSize.y});
  ui::PushItemWidth(widgetSize.x);

  // header label
  {
    auto textWidth = ImGui::CalcTextSize("Settings").x;
    ImGui::SetCursorPosX((windowSize.x - textWidth) * 0.5f);
    ui::LabelText("##settingslbl", "Settings");
  }

  // vsync label + checkbox
  {
    ui::LabelText("##vsynclbl", "Enable VSync"); ui::SameLine();
    ui::CheckboxCt("##vsync", &settings.vSync);
  }

  // sceen mode label + combobox
  {
    ui::LabelText("##fullscreenmodelbl", "Screen Mode"); ui::SameLine();
    std::vector<std::string> modes = {"WINDOWED", "BORDERLESS", "FULLSCREEN"};
    if (ui::BeginComboCt("##fullcreenmmode", modes[settings.fullScreenMode].c_str()))
    {
      for (int i = 0; i < modes.size(); ++i)
      {
        const bool is_selected = (settings.fullScreenMode == i);
        if (ui::Selectable(modes[i].c_str(), is_selected))
        {
          WindowManager::instance().setFullScreenMode(static_cast<FULLSCREEN_MODE>(i));
          MapFunctions::instance().refreshVisibleMap();
        }
        if (is_selected)
          ui::SetItemDefaultFocus();
      }
      ui::EndCombo();
    }
  }

  // screen resolution label + combobox
  {
    ui::LabelText("##screenreslbl", "Resolution"); ui::SameLine();
    std::string currentResolution = std::to_string(settings.screenWidth) + " x " + std::to_string(settings.screenHeight);
    if (ui::BeginComboCt("##screenres", currentResolution.c_str()))
    {
      for (const auto &mode : WindowManager::instance().getSupportedScreenResolutions())
      {
        std::string resolution = std::to_string(mode->w) + " x " + std::to_string(mode->h);
        const bool is_selected = (settings.screenHeight == mode->h && settings.screenWidth == mode->w);
        if (ui::Selectable(resolution.c_str(), is_selected))
        {
          WindowManager::instance().setScreenResolution((int)(&mode - &WindowManager::instance().getSupportedScreenResolutions().front()));
          MapFunctions::instance().refreshVisibleMap();
        }
        if (is_selected)
          ui::SetItemDefaultFocus();
      }
      ui::EndCombo();
    }
  }

  // menu layout label + combobox
  {
    ui::LabelText("##menulayoutlbl", "Build Menu Pos"); ui::SameLine();
    int buildMenuLayoutIdx = (int)uiManager.buildMenuLayout();
    std::vector<const char *> layouts = {"LEFT", "RIGHT", "TOP", "BOTTOM"};
    if (ui::BeginComboCt("##menulayoutcombo", layouts[buildMenuLayoutIdx]))
    {
      for (size_t n = 0, size = layouts.size(); n < size; n++)
      {
        const bool is_selected = (buildMenuLayoutIdx == n);
        if (ui::Selectable(layouts[n], is_selected))
        {
          settings.buildMenuPosition = layouts[n];
          uiManager.setBuildMenuLayout(static_cast<BUILDMENU_LAYOUT>(n));
        }

        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        if (is_selected)
          ui::SetItemDefaultFocus();
      }
      ui::EndCombo();
    }
  }

  // music volume label + slider
  {
    ui::LabelText("##musicvol", "Music Volume"); ui::SameLine();
    float saveVolume = settings.musicVolume;
    ui::SliderFloatCt("##musicvolsl", &settings.musicVolume, 0.f, 1.f, "", ImGuiSliderFlags_NoText);

    if (saveVolume != settings.musicVolume)
    {
#ifdef USE_AUDIO
      AudioMixer::instance().setMusicVolume(settings.musicVolume);
#endif
    }
  }

  // ui scale settings
  {
    ui::LabelText("##uiscale", "UI Scale"); ui::SameLine();
    static float saveScale = io.FontGlobalScale;
    float deltaScale = 0.25;
    ui::BeginChild("##uiscaleitems");
    //ui::SliderFloatCt("##uiscalesl", &saveScale, 0.5f, 2.f, "", ImGuiSliderFlags_NoText); // Scale everything
    if (ui::InputScalarCt("##uiscalesl", ImGuiDataType_Float, &saveScale, &deltaScale, nullptr, "%0.2f", ImGuiInputTextFlags_ButtonsOnly)) {
      saveScale = ImClamp(saveScale, 0.5f, 2.f);
      io.FontGlobalScale = saveScale;
    }
    ui::EndChild();
  }
  
  ui::PopItemWidth();

  ImVec2 btnSize(windowSize.x / 4, btnh * 2.f);

  ui::SetCursorPosY(windowSize.y - btnSize.y * 2);
  ui::Dummy({btnSize.x / 2, 0.f}); ui::SameLine();
  if (ui::ButtonCt("OK", btnSize))
  {
    uiManager.closeMenu();
  }

  ui::SameLine();
  if (ui::ButtonCt("Cancel", btnSize))
  {
    uiManager.closeMenu();
  }

  ui::SameLine();
  if (ui::ButtonCt("Reset", btnSize))
  {
    uiManager.closeMenu();
    Settings::instance().resetSettingsToDefaults(); 
  }
 

  ui::PopFont();
  ui::PopStyleVar(3);

  ui::End();
}

SettingsMenu::~SettingsMenu() {
}