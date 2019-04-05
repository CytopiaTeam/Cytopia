#include "Layout.hxx"
#include "../../basics/log.hxx"
#include "../../basics/settings.hxx"
#include "../../tileManager.hxx"
#include "../../basics/signal.hxx"
#include "../../uiManager.hxx"

void Layout::arrangeElements()
{
  SDL_Point screenCenter{Settings::instance().settings.screenWidth / 2, Settings::instance().settings.screenHeight / 2};
  SDL_Point screenSize{Settings::instance().settings.screenWidth, Settings::instance().settings.screenHeight};
  // First loop gets total width / height for all layouted groups
  for (auto &group : UIManager::instance().getAllLayoutGroups())
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
        if (groupLayout.groupHeight < element->getUiElementRect().h)
        {
          groupLayout.groupHeight = element->getUiElementRect().h;
        }
      }
      else if (groupLayout.layoutType == "VERTICAL")
      {
        groupLayout.groupHeight += element->getUiElementRect().h;
        if (groupLayout.groupWidth < element->getUiElementRect().w)
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
  for (auto &group : UIManager::instance().getAllLayoutGroups())
  {
    LayoutData &groupLayout = group.second.layout;

    // skip elements without layout parameters
    if (groupLayout.layoutType.empty() || groupLayout.alignment.empty())
    {
      continue;
    }

    int xOffset = 0;
    int yOffset = 0;
    int x = 0;
    int y = 0;

    if (groupLayout.layoutType == "VERTICAL")
    {
      yOffset = screenCenter.y - groupLayout.groupHeight / 2;
    }

    // iterator variable
    int currentElement = 1;
    int currentLength = 0;

    // Set horizontal layout
    for (const auto &element : group.second.uiElements)
    {
      // Align elements to it's parent
      if (!group.second.layout.layoutParentElementID.empty())
      {
        // get parent element and check if it exists
        UiElement *parentElement = UIManager::instance().getUiElementByID(group.second.layout.layoutParentElementID);
        if (!parentElement)
        {
          LOG(LOG_ERROR) << "Cannot align UiGroup " << group.first << " to a parent because it has no ParentElementID set!";
        }

        // Align the element to its parent
        if (groupLayout.alignment == "ALIGN_ABOVE_PARENT" || groupLayout.alignment == "ALIGN_BENEATH_PARENT")
        {
          xOffset = (parentElement->getUiElementRect().x - groupLayout.groupWidth / 2) + parentElement->getUiElementRect().w / 2;
        }
        else if (groupLayout.alignment == "ALIGN_RIGHT_TO_PARENT" || groupLayout.alignment == "ALIGN_LEFT_TO_PARENT")
        {
          yOffset = (parentElement->getUiElementRect().y - groupLayout.groupHeight / 2) + parentElement->getUiElementRect().h / 2;
        }
      }

      if (groupLayout.alignment == "TOP_LEFT")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = 0;
          x = static_cast<int>(xOffset + currentLength);
          y = 0;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = 0;
          x = 0;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "LEFT_CENTER")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = 0;
          x = static_cast<int>(xOffset + currentLength);
          y = screenCenter.y - groupLayout.groupHeight / 2;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = screenCenter.y - groupLayout.groupHeight / 2;
          x = 0;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "BOTTOM_LEFT")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = 0;
          x = static_cast<int>(xOffset + currentLength);
          y = Settings::instance().settings.screenHeight - groupLayout.groupHeight;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = Settings::instance().settings.screenHeight - groupLayout.groupHeight;
          x = 0;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "TOP_CENTER")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = screenCenter.x - groupLayout.groupWidth / 2;
          x = static_cast<int>(xOffset + currentLength);
          y = 0;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = 0;
          x = screenCenter.x - groupLayout.groupWidth / 2;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "CENTER")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = screenCenter.x - groupLayout.groupWidth / 2;
          x = static_cast<int>(xOffset + currentLength);
          y = screenCenter.y;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = screenCenter.y - groupLayout.groupHeight / 2;
          x = screenCenter.x;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "BOTTOM_CENTER")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = screenCenter.x - groupLayout.groupWidth / 2;
          x = static_cast<int>(xOffset + currentLength);
          y = Settings::instance().settings.screenHeight - groupLayout.groupHeight;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = screenSize.y - groupLayout.groupHeight;
          x = screenCenter.x - groupLayout.groupWidth / 2;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "TOP_RIGHT")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = screenSize.x - groupLayout.groupWidth;
          x = static_cast<int>(xOffset + currentLength);
          y = 0;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = 0;
          x = screenSize.x - groupLayout.groupWidth;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "RIGHT_CENTER")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = screenSize.x - groupLayout.groupWidth;
          x = static_cast<int>(xOffset + currentLength);
          y = screenCenter.y - groupLayout.groupHeight / 2;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = screenCenter.y - groupLayout.groupHeight / 2;
          x = Settings::instance().settings.screenWidth - element->getUiElementRect().w;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "BOTTOM_RIGHT")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = screenSize.x - groupLayout.groupWidth;
          x = static_cast<int>(xOffset + currentLength);
          y = screenSize.y - groupLayout.groupHeight;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = screenSize.y - groupLayout.groupHeight;
          x = screenSize.x - groupLayout.groupWidth;
          y = static_cast<int>(yOffset + currentLength);
        }
      }

      // Handling for items that are aligned to a parent
      if (!group.second.layout.layoutParentElementID.empty())
      {
        // get parent element and check if it exists
        UiElement *parentElement = UIManager::instance().getUiElementByID(group.second.layout.layoutParentElementID);
        if (!parentElement)
        {
          LOG(LOG_ERROR) << "Cannot align UiGroup " << group.first << " to a parent because it has no ParentElementID set!";
        }

        if (groupLayout.alignment == "ALIGN_ABOVE_PARENT")
        {
          x = static_cast<int>(xOffset + currentLength);
          y = (parentElement->getUiElementRect().y - groupLayout.groupHeight - groupLayout.paddingToParent);
        }

        else if (groupLayout.alignment == "ALIGN_BENEATH_PARENT")
        {
          x = static_cast<int>(xOffset + currentLength);
          y = (parentElement->getUiElementRect().y + parentElement->getUiElementRect().h + groupLayout.paddingToParent);
        }
        else if (groupLayout.alignment == "ALIGN_RIGHT_TO_PARENT")
        {
          x = (parentElement->getUiElementRect().x + parentElement->getUiElementRect().w + group.second.layout.padding);
          y = static_cast<int>(yOffset + currentLength);
        }
        else if (groupLayout.alignment == "ALIGN_LEFT_TO_PARENT")
        {
          x = (parentElement->getUiElementRect().x - element->getUiElementRect().w - group.second.layout.padding);

          y = static_cast<int>(yOffset + currentLength);
        }
      }

      element->setPosition(x, y);
      currentElement++;
      // add the distance from the current element for the next element.
      currentLength += (element->getUiElementRect().w + groupLayout.padding);
    }
  }
}
