#include "tooltip.hxx"

Tooltip::Tooltip() : UiElement(0, 0, 0, 0, -1, 0, 0, "") {}

void Tooltip::draw()
{
  // only draw tooltip, if the mouse hovers longer then 1,5 seconds over a button.
  if (tooltipTimer.getDeltaTime() >= 1500)
  {
    drawTextFrame();
    renderTexture();
  }
}
void Tooltip::setText(const std::string &text)
{
  //_toolTipText.assign(text);
  drawText(text, SDL_Color{255, 255, 255});
}

void Tooltip::startTimer() { tooltipTimer.start(); }
