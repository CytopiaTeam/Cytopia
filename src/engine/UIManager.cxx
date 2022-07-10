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
#include "Camera.hxx"

#ifdef USE_AUDIO
#include "../services/AudioMixer.hxx"
#endif

#include <cmath>
#include <array>

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

#ifdef MICROPROFILE_ENABLED
#include "microprofile/microprofile.h"
#endif

using json = nlohmann::json;
namespace ui = ImGui;

BETTER_ENUM(ElementType, int, ImageButton, TextButton, Text, Frame)
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
  auto it = m_loadedFonts.find(hashName);
  if (it != m_loadedFonts.end())
    return it->second;

  ImFont *newFont = uiFonts->AddFontFromFileTTF(fontPath.c_str(), (float)size);
  m_loadedFonts[hashName] = newFont;
  uiFonts->Build();

  return newFont;
}

void UIManager::initializeImGuiFonts() {
  std::string fontPath = fs::getBasePath() + Settings::instance().fontFileName.get(); // fix for macos, need to use abs path

  std::array<const char *, 4> names = {"MainMenuButtons", "PauseMenuButtons", "LoadDialogButtons", "BuildMenuButtons"};
  for (const auto &name : names)
  {
    auto it = m_layoutGroups.find(name);
    if (it == m_layoutGroups.end())
      continue;

    auto &layout = it->second.layout;
    layout.font = loadFont(fontPath, layout.fontSize);
  }
}

void UIManager::loadSettings(json& uiLayout) {
  std::string jsonFileContent = fs::readFileAsString(Settings::instance().uiLayoutJSONFile.get());
  uiLayout = json::parse(jsonFileContent, nullptr, false);

  // check if json file can be parsed
  if (uiLayout.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + Settings::instance().uiLayoutJSONFile.get());
}

void UIManager::parseLayouts(const json& uiLayout)
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
        LayoutGroup layoutGroup;
        layoutGroup.layout.layoutType = uiLayout["LayoutGroups"][it.key()][id].value("LayoutType", "");
        layoutGroup.layout.alignment = uiLayout["LayoutGroups"][it.key()][id].value("Alignment", "");

        if (layoutGroup.layout.layoutType.empty())
        {
          LOG(LOG_WARNING) << "Skipping LayoutGroup " << layoutGroupName
                           << " because it has no parameter \"LayoutType\" set. Check your UiLayout.json file.";
          continue;
        }

        if (layoutGroup.layout.alignment.empty())
        {
          LOG(LOG_WARNING) << "Skipping LayoutGroup " << layoutGroupName
                           << " because it has no parameter \"Alignment\" set. Check your UiLayout.json file.";
          continue;
        }

        // set parent and check if the element exists
        layoutGroup.layout.layoutParentElementID = uiLayout["LayoutGroups"][it.key()][id].value("LayoutParentElementID", "");

        if (!layoutGroup.layout.layoutParentElementID.empty() && !getUiElementByID(layoutGroup.layout.layoutParentElementID))
        {
          LOG(LOG_WARNING) << "Skipping a non-existant UIElement with ID " << layoutGroup.layout.layoutParentElementID
                           << "that was set for LayoutGroup " << layoutGroupName;
          continue;
        }

        layoutGroup.layout.fontSize = uiLayout["LayoutGroups"][it.key()][id].value("FontSize", Settings::instance().defaultFontSize);
        layoutGroup.layout.padding = uiLayout["LayoutGroups"][it.key()][id].value("Padding", 0);
        layoutGroup.layout.paddingToParent = uiLayout["LayoutGroups"][it.key()][id].value("PaddingToParent", 0);
        layoutGroup.layout.alignmentOffset = uiLayout["LayoutGroups"][it.key()][id].value("AlignmentOffset", 0.0F);

        // add layout group information to container
        m_layoutGroups[layoutGroupName] = layoutGroup;
      }
      else
      {
        LOG(LOG_WARNING) << "Cannot add a Layout Group without a name. Check your UiLayout.json file.";
      }
    }
  }
}

void UIManager::setFPSCounterText(const std::string &fps) const { m_fpsCounter->setText(fps); }

void UIManager::closeOpenMenus()
{
  for (auto &m : m_persistentMenu)
  {
    m->closeSubmenus();
  }
}

void UIManager::openMenu(GameMenu::Ptr menu) {
  auto it = std::find(m_menuStack.begin(), m_menuStack.end(), menu);

  if (it != m_menuStack.end())
    return;

  m_menuStack.push_back(menu);

  Camera::instance().canScale(false);
}

void UIManager::closeMenu() {
  if (m_menuStack.empty())
    return;

  m_menuStack.pop_back();

  Camera::instance().canScale(m_menuStack.empty());
}

void UIManager::addPersistentMenu(GameMenu::Ptr menu)
{
  auto it = std::find(m_persistentMenu.begin(), m_persistentMenu.end(), menu);

  if (it == m_persistentMenu.end())
  {
    m_persistentMenu.push_back(menu);
  }
}

bool UIManager::isMouseHovered() const
{
  return ImGui::IsAnyItemHovered();
}

void UIManager::drawUI() const
{
#ifdef MICROPROFILE_ENABLED
  MICROPROFILE_SCOPEI("UI", "draw UI", MP_BLUE);
#endif
  for (const auto &it : m_uiElements)
  {
    if (it->isVisible())
    {
      it->draw();
    }
  }

  if (m_showDebugMenu)
  {
    m_fpsCounter->draw();
  }

  if (!m_menuStack.empty())
    m_menuStack.back()->draw();

  for (const auto &m : m_persistentMenu)
  {
    m->draw();
  }

  if (!m_tooltip.empty()) {
    ImVec2 pos = ui::GetMousePos();
    ui::SetCursorScreenPos(pos);
    ui::Text(m_tooltip.c_str());
  }
}

void UIManager::setGroupVisibility(const std::string &groupID, bool visible)
{
  for (const auto &it : m_uiGroups[groupID])
    it->setVisibility(visible);
}

void UIManager::setTooltip(const std::string &tooltipText)
{
  m_tooltip = tooltipText;
}

void UIManager::clearTooltip() { m_tooltip.clear(); }

UIElement *UIManager::getUiElementByID(const std::string &UiElementID) const
{
  for (auto &it : m_uiElements)
    if (it->getUiElementData().elementID == UiElementID)
      return it.get();
  return nullptr;
}

const std::vector<UIElement *> *UIManager::getUiElementsOfGroup(const std::string &groupID) const
{
  if (m_uiGroups.find(groupID) != m_uiGroups.end())
    return &m_uiGroups.find(groupID)->second;
  return nullptr;
}

void UIManager::addToLayoutGroup(const std::string &groupName, UIElement *widget)
{
  if (widget)
  {
    widget->setLayoutGroupName(groupName);
    m_layoutGroups[groupName].uiElements.push_back(widget);
  }
}
