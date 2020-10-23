#!/bin/sh

set -eux

if [ "$TRAVIS_PULL_REQUEST" = "false" ] && [ "$TRAVIS_BRANCH" = "master" ]; then
  # Get butler
  curl "https://raw.githubusercontent.com/AnotherFoxGuy/ci-scripts/main/install-butler.sh" | sudo bash

  # Setup redist folder and push it to itch
  butler push ./platforms/android/build/Cytopia/outputs/apk/debug/Cytopia-debug.apk cytopia/cytopia:android-ci --userversion 0.2-CIBuild-"$TRAVIS_JOB_NUMBER"
fi
