#ifndef TEXT_HXX_
#define TEXT_HXX_

#include <string>

#include "../basics/UIElement.hxx"

class Text : public UIElement
{
public:
  /**
   * @brief default Constructor
   */
  Text() = default;
  /**
   * @brief Constructs element at a certain position
   */
  Text(const SDL_Rect &uiElementRect) : UIElement(uiElementRect){};
  /**
   * @brief Constructs element at a certain position with text
   */
  Text(const SDL_Rect &uiElementRect, const std::string &text);
  Text(const std::string &text);

  /**
   * @brief Destructor
   */
  ~Text() override = default;

  /**
   * @brief implements draw function. Renders the SDL_Texture generated from the TTF.
   */
  void draw() override;

  /**
   * @brief sets the text for this text widget.
   * @param text The text that should be set
   */
  void setText(const std::string &text);

  /**
   * @brief Sets the Font size of this element
   * @param fontSize the size the font should be set to.
   */
  void setFontSize(int fontSize);

  int getFontSize() const { return m_fontSize; }

private:
  /** \brief Draws a text string to the screen
  * Draws a text string to the screen. This could either be a standalone text, a text on another texture
  * or a text on a GUI Element drawn with shape functions.
  * @param text Text to draw
  * @param textColor Color that should be used for the text in SDL_Color format
  */
  void createTextTexture(const std::string &text, const SDL_Color &textColor);

  int m_fontSize = 20;
};

#endif