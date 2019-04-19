#ifndef IMAGE_HXX_
#define IMAGE_HXX_

#include <string>

#include "../basics/uiElement.hxx"

class Image : public UiElement
{
public:
  Image() = default;
  ~Image() override = default;

  void draw() override;

  void setOpacity(Uint8 alphaLevel) { m_alphaLevel = alphaLevel; };

private:
  Uint8 m_alphaLevel = 255;
};

#endif