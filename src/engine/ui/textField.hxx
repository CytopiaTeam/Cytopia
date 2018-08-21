#ifndef TEXTFIELD_HXX_
#define TEXTFIELD_HXX_

#include "uiElement.hxx"
#include "text.hxx"

class TextField : public UiElement
{
public:
  TextField() = default;
  ~TextField() override = default;

  void addText(std::string text);

  void draw() override;

private:
  std::vector<Text *> _texts;
};

#endif