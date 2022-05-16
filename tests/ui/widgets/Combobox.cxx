#include <catch.hpp>
#include <SDL_ttf.h>
#include "../../src/engine/ui/widgets/Combobox.hxx"

TEST_CASE("I can create a Combobox widget", "[ui][widgets][combobox]")
{
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  CHECK(cb != nullptr);
  CHECK(cb->count() == 0);
  // that there is no active text
  CHECK(cb->getActiveID() == -1);
}

TEST_CASE("I can draw a Combobox widget", "[ui][widgets][combobox]")
{
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  //@todo actually verify it draws the combobox
  CHECK_NOTHROW(cb->draw());
}

TEST_CASE("I can reposition a Combobox widget", "[ui][widgets][combobox]")
{
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  cb->setPosition(5, 5);
  CHECK(cb->getUiElementRect().x == 5);
  CHECK(cb->getUiElementRect().y == 5);
  // TODO: Check that the text is also repositioned correctly. if possible.
}

TEST_CASE("I can add an element to a Combobox widget", "[ui][widgets][combobox]")
{
  TTF_Init();
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  int oldCount = cb->count();
  int oldID = cb->getActiveID();
  std::string oldText = cb->getActiveText();
  cb->addElement("New element");
  CHECK(cb->count() == oldCount + 1);
  CHECK(cb->getTextFromID(oldCount) == "New element");
  // make sure that adding an element doesn't change the selection
  CHECK(cb->getActiveText() == oldText);
  CHECK(cb->getActiveID() == oldID);
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
  CHECK(cb->getActiveID() == -1);
  CHECK(cb->getActiveText() == "");
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
  CHECK(cb->getActiveText() == "Active");
}

TEST_CASE("Getting the text of an element outside of range of a Combobox widget returns an empty string", "[ui][widgets][combobox]")
{
  std::unique_ptr<ComboBox> cb = std::make_unique<ComboBox>(SDL_Rect{0, 0, 0, 0});
  CHECK(cb->getTextFromID(-5) == "");
  CHECK(cb->getTextFromID(cb->count() + 10) == "");
}