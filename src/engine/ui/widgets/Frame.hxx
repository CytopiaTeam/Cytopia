#ifndef FRAME_HXX_
#define FRAME_HXX_

#include "../basics/UIElement.hxx"

/**
 * @brief A Frame GUI Widget
 */
class Frame : public UIElement
{
public:
  Frame(const SDL_Rect &uiElementRect);
  ~Frame() override = default;

  void draw() override;
  bool isMouseOverHoverableArea(int, int) override { return false; };
};

#endif