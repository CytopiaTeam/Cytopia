#include "UIManager.hxx"

#include "Constants.hxx"
#include "ResourcesManager.hxx"
#include <MapFunctions.hxx>
#include "Map.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "basics/utils.hxx"
#include "LOG.hxx"
#include "Exception.hxx"
#include "GameStates.hxx"
#include "MapLayers.hxx"
#include "Filesystem.hxx"
#include "json.hxx"
#include "betterEnums.hxx"
#include <Camera.hxx>

#ifdef USE_AUDIO
#include "../services/AudioMixer.hxx"
#endif

#include <cmath>
#include <array>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#include <Settings.hxx>

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif

namespace ui = ImGui;

BETTER_ENUM(Action, int, RaiseTerrain, LowerTerrain, QuitGame, Demolish, ChangeTileType, ToggleVisibilityOfGroup, NewGame,
            SaveGame, LoadGame, SaveSettings, ChangeResolution)

void UIManager::init()
{
  json uiLayout;

  loadSettings(uiLayout);
  parseLayouts(uiLayout);

  initializeImGuiFonts();
}

ImFont *UIManager::loadFont(const std::string &fontPath, uint32_t size)
{
  auto *uiFonts = ImGui::GetIO().Fonts;
  if (!fontDefault)
    fontDefault = uiFonts->AddFontDefault();

  std::string hashName = fontPath;
  hashName.append(std::to_string(size));
  if (const auto it = m_loadedFonts.find(hashName); it != m_loadedFonts.end())
    return it->second;

  ImFont *newFont = uiFonts->AddFontFromFileTTF(fontPath.c_str(), (float)size);
  m_loadedFonts[hashName] = newFont;
  uiFonts->Build();

  return newFont;
}

void UIManager::initializeImGuiFonts()
{
  std::string fontPath = fs::getBasePath() + Settings::instance().fontFileName.get(); // fix for macos, need to use abs path

  const auto names = {"MainMenuButtons", "PauseMenuButtons", "LoadDialogButtons", "BuildMenuButtons"};
  for (const auto &name : names)
  {
    if (const auto it = m_layouts.find(name); it != m_layouts.end())
    {
      auto &layout = it->second;
      layout.font = loadFont(fontPath, layout.fontSize);
    }
  }
}

void UIManager::loadSettings(json &uiLayout)
{
  std::string jsonFileContent = fs::readFileAsString(Settings::instance().uiLayoutJSONFile.get());
  uiLayout = json::parse(jsonFileContent, nullptr, false);

  // check if json file can be parsed
  if (uiLayout.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + Settings::instance().uiLayoutJSONFile.get());
}

void UIManager::parseLayouts(const json &uiLayout)
{
  // parse Layout
  for (const auto &it : uiLayout["LayoutGroups"].items())
  {
    std::string layoutGroupName;

    // prepare empty layout groups with layout information from json
    for (size_t id = 0; id < uiLayout["LayoutGroups"][it.key()].size(); id++)
    {
      layoutGroupName = uiLayout["LayoutGroups"][it.key()][id].value("GroupName", "");

      if (!layoutGroupName.empty())
      {
        LayoutData layout;
        layout.layoutType = uiLayout["LayoutGroups"][it.key()][id].value("LayoutType", "");
        layout.alignment = uiLayout["LayoutGroups"][it.key()][id].value("Alignment", "");

        if (layout.layoutType.empty())
        {
          LOG(LOG_WARNING) << "Skipping LayoutGroup " << layoutGroupName
                           << " because it has no parameter \"LayoutType\" set. Check your UiLayout.json file.";
          continue;
        }

        if (layout.alignment.empty())
        {
          LOG(LOG_WARNING) << "Skipping LayoutGroup " << layoutGroupName
                           << " because it has no parameter \"Alignment\" set. Check your UiLayout.json file.";
          continue;
        }

        layout.fontSize = uiLayout["LayoutGroups"][it.key()][id].value("FontSize", Settings::instance().defaultFontSize);
        layout.padding = uiLayout["LayoutGroups"][it.key()][id].value("Padding", 0);
        layout.paddingToParent = uiLayout["LayoutGroups"][it.key()][id].value("PaddingToParent", 0);
        layout.alignmentOffset = uiLayout["LayoutGroups"][it.key()][id].value("AlignmentOffset", 0.0F);

        // add layout group information to container
        m_layouts[layoutGroupName] = layout;
      }
      else
      {
        LOG(LOG_WARNING) << "Cannot add a Layout Group without a name. Check your UiLayout.json file.";
      }
    }
  }
}

void UIManager::setFPSCounterText(const std::string &fps) { m_fpsCounter = fps; }

void UIManager::closeOpenMenus()
{
  for (auto &m : m_persistentMenu)
  {
    m->closeSubmenus();
  }
}

void UIManager::openMenu(GameMenu::Ptr menu)
{
  if (std::none_of(std::begin(m_menuStack), std::end(m_menuStack), [&](const auto &m) { return m == menu; }))
  {
    m_menuStack.push_back(menu);

    Camera::instance().canScale(false);
    Camera::instance().canMove(false);
  }
}

void UIManager::closeMenu()
{
  if (m_menuStack.empty())
    return;

  m_menuStack.pop_back();

  Camera::instance().canScale(m_menuStack.empty());
  Camera::instance().canMove(m_menuStack.empty());
}

void UIManager::addPersistentMenu(GameMenu::Ptr menu)
{
  if (std::none_of(std::begin(m_persistentMenu), std::end(m_persistentMenu), [&](const auto &m) { return m == menu; }))
  {
    m_persistentMenu.push_back(menu);
  }
}

bool UIManager::isMouseHovered() const { return ImGui::IsAnyItemHovered(); }

void UIManager::drawUI()
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("UI", "draw UI", MP_BLUE);
#endif
  if (!m_menuStack.empty())
  {
    m_menuStack.back()->draw();
  }

  for (const auto &m : m_persistentMenu)
  {
    m->draw();
  }

  if (!m_tooltip.empty())
  {
    ImVec2 pos = ui::GetMousePos();
    ui::SetCursorScreenPos(pos);
    ui::Text("%s", m_tooltip.c_str());
  }

  if (m_showFpsCounter)
  {
    ui::SetNextWindowPos(ImVec2(0, 0));
    ui::SetNextWindowSize(ImVec2(140, 20));

    bool open = true;
    ui::Begin("##fpswindow", &open,
              ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar |
                  ImGuiWindowFlags_NoScrollWithMouse);
    ui::Text("%s", m_fpsCounter.c_str());
    ui::SameLine();
    ui::Checkbox("debug", &m_showDebugMenu);
    ui::End();
  }
}

void UIManager::setTooltip(const std::string &tooltipText) { m_tooltip = tooltipText; }

void UIManager::clearTooltip() { m_tooltip.clear(); }
