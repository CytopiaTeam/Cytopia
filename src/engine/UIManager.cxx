#include "UIManager.hxx"

#include "ResourcesManager.hxx"
#include "Engine.hxx"
#include "Map.hxx"
#include "basics/mapEdit.hxx"
#include "basics/Settings.hxx"
#include "basics/utils.hxx"
#include "basics/LOG.hxx"

#include "../ThirdParty/json.hxx"

#ifdef MICROPROFILE_ENABLED
#include "microprofile.h"
#endif

using json = nlohmann::json;

void UIManager::init()
{
  json uiLayout;

  std::ifstream i(SDL_GetBasePath() + Settings::instance().uiLayoutJSONFile);

  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << Settings::instance().uiLayoutJSONFile << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without textureData we can't continue
    return;
  }

  // check if json file can be parsed
  uiLayout = json::parse(i, nullptr, false);

  if (uiLayout.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << Settings::instance().uiLayoutJSONFile;
    return;
  }

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
          LOG(LOG_ERROR) << "LayoutGroup " << layoutGroupName
                         << " has no parameter \"LayoutType\" set. Check your UiLayout.json file.";
          continue;
        }

        if (layoutGroup.layout.alignment.empty())
        {
          LOG(LOG_ERROR) << "LayoutGroup " << layoutGroupName
                         << " has no parameter \"Alignment\" set. Check your UiLayout.json file.";
          continue;
        }

        // set parent and check if the element exists
        layoutGroup.layout.layoutParentElementID = uiLayout["LayoutGroups"][it.key()][id].value("LayoutParentElementID", "");

        if (!layoutGroup.layout.layoutParentElementID.empty())
        {
          if (!getUiElementByID(layoutGroup.layout.layoutParentElementID))
          {
            LOG(LOG_ERROR) << "Non existing UIElement with ID " << layoutGroup.layout.layoutParentElementID
                           << "has been set for LayoutGroup " << layoutGroupName;
            continue;
          }
        }

        layoutGroup.layout.padding = uiLayout["LayoutGroups"][it.key()][id].value("Padding", 0);
        layoutGroup.layout.paddingToParent = uiLayout["LayoutGroups"][it.key()][id].value("PaddingToParent", 0);
        layoutGroup.layout.alignmentOffset = uiLayout["LayoutGroups"][it.key()][id].value("AlignmentOffset", 0.0f);

        // add layout group information to container
        m_layoutGroups[layoutGroupName] = layoutGroup;
      }
      else
      {
        LOG(LOG_ERROR) << "Cannot add a Layout Group without a name. Check your UiLayout.json file.";
      }
    }
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
        if (uiElementType.empty())
        {
          LOG(LOG_ERROR) << "An element without a type can not be created, check your UiLayout JSON File "
                         << Settings::instance().uiLayoutJSONFile;
          continue;
        }
        else if (uiElementType == "ImageButton")
        {
          uiElement = std::make_unique<Button>(elementRect);
          uiElement->setTextureID(textureID);
        }
        else if (uiElementType == "TextButton")
        {
          uiElement = std::make_unique<Button>(elementRect);
          uiElement->setText(text);
        }
        else if (uiElementType == "Text")
        {
          uiElement = std::make_unique<Text>(elementRect);
          uiElement->setText(text);
        }
        else if (uiElementType == "Frame")
        {
          uiElement = std::make_unique<Frame>(elementRect);
        }
        else if (uiElementType == "Checkbox")
        {
          uiElement = std::make_unique<Checkbox>(elementRect);
        }
        else if (uiElementType == "ComboBox")
        {
          uiElement = std::make_unique<ComboBox>(elementRect);
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
            LOG(LOG_ERROR) << "Cannot add elements to non existing LayoutGroup " << layoutGroupName
                           << ". Add the group first, before trying to add elements to it.";
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
  if (groupID.empty())
  {
    LOG(LOG_ERROR) << "UI Element with Action \"ToggleVisibilityOfGroup\" was called without a parameter.";
    return;
  }

  if (sender)
  {
    Button *button = dynamic_cast<Button *>(sender);

    if (button)
    {
      // cast the object to a Button to check if it's a toggle button.
      if (button->getUiElementData().isToggleButton)
      {
        for (const auto &it : m_uiGroups[groupID])
        {
          it->setVisibility(button->checkState());
        }

        return;
      }
    }
  }

  for (const auto &it : m_uiGroups[groupID])
  {
    it->setVisibility(!it->isVisible());
  }
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
  {
    if (it->getUiElementData().elementID == UiElementID)
    {
      return it.get();
    }
  }

  return nullptr;
}

const std::vector<UIElement *> *UIManager::getUiElementsOfGroup(const std::string &groupID) const
{
  if (m_uiGroups.find(groupID) != m_uiGroups.end())
  {
    return &m_uiGroups.find(groupID)->second;
  }

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
          button->checkState() ? tileTypeEditMode = actionParameter : tileTypeEditMode = "";
          button->checkState() ? highlightSelection = true : highlightSelection = false;
          return;
        }

        tileTypeEditMode == actionParameter ? tileTypeEditMode = "" : tileTypeEditMode = actionParameter;
        tileTypeEditMode == actionParameter ? highlightSelection = true : highlightSelection = false;
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
  for (auto &tile : TileManager::instance().getAllTileData())
  {
    std::string category = tile.second.category;

    // Skip all items that have no button group
    if (category == "Water" || category == "Terrain")
    {
      continue;
    }

    if (m_buttonGroups.find(category) == m_buttonGroups.end())
    {
      std::string newCategory = "Debug_" + category;
      if (m_buttonGroups.find("Debug_" + category) == m_buttonGroups.end())
      {
        m_buttonGroups["Debug_" + category] = new ButtonGroup;
        if (m_buttonGroups.find("Debug_sub") == m_buttonGroups.end())
        {
          m_buttonGroups["Debug_sub"] = new ButtonGroup;
        }
        Button *button = new Button({0, 0, 0, 0});

        button->setTextureID("Button_NoIcon");
        button->drawImageButtonFrame(true);
        button->setVisibility(false);
        button->setToggleButton(true);
        button->setActionID("ToggleVisibilityOfGroup");
        button->setActionParameter(newCategory);
        button->setMenuGroupID("Debug_sub");
        button->setTooltipText(category);
        button->setUIElementID("Debug_sub" + std::to_string(idx));
        m_buttonGroups["Debug_sub"]->addToGroup(button);
        m_uiElements.push_back(std::unique_ptr<UIElement>(dynamic_cast<UIElement *>(button)));
      }
      Button *button = new Button({0, 0, 0, 0});

      button->setTextureID("Button_NoIcon");
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
    else
    {
      Button *button = new Button({0, 0, 0, 0});

      // TODO: Check if icon empty.

      // Set button properties
      button->setTextureID("Button_NoIcon");
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
          LOG(LOG_ERROR) << "Attempting to add element with ID \"" << button->getUiElementData().elementID << "\" to category \""
                         << parentGroupName << "\" but the Category doesn't exist.";
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

  switch (buildMenuLayout)
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
  default:
  case BUILDMENU_LAYOUT::BOTTOM:
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
          LOG(LOG_ERROR) << "Can not use element ID $BuildMenuLayout for an element other than a combobox!";
          continue;
        }

        combobox->clear();
        combobox->addElement("LEFT");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "LEFT")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }
        combobox->addElement("RIGHT");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "RIGHT")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }
        combobox->addElement("TOP");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "TOP")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }
        combobox->addElement("BOTTOM");
        // TODO: #97 Ugly workaround until we have BetterEnums
        if (Settings::instance().buildMenuPosition == "BOTTOM")
        {
          combobox->setActiveID(static_cast<int>(combobox->count() - 1));
          buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(static_cast<int>(combobox->count() - 1));
        }

        combobox->registerCallbackFunction(Signal::slot(this, &UIManager::setBuildMenuPosition));
      }

      else if (it->getUiElementData().elementID == "$ScreenResolutionSelector")
      {
        // This must be a ComboBox
        ComboBox *combobox = dynamic_cast<ComboBox *>(it.get());

        if (!combobox)
        {
          LOG(LOG_ERROR) << "Can not use element ID $BuildMenuLayout for an element other than a combobox!";
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
          LOG(LOG_ERROR) << "Can not use element ID FullScreenSelector for an element other than a combobox!";
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
    buildMenuLayout = static_cast<BUILDMENU_LAYOUT>(comboBox->getActiveID());
    setBuildMenuLayout();
    Layout::arrangeElements();
  }
  else
  {
    LOG(LOG_ERROR) << "Only a combobox can have setBuildMenuPosition() as callback function";
  }
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
}

void UIManager::changeFullScreenMode(UIElement *sender)
{
  // TODO: Save settings
  ComboBox *combobox = dynamic_cast<ComboBox *>(sender);
  WindowManager::instance().setFullScreenMode(static_cast<FULLSCREEN_MODE>(combobox->getActiveID()));
  Layout::arrangeElements();
}
