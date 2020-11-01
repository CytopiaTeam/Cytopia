#include "Layout.hxx"

#include "../../util/LOG.hxx"
#include "../../util/Exception.hxx"
#include "../../basics/Settings.hxx"
#include "../../UIManager.hxx"
#include "../../view/Window.hxx"

void Layout::arrangeElements()
{
  if(!m_Window) {
    throw CytopiaError{TRACE_INFO "Cannot arrange UI elements without a Window"};
  }
  int screenWidth = m_Window->getBounds().width();
  int screenHeight = m_Window->getBounds().height();
  const SDL_Point screenCenter{screenWidth / 2, screenHeight / 2};
  const SDL_Point screenSize{screenWidth, screenHeight};

  calculateLayoutGroupDimensions();

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

    int currentLength = 0;
    // Set layout for all non-child elements
    for (const auto &element : group.second.uiElements)
    {

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
          y = screenHeight - groupLayout.groupHeight;
        }
        else if (groupLayout.layoutType == "VERTICAL")
        {
          yOffset = screenHeight - groupLayout.groupHeight;
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
          x = screenCenter.x - groupLayout.groupWidth / 2;
          y = static_cast<int>(yOffset + currentLength);
        }
      }
      else if (groupLayout.alignment == "BOTTOM_CENTER")
      {
        if (groupLayout.layoutType == "HORIZONTAL")
        {
          xOffset = screenCenter.x - groupLayout.groupWidth / 2;
          x = static_cast<int>(xOffset + currentLength);
          y = screenHeight - groupLayout.groupHeight;
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
          x = screenWidth - element->getUiElementRect().w;
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

      element->setPosition(x, y);

      // add the distance from the current element for the next element.
      if (groupLayout.layoutType == "HORIZONTAL")
      {
        currentLength += (element->getUiElementRect().w + groupLayout.padding);
      }
      else
      {
        currentLength += (element->getUiElementRect().h + groupLayout.padding);
      }
    }
  }

  // Take care of elements that are aligned to a parent
  for (auto &group : UIManager::instance().getAllLayoutGroups())
  {
    LayoutData &groupLayout = group.second.layout;

    if (!groupLayout.layoutParentElementID.empty())
    {
      arrangeChildElements(groupLayout, group.second.uiElements);
    }
  }
}

void Layout::arrangeChildElements(LayoutData &groupLayout, std::vector<UIElement *> groupElements)
{
  
  int screenWidth = m_Window->getBounds().width();
  int screenHeight = m_Window->getBounds().height();
  int xOffset = 0;
  int yOffset = 0;
  int x = 0;
  int y = 0;

  if (groupLayout.layoutType == "VERTICAL")
  {
    yOffset = m_Window->getBounds().height() / 2 - groupLayout.groupHeight / 2;
  }

  int currentLength = 0;

  // This loop handles child elements
  for (const auto &element : groupElements)
  {
    // Align elements to it's parent
    if (groupLayout.layoutParentElementID.empty())
    {
      continue;
    }

    // get parent element and check if it exists
    UIElement *parentElement = UIManager::instance().getUiElementByID(groupLayout.layoutParentElementID);
    if (!parentElement)
    {
      LOG(LOG_WARNING) << "Cannot align element " << element->getUiElementData().elementID
                       << " to a parent because it has no ParentElementID set!";
      continue;
    }

    //make sure that the parent is processed first, if the parent also has a parent
    if (parentElement->getUiElementData().parent)
    {
      std::string parentsLayoutGroup = parentElement->getUiElementData().layoutGroupName;
      LayoutData lay = UIManager::instance().getAllLayoutGroups()[parentsLayoutGroup].layout;

      std::vector<UIElement *> parentsUiElements = UIManager::instance().getAllLayoutGroups()[parentsLayoutGroup].uiElements;

      arrangeChildElements(lay, parentsUiElements);
    }

    // Align the element to its parent
    if (groupLayout.alignment == "ALIGN_ABOVE_PARENT" || groupLayout.alignment == "ALIGN_BENEATH_PARENT")
    {
      xOffset = (parentElement->getUiElementRect().x - groupLayout.groupWidth / 2) + parentElement->getUiElementRect().w / 2;
      if (xOffset < 0 || xOffset + groupLayout.groupWidth > screenWidth)
        xOffset = (screenWidth / 2) - groupLayout.groupWidth / 2;

      //LOG(LOG_INFO) << "X Offset " << xOffset;
      yOffset = 0;
    }
    else if (groupLayout.alignment == "ALIGN_RIGHT_TO_PARENT" || groupLayout.alignment == "ALIGN_LEFT_TO_PARENT")
    {
      xOffset = 0;
      yOffset = (parentElement->getUiElementRect().y - groupLayout.groupHeight / 2) + parentElement->getUiElementRect().h / 2;
    }

    // Handling for items that are aligned to a parent

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
      x = (parentElement->getUiElementRect().x + parentElement->getUiElementRect().w + groupLayout.padding);
      y = static_cast<int>(yOffset + currentLength);
    }
    else if (groupLayout.alignment == "ALIGN_LEFT_TO_PARENT")
    {
      x = (parentElement->getUiElementRect().x - element->getUiElementRect().w - groupLayout.padding);
      y = static_cast<int>(yOffset + currentLength);
    }

    element->setPosition(x, y);

    // add the distance from the current element for the next element.
    if (groupLayout.layoutType == "HORIZONTAL")
    {
      currentLength += (element->getUiElementRect().w + groupLayout.padding);
    }
    else
    {
      currentLength += (element->getUiElementRect().h + groupLayout.padding);
    }
  }
}

void Layout::calculateLayoutGroupDimensions()
{
  // First loop gets total width / height for all layouted groups
  for (auto &group : UIManager::instance().getAllLayoutGroups())
  {
    LayoutData &groupLayout = group.second.layout;

    // skip elements without layout parameters
    if (groupLayout.layoutType.empty() || groupLayout.alignment.empty())
    {
      continue;
    }
    // reset group Height / Width
    groupLayout.groupWidth = 0;
    groupLayout.groupHeight = 0;

    // arrange elements in group
    for (const auto &element : group.second.uiElements)
    {
      // calculate total width / height for all elements
      if (groupLayout.layoutType == "HORIZONTAL")
      {
        groupLayout.groupWidth += element->getUiElementRect().w;
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
}

void Layout::setWindow(Window * window)
{
  m_Window = window;
}

Layout::~Layout()
{
  LOG(LOG_INFO) << "Destroying Layout";
}
