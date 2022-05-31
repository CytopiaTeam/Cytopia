#include <catch.hpp>
#include <limits>

#include "../../src/services/AudioMixer.hxx"
#include "../../src/engine/basics/Settings.hxx"
#include "Exception.hxx"

TEST_CASE("Get AudioMixer instance", "[engine][audio]")
{
  void *singleton1 = static_cast<void *>(&AudioMixer::instance());
  void *singleton2 = static_cast<void *>(&AudioMixer::instance());
  CHECK(singleton1 == singleton2);
}

SCENARIO("I can change the music volume", "[engine][audio]")
{
  GIVEN("The music volume is currently 0")
  {
    AudioMixer::instance().setMusicVolume(0);
    REQUIRE(Settings::instance().musicVolume == 0);
    WHEN("I set the music volume to 0.5")
    {
      AudioMixer::instance().setMusicVolume(0.5);
      THEN("My music volume setting becomes 0.5")
      {
        CHECK(Settings::instance().musicVolume == 0.5);
      }
    }
  }
  GIVEN("The music volume is currently 0.5")
  {
    AudioMixer::instance().setMusicVolume(0.5);
    REQUIRE(Settings::instance().musicVolume == 0.5);
    WHEN("I set the music volume to 0")
    {
      AudioMixer::instance().setMusicVolume(0);
      THEN("My music volume setting becomes 0")
      {
        CHECK(Settings::instance().musicVolume == 0); 
      }
    }
  }
}

SCENARIO("I can change the sound effects volume", "[engine][audio]")
{
  GIVEN("The sound effects volume is currently 0")
  {
    AudioMixer::instance().setSoundEffectVolume(0);
    REQUIRE(Settings::instance().soundEffectsVolume == 0);
    WHEN("I set the sound effects volume to 0.5")
    {
      AudioMixer::instance().setSoundEffectVolume(0.5);
      THEN("My sound effects volume setting becomes 0.5")
      {
        CHECK(Settings::instance().soundEffectsVolume == 0.5); 
      }
    }
  }
  GIVEN("The sound effects volume is currently 0.5")
  {
    AudioMixer::instance().setSoundEffectVolume(0.5);
    REQUIRE(Settings::instance().soundEffectsVolume == 0.5);
    WHEN("I set the sound effects volume to 0")
    {
      AudioMixer::instance().setSoundEffectVolume(0);
      THEN("My sound effects volume setting becomes 0")
      {
        CHECK(Settings::instance().soundEffectsVolume == 0);
      }
    }
  }
}

TEST_CASE("AudioMixer throws UnimplementedError when setMuted is called", "[engine][audio]") 
{
  REQUIRE_THROWS_AS(AudioMixer::instance().setMuted(true), UnimplementedError);
  REQUIRE_THROWS_AS(AudioMixer::instance().setMuted(false), UnimplementedError);
}