#include "UIManager.hxx"

#include "ResourcesManager.hxx"
#include "Engine.hxx"
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

#include <cmath>

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

using json = nlohmann::json;

BETTER_ENUM(ElementType, int, ImageButton, TextButton, Text, Frame, Checkbox, ComboBox, Slider)
BETTER_ENUM(Action, int, RaiseTerrain, LowerTerrain, QuitGame, Demolish, ChangeTileType, ToggleVisibilityOfGroup, NewGame,
            SaveGame, LoadGame, SaveSettings, ChangeResolution)

void UIManager::init()
{
  std::string jsonFileContent = fs::readFileAsString(Settings::instance().uiLayoutJSONFile.get());
  json uiLayout = json::parse(jsonFileContent, nullptr, false);

  // check if json file can be parsed
  if (uiLayout.is_discarded())
    throw ConfigurationError(TRACE_INFO "Error parsing JSON File " + Settings::instance().uiLayoutJSONFile.get());

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

    // set FPS Counter position
    m_fpsCounter->setPosition(40, 20);
  }

  // parse UiElements
  for (const auto &it : uiLayout["UiElements"].items())
  {
    std::string groupID;
    groupID = it.key();
    std::string layoutGroupName;

    bool visible = true;

    auto &elements = uiLayout["UiElements"][it.key()];

    // parse UiElements
    for (size_t id = 0; id < elements.size(); id++)
    {
      // we need to reference the item or else the .is_null() check will fail
      auto &element = elements[id];

      if (!element["GroupVisibility"].is_null())
      {
        visible = element["GroupVisibility"].get<bool>();
      }

      // check if there is a global layoutGroup parameter set
      if (!element["GroupSettings"].is_null())
      {
        layoutGroupName = element["GroupSettings"].value("LayoutGroup", "");
      }

      // check if a single item is associated with a layout group and override group settings, if applicable
      if (!element["LayoutGroup"].is_null())
      {
        layoutGroupName = element.value("LayoutGroup", "");
      }

      if (!element["Type"].is_null())
      {
        bool toggleButton = element.value("ToggleButton", false);
        bool drawFrame = element.value("DrawFrame", false);

        auto uiElementID = element.value("ID", "");
        auto actionID = element.value("Action", "");
        auto actionParameter = element.value("ActionParameter", "");
        auto tooltipText = element.value("TooltipText", "");
        auto text = element.value("Text", "");
        auto textureID = element.value("SpriteID", "");
        auto uiElementType = element.value("Type", "");
        auto buttonGroupID = element.value("ButtonGroup", "");
        auto buildMenuID = element.value("BuildMenuID", "");

        SDL_Rect elementRect{0, 0, 0, 0};
        elementRect.x = element.value("Position_x", 0);
        elementRect.y = element.value("Position_y", 0);
        elementRect.w = element.value("Width", 0);
        elementRect.h = element.value("Height", 0);

        std::unique_ptr<UIElement> uiElement;

        // Create the ui elements
        ElementType elementType = ElementType::_from_string(uiElementType.c_str());
        switch (elementType)
        {
        case ElementType::ImageButton:
          uiElement = std::make_unique<Button>(elementRect);
          uiElement->setTextureID(textureID);
          break;
        case ElementType::TextButton:
          uiElement = std::make_unique<Button>(elementRect);
          dynamic_cast<Button *>(uiElement.get())->setText(text);
          break;
        case ElementType::Text:
          uiElement = std::make_unique<Text>();
          dynamic_cast<Text *>(uiElement.get())->setPosition(elementRect.x, elementRect.y);
          dynamic_cast<Text *>(uiElement.get())->setText(text);
          break;
        case ElementType::Frame:
          uiElement = std::make_unique<Frame>(elementRect);
          break;
        case ElementType::Checkbox:
          uiElement = std::make_unique<Checkbox>(elementRect);
          break;
        case ElementType::ComboBox:
          uiElement = std::make_unique<ComboBox>(elementRect);
          break;
        case ElementType::Slider:
          uiElement = std::make_unique<Slider>(elementRect);
          break;
        default:
          LOG(LOG_WARNING) << "An element without a type can not be created, check your UiLayout JSON File "
                           << Settings::instance().uiLayoutJSONFile.get();
          break;
        }

        uiElement->setVisibility(visible);
        uiElement->setTooltipText(tooltipText);
        uiElement->setActionID(actionID);
        uiElement->setActionParameter(actionParameter);
        uiElement->setToggleButton(toggleButton);
        uiElement->setUIElementID(uiElementID);
        uiElement->drawImageButtonFrame(drawFrame);
        uiElement->setMenuGroupID(buildMenuID);

        // only add UiElements to buttonGroups / eventHandling if they have no MenuGroupID property set
        if (buildMenuID.empty())
        {
          if (!buttonGroupID.empty())
          {
            if (m_buttonGroups.find("buttonGroupID") == m_buttonGroups.end())
            {
              m_buttonGroups[buttonGroupID] = new ButtonGroup();
            }

            m_buttonGroups[buttonGroupID]->addToGroup(dynamic_cast<Button *>(uiElement.get()));
          }
          else
          {
            // ButtonGroup Elements will be added later
            m_uiElementsForEventHandling.push_back(uiElement.get());
          }

          if (!groupID.empty())
          {
            m_uiGroups[groupID].push_back(uiElement.get());
          }
        }

        if (!layoutGroupName.empty())
        {
          if (m_layoutGroups.find(layoutGroupName) != m_layoutGroups.end())
          {
            addToLayoutGroup(layoutGroupName, uiElement.get());
          }
          else
          {
            throw UIError(TRACE_INFO "Cannot add elements to non existing LayoutGroup " + layoutGroupName +
                          ". Add the group first, before trying to add elements to it.");
          }
        }

        // store the element in a vector
        m_uiElements.emplace_back(std::move(uiElement));
      }
    }
  }

  m_tooltip->setVisibility(false);

  initializeDollarVariables();
  createBuildMenu();
  setCallbackFunctions();

  // Add all buttonGroups to EventHandling container
  for (auto it : m_buttonGroups)
  {
    m_uiElementsForEventHandling.push_back(it.second);
  }

  setBuildMenuLayout();

  Layout::arrangeElements();
}

void UIManager::setFPSCounterText(const std::string &fps) const { m_fpsCounter->setText(fps); }

void UIManager::closeOpenMenus()
{
  for (const auto &[key, value] : m_uiGroups)
  {
    if (key == "_BuildMenu_")
    {
      continue;
    }
    for (auto element : value)
    {
      element->setVisibility(false);
    }
  }

  return;
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

  m_tooltip->draw();
}

void UIManager::toggleGroupVisibility(const std::string &groupID, UIElement *sender)
{
  if (sender)
  {
    Button *button = dynamic_cast<Button *>(sender);
    if (groupID == "SettingsMenu" && !(sender->getUiElementData().text == "Cancel" || sender->getUiElementData().text == "OK"))
    {
      closeOpenMenus();
    }
    // cast the object to a Button to check if it's a toggle button.
    if (button && button->getUiElementData().isToggleButton)
    {
      for (const auto &it : m_uiGroups[groupID])
        it->setVisibility(button->checkState());
      return;
    }
  }

  for (const auto &it : m_uiGroups[groupID])
    it->setVisibility(!it->isVisible());
}

void UIManager::startTooltip(SDL_Event &event, const std::string &tooltipText) const
{
  m_tooltip->setText(tooltipText);

  auto x = event.button.x - m_tooltip->getUiElementRect().w / 2;
  auto y = event.button.y - m_tooltip->getUiElementRect().h;
  m_tooltip->setPosition(x, y);

  m_tooltip->startTimer();
}

void UIManager::stopTooltip() const { m_tooltip->reset(); }

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

void UIManager::setCallbackFunctions()
{
  for (auto &uiElement : m_uiElements)
  {
    std::string actionParameter = uiElement->getUiElementData().actionParameter;

    if (uiElement->getUiElementData().actionID == "RaiseTerrain")
    {
      uiElement->registerCallbackFunction([](UIElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);

        if (button && button->getUiElementData().isToggleButton)
        {
          terrainEditMode = button->checkState() ? TerrainEdit::RAISE : TerrainEdit::NONE;
          highlightSelection = button->checkState();
          return;
        }

        terrainEditMode = (terrainEditMode == TerrainEdit::RAISE) ? TerrainEdit::NONE : TerrainEdit::RAISE;
        highlightSelection = terrainEditMode != TerrainEdit::NONE;
      });
    }
    else if (uiElement->getUiElementData().actionID == "LowerTerrain")
    {
      uiElement->registerCallbackFunction([](UIElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);

        if (button && button->getUiElementData().isToggleButton)
        {
          button->checkState() ? terrainEditMode = TerrainEdit::LOWER : terrainEditMode = TerrainEdit::NONE;
          button->checkState() ? highlightSelection = true : highlightSelection = false;
          return;
        }

        terrainEditMode == TerrainEdit::LOWER ? terrainEditMode = TerrainEdit::NONE : terrainEditMode = TerrainEdit::LOWER;
        terrainEditMode == TerrainEdit::NONE ? highlightSelection = true : highlightSelection = false;
      });
    }
    else if (uiElement->getUiElementData().actionID == "QuitGame")
    {
      uiElement->registerCallbackFunction(Signal::slot(Engine::instance(), &Engine::quitGame));
    }
    else if (uiElement->getUiElementData().actionID == "Demolish")
    {
      uiElement->registerCallbackFunction([](UIElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);

        if (button && button->getUiElementData().isToggleButton)
        {
          button->checkState() ? demolishMode = true : demolishMode = false;
          button->checkState() ? highlightSelection = true : highlightSelection = false;
          if (demolishMode)
          {
            GameStates::instance().placementMode = PlacementMode::RECTANGLE;
          }
          return;
        }

        demolishMode = !demolishMode;
        demolishMode ? highlightSelection = true : highlightSelection = false;
      });
    }
    else if (uiElement->getUiElementData().actionID == "ChangeTileType")
    {
      uiElement->registerCallbackFunction([actionParameter](UIElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);

        if (button && button->getUiElementData().isToggleButton)
        {
          button->checkState() ? tileToPlace = actionParameter : tileToPlace = "";
          button->checkState() ? highlightSelection = true : highlightSelection = false;
          if (GameStates::instance().layerEditMode == LayerEditMode::BLUEPRINT)
          {
            GameStates::instance().layerEditMode = LayerEditMode::TERRAIN;
            MapLayers::setLayerEditMode(GameStates::instance().layerEditMode);
          }
          if (!tileToPlace.empty())
          {

            if (TileManager::instance().getTileData(tileToPlace))
              switch (TileManager::instance().getTileData(tileToPlace)->tileType)
              {
              case +TileType::DEFAULT:
                GameStates::instance().placementMode = PlacementMode::SINGLE;
                break;
              case +TileType::ROAD:
              case +TileType::AUTOTILE:
                GameStates::instance().placementMode = PlacementMode::LINE;
                break;
              case +TileType::GROUNDDECORATION:
              case +TileType::WATER:
              case +TileType::ZONE:
                GameStates::instance().placementMode = PlacementMode::RECTANGLE;
                break;
              case +TileType::UNDERGROUND:
                GameStates::instance().placementMode = PlacementMode::LINE;
                GameStates::instance().layerEditMode = LayerEditMode::BLUEPRINT;
                MapLayers::setLayerEditMode(GameStates::instance().layerEditMode);
                break;
              }
          }
          return;
        }

        tileToPlace == actionParameter ? tileToPlace = "" : tileToPlace = actionParameter;
        tileToPlace == actionParameter ? highlightSelection = true : highlightSelection = false;
      });
      if (uiElement->getUiElementData().actionParameter == "underground_pipes")
        uiElement->registerCallbackFunction([actionParameter](UIElement *sender) {
          Button *button = dynamic_cast<Button *>(sender);

          if (button->getButtonState() == ButtonState::BUTTONSTATE_CLICKED)
            GameStates::instance().layerEditMode = LayerEditMode::BLUEPRINT;
          else
            GameStates::instance().layerEditMode = LayerEditMode::TERRAIN;
        });
    }
    else if (uiElement->getUiElementData().actionID == "ToggleVisibilityOfGroup")
    {
      uiElement->registerCallbackFunction(Signal::slot(this, &UIManager::toggleGroupVisibility));
      if (m_layoutGroups.find(uiElement->getUiElementData().actionParameter) != m_layoutGroups.end())
      {
        // set a pointer to the parent element for all UiElements that belong to the group that.
        for (const auto it : m_layoutGroups[uiElement->getUiElementData().actionParameter].uiElements)
        {
          it->setParent(uiElement.get());
        }

        // If we layout a BuildMenu sub item (item that has a buildMenuID), it's layout-parent is always the calling button unless it already has a parentElement assigned
        if (!uiElement->getUiElementData().buildMenuID.empty() &&
            m_layoutGroups[uiElement->getUiElementData().actionParameter].layout.layoutParentElementID.empty())
        {
          m_layoutGroups[uiElement->getUiElementData().actionParameter].layout.layoutParentElementID =
              uiElement->getUiElementData().elementID;
        }
      }
    }
    else if (uiElement->getUiElementData().actionID == "NewGame")
    {
      uiElement->registerCallbackFunction([]() { Engine::instance().newGame(); });
    }
    else if (uiElement->getUiElementData().actionID == "SaveGame")
    {
      uiElement->registerCallbackFunction([]() { Engine::instance().saveGame("resources/save.cts"); });
    }
    else if (uiElement->getUiElementData().actionID == "LoadGame")
    {
      uiElement->registerCallbackFunction([]() { Engine::instance().loadGame("resources/save.cts"); });
    }
    else if (uiElement->getUiElementData().actionID == "SaveSettings")
    {
      uiElement->registerCallbackFunction([]() { Settings::instance().writeFile(); });
    }
    else if (uiElement->getUiElementData().actionID == "ChangeResolution")
    {
      uiElement->registerCallbackFunction(Signal::slot(this, &UIManager::changeResolution));
    }
  }
}

void UIManager::scaleCenterButtonImage(SDL_Rect &ret, int btnW, int btnH, int imgW, int imgH)
{
  if (imgW > imgH)
  {
    float ratio = (float)imgH / (float)imgW;
    ret.w = btnW;
    ret.h = static_cast<int>(ceil(btnH * ratio));
    ret.x = 0;
    ret.y = btnH - ret.h;
  }
  else
  {
    float ratio = (float)imgW / (float)imgH;
    ret.w = static_cast<int>(ceil(btnW * ratio));
    ret.h = btnH;
    ret.x = static_cast<int>(ceil((btnW - ret.w) / 2));
    ret.y = 0;
  }
}

void UIManager::setupButtonTileImage(Button *button, const std::pair<std::string, TileData> &tile)
{
  int bWid = Settings::instance().subMenuButtonWidth;  //UI button width for sub menues
  int bHei = Settings::instance().subMenuButtonHeight; //UI button height for sub menues

  if (TileManager::instance().getTexture(tile.first) == nullptr)
  {
    button->setTextureID("Button_NoIcon");
  }
  SDL_Rect destRect{button->getUiElementRect().x, button->getUiElementRect().y, 0, 0};
  scaleCenterButtonImage(destRect, bWid, bHei, tile.second.tiles.clippingWidth, tile.second.tiles.clippingHeight);
  button->setTextureID(TileManager::instance().getTexture(tile.first),
                       {tile.second.tiles.clippingWidth * tile.second.tiles.offset, 0, tile.second.tiles.clippingWidth,
                        tile.second.tiles.clippingHeight},
                       destRect);
}

void UIManager::createBuildMenu()
{
  std::string subMenuSuffix = "_sub";

  // Create ButtonGroup "_BuildMenu_" for the parent elements
  if (m_buttonGroups.find("_BuildMenu_") == m_buttonGroups.end())
  {
    m_buttonGroups["_BuildMenu_"] = new ButtonGroup;
  }

  // create buttonGroups for all main-buttons in BuildMenu (without the _sub prefix)
  for (const auto &element : m_uiElements)
  {
    std::string parentGroupName = element->getUiElementData().buildMenuID;

    // check if the element is part of the BuildMenu and not of a SubMenu
    if (!parentGroupName.empty() && parentGroupName.find(subMenuSuffix) == std::string::npos &&
        m_buttonGroups.find(parentGroupName) == m_buttonGroups.end())
    {
      m_buttonGroups[parentGroupName] = new ButtonGroup;
    }
  }

  // Add elements from TileData.json to the BuildMenu, if there is a button whose BuildMenuID matches the category
  // check if there's a corresponding category for tiles for this menu ID.
  int idx = 0;

  int bWid = Settings::instance().subMenuButtonWidth;  //UI button width for sub menues
  int bHei = Settings::instance().subMenuButtonHeight; //UI button height for sub menues

  for (auto &tile : TileManager::instance().getAllTileData())
  {
    std::string category = tile.second.category;
    std::string subCategory = tile.second.subCategory;

    // Skip all items that have no button group
    if (category == "Water" || category == "Terrain")
    {
      continue;
    }

    if (m_buttonGroups.find(category) == m_buttonGroups.end())
    {
      std::string newSubCategory = "Debug_" + subCategory + "_sub";
      std::string newCategory = "Debug_" + category + "_sub";
      std::string newParentCategory = "Debug_sub";
      if (m_buttonGroups.find(newCategory) == m_buttonGroups.end())
      {
        m_buttonGroups[newCategory] = new ButtonGroup;
        if (m_buttonGroups.find(newParentCategory) == m_buttonGroups.end())
        {
          m_buttonGroups[newParentCategory] = new ButtonGroup;
        }

        Button *button = new Button({0, 0, bWid, bHei});

        setupButtonTileImage(button, tile);
        button->drawImageButtonFrame(true);
        button->setVisibility(false);
        button->setToggleButton(true);
        button->setActionID("ToggleVisibilityOfGroup");
        button->setActionParameter(newCategory);
        button->setMenuGroupID(newParentCategory);
        button->setTooltipText(category);
        button->setUIElementID(newParentCategory + std::to_string(idx));
        m_buttonGroups[newParentCategory]->addToGroup(button);
        m_uiElements.push_back(std::unique_ptr<UIElement>(dynamic_cast<UIElement *>(button)));
      }
      if (!subCategory.empty())
        continue;

      Button *button = new Button({0, 0, bWid, bHei});

      setupButtonTileImage(button, tile);
      button->drawImageButtonFrame(true);
      button->setVisibility(false);
      button->setToggleButton(true);
      button->setActionID("ChangeTileType");
      button->setActionParameter(tile.first);
      button->setMenuGroupID(newCategory + "_sub");
      button->setTooltipText(tile.second.title);
      button->setUIElementID(newCategory + std::to_string(idx++));
      m_uiElements.push_back(std::unique_ptr<UIElement>(dynamic_cast<UIElement *>(button)));
    }
    else if (subCategory.empty())
    {
      Button *button = new Button({0, 0, bWid, bHei});

      setupButtonTileImage(button, tile);
      button->drawImageButtonFrame(true);
      button->setVisibility(false);
      button->setToggleButton(true);
      button->setActionID("ChangeTileType");
      button->setActionParameter(tile.first);
      button->setMenuGroupID(category + "_sub");
      button->setTooltipText(tile.second.title);

      // Add the newly created button to the container holding all UiElements
      m_uiElements.push_back(std::unique_ptr<UIElement>(dynamic_cast<UIElement *>(button)));
    }
  }
  // SUBCATEGORIES
  for (auto &tile : TileManager::instance().getAllTileData())
  {
    std::string category = tile.second.category;
    std::string subCategory = tile.second.subCategory;
    if (m_buttonGroups.find(subCategory) == m_buttonGroups.end() && !subCategory.empty())
    {
      std::string newCategory = "Debug_sc_" + subCategory + "_sub";
      std::string newParentCategory = "Debug_" + category + "_sub";
      if (m_buttonGroups.find(newCategory) == m_buttonGroups.end())
      {
        m_buttonGroups[newCategory] = new ButtonGroup;
        if (m_buttonGroups.find(newParentCategory) == m_buttonGroups.end())
        {
          m_buttonGroups[newParentCategory] = new ButtonGroup;
        }
        Button *button = new Button({0, 0, bWid, bHei});

        setupButtonTileImage(button, tile);
        button->drawImageButtonFrame(true);
        button->setVisibility(false);
        button->setToggleButton(true);
        button->setActionID("ToggleVisibilityOfGroup");
        button->setActionParameter(newCategory);
        button->setMenuGroupID(newParentCategory + "_sub");
        button->setTooltipText(subCategory);
        button->setUIElementID(newParentCategory + std::to_string(idx));
        m_buttonGroups[newParentCategory]->addToGroup(button);
        m_uiElements.push_back(std::unique_ptr<UIElement>(dynamic_cast<UIElement *>(button)));
      }
      Button *button = new Button({0, 0, bWid, bHei});

      setupButtonTileImage(button, tile);
      button->drawImageButtonFrame(true);
      button->setVisibility(false);
      button->setToggleButton(true);
      button->setActionID("ChangeTileType");
      button->setActionParameter(tile.first);
      button->setMenuGroupID(newCategory + "_sub");
      button->setTooltipText(tile.second.title);
      button->setUIElementID(newCategory + std::to_string(idx++));
      m_uiElements.push_back(std::unique_ptr<UIElement>(dynamic_cast<UIElement *>(button)));
    }
    else if (!subCategory.empty())
    {
      Button *button = new Button({0, 0, bWid, bHei});

      setupButtonTileImage(button, tile);
      button->drawImageButtonFrame(true);
      button->setVisibility(false);
      button->setToggleButton(true);
      button->setActionID("ChangeTileType");
      button->setActionParameter(tile.first);
      button->setMenuGroupID(subCategory + "_sub");
      button->setTooltipText(tile.second.title);

      // Add the newly created button to the container holding all UiElements
      m_uiElements.push_back(std::unique_ptr<UIElement>(dynamic_cast<UIElement *>(button)));
    }
  }
  // SUBCATEGORIES END

  // iterate over all elements and add everything that has a BuildMenu ID.
  for (const auto &element : m_uiElements)
  {
    // Only buttons can be added to the build menu
    Button *button = dynamic_cast<Button *>(element.get());

    // check if the element is part of the BuildMenu
    if (button && !button->getUiElementData().buildMenuID.empty())
    {
      // get all uiElements that have a _sub suffix in their MenuGroupID
      if (utils::strings::endsWith(button->getUiElementData().buildMenuID, subMenuSuffix))
      {
        std::string parentGroupName;
        parentGroupName = element->getUiElementData().buildMenuID;
        utils::strings::removeSubString(parentGroupName, subMenuSuffix);

        // add the element to both buttonGroup and uiGroup container
        if (m_buttonGroups.find(parentGroupName) != m_buttonGroups.end())
        {
          m_buttonGroups[parentGroupName]->addToGroup(button);
          m_uiGroups[parentGroupName].push_back(button);
          addToLayoutGroup(parentGroupName, button);
        }
        else
        {
          LOG(LOG_WARNING) << "Attempting to add element with ID \"" << button->getUiElementData().elementID
                           << "\" to category \"" << parentGroupName << "\" but the Category doesn't exist.";
        }
      }
      //  A base-button toggles a group with the same name as the MenuGroupID, so set ActionID and ActionParameter for all base buttons
      else
      {
        // create buttons in the main menu
        button->setActionID("ToggleVisibilityOfGroup");
        button->setActionParameter(button->getUiElementData().buildMenuID);
        m_buttonGroups["_BuildMenu_"]->addToGroup(button);

        m_uiGroups["_BuildMenu_"].push_back(button);
        addToLayoutGroup("_BuildMenu_", button);
      }
    }
  }

  setBuildMenuLayout();
}

void UIManager::setBuildMenuLayout()
{
  std::string subMenuSuffix = "_sub";
  std::string alignment = "BOTTOM_CENTER";
  std::string layoutType = "HORIZONTAL";
  std::string subMenuAlignment = "HORIZONTAL";

  switch (m_buildMenuLayout)
  {
  case BUILDMENU_LAYOUT::LEFT:
    alignment = "LEFT_CENTER";
    layoutType = "VERTICAL";
    subMenuAlignment = "ALIGN_RIGHT_TO_PARENT";
    break;
  case BUILDMENU_LAYOUT::RIGHT:
    alignment = "RIGHT_CENTER";
    layoutType = "VERTICAL";
    subMenuAlignment = "ALIGN_LEFT_TO_PARENT";
    break;
  case BUILDMENU_LAYOUT::TOP:
    alignment = "TOP_CENTER";
    layoutType = "HORIZONTAL";
    subMenuAlignment = "ALIGN_BENEATH_PARENT";
    break;
  default: // AKA BUILDMENU_LAYOUT::BOTTOM
    alignment = "BOTTOM_CENTER";
    layoutType = "HORIZONTAL";
    subMenuAlignment = "ALIGN_ABOVE_PARENT";
    break;
  }

  // iterate over all elements and add everything that has a BuildMenu ID.
  for (const auto &element : m_uiElements)
  {
    // get all uiElements that have a _sub suffix in their MenuGroupID
    if (utils::strings::endsWith(element->getUiElementData().buildMenuID, subMenuSuffix))
    {
      std::string parentGroupName;
      parentGroupName = element->getUiElementData().buildMenuID;
      utils::strings::removeSubString(parentGroupName, subMenuSuffix);

      // set Layout parameters
      auto &parentLayout = m_layoutGroups[parentGroupName].layout;
      parentLayout.alignment = subMenuAlignment;
      parentLayout.layoutType = layoutType;
      parentLayout.padding = 8;
      parentLayout.paddingToParent = 16;
    }
  }

  auto &layout = m_layoutGroups["_BuildMenu_"].layout;
  layout.alignment = alignment;
  layout.layoutType = layoutType;
  layout.padding = 16;
}

void UIManager::initializeDollarVariables()
{
  // get all elements that start with a dollar.
  for (const auto &it : getAllUiElements())
  {
    if (utils::strings::startsWith(it->getUiElementData().elementID, "$"))
    {
      if (it->getUiElementData().elementID == "$BuildMenuLayout")
      {
        // This must be a ComboBox
        ComboBox *combobox = dynamic_cast<ComboBox *>(it.get());

        if (!combobox)
        {
          LOG(LOG_WARNING) << "Can not use element ID $BuildMenuLayout for an element other than a combobox!";
          continue;
        }

        combobox->clear();
        combobox->addElement("LEFT");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "LEFT")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          m_buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }
        combobox->addElement("RIGHT");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "RIGHT")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          m_buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }
        combobox->addElement("TOP");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "TOP")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          m_buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }
        combobox->addElement("BOTTOM");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "BOTTOM")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          m_buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }

        combobox->registerCallbackFunction(Signal::slot(this, &UIManager::setBuildMenuPosition));
      }

      else if (it->getUiElementData().elementID == "$ScreenResolutionSelector")
      {
        // This must be a ComboBox
        ComboBox *combobox = dynamic_cast<ComboBox *>(it.get());

        if (!combobox)
        {
          LOG(LOG_WARNING) << "Can not use element ID $BuildMenuLayout for an element other than a combobox!";
          continue;
        }

        combobox->clear();
        for (const auto &mode : WindowManager::instance().getSupportedScreenResolutions())
        {
          std::string resolution = std::to_string(mode->w) + " x " + std::to_string(mode->h);
          combobox->addElement(resolution);
          // check if the added element is the active screen resolution
          if (Settings::instance().screenHeight == mode->h && Settings::instance().screenWidth == mode->w)
          {
            combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          }
        }

        combobox->registerCallbackFunction(Signal::slot(this, &UIManager::changeResolution));
      }
      else if (it->getUiElementData().elementID == "$FullScreenSelector")
      {
        // This must be a ComboBox
        ComboBox *combobox = dynamic_cast<ComboBox *>(it.get());

        if (!combobox)
        {
          LOG(LOG_WARNING) << "Can't use element ID FullScreenSelector for an element other than a combobox!";
          continue;
        }

        combobox->clear();
        combobox->addElement("WINDOWED");
        combobox->addElement("BORDERLESS");
        combobox->addElement("FULLSCREEN");

        combobox->setActiveID(Settings::instance().fullScreenMode);
        combobox->registerCallbackFunction(Signal::slot(this, &UIManager::changeFullScreenMode));
      }
    }
  }
}

void UIManager::setBuildMenuPosition(UIElement *sender)
{
  ComboBox *comboBox = dynamic_cast<ComboBox *>(sender);

  if (comboBox)
  {
    Settings::instance().buildMenuPosition = comboBox->activeText;
    m_buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(comboBox->getActiveID());
    setBuildMenuLayout();
    Layout::arrangeElements();
  }
  else
    throw UIError(TRACE_INFO "Only a combobox can have setBuildMenuPosition() as callback function");
}

void UIManager::addToLayoutGroup(const std::string &groupName, UIElement *widget)
{
  if (widget)
  {
    widget->setLayoutGroupName(groupName);
    m_layoutGroups[groupName].uiElements.push_back(widget);
  }
}

void UIManager::changeResolution(UIElement *sender)
{
  // TODO: Save settings
  ComboBox *combobox = dynamic_cast<ComboBox *>(sender);
  WindowManager::instance().setScreenResolution(combobox->getActiveID());
  Layout::arrangeElements();

  if (Engine::instance().map != nullptr)
  {
    Engine::instance().map->refresh();
  }
}

void UIManager::changeFullScreenMode(UIElement *sender)
{
  // TODO: Save settings
  ComboBox *combobox = dynamic_cast<ComboBox *>(sender);
  WindowManager::instance().setFullScreenMode(static_cast<FULLSCREEN_MODE>(combobox->getActiveID()));
  WindowManager::instance().setLastScreenResolution();
  Layout::arrangeElements();

  if (Engine::instance().map != nullptr)
  {
    Engine::instance().map->refresh();
  }
}
