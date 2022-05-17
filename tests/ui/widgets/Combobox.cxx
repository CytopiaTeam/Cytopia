#include <catch.hpp>
#include <SDL_ttf.h>
#include "../../src/engine/ui/widgets/Combobox.hxx"

TEST_CASE("I can create a Combobox widget", "[ui][widgets][combobox]")
{
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  CHECK(cb != nullptr);
  CHECK(cb->count() == 0);
  // that there is no active text
  // no active item
}

TEST_CASE("I can add an element to a Combobox widget", "[ui][widgets][combobox]")
{
  TTF_Init();
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  int oldCount = cb->count();
  cb->addElement("New element");
  CHECK(cb->count() == oldCount + 1);
  CHECK(cb->getTextFromID(oldCount) == "New element");
}

TEST_CASE("I can clear the elements of a Combobox widget", "[ui][widgets][combobox]") 
{
  TTF_Init();
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  for (int x = 0; x < 5; x++)
    cb->addElement("Element");
  CHECK(cb->count() != 0);
  cb->clear();
  CHECK(cb->count() == 0);
}

TEST_CASE("I can set the active element of a Combobox widget", "[ui][widgets][combobox]") 
{
  TTF_Init();
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  cb->addElement("Inactive");
  cb->addElement("Active");
  cb->addElement("Inactive");
  cb->setActiveID(1);
  CHECK(cb->getActiveID() == 1);
  CHECK(cb->getTextFromID(1) == "Active");
  CHECK(cb->activeText == "Active");
}