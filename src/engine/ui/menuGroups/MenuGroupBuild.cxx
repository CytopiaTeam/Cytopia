#include "MenuGroupBuild.hxx"
#include "../../basics/log.hxx"
#include "../../basics/settings.hxx"
#include "../../tileManager.hxx"

void MenuGroupBuild::draw() const
{
  for (const auto &it : m_groupElements)
  {
    it->draw();
  }
  for (auto it : m_buildSubMenuGroups)
  {
    it.second->draw();
  }
}

void MenuGroupBuild::constructMenu()
{
  SDL_Point screenCenter{Settings::instance().settings.screenWidth / 2, Settings::instance().settings.screenHeight / 2};
  SDL_Rect elementSize{0, 0, 0, 0};

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
        elementSize = it->getUiElementRect();
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

  // this only works for one element. get width in a for loop instead.
  int width = static_cast<int>(m_buildMenuGroup->count() * elementSize.w - elementSize.w / 4 * (m_buildMenuGroup->count() - 1));
  int startX = screenCenter.x - width;
  int elementNumber = 1;
  for (auto it : m_buildMenuGroup->getAllButtons())
  {
    if (!it->getUiElementData().menuGroupID.empty())
    {
      int x = static_cast<int>(startX + (elementSize.w * elementNumber) + elementSize.w / 4 * (elementNumber - 1));
      it->setPosition(x, screenCenter.y);
      elementNumber++;
    }
    // check if there's a corresponding category for tiles for this menu ID.
    for (auto &tile : TileManager::instance().getAllTileData())
    {
      if (tile.second.category == it->getUiElementData().menuGroupID)
      {
        //TODO: take care of ownership / add the created elements to UiManager to properly delete them
        Button *button = new Button({0, 0, 0, 0});

        // TODO: Check if icon empty.
        button->setTextureID("Button_NoIcon");
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

  // loop for re-arranging buttons that are in subgroups.
  for (auto it : m_buildSubMenuGroups)
  {
    //m_buildMenuGroup->
    int number = 1;
    for (auto button : m_buildSubMenuGroups[it.first]->getAllButtons())
    {
      button->setPosition(screenCenter.y - 30, 40 * number);
      number++;
    }
    LOG() << "Size of " << it.first << " is " << m_buildSubMenuGroups[it.first]->count();
  }
}
