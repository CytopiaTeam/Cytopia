#include "MenuGroupBuild.hxx"
#include "../../basics/log.hxx"
#include "../../basics/settings.hxx"
#include "../../tileManager.hxx"
#include "../../basics/signal.hxx"

void MenuGroupBuild::draw() const
{
  for (const auto &it : m_groupElements)
  {
    if (it->isVisible())
    {
      it->draw();
    }
  }
  for (auto it : m_buildSubMenuGroups)
  {
    for (auto uielement : it.second->getAllButtons())
    {
      if (uielement->isVisible())
      {
        uielement->draw();
      }
    }
  }
}

void MenuGroupBuild::constructMenu()
{
  std::string stringToCut = "_sub";
  std::string::size_type sizeOfStringToCut;
  std::string newString;

  for (auto it : m_groupElements)
  {
    // TODO: replace by an enum, when BetterEnums is added.
    // TODO: parse all non - _sub elements first, to make sure the main group exists.
    if (!it->getUiElementData().menuGroupID.empty())
    {
      sizeOfStringToCut = it->getUiElementData().menuGroupID.find("_sub");
      // check if it the button is a submenu
      if (it->getUiElementData().menuGroupID.find(stringToCut) == std::string::npos)
      {
        // create an empty ButtonGroup in the m_buildSubmenuGroups, so we know that elements can be placed here
        m_buildSubMenuGroups[it->getUiElementData().menuGroupID] = new ButtonGroup;
        m_buildMenuGroup->addToGroup(it);
      }
      else
      {
        newString = it->getUiElementData().menuGroupID;

        newString.erase(sizeOfStringToCut, stringToCut.size());
        if (m_buildSubMenuGroups.find("newString") != m_buildSubMenuGroups.end())
        {
          m_buildSubMenuGroups[newString]->addToGroup(it);
        }
        else
        {
          LOG(LOG_ERROR) << "You are trying to add an UiElement to the Group " << newString
                         << ", but the main group does not exist.";
        }
      }
    }
  }

  for (auto it : m_buildMenuGroup->getAllButtons())
  {
    // check if there's a corresponding category for tiles for this menu ID.
    for (auto &tile : TileManager::instance().getAllTileData())
    {
      if (tile.second.category == it->getUiElementData().menuGroupID)
      {
        //TODO: take care of ownership / add the created elements to UiManager to properly delete them
        Button *button = new Button({0, 0, 0, 0});

        // TODO: Check if icon empty.
        button->setTextureID("Button_NoIcon");
        button->drawImageButtonFrame(true);
        button->setVisibility(false);
        button->setToggleButton(true);

        if (m_buildSubMenuGroups[tile.second.category])
        {
          m_buildSubMenuGroups[tile.second.category]->addToGroup(button);
        }
        else
        {
          LOG(LOG_ERROR) << "Attempting to add element " << tile.first << " to category " << tile.second.category
                         << ". But the Category doesn't exist.";
        }
        LOG() << "Found matching category " << tile.first;
      }
    }
  }

  // set actionID
  for (auto it : m_buildMenuGroup->getAllButtons())
  {

    if (m_buildSubMenuGroups.count(it->getUiElementData().menuGroupID))
    {
      it->setActionID("ToggleVisibilityOfGroup");
      it->setActionParameter(it->getUiElementData().menuGroupID);
      LOG() << "Adding action for: " << it->getUiElementData().menuGroupID;
    }
  }

  arrangeElements();
}

void MenuGroupBuild::arrangeElements()
{
  SDL_Point screenCenter{Settings::instance().settings.screenWidth / 2, Settings::instance().settings.screenHeight / 2};
  int width = 0;
  //TODO: Make Padding an attribute
  int padding = 16;
  int x = 0;

  // get width for all main elements
  for (auto it : m_buildMenuGroup->getAllButtons())
  {
    width += it->getUiElementRect().w;
  }
  // add padding between main elements to width
  width -= static_cast<int>(padding * (m_buildMenuGroup->count()-1));

  int xOffset = screenCenter.x - width;
  int currentElement = 1;

  // set position for main elements
  for (auto it : m_buildMenuGroup->getAllButtons())
  {
    int elementWidth = it->getUiElementRect().w;
    if (!it->getUiElementData().menuGroupID.empty())
    {
      x = static_cast<int>(xOffset + (elementWidth * currentElement) + elementWidth / 4 * (currentElement - 1));
      it->setPosition(x, screenCenter.y);
      currentElement++;
    }
  }

  // loop for re-arranging buttons that are in subgroups.
  for (auto it : m_buildSubMenuGroups)
  {
    int number = 1;
    for (auto button : m_buildSubMenuGroups[it.first]->getAllButtons())
    {
      button->setPosition(screenCenter.x - 40 * number, screenCenter.y);
      number++;
    }
  }
}