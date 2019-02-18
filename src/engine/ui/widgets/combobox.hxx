#ifndef COMBOBOX_HXX_
#define COMBOBOX_HXX_

#include <memory>

#include "../basics/uiElement.hxx"
#include "textField.hxx"

class ComboBox : public UiElement
{
public:
  ComboBox(const SDL_Rect &uiElementRect);
  ~ComboBox() override = default;

  void onMouseButtonUp(const SDL_Event &event) override;
  void onMouseButtonDown(const SDL_Event &event) override;
  void onMouseLeave(const SDL_Event &event) override;
  void onMouseMove(const SDL_Event &event) override;

  void draw() override;

  /** \brief Get ID of selected element
  * returns the ID of the selected element in the comboBox
  * @return selected ID
  */
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

  bool _isMenuOpened = false;
};

#endif