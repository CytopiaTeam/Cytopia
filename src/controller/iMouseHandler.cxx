#include <SDL2/SDL.h>

#include "iMouseHandler.hxx"
#include "../util/iShape.hxx"
#include "../util/LOG.hxx"

iMouseHandler::~iMouseHandler() = default;

void iMouseHandler::onMouseClick(ClickEvent &&) { }
void iMouseHandler::onMouseHover() { }
void iMouseHandler::onMouseLeave() { }
void iMouseHandler::onScroll(ScrollEvent &&) { }
