#!/bin/sh
set -eu

# Cleanup
if [ -d languages ]; then
	rm -r languages
fi

# Moves po files from local tx repo to folder structure
cd tx/translations/cytopia.cytopiapot
for file in *
do
	file="${file%.*}"
	echo "Now copying $file from Cytopia"
	mkdir -p ../../../languages/$file/
	cp $file.po ../../../languages/$file/Cytopia.po
done

cd ../cytopia.tiledatauipot
for file in *
do
	file="${file%.*}"
	echo "Now copying $file from TileDataUI"
	mkdir -p ../../../languages/$file/
	cp $file.po ../../../languages/$file/TileDataUI.po
done
