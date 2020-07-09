#!/bin/sh

set -eux

if [ "$TRAVIS_PULL_REQUEST" = "false" ] && [ "$TRAVIS_BRANCH" = "master" ]
then
  # Get butler
    wget https://broth.itch.ovh/butler/linux-amd64/LATEST/archive/default -O butler.zip
    7z x butler.zip
    sudo chmod a+x ./butler

    # Setup redist folder and push it to itch
    ./butler push ./platforms/android/build/Cytopia/outputs/apk/debug/Cytopia-debug.apk cytopia/cytopia:android-ci --userversion 0.2-CIBuild-"$TRAVIS_JOB_NUMBER"
fi
