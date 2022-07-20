#ifndef UIELEMENT_HXX_
#define UIELEMENT_HXX_

#include <string>

#include "../../basics/mapEdit.hxx"
#include "../../ResourcesManager.hxx"
#include "../../WindowManager.hxx"
// #include "../../basics/signal.hxx"
#include <Signal.hxx>

#include <SDL.h>

/** @brief Base class for UI Elements
  * @details Provides functions and variables for derived user interface elements.
  */
class UIElement
{
protected:
  struct ElementData
  {
    std::string elementID;       ///< ID to reference to this element
    std::string text;            ///< Text label that is shown on the button
    std::string tooltipText;     ///< Tooltip text that is shown when hovering over the button
    std::string actionID;        ///< ID of the action this button invokes
    std::string actionParameter; ///< Parameter for the function that's been called when the button is clicked.
    std::string textureID;       ///< ID of the texture this item uses
    std::string buildMenuID;
    std::string layoutGroupName;
    UIElement *parent = nullptr; ///< The object that toggles this items visibility
  };

  ElementData elementData;

public:
  //Initializes variables
  UIElement() = default;
  explicit UIElement(const SDL_Rect &uiElementRect) : m_uiElementRect(uiElementRect){};
  virtual ~UIElement() = default;

  virtual void registerCallbackFunction(std::function<void()> const &){};
  virtual void registerCallbackFunction(std::function<void(UIElement *sender)> const &){};
  virtual void registerCallbackFunction(std::function<void(const std::string &, UIElement *sender)> const &){};

  // empty virtual function that can be overridden in the derived Ui Elements
  virtual bool onMouseButtonUp(const SDL_Event &) { return false; };
  virtual bool onMouseButtonDown(const SDL_Event &) { return false; };
  virtual void onMouseEnter(const SDL_Event &){};
  virtual void onMouseLeave(const SDL_Event &){};
  virtual void onMouseMove(const SDL_Event &){};
  virtual bool onKeyDown(const SDL_Event &) { return false; };

  /** \brief Draw the UI Element and/or render it's textures to the screen
  * Renders the texture of the Ui Element. Function is over
  */
  virtual void draw(){};

  /** \brief Sets the x,y position of this ui element
  * @param x the new x coordinate of the ui element
  * @param y the new y coordinate of the ui element
  */
  virtual void setPosition(int x, int y)
  {
    m_uiElementRect.x = x;
    m_uiElementRect.y = y;
  };

  /** \brief Get the position and the size of this ui element
  * @return Position and size as SDL_Rect
  */
  const SDL_Rect &getUiElementRect() const { return m_uiElementRect; };

  /** \brief Checks if the mouse cursor is over the current UI Element
  * @details Check if the coordinates match the ones stored in m_uiElementRect
  * @param x, y coordinates of the mouse
  * @return Whether the element is hovered over
  */
  virtual bool isMouseOver(int x, int y);

  /** \brief Handling for the hovering event
  * @details Check if the mouse cursor is hovering over the element. Virtual function to
  * provide custom handling if necessary (e.g. Combobox) 
  * Per default, it's the same implementation as isMouseOver
  * @param x, y coordinates of the mouseclick
  * @return Whether the element is hovered over
  */
  virtual bool isMouseOverHoverableArea(int x, int y);

  /** \brief Check the UI Element's visibility.
  * @return Visibility of the UI Element.
  */
  bool isVisible() { return m_visible; };

  /** \brief Sets the UI Element's visibility.
  * @param visibility a boolean value to use as element visibility
  */
  void setVisibility(bool visibility) { m_visible = visibility; };

  /** \brief Set the Action ID of the UI Element.
  * @details Sets the ID of the action the UI Element should execute when it's clicked.
  * For more details see our github wiki page
  * @param The Action ID as int
  */
  void setActionID(const std::string &actionID) { elementData.actionID = actionID; };

  /** \brief Set the Action Parameter of the UI Element.
  * @details Sets the Parameter of the action the UI Element should execute when it's clicked.
  * For more details see our github wiki page
  * @param The Action ID as int
  */
  void setActionParameter(const std::string &actionParameter) { elementData.actionParameter = actionParameter; };

  /** \brief Set the uiElements tooltip text
  * @details Set the tooltip text for this ui element. Tooltip is shown when hovering over a ui Element.
  * @param text string to use as the tooltip text
  */
  void setTooltipText(const std::string &text) { elementData.tooltipText = text; };

  /** \brief Get the uiElements tooltip text
  * @details Get the tooltip text for this ui element. Tooltip is shown when hovering over a ui Element.
  * @param elementID string that will be the elements new ID
  */
  void setUIElementID(const std::string &elementID) { elementData.elementID = elementID; };

  /** \brief Set the uiElements texture ID
  * Set the texture ID this ui element. The texture will be retrieved from the textureManager for the according ID.
  * textureIDs are mapped to image files in the UIData.json file in the resources/data dir.
  * @param textureID the ID of the new texture
  */
  void setTextureID(const std::string &textureID);

  //Just incase you want to set the texture directly
  void setTextureID(SDL_Texture *texture, const SDL_Rect &clipRect, const SDL_Rect &textureRect);

  /**
  * @brief sets the element's parent element
  * @param parent the element to be added as a parent to the current element
  */
  void setParent(UIElement *parent) { elementData.parent = parent; };
  void setMenuGroupID(const std::string &buildMenuID) { elementData.buildMenuID = buildMenuID; };
  void setLayoutGroupName(const std::string &layoutGroupName) { elementData.layoutGroupName = layoutGroupName; };

  const ElementData &getUiElementData() const { return elementData; };

  virtual SDL_Point screenPointToLocalPointInRect(SDL_Point screenCoordinates);

private:
  SDL_Renderer *m_renderer = WindowManager::instance().getRenderer();
  SDL_Window *m_window = WindowManager::instance().getWindow();

  bool m_visible = true;

  bool m_directTexture = false; //was this texture set directly?

protected:
  SDL_Texture *m_texture = nullptr; ///< a pointer to the element's texture
  SDL_Rect m_uiElementRect{0, 0, 0, 0};
  SDL_Rect m_uiElementClipRect{0, 0, 0, 0};
  SDL_Rect m_uiTextureRect{0, 0, 0, 0};

  void renderTexture() const;

  /** \brief Draws a filled rectangle to the screen.
  * Draws a filled rectangle to the screen.
  * @param rect the coordinates of the shape to draw in SDL_Rect format.
  * @param color the color for the rectangle.
  */
  void drawSolidRect(SDL_Rect rect, const SDL_Color &color) const;

  /** \brief Draws a line to the screen.
  * @param x1, y1 The line's starting point.
  * @param x2, y2 The line's ending point.
  * @param color The color of the line.
  */
  void drawLine(int x1, int y1, int x2, int y2, const SDL_Color &color) const;

  /** \brief Draws a frame around a textfield
  * @details Draws a frame around around an existing textfield. This function cannot be used without an existing textfield.
  */
  void drawTextFrame() const;

  void drawButtonFrame(SDL_Rect rect, bool isHighlightable = true);

  void drawFrame(SDL_Rect rect) const;

  /** \brief Change the Texture of the button
  * @param texture A texture in SDL_Texture* format
  */
  void changeTexture(SDL_Texture *texture) { m_texture = texture; };
};

#endif
