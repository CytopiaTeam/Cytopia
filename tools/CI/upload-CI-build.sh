#!/bin/sh

set -eux

if [ "${TRAVIS_PULL_REQUEST}" = "false" ] && [ "${TRAVIS_BRANCH}" = "master" ]; then
  if [ "${TARGET}" = "macOS" ]; then
    # Get butler
    curl -JOL https://broth.itch.ovh/butler/darwin-amd64/LATEST/archive/default
    mkdir redist
    mv bin/Cytopia.app/ redist/
    unzip butler-darwin-amd64.zip
    ./butler push redist cytopia/cytopia:osx-ci --userversion 0.2-CIBuild-${TRAVIS_JOB_NUMBER}
  else
    # Get butler
    curl "https://raw.githubusercontent.com/AnotherFoxGuy/ci-scripts/main/install-butler.sh" | sudo bash

    # Setup redist folder and push it to itch
    ninja install
    butler push redist cytopia/cytopia:linux-ci --userversion 0.2-CIBuild-${TRAVIS_JOB_NUMBER}
  fi
fi
