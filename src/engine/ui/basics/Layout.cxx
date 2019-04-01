#include "Layout.hxx"
#include "../../basics/log.hxx"
#include "../../basics/settings.hxx"
#include "../../tileManager.hxx"
#include "../../basics/signal.hxx"
#include "../../uiManager.hxx"

void Layout::arrangeElements()
{
  SDL_Point screenCenter{Settings::instance().settings.screenWidth / 2, Settings::instance().settings.screenHeight / 2};
  // First loop gets total width / height for all layouted groups
  for (auto &group : UIManager::instance().getAllUiGroups())
  {
    LayoutData &groupLayout = group.second.layout;

    // skip elements without layout parameters
    if (groupLayout.layoutType.empty() || groupLayout.alignment.empty())
    {
      continue;
    }

    // arrange elements in group
    for (const auto &element : group.second.uiElements)
    {

      // calculate total width / height for all elements
      if (groupLayout.layoutType == "HORIZONTAL")
      {
        group.second.layout.groupWidth += element->getUiElementRect().w;
        if (groupLayout.groupHeight > element->getUiElementRect().h)
        {
          groupLayout.groupHeight = element->getUiElementRect().h;
        }
      }
      else if (groupLayout.layoutType == "VERTICAL")
      {
        groupLayout.groupHeight += element->getUiElementRect().h;
        if (groupLayout.groupWidth > element->getUiElementRect().w)
        {
          groupLayout.groupWidth = element->getUiElementRect().w;
        }
      }
    }

    // add total padding to total height / width
    if (groupLayout.layoutType == "HORIZONTAL")
    {
      groupLayout.groupWidth += static_cast<int>(groupLayout.padding * (group.second.uiElements.size() - 1));
    }
    else if (groupLayout.layoutType == "VERTICAL")
    {
      groupLayout.groupHeight += static_cast<int>(groupLayout.padding * (group.second.uiElements.size() - 1));
    }
  }

  // Second loop gets total width / height for all layouted groups
  for (auto &group : UIManager::instance().getAllUiGroups())
  {
    LayoutData &groupLayout = group.second.layout;

    // skip elements without layout parameters
    if (groupLayout.layoutType.empty() || groupLayout.alignment.empty())
    {
      continue;
    }

    int xOffset = 0;
    int yOffset = 0;

    // calculate x starting point
    if (groupLayout.layoutType == "HORIZONTAL")
    {
      xOffset = screenCenter.x - groupLayout.groupWidth / 2;
    }
    else if (groupLayout.layoutType == "VERTICAL")
    {
      yOffset = screenCenter.y - groupLayout.groupHeight;
    }

    // iterator variable
    int currentElement = 1;
    int currentLength = 0;
    int x = 0;

    // set x and y coordinates for each element
    for (const auto &element : group.second.uiElements)
    {
      if (groupLayout.alignment == "CENTER" || groupLayout.alignment == "CENTER_HORIZONTAL" ||
          groupLayout.alignment == "BUILDMENU")
      {
        // start off at the xOffset with the first element.
        x = static_cast<int>(xOffset + currentLength);
      }
      // Special handling for Buildmenu. Note: This should only be used internally
      else if (groupLayout.alignment == "BUILDMENU_SUB")
      {
        // If it's a subelement of the BuildMenu, we need to find the button that toggles it.
        for (auto &it : UIManager::instance().getAllUiElementsForEventHandling())
        {
          ButtonGroup *buttonGroup = dynamic_cast<ButtonGroup *>(it);

          if (buttonGroup)
          {
            // If it's a group, find the matching button
            for (auto &elementInButtonGroup : buttonGroup->getAllButtons())
            {
              if (elementInButtonGroup->getUiElementData().actionParameter == group.first)
              {
                // get parents x coordinate, calculate the center of the button and use it as x Offset
                xOffset = (elementInButtonGroup->getUiElementRect().x - groupLayout.groupWidth / 2) +
                          elementInButtonGroup->getUiElementRect().w / 2;
              }
            }
          }
          else if (it->getUiElementData().actionParameter == group.first)
          {
            // get parents x coordinate, calculate the center of the button and use it as x Offset
            xOffset = it->getUiElementRect().x - groupLayout.groupWidth / 2 + it->getUiElementRect().w / 2;
          }
          x = static_cast<int>(xOffset + currentLength);
        }
      }
      // TODO: special handling for parents
      // TODO: y coordinate
      int y = Settings::instance().settings.screenHeight - element->getUiElementRect().h - groupLayout.paddingParent;
      element->setPosition(x, y);
      currentElement++;
      // add the distance from the current element for the next element.
      currentLength += (element->getUiElementRect().w + groupLayout.padding);
    }
  }
}
