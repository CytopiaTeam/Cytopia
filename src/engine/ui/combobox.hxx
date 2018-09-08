#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include "uiElement.hxx"
#include "textField.hxx"

class ComboBox : public UiElement
{
public:
  ComboBox(const SDL_Rect &uiElementRect);
  ~ComboBox() override = default;

  void draw() override;
  int getClickedID(int x, int y) override;

  // override isClicked function to check for menu too if the combobox's opened
  void clickedEvent(int x, int y) override;

  bool isMouseOver(int x, int y) override;
  bool isHovering(int x, int y) override;

private:
  SDL_Rect _comboBoxRect;
  SDL_Rect menuRect;


  std::shared_ptr<TextField> _textField;

  std::string activeText;

  bool _isPressed = false;
  bool _isMouseOver = false;
  bool isMenuOpened = true;
  int _selectedID = 0;
};

#endif