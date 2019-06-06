#include <catch2/catch.hpp>
#include <limits>

#include "../src/AudioMixer.hxx"

template <class T>
using Limits = std::numeric_limits<T>;

TEST_CASE("Create an destroy AudioMixer", "[engine]") {
  REQUIRE_NOTHROW(
      AudioMixer* mixer = new AudioMixer()
  );
  REQUIRE_NOTHROW(
    delete mixer
  );
}

TEST_CASE("Set the music volume", "[engine]") {
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

TEST_CASE("Set sound effects volume", "[engine]") {
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

TEST_CASE("Play music", "[engine]") {
  AudioMixer mixer;
  REQUIRE_NOTHROW(
    mixer.playMusic()
  );
}

