#include <catch2/catch.hpp>
#include "../../src/engine/ui/widgets/Text.hxx"

using string = std::string;

TEST_CASE("Create a Text widget", "[widgets]")
{
  Text *text = new Text;
  CHECK(text != nullptr);
}

TEST_CASE("I can set text on a Text widget", "[widgets]")
{
  Text text;
  text.setText("Test");
  CHECK(text.getUiElementData().text == "Test");
}

TEST_CASE("I can change the font size of a Text widget", "[widgets]")
{
  Text text;
  text.setFontSize(30);
  text.setText("Test");
  text.setFontSize(100);
  CHECK(text.getUiElementData().text == "Test");
  CHECK(text.getFontSize() == 100);
}
