#include <catch.hpp>
#include <limits>

#include "../../src/services/AudioMixer.hxx"
#include "../../src/engine/basics/Settings.hxx"

template <class T> using Limits = std::numeric_limits<T>;

TEST_CASE("Set the music volume", "[engine][audio]")
{
  /* @todo: Get Service Tuple
  AudioMixer mixer;
  REQUIRE_NOTHROW(mixer.setMusicVolume(10));
  REQUIRE_NOTHROW(mixer.setMusicVolume(0));
  REQUIRE_NOTHROW(mixer.setMusicVolume(Limits<uint8_t>::max()));
  REQUIRE_NOTHROW(mixer.setMusicVolume(Limits<uint8_t>::lowest()));
  */
}

TEST_CASE("Set sound effects volume", "[engine][audio]")
{
  /* @todo: Get Service Tuple
  AudioMixer mixer;
  REQUIRE_NOTHROW(mixer.setSoundEffectVolume(10));
  REQUIRE_NOTHROW(mixer.setSoundEffectVolume(0));
  REQUIRE_NOTHROW(mixer.setSoundEffectVolume(Limits<uint8_t>::max()));
  REQUIRE_NOTHROW(mixer.setSoundEffectVolume(Limits<uint8_t>::lowest()));
  */
}

TEST_CASE("Play music", "[engine][audio]")
{
  /* @todo: Get Service Tuple
  AudioMixer mixer;
  REQUIRE_NOTHROW(mixer.playMusic());
  */
}

SCENARIO("I can change the music volume", "[engine][audio]")
{
  GIVEN("The music volume is currently 0")
  {
    /* @todo: Get Service Tuple
    AudioMixer mixer;
    mixer.setMusicVolume(0);
    REQUIRE(Settings::instance().musicVolume == 0);
    */
    WHEN("I set the music volume to 128")
    {
      /* @todo: Get Service Tuple
      mixer.setMusicVolume(128);
      */
      THEN("My music volume setting becomes 128")
      {
        /* @todo: Get Service Tuple
        REQUIRE(Settings::instance().musicVolume == 128);
        */
      }
    }
  }
  GIVEN("The music volume is currently 128")
  {
    /* @todo: Get Service Tuple
    AudioMixer mixer;
    mixer.setMusicVolume(128);
    REQUIRE(Settings::instance().musicVolume == 128);
    */
    WHEN("I set the music volume to 0")
    {
      /* @todo: Get Service Tuple
      mixer.setMusicVolume(0);
      */
      THEN("My music volume setting becomes 0")
      {
        /* @todo: Get Service Tuple
        REQUIRE(Settings::instance().musicVolume == 0); 
        */
      }
    }
  }
}

SCENARIO("I can change the sound effects volume", "[engine][audio]")
{
  GIVEN("The sound effects volume is currently 0")
  {
    /* @todo: Get Service Tuple
    AudioMixer mixer;
    mixer.setSoundEffectVolume(0);
    REQUIRE(Settings::instance().soundEffectsVolume == 0);
    */
    WHEN("I set the sound effects volume to 128")
    {
      /* @todo: Get Service Tuple
      mixer.setSoundEffectVolume(128);
      */
      THEN("My sound effects volume setting becomes 128")
      {
        /* @todo: Get Service Tuple
        REQUIRE(Settings::instance().soundEffectsVolume == 128); 
        */
      }
    }
  }
  GIVEN("The sound effects volume is currently 128")
  {
    /* @todo: Get Service Tuple
    AudioMixer mixer;
    mixer.setSoundEffectVolume(128);
    REQUIRE(Settings::instance().soundEffectsVolume == 128);
    */
    WHEN("I set the sound effects volume to 0")
    {
      /* @todo: Get Service Tuple
      mixer.setSoundEffectVolume(0);
      */
      THEN("My sound effects volume setting becomes 0")
      {
        /* @todo: Get Service Tuple
        REQUIRE(Settings::instance().soundEffectsVolume == 0); 
        */
      }
    }
  }
}

SCENARIO("I can play music", "[engine][audio]")
{
  GIVEN("I am not hearing anything")
  {
    /* @todo: Get Service Tuple
    AudioMixer mixer;
    mixer.mute();
    REQUIRE(Mix_Playing(-1) == 0);
    */
    WHEN("I play music")
    {
      /* @todo: Get Service Tuple
      mixer.playMusic();
      */
      THEN("I hear music")
      {
        /* @todo: Get Service Tuple
        REQUIRE(Mix_Playing(-1) > 0); 
        */
      }
    }
  }
}

SCENARIO("I can mute sounds", "[engine][audio]")
{
  GIVEN("I am playing something")
  {
    /* @todo: Get Service Tuple
    AudioMixer mixer;
    mixer.playMusic();
    REQUIRE(Mix_Playing(-1) > 0);
    */
    WHEN("I mute all sounds")
    {
      /* @todo: Get Service Tuple
      mixer.mute();
      */
      THEN("I am not hearing anything")
      {
        /* @todo: Get Service Tuple
        REQUIRE(Mix_Playing(-1) == 0); 
        */
      }
    }
  }
}
