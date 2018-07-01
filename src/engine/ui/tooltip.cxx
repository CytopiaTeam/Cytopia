#include "tooltip.hxx"

void Tooltip::draw()
{
  // only draw tooltip, if the mouse hovers longer then 1,5 seconds over a button.
  if (_tooltipTimer.getDeltaTime() >= 1500)
  {
    drawTextFrame();
    renderTexture();
  }
}
void Tooltip::setText(const std::string &text)
{
  _toolTipText.assign(text);
  drawText(_toolTipText, SDL_Color{255, 255, 255});
}
