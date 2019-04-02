#include "uiManager.hxx"

#include "resourcesManager.hxx"
#include "engine.hxx"
#include "map.hxx"

#include "basics/mapEdit.hxx"
#include "basics/settings.hxx"
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
  }

  for (const auto &it : uiLayout.items())
  {
    std::string groupID;
    groupID = it.key();

    bool visible = false;

    for (size_t id = 0; id < uiLayout[it.key()].size(); id++)
    {

      if (!uiLayout[it.key()][id]["groupVisibility"].is_null())
      {
        visible = uiLayout[it.key()][id]["groupVisibility"].get<bool>();
      }

      if (!uiLayout[it.key()][id]["Type"].is_null())
      {
        bool toggleButton = uiLayout[it.key()][id].value("ToggleButton", false);
        bool drawFrame = uiLayout[it.key()][id].value("DrawFrame", false);
        std::string uiElementID = uiLayout[it.key()][id].value("ID", "");
        std::string actionID = uiLayout[it.key()][id].value("Action", "");
        std::string actionParameter = uiLayout[it.key()][id].value("ActionParameter", "");
        std::string tooltipText = uiLayout[it.key()][id].value("TooltipText", "");
        std::string text = uiLayout[it.key()][id].value("Text", "");
        std::string textureID = uiLayout[it.key()][id].value("SpriteID", "");
        std::string uiElementType = uiLayout[it.key()][id].value("Type", "");
        std::string buttonGroupID = uiLayout[it.key()][id].value("ButtonGroup", "");
        std::string buildMenuID = uiLayout[it.key()][id].value("BuildMenuID", "");

        SDL_Rect elementRect{0, 0, 0, 0};
        elementRect.x = uiLayout[it.key()][id].value("Position_x", 0);
        elementRect.y = uiLayout[it.key()][id].value("Position_y", 0);
        elementRect.w = uiLayout[it.key()][id].value("Width", 0);
        elementRect.h = uiLayout[it.key()][id].value("Height", 0);

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
          uiElement->setText(text);
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
            m_uiGroups[groupID].uiElements.push_back(uiElement.get());
          }
        }

        // store the element in a vector
        m_uiElements.emplace_back(std::move(uiElement));
      }
    }
  }
  m_tooltip->setVisibility(false);

  createBuildMenu();
  setCallbackFunctions();

  // Add all buttongroups to EventHandling container
  for (auto it : m_buttonGroups)
  {
    m_uiElementsForEventHandling.push_back(it.second);
  }

  Layout layout;
  layout.arrangeElements();
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
        for (const auto &it : m_uiGroups[groupID].uiElements)
        {
          it->setVisibility(button->checkState());
        }
        return;
      }
    }
  }
  for (const auto &it : m_uiGroups[groupID].uiElements)
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
    return &m_uiGroups.find(groupID)->second.uiElements;
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

      if (m_uiGroups.find(uiElement.get()->getUiElementData().actionParameter) != m_uiGroups.end())
      {
        // set a pointer to the parent element for all UiElements that belong to the group that.
        for (const auto it : m_uiGroups[uiElement.get()->getUiElementData().actionParameter].uiElements)
        {
          it->setParent(uiElement.get());
        }

        // If we layout a Buildmenu sub item, it's layout-parent is always the calling button.
        if (m_uiGroups[uiElement.get()->getUiElementData().actionParameter].layout.alignment == "BUILDMENU_SUB")
        {
          m_uiGroups[uiElement.get()->getUiElementData().actionParameter].layout.layoutParentElement =
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

// TODO: Rename to create menugroup buttons
void UIManager::createBuildMenu()
{
  std::string subMenuSuffix = "_sub";
  std::string::size_type sizeOfStringToCut;

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
  for (const auto &element : m_uiElements)
  {
    // check if there's a corresponding category for tiles for this menu ID.
    for (auto &tile : TileManager::instance().getAllTileData())
    {
      if (tile.second.category == element->getUiElementData().buildMenuID)
      {
        Button *button = new Button({0, 0, 0, 0});

        // TODO: Check if icon empty.
        button->setTextureID("Button_NoIcon");
        button->drawImageButtonFrame(true);
        button->setVisibility(false);
        button->setToggleButton(true);

        button->setActionID("ChangeTileType");
        button->setActionParameter(tile.first);
        button->setMenuGroupID(tile.second.category + "_sub");

        m_uiElements.push_back(std::unique_ptr<UiElement>(dynamic_cast<UiElement *>(button)));
      }
    }
  }

  // iterate over all elements and add everything that has a buildMenuID.
  for (const auto &element : m_uiElements)
  {

    // Only buttons can be added to the build menu
    Button *button = dynamic_cast<Button *>(element.get());
    if (button)
    {
      sizeOfStringToCut = button->getUiElementData().buildMenuID.find("_sub");
      // check if the element is part of the BuildMenu
      if (!button->getUiElementData().buildMenuID.empty())
      {
        // get all uiElements that have a _sub suffix in their MenuGroupID
        if (button->getUiElementData().buildMenuID.find(subMenuSuffix) != std::string::npos)
        {
          std::string parentGroupName;
          parentGroupName = element->getUiElementData().buildMenuID;

          parentGroupName.erase(sizeOfStringToCut, subMenuSuffix.size());

          // add the element to both buttonGroup and uiGroup container
          if (m_buttonGroups.find(parentGroupName) != m_buttonGroups.end())
          {
            m_buttonGroups[parentGroupName]->addToGroup(button);
            m_uiGroups[parentGroupName].uiElements.push_back(button);
          }
          else
          {
            LOG(LOG_ERROR) << "Attempting to add element with ID \"" << button->getUiElementData().elementID
                           << "\" to category \"" << parentGroupName << "\" but the Category doesn't exist.";
          }

          // set Layout parameters
          m_uiGroups[parentGroupName].layout.alignment = "BUILDMENU_SUB";
          m_uiGroups[parentGroupName].layout.layoutType = "HORIZONTAL";
          m_uiGroups[parentGroupName].layout.padding = 8;
          m_uiGroups[parentGroupName].layout.paddingToParent = 16;
        }
        //  A base-button toggles a group with the same name as the MenuGroupID, so set ActionID and ActionParameter for all base buttons
        else
        {
          // create buttons in the main menu
          button->setActionID("ToggleVisibilityOfGroup");
          button->setActionParameter(button->getUiElementData().buildMenuID);
          m_buttonGroups["_BuildMenu_"]->addToGroup(button);

          m_uiGroups["_BuildMenu_"].uiElements.push_back(button);
        }
      }
    }
  }
  m_uiGroups["_BuildMenu_"].layout.alignment = "BUILDMENU";
  m_uiGroups["_BuildMenu_"].layout.layoutType = "HORIZONTAL";
  m_uiGroups["_BuildMenu_"].layout.padding = 16;
}