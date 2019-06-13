#!/bin/sh

# Compiles translated *.po files to binary *.mo files for gettext
# Setup:
# ./compile_mo.sh (this script)
# ./languages/<lang_code>/LC_MESSAGES/ror.po (po file to be baked into *.mo file)

cd languages
for dir in *
do
	cd $dir
	echo "Now processing language $dir"
  for potfile in *.po
  do
  	echo "   compiling $potfile"
  	msgfmt -o "${potfile%.*}".mo $potfile
    rm $potfile
  done
	cd ..
done
