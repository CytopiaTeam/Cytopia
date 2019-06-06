#include <catch2/catch.hpp>
#include <limits>

#include "../../src/engine/AudioMixer.hxx"

template <class T>
using Limits = std::numeric_limits<T>;

TEST_CASE("Create an destroy AudioMixer", "[engine][audio]") {
  AudioMixer* mixer = nullptr;
  REQUIRE_NOTHROW(
      mixer = new AudioMixer
  );
  REQUIRE_NOTHROW(
    delete mixer
  );
}

TEST_CASE("Set the music volume", "[engine][audio]") {
  AudioMixer mixer;
  REQUIRE_NOTHROW(
    mixer.setMusicVolume(10)
  );
  REQUIRE_NOTHROW(
    mixer.setMusicVolume(-10)
  );
  REQUIRE_NOTHROW(
    mixer.setMusicVolume(0)
  );
  REQUIRE_NOTHROW(
    mixer.setMusicVolume(Limits<int>::max())
  );
  REQUIRE_NOTHROW(
    mixer.setMusicVolume(Limits<int>::lowest())
  );
}

TEST_CASE("Set sound effects volume", "[engine][audio]") {
  AudioMixer mixer;
  REQUIRE_NOTHROW(
    mixer.setSoundEffectVolume(10)
  );
  REQUIRE_NOTHROW(
    mixer.setSoundEffectVolume(-10)
  );
  REQUIRE_NOTHROW(
    mixer.setSoundEffectVolume(0)
  );
  REQUIRE_NOTHROW(
    mixer.setSoundEffectVolume(Limits<int>::max())
  );
  REQUIRE_NOTHROW(
    mixer.setSoundEffectVolume(Limits<int>::lowest())
  );
}

TEST_CASE("Play music", "[engine][audio]") {
  AudioMixer mixer;
  REQUIRE_NOTHROW(
    mixer.playMusic()
  );
}

SCENARIO("I can change the music volume", "[engine][audio]") {
  GIVEN("The music volume is currently 0") {
    AudioMixer mixer;
    mixer.setMusicVolume(0);
    REQUIRE(Settings::instance().settings.musicVolume == 0);
    WHEN("I set the music volume to 128") {
      mixer.setMusicVolume(128);
      THEN("My music volume setting becomes 128") {
        REQUIRE(Settings::instance().settings.musicVolume == 128);
      }
    }
  }
  GIVEN("The music volume is currently 128") {
    Audiomixer mixer;
    mixer.setMusicVolume(128);
    REQUIRE(Settings::instance().settings.musicVolume == 128);
    WHEN("I set the music volume to 0") {
      mixer.setMusicVolume(0);
      THEN("My music volume setting becomes 0") {
        REQUIRE(Settings::instance().settings.musicVolume == 0);
      }
    }
  }
}

SCENARIO("I can change the sound effects volume", "[engine][audio]") {
  GIVEN("The sound effects volume is currently 0") {
    AudioMixer mixer;
    mixer.setSoundEffectsVolume(0);
    REQUIRE(Settings::instance().settings.soundEffectsVolume == 0);
    WHEN("I set the sound effects volume to 128") {
      mixer.setSoundEffectsVolume(128);
      THEN("My sound effects volume setting becomes 128") {
        REQUIRE(Settings::instance().settings.soundEffectsVolume == 128);
      }
    }
  }
  GIVEN("The sound effects volume is currently 128") {
    Audiomixer mixer;
    mixer.setSoundEffectsVolume(128);
    REQUIRE(Settings::instance().settings.soundEffectsVolume == 128);
    WHEN("I set the sound effects volume to 0") {
      mixer.setSoundEffectsVolume(0);
      THEN("My sound effects volume setting becomes 0") {
        REQUIRE(Settings::instance().settings.soundEffectsVolume == 0);
      }
    }
  }
}

SCENARIO("I can play music", "[engine][audio]") {
  GIVEN("I am not hearing anything") {
    AudioMixer mixer;
    mixer.mute();
    REQUIRE(Mix_Playing(-1) == 0);
    WHEN("I play music") {
      mixer.playMusic();
      THEN("I hear music") {
        REQUIRE(Mix_Playing(-1) > 0);
      }
    }
  }
}

SCENARIO("I can mute sounds", "[engine][audio]") {
  GIVEN("I am playing something") {
    Audiomixer mixer;
    mixer.plauMusic();
    REQUIRE(Mix_Playing(-1) > 0);
    WHEN("I mute all sounds") {
      AudioMixer mixer;
      mixer.mute();
      THEN("I am not hearing anything") {
        REQUIRE(Mix_Playing(-1) == 0);
      }
    }
  }
}

