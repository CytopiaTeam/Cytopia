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

  for (auto it : m_groupElements)
  {
    // TODO: replace by an enum, when BetterEnums is added.

    if (!it->getUiElementData().menuGroupID.empty())
    {
      m_buildMenuGroup->addToGroup(it);
      it->setPosition(400, 300);

      // check if it the button is a submenu
      if (!it->getUiElementData().menuGroupID.find("_sub") != std::string::npos)
      {
        // create an empty ButtonGroup in the m_buildSubmenuGroups, so we know that elements can be placed here
        m_buildSubMenuGroups[it->getUiElementData().menuGroupID] = new ButtonGroup;
        m_buildMenuGroup->addToGroup(it);
      }
      else
      {
        LOG() << "found a submenu!" << '\n';
      }
    }
  }

  for (auto it : m_buildMenuGroup->getAllButtons())
  {
    if (!it->getUiElementData().menuGroupID.empty())
    {
    }
  }
}