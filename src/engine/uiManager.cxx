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

  std::ifstream i(Settings::instance().settings.uiLayoutJSONFile);
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
        bool toggleButton = false;
        bool drawFrame = false;
        std::string uiElementID;
        std::string actionID;
        std::string parentOf;
        std::string tooltipText;
        std::string text;
        std::string textureID;

        SDL_Rect elementRect = {0, 0, 0, 0};

        // Each element must have x and y values
        elementRect.x = uiLayout[it.key()][id]["Position_x"].get<int>();
        elementRect.y = uiLayout[it.key()][id]["Position_y"].get<int>();

        if (!uiLayout[it.key()][id]["Width"].is_null())
        {
          elementRect.w = uiLayout[it.key()][id]["Width"].get<int>();
        }
        if (!uiLayout[it.key()][id]["Height"].is_null())
        {
          elementRect.h = uiLayout[it.key()][id]["Height"].get<int>();
        }
        if (!uiLayout[it.key()][id]["SpriteID"].is_null())
        {
          textureID = uiLayout[it.key()][id]["SpriteID"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["TooltipText"].is_null())
        {
          tooltipText = uiLayout[it.key()][id]["TooltipText"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["ToggleButton"].is_null())
        {
          toggleButton = uiLayout[it.key()][id]["ToggleButton"].get<bool>();
        }
        if (!uiLayout[it.key()][id]["ParentOfGroup"].is_null())
        {
          parentOf = uiLayout[it.key()][id]["ParentOfGroup"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["Action"].is_null())
        {
          actionID = uiLayout[it.key()][id]["Action"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["Text"].is_null())
        {
          text = uiLayout[it.key()][id]["Text"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["ID"].is_null())
        {
          uiElementID = uiLayout[it.key()][id]["ID"].get<std::string>();
        }
        if (!uiLayout[it.key()][id]["DrawFrame"].is_null())
        {
          drawFrame = uiLayout[it.key()][id]["DrawFrame"].get<bool>();
        }

        std::shared_ptr<UiElement> uiElement;
        // Create the ui elements
        if (uiLayout[it.key()][id]["Type"] == "ImageButton")
        {
          uiElement = std::make_shared<Button>(elementRect);
          uiElement->setTextureID(textureID);
        }
        if (uiLayout[it.key()][id]["Type"] == "TextButton")
        {
          uiElement = std::make_shared<Button>(elementRect);
          uiElement->setText(text);
        }
        if (uiLayout[it.key()][id]["Type"] == "Text")
        {
          uiElement = std::make_shared<Text>(elementRect);
          uiElement->setText(text);
        }
        if (uiLayout[it.key()][id]["Type"] == "Frame")
        {
          uiElement = std::make_shared<Frame>(elementRect);
        }
        if (uiLayout[it.key()][id]["Type"] == "Checkbox")
        {
          uiElement = std::make_shared<Checkbox>(elementRect);
        }
        if (uiLayout[it.key()][id]["Type"] == "ComboBox")
        {
          uiElement = std::make_shared<ComboBox>(elementRect);
          uiElement->setText(text);
        }

        uiElement->setVisibility(visible);
        uiElement->setTooltipText(tooltipText);
        uiElement->setActionID(actionID);
        uiElement->setParentID(parentOf);
        uiElement->setGroupID(groupID);
        uiElement->setToggleButton(toggleButton);
        uiElement->setUIElementID(uiElementID);
        uiElement->drawImageButtonFrame(drawFrame);

        if (!parentOf.empty())
        {
          uiElement->registerToggleUIFunction(Signal::slot(this, &UIManager::toggleGroupVisibility));
        }
        if (actionID == "RaiseTerrain")
        {
          uiElement->registerCallbackFunction([]() {
            terrainEditMode == TerrainEdit::RAISE ? terrainEditMode = TerrainEdit::NONE : terrainEditMode = TerrainEdit::RAISE;
            terrainEditMode == TerrainEdit::NONE ? highlightSelection = false : highlightSelection = true;
          });
        }
        else if (actionID == "LowerTerrain")
        {
          uiElement->registerCallbackFunction([]() {
            terrainEditMode == TerrainEdit::LOWER ? terrainEditMode = TerrainEdit::NONE : terrainEditMode = TerrainEdit::LOWER;
            terrainEditMode == TerrainEdit::NONE ? highlightSelection = false : highlightSelection = true;
          });
        }
        else if (actionID == "QuitGame")
        {
          uiElement->registerCallbackFunction(Signal::slot(Engine::instance(), &Engine::quitGame));
        }
        else if (actionID == "Demolish")
        {
          uiElement->registerCallbackFunction([]() {
            demolishMode = !demolishMode;
            demolishMode ? highlightSelection = true : highlightSelection = false;
          });
        }
        else if (actionID == "ChangeTileType")
        {
          std::string type = uiLayout[it.key()][id].value("TileType", "");
          uiElement->registerCallbackFunction([type]() {
            tileTypeEditMode == type ? tileTypeEditMode = "" : tileTypeEditMode = type;
            tileTypeEditMode == type ? highlightSelection = true : highlightSelection = false;
          });
        }
        // store the element in a vector
        _uiElements.emplace_back(uiElement);
      }
    }
  }
  _tooltip->setVisibility(false);
}

void UIManager::setFPSCounterText(const std::string &fps) { _fpsCounter->setText(fps); }

void UIManager::drawUI()
{
  for (const auto &it : _uiElements)
  {
    if (it->isVisible())
    {
      it->draw();
    }
  }

  if (_showDebugMenu)
  {
    _fpsCounter->draw();
  }
  _tooltip->draw();
}

void UIManager::toggleGroupVisibility(const std::string &groupID)
{
  for (const std::shared_ptr<UiElement> &it : _uiElements)
  {
    if (it->getUiElementData().groupName == groupID)
    {
      it->setVisibility(!it->isVisible());
    }
  }
}

void UIManager::startTooltip(SDL_Event &event, const std::string &tooltipText)
{
  _tooltip->setText(tooltipText);

  _tooltip->setPosition(event.button.x - _tooltip->getUiElementRect().w / 2, event.button.y - _tooltip->getUiElementRect().h);

  _tooltip->startTimer();
}

void UIManager::stopTooltip() { _tooltip->reset(); }

std::shared_ptr<UiElement> UIManager::getUiElementByID(const std::string &UiElementID)
{
  for (auto &it : _uiElements)
  {
    if (it->getUiElementData().elementID == UiElementID)
      return it;
  }
  return nullptr;
}
