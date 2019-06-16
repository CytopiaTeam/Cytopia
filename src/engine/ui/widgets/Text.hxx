#ifndef TEXT_HXX_
#define TEXT_HXX_

#include <string>

#include "../basics/UIElement.hxx"

class Text : public UIElement
{
public:
  Text() = default;
  Text(const SDL_Rect &uiElementRect) : UIElement(uiElementRect){};
  Text(const SDL_Rect &uiElementRect, const std::string &text);
  Text(const std::string &text);
  ~Text() override = default;

  void setText(const std::string &text);

private:
  /** \brief Draws a text string to the screen
  * Draws a text string to the screen. This could either be a standalone text, a text on another texture
  * or a text on a GUI Element drawn with shape functions.
  * @param text Text to draw
  * @param textColor Color that should be used for the text in SDL_Color format
  */
  void createTextTexture(const std::string &text, const SDL_Color &textColor);
};

#endif