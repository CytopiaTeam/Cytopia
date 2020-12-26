#include "iMouseHandler.hxx"
#include "../util/iShape.hxx"
#include "../util/LOG.hxx"
#include "../events/MouseEvents.hxx"

iMouseHandler::~iMouseHandler() = default;

void iMouseHandler::onMouseClick(class ClickEvent &&) { }
void iMouseHandler::onMouseHover() { }
void iMouseHandler::onMouseLeave() { }
void iMouseHandler::onScroll(ScrollEvent &&) { }
