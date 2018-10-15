#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include <memory>

#include "uiElement.hxx"
#include "textField.hxx"

class ComboBox : public UiElement
{
public:
  ComboBox(const SDL_Rect &uiElementRect);
  ~ComboBox() override = default;

  void onMouseButtonUp(const SDL_Event &event) override;
  void onMouseButtonDown(const SDL_Event &event) override;
  void onMouseEnter(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;

  void draw() override;
  int getClickedID(int x, int y) override;

  int getActiveID() { return _activeID; };
  std::string getActiveText() { return _activeText; };

  bool isMouseOver(int x, int y) override;
  bool isMouseOverHoverableArea(int x, int y) override;

private:
  SDL_Rect _comboBoxRect;
  SDL_Rect _menuRect;

  std::unique_ptr<TextField> _textField;

  std::string _activeText;
  int _activeID = 0;

  bool _isPressed = false;
  bool _isMouseOver = false;
  bool _isMenuOpened = false;
};

#endif