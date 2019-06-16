#include <catch2/catch.hpp>
#include "../../src/engine/ui/widgets/Text.hxx"
#include "../../src/engine/basics/Settings.hxx"

using string = std::string;

TEST_CASE("I can create a Text widget", "[ui][widgets][text]")
{
  Text *text = new Text;
  CHECK(text != nullptr);
}

TEST_CASE("I can set text on a Text widget", "[ui][widgets][text]")
{
  Text text;
  text.setText("Test");
  CHECK(text.getUiElementData().text == "Test");
}

TEST_CASE("I can change the font size of a Text widget", "[ui][widgets][text]")
{
  Text text;
  text.setFontSize(30);
  text.setText("Test");
  text.setFontSize(100);
  CHECK(text.getUiElementData().text == "Test");
  CHECK(text.getFontSize() == 100);
}

TEST_CASE("I can draw a Text widget", "[ui][widgets][text]")
{
  Text *text = new Text;
  text->draw();
  text->setText("This is a test");
  text->draw();
  CHECK(text != nullptr);
}

TEST_CASE("I can't load the font", "[ui][widgets][!mayfail]")
{
  Text *text = new Text;
  Settings::instance().fontFileName = "I DO NOT EXIST";
  text->draw();
  CHECK(text != nullptr);
}