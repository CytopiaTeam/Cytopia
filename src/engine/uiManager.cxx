#include "uiManager.hxx"

#include "resourcesManager.hxx"
#include "engine.hxx"
#include "map.hxx"

#include "basics/mapEdit.hxx"
#include "basics/settings.hxx"
#include "basics/log.hxx"

#include "../ThirdParty/json.hxx"

using json = nlohmann::json;

void UIManager::init()
{
  json uiLayout;

  //char*  full_text = new char[strlen(base_path) + strlen(SETTINGS_FILE_NAME) + 1];
  //strcpy(full_text, SDL_GetBasePath());
  //strcat(full_text, SETTINGS_FILE_NAME);
  //std::ifstream i(full_text);
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
        std::string layout = uiLayout[it.key()][id].value("Layout", "");

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

        if (!layout.empty())
        {
          m_buttonGroups[layout].addToGroup(dynamic_cast<Button *>(uiElement.get()));
        }
        else
        {
          m_uiElementsWithoutGroup.push_back(uiElement.get());
        }

        if (!groupID.empty())
        {
          m_uiGroups[groupID].push_back(uiElement.get());
        }

        if (actionID == "RaiseTerrain")
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
        else if (actionID == "LowerTerrain")
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
        else if (actionID == "QuitGame")
        {
          uiElement->registerCallbackFunction(Signal::slot(Engine::instance(), &Engine::quitGame));
        }
        else if (actionID == "Demolish")
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
        else if (actionID == "ChangeTileType")
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
        else if (actionID == "ToggleVisibilityOfGroup")
        {
          uiElement->registerCallbackFunction(Signal::slot(this, &UIManager::toggleGroupVisibility));
        }
        else if (actionID == "SaveGame")
        {
          uiElement->registerCallbackFunction([]() { Engine::instance().saveGame("resources/save.cts"); });
        }
        else if (actionID == "LoadGame")
        {
          uiElement->registerCallbackFunction([]() { Engine::instance().loadGame("resources/save.cts"); });
        }
        // store the element in a vector
        m_uiElements.emplace_back(std::move(uiElement));
      }
    }
  }
  m_tooltip->setVisibility(false);
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
