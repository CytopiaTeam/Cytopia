#include "iMouseHandler.hxx"
#include "../util/iShape.hxx"
#include "../util/LOG.hxx"
#include "../events/MouseEvents.hxx"

iMouseHandler::~iMouseHandler() = default;

void iMouseHandler::onMouseLeftButtonDown(class ClickEvent &&) {}
void iMouseHandler::onMouseMiddleButtonDown(class ClickEvent &&) {}
void iMouseHandler::onMouseRightButtonDown(class ClickEvent &&) {}
void iMouseHandler::onMouseLeftButtonUp(class ClickEvent &&) {}
void iMouseHandler::onMouseMiddleButtonUp(class ClickEvent &&) {}
void iMouseHandler::onMouseRightButtonUp(class ClickEvent &&) {}
void iMouseHandler::onMouseMove(class MousePositionEvent &&) {}
void iMouseHandler::onMouseHover() {}
void iMouseHandler::onMouseLeave() {}
void iMouseHandler::onScroll(ScrollEvent &&) {}
