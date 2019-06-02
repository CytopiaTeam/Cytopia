#!/bin/sh

set -eu

if [[ "${TRAVIS_PULL_REQUEST}" = "false" ]]
then
	# Get butler
	wget https://broth.itch.ovh/butler/linux-amd64/LATEST/archive/default -O butler.zip
	7z x butler.zip
	sudo chmod a+x ./butler

	# Setup redist folder and push it to itch
	ninja install
	./butler push redist rcytopia/cytopia:linux-ci --userversion 0.2-CIBuild-${TRAVIS_JOB_NUMBER}
fi
