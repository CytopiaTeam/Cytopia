#include "MenuGroupBuild.hxx"
#include "../../basics/log.hxx"
#include "../../basics/settings.hxx"

void MenuGroupBuild::draw() const
{
  for (const auto &it : m_groupElements)
  {
    it->draw();
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
        m_buildSubMenuGroups[newString]->addToGroup(it);
        //LOG() << "found a submenu!" << it->getUiElementData().menuGroupID;
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
  }
}
