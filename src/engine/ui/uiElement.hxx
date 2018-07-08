#ifndef UIELEMENT_HXX_
#define UIELEMENT_HXX_

#include <string>
#include "../basics/point.hxx"
#include "../basics/resources.hxx"
#include "../textureManager.hxx"
#include "../basics/log.hxx"

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

/** @brief Base class for UI Elements
  * Provides functions and variables for derived user interface elements.
  */
class UiElement
{
public:
  //Initializes variables
  UiElement(const SDL_Rect &uiElementRect);
  virtual ~UiElement() = default;

  /** \brief Draw the UI Element and/or render it's textures to the screen
    * Renders the texture of the Ui Element. Function is over
    */
  virtual void draw();

  /** \brief Sets the x,y position of this ui element
    * Sets the x,y position of the ui element
    * @param x the new x coordinate of the ui element
    * @param y the new y coordinate of the ui element
    */
  void setPosition(int x, int y)
  {
    _uiElementRect.x = x;
    _uiElementRect.y = y;
  };

  /** \brief Get the position and the size of this ui element
    * Gets the position and the size of this ui element
    * @return Position and size as SDL_Rect
    */
  SDL_Rect getUiElementRect() { return _uiElementRect; };

  /** \brief Checks if the current UI Element is clicked
    * Check if the coordinates match the ones stored in _uiElementRect
    * @param x, y coordinates of the mouseclick
    */
  bool isClicked(int x, int y);

  /** \brief Check the UI Elements visibility.
    * Check if the UI Element is visibile
    * @return Visibility of the UI Element.
    */
  bool isVisible() { return _visible; };

  /** \brief Sets the UI Elements visibility.
  * Set the UI Elements visibility.
  * @param Visibility of the UI Element.
  */
  void setVisibility(bool visibility) { _visible = visibility; };

  /** \brief Set the Group ID (name) of the UI Element.
  * Sets the Name of the group the UI Elements belongs to.
  * Group ID Name must be the same as used for groups in the UI Layout JSON File.
  * @param The name of the group the element nbelongs to as string
  */
  void setGroupID(const std::string &groupName) { _groupName = groupName; };

  /** \brief Get the Group ID of the UI Element.
  * Retrieves the ID of the group the UI Elements belongs to.
  * Group ID Name must be the same as used for groups in the UI Layout JSON File.
  * @return The name of the group the element nbelongs to as string
  */
  std::string getGroupID() { return _groupName; };

  /** \brief Set the ParentOf ID of the UI Element.
  * Retrieves the ID of the group that the UI Element is the parent of.
  * Group ID Name must be the same as used for groups in the UI Layout JSON File.
  * For more details see our github wiki page
  * @param The parentOf ID as string
  */
  void setParentID(const std::string &parentOf) { _parentOf = parentOf; };

  /** \brief Get the ParentOf ID of the UI Element.
  * Retrieves the ID of the group that the UI Element is the parent of. 
  * Group ID Name must be the same as used for groups in the UI Layout JSON File.
  * For more details see our github wiki page
  * @return The parentOf ID as string
  */
  std::string getParentID() { return _parentOf; };

  /** \brief Set the Action ID of the UI Element.
  * Sets the ID of the action the UI Element should execute when it's clicked.
  * For more details see our github wiki page
  * @param The Action ID as int
  */
  void setActionID(int actionID) { _actionID = actionID; };

  /** \brief Get the Action ID of the UI Element.
  * Retrieves the ID of the action the UI Element should execute when it's clicked.
  * For more details see our github wiki page
  * @return The Action ID as int
  */
  int getActionID() { return _actionID; };

  /** \brief Set the button state
  * Sets the mouse button pressed state. 
  * @param state enum buttonstate
  * @see TextureManager::buttonState
  */
  void changeButtonState(int state);

  /** \brief Get the button state
  * Get the current mouse button pressed state.
  * @return state enum buttonstate
  * @see TextureManager::buttonState
  */
  int getButtonState() { return _buttonState; };

  /** \brief Set the button mode to toggleButton
  * Set the button mode to toggle button, which means it stays pressed when its clicked.
  * @param toggleable if this is button a togglebutton as bool
  */
  void setToggleButton(bool toggleable) { _toggleButton = toggleable; };

  /** \brief Get the button mode (check if it is a toggleButton)
  * Get the button mode (wether this is a toggle button or not), which means it stays pressed when its clicked.
  * @return toggleable if this is button a togglebutton as bool
  */
  bool isToogleButton() { return _toggleButton; };

  /** \brief Set the uiElements tooltip text
  * Set the tooltip text for this ui element. Tooltip is shown when hovering over a ui Element.
  * @param tooltiptext as string
  */
  void setTooltipText(const std::string &text) { _tooltipText = text; };

  /** \brief Get the uiElements tooltip text
  * Get the tooltip text for this ui element. Tooltip is shown when hovering over a ui Element.
  * @return tooltiptext as string
  */
  std::string getTooltipText() { return _tooltipText; };

  /** \brief Set the uiElements sprite ID
  * Set the sprite ID this ui element. The texture will be retrieved from the textureManager for the according ID.
  * spriteIDs are mapped to image files in the UIData.json File in the resources/data dir.
  * @param spriteID that should be used for this element as int.
  */
  void setSpriteID(int uiSpriteID);

  void setText(const std::string &text);

  void drawButtonFrame(SDL_Rect rect, bool isHighlightable = true);

private:
  SDL_Texture *_texture = nullptr;
  SDL_Renderer *_renderer = Resources::getRenderer();
  SDL_Window *_window = Resources::getWindow();
  TTF_Font *_font;

  /// This rect represents the whole ui element
  SDL_Rect _uiElementRect{0, 0, 0, 0};
  /// This rect represents the text texture only
  SDL_Rect _textRect{0, 0, 0, 0};

  SDL_Surface *_surface = nullptr;

  SDL_Color _color{255, 255, 255};

  int _buttonState = TextureManager::DEFAULT;

  /// set to -1 for no sprite texture
  int _uiSpriteID = -1;
  int _uiID;
  int _actionID = 0;
  bool _visible = true;

  std::string _tooltipText = "";
  std::string _parentOf = "";
  std::string _groupName = "";
  std::string _text = "";
  /// is this a toggle button
  bool _toggleButton = false;
  bool _textBlittedToTexture = false;

protected:
  void renderTexture();

  /** \brief Draws a filled rectangle to the screen.
  * Draws a filled rectangle to the screen.
  * @param rect the coordinates of the shape to draw in SDL_Rect format.
  * @param color The color for the rectangle.
  */
  void drawSolidRect(SDL_Rect rect, const SDL_Color &color);

  /** \brief Draws a line to the screen.
  * Draws a line to the screen.
  * @param x1, y1 The lines starting point.
  * @param x2, y2 The lines ending point.
  * @param color The color of the line.
  */
  void drawLine(int x1, int y1, int x2, int y2, const SDL_Color &color);

  /** \brief Draws a text string to the screen
  * Draws a text string to the screen. This could either be a standalone text, a text on another texture
  * or a text on a GUI Element drawn with shape functions.
  * @param text Text to draw
  * @param textColor Color that should be used for the text in SDL_Color format
  */
  void drawText(const std::string &text, const SDL_Color &textColor);

  /** \brief Draws a frame around a textfield
  * Draws a frame around around an existing textfield. This function cannot be used without an existing textfield.
  */
  void drawTextFrame();

  /** \brief Change the Texture of the button
  * Changes the texture.
  * @param texture A texture in SDL_Texture* format
  */
  void changeTexture(SDL_Texture *texture) { _texture = texture; };
};

#endif