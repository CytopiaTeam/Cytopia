#!/bin/sh
set -eu

# Cleanup
rm -r languages

# Moves po files from local tx repo to folder structure
cd tx/translations/cytopia.cytopia_jsonpot
for file in *
do
	file="${file%.*}"
	echo "Now copying $file from CytopiaJson"
	mkdir -p ../../../languages/$file/
	cp $file.po ../../../languages/$file/CytopiaJson.po
done

cd ../cytopia.tiledatauipot
for file in *
do
	file="${file%.*}"
	echo "Now copying $file from TileDataUI"
	mkdir -p ../../../languages/$file/
	cp $file.po ../../../languages/$file/TileDataUI.po
done
