#include <catch2/catch.hpp>
#include "../../src/engine/ui/widgets/Text.hxx"
#include "../../src/engine/basics/Settings.hxx"
#include "../../src/Game.hxx"
#include "../../GameFixture.hxx"

using string = std::string;

TEST_CASE_METHOD(GameFixture, "I can create a Text widget", "[ui][widgets][text]")
{
  std::unique_ptr<Text> text = std::make_unique<Text>();
  CHECK(text != nullptr);
  CHECK(text->getUiElementData().text.empty());
}

TEST_CASE_METHOD(GameFixture, "I can set text on a Text widget", "[ui][widgets][text]")
{
  std::unique_ptr<Text> text = std::make_unique<Text>();
  text->setText("Test");
  CHECK(text->getUiElementData().text == "Test");
}

TEST_CASE_METHOD(GameFixture, "I can change the font size of a Text widget", "[ui][widgets][text]")
{
  std::unique_ptr<Text> text = std::make_unique<Text>();
  text->setText("Test");
  CHECK(text->getFontSize() == 20);
  text->setFontSize(30);
  CHECK(text->getFontSize() == 30);
  text->setFontSize(100);
  CHECK(text->getUiElementData().text == "Test");
  CHECK(text->getFontSize() == 100);
}

TEST_CASE_METHOD(GameFixture, "I can draw a Text widget", "[ui][widgets][text]")
{
  std::unique_ptr<Text> text = std::make_unique<Text>();
  text->draw();
  text->setText("This is a test");
  text->draw();
  //@todo actually verify it draws the text by creating reference screenshots and comparing it.
  CHECK_NOTHROW(text->draw());
  CHECK(text != nullptr);
}

TEST_CASE_METHOD(GameFixture, "I can't load the font", "[ui][widgets]")
{
  std::unique_ptr<Text> text = std::make_unique<Text>();
  Settings::instance().fontFileName = "I DO NOT EXIST";
  text->draw();
  //@todo This should throw an exception, and you should catch it here.
  CHECK(text != nullptr);
}

//@todo Add a test where an invisible object is drawn and compare if it has been drawn correctly
//@todo Add a test for when moFileReader.Lookup() returns something different (in .setText)
