#include "uiManager.hxx"

#include "resourcesManager.hxx"
#include "engine.hxx"
#include "map.hxx"
#include "basics/mapEdit.hxx"
#include "basics/settings.hxx"
#include "basics/utils.hxx"
#include "basics/log.hxx"
#include "ui/basics/Layout.hxx"

#include "../ThirdParty/json.hxx"

using json = nlohmann::json;

void UIManager::init()
{
  json uiLayout;

  std::ifstream i(SDL_GetBasePath() + Settings::instance().settings.uiLayoutJSONFile);
  if (i.fail())
  {
    LOG(LOG_ERROR) << "File " << Settings::instance().settings.uiLayoutJSONFile
                   << " does not exist! Cannot load settings from INI File!";
    // Application should quit here, without textureData we can't continue
    return;
  }

  // check if json file can be parsed
  uiLayout = json::parse(i, nullptr, false);
  if (uiLayout.is_discarded())
  {
    LOG(LOG_ERROR) << "Error parsing JSON File " << Settings::instance().settings.uiLayoutJSONFile;
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
            LOG(LOG_ERROR) << "Non existing UiElement with ID " << layoutGroup.layout.layoutParentElementID
                           << "has been set for LayoutGroup " << layoutGroupName;
            continue;
          }
        }

        layoutGroup.layout.padding = uiLayout["LayoutGroups"][it.key()][id].value("Padding", 0);
        layoutGroup.layout.paddingToParent = uiLayout["LayoutGroups"][it.key()][id].value("PaddingToParent", 0);
        layoutGroup.layout.alignmentOffset = uiLayout["LayoutGroups"][it.key()][id].value("AlignmentOffeset", 0.0f);

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

    // parse UiElements
    for (size_t id = 0; id < uiLayout["UiElements"][it.key()].size(); id++)
    {
      if (!uiLayout["UiElements"][it.key()][id]["GroupVisibility"].is_null())
      {
        visible = uiLayout["UiElements"][it.key()][id]["GroupVisibility"].get<bool>();
      }
      // check if their is a global layoutgroup paramter set
      if (!uiLayout["UiElements"][it.key()][id]["GroupSettings"].is_null())
      {
        layoutGroupName = uiLayout["UiElements"][it.key()][id]["GroupSettings"].value("LayoutGroup", "");
      }
      // check if a single item is associated with a layout group and override group settings, if applicable
      if (!uiLayout["UiElements"][it.key()][id]["LayoutGroup"].is_null())
      {
        layoutGroupName = uiLayout["UiElements"][it.key()][id].value("LayoutGroup", "");
      }

      if (!uiLayout["UiElements"][it.key()][id]["Type"].is_null())
      {
        bool toggleButton = uiLayout["UiElements"][it.key()][id].value("ToggleButton", false);
        bool drawFrame = uiLayout["UiElements"][it.key()][id].value("DrawFrame", false);
        std::string uiElementID = uiLayout["UiElements"][it.key()][id].value("ID", "");
        std::string actionID = uiLayout["UiElements"][it.key()][id].value("Action", "");
        std::string actionParameter = uiLayout["UiElements"][it.key()][id].value("ActionParameter", "");
        std::string tooltipText = uiLayout["UiElements"][it.key()][id].value("TooltipText", "");
        std::string text = uiLayout["UiElements"][it.key()][id].value("Text", "");
        std::string textureID = uiLayout["UiElements"][it.key()][id].value("SpriteID", "");
        std::string uiElementType = uiLayout["UiElements"][it.key()][id].value("Type", "");
        std::string buttonGroupID = uiLayout["UiElements"][it.key()][id].value("ButtonGroup", "");
        std::string buildMenuID = uiLayout["UiElements"][it.key()][id].value("BuildMenuID", "");

        SDL_Rect elementRect{0, 0, 0, 0};
        elementRect.x = uiLayout["UiElements"][it.key()][id].value("Position_x", 0);
        elementRect.y = uiLayout["UiElements"][it.key()][id].value("Position_y", 0);
        elementRect.w = uiLayout["UiElements"][it.key()][id].value("Width", 0);
        elementRect.h = uiLayout["UiElements"][it.key()][id].value("Height", 0);

        std::unique_ptr<UiElement> uiElement;

        // Create the ui elements
        if (uiElementType.empty())
        {
          LOG(LOG_ERROR) << "An element without a type can not be created, check your UiLayout JSON File "
                         << Settings::instance().settings.uiLayoutJSONFile;
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

        // only add UiElements to buttongroups / eventhandling if they have no MenuGroupID property set
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
            m_layoutGroups[layoutGroupName].uiElements.push_back(uiElement.get());
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

  // Add all buttongroups to EventHandling container
  for (auto it : m_buttonGroups)
  {
    m_uiElementsForEventHandling.push_back(it.second);
  }

  Layout::arrangeElements();
}

void UIManager::setFPSCounterText(const std::string &fps) { m_fpsCounter->setText(fps); }

void UIManager::drawUI() const
{
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

void UIManager::toggleGroupVisibility(const std::string &groupID, UiElement *sender)
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

void UIManager::startTooltip(SDL_Event &event, const std::string &tooltipText)
{
  m_tooltip->setText(tooltipText);
  m_tooltip->setPosition(event.button.x - m_tooltip->getUiElementRect().w / 2, event.button.y - m_tooltip->getUiElementRect().h);
  m_tooltip->startTimer();
}

void UIManager::stopTooltip() const { m_tooltip->reset(); }

UiElement *UIManager::getUiElementByID(const std::string &UiElementID) const
{
  for (auto &it : m_uiElements)
  {
    if (it->getUiElementData().elementID == UiElementID)
      return it.get();
  }
  return nullptr;
}

const std::vector<UiElement *> *UIManager::getUiElementsOfGroup(const std::string &groupID) const
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
      uiElement->registerCallbackFunction([](UiElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);
        if (button)
        {
          if (button->getUiElementData().isToggleButton)
          {
            button->checkState() ? terrainEditMode = TerrainEdit::RAISE : terrainEditMode = TerrainEdit::NONE;
            button->checkState() ? highlightSelection = true : highlightSelection = false;
            return;
          }
        }
        terrainEditMode == TerrainEdit::RAISE ? terrainEditMode = TerrainEdit::NONE : terrainEditMode = TerrainEdit::RAISE;
        terrainEditMode == TerrainEdit::NONE ? highlightSelection = false : highlightSelection = true;
      });
    }
    else if (uiElement->getUiElementData().actionID == "LowerTerrain")
    {
      uiElement->registerCallbackFunction([](UiElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);
        if (button)
        {
          if (button->getUiElementData().isToggleButton)
          {
            button->checkState() ? terrainEditMode = TerrainEdit::LOWER : terrainEditMode = TerrainEdit::NONE;
            button->checkState() ? highlightSelection = true : highlightSelection = false;
            return;
          }
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
      uiElement->registerCallbackFunction([](UiElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);
        if (button)
        {
          if (button->getUiElementData().isToggleButton)
          {
            button->checkState() ? demolishMode = true : demolishMode = false;
            button->checkState() ? highlightSelection = true : highlightSelection = false;
            return;
          }
        }

        demolishMode = !demolishMode;
        demolishMode ? highlightSelection = true : highlightSelection = false;
      });
    }
    else if (uiElement->getUiElementData().actionID == "ChangeTileType")
    {
      uiElement->registerCallbackFunction([actionParameter](UiElement *sender) {
        Button *button = dynamic_cast<Button *>(sender);
        if (button)
        {
          if (button->getUiElementData().isToggleButton)
          {
            button->checkState() ? tileTypeEditMode = actionParameter : tileTypeEditMode = "";
            button->checkState() ? highlightSelection = true : highlightSelection = false;
            return;
          }
        }
        tileTypeEditMode == actionParameter ? tileTypeEditMode = "" : tileTypeEditMode = actionParameter;
        tileTypeEditMode == actionParameter ? highlightSelection = true : highlightSelection = false;
      });
    }
    else if (uiElement->getUiElementData().actionID == "ToggleVisibilityOfGroup")
    {
      uiElement->registerCallbackFunction(Signal::slot(this, &UIManager::toggleGroupVisibility));

      if (m_layoutGroups.find(uiElement.get()->getUiElementData().actionParameter) != m_layoutGroups.end())
      {
        // set a pointer to the parent element for all UiElements that belong to the group that.
        for (const auto it : m_layoutGroups[uiElement.get()->getUiElementData().actionParameter].uiElements)
        {
          it->setParent(uiElement.get());
        }

        // If we layout a Buildmenu sub item (item that has a buildMenuID), it's layout-parent is always the calling button unless it already has a parentElement assigned
        if (!uiElement.get()->getUiElementData().buildMenuID.empty() &&
            m_layoutGroups[uiElement.get()->getUiElementData().actionParameter].layout.layoutParentElementID.empty())
        {
          m_layoutGroups[uiElement.get()->getUiElementData().actionParameter].layout.layoutParentElementID =
              uiElement.get()->getUiElementData().elementID;
        }
      }
    }
    else if (uiElement->getUiElementData().actionID == "SaveGame")
    {
      uiElement->registerCallbackFunction([]() { Engine::instance().saveGame("resources/save.cts"); });
    }
    else if (uiElement->getUiElementData().actionID == "LoadGame")
    {
      uiElement->registerCallbackFunction([]() { Engine::instance().loadGame("resources/save.cts"); });
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

  // create buttongroups for all main-buttons in buildmenu (without the _sub prefix)
  for (const auto &element : m_uiElements)
  {
    std::string parentGroupName = element->getUiElementData().buildMenuID;

    // check if the element is part of the BuildMenu and not of a SubMenu
    if (!parentGroupName.empty() && parentGroupName.find(subMenuSuffix) == std::string::npos)
    {
      if (m_buttonGroups.find(parentGroupName) == m_buttonGroups.end())
      {
        m_buttonGroups[parentGroupName] = new ButtonGroup;
      }
    }
  }

  // Add elements from TileData.json to the Buildmenu, if there is a button whose BuildMenuID matches the category
  // check if there's a corresponding category for tiles for this menu ID.
  for (auto &tile : TileManager::instance().getAllTileData())
  {
    std::string category = tile.second.category;

    // Skip all items from specific categories
    if (category == "Water" || category == "Terrain")
    {
      continue;
    }

    // If there's no matching buttongroup, there's also no button. Add them to the debug menu instead
    if (m_buttonGroups.find(category) == m_buttonGroups.end())
    {
      category = "Debug";
    }
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
    m_uiElements.push_back(std::unique_ptr<UiElement>(dynamic_cast<UiElement *>(button)));
  }

  // iterate over all elements and add everything that has a BuildMenu ID.
  for (const auto &element : m_uiElements)
  {
    // Only buttons can be added to the build menu
    Button *button = dynamic_cast<Button *>(element.get());
    if (button)
    {
      // check if the element is part of the BuildMenu
      if (!button->getUiElementData().buildMenuID.empty())
      {
        // get all uiElements that have a _sub suffix in their MenuGroupID
        if (button->getUiElementData().buildMenuID.find(subMenuSuffix) != std::string::npos)
        {
          std::string parentGroupName;
          parentGroupName = element->getUiElementData().buildMenuID;
          utils::strings::removeSubString(parentGroupName, subMenuSuffix);

          // add the element to both buttonGroup and uiGroup container
          if (m_buttonGroups.find(parentGroupName) != m_buttonGroups.end())
          {
            m_buttonGroups[parentGroupName]->addToGroup(button);
            m_uiGroups[parentGroupName].push_back(button);
            m_layoutGroups[parentGroupName].uiElements.push_back(button);
          }
          else
          {
            LOG(LOG_ERROR) << "Attempting to add element with ID \"" << button->getUiElementData().elementID
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
          m_layoutGroups["_BuildMenu_"].uiElements.push_back(button);
        }
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

  if (Settings::instance().settings.buildMenuPosition == "BOTTOM")
  {
    alignment = "BOTTOM_CENTER";
    layoutType = "HORIZONTAL";
    subMenuAlignment = "ALIGN_ABOVE_PARENT";
  }
  else if (Settings::instance().settings.buildMenuPosition == "TOP")
  {
    alignment = "TOP_CENTER";
    layoutType = "HORIZONTAL";
    subMenuAlignment = "ALIGN_BENEATH_PARENT";
  }
  else if (Settings::instance().settings.buildMenuPosition == "LEFT")
  {
    alignment = "LEFT_CENTER";
    layoutType = "VERTICAL";
    subMenuAlignment = "ALIGN_RIGHT_TO_PARENT";
  }
  else if (Settings::instance().settings.buildMenuPosition == "RIGHT")
  {
    alignment = "RIGHT_CENTER";
    layoutType = "VERTICAL";
    subMenuAlignment = "ALIGN_LEFT_TO_PARENT";
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
      m_layoutGroups[parentGroupName].layout.alignment = subMenuAlignment;
      m_layoutGroups[parentGroupName].layout.layoutType = layoutType;
      m_layoutGroups[parentGroupName].layout.padding = 8;
      m_layoutGroups[parentGroupName].layout.paddingToParent = 16;
    }
  }

  m_layoutGroups["_BuildMenu_"].layout.alignment = alignment;
  m_layoutGroups["_BuildMenu_"].layout.layoutType = layoutType;
  m_layoutGroups["_BuildMenu_"].layout.padding = 16;
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

        combobox->addElement("LEFT");
        combobox->addElement("RIGHT");
        combobox->addElement("TOP");
        combobox->addElement("BOTTOM");
      }
    }
  }
}