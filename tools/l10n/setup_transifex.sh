#!/bin/sh
set -eu

# Set's up local repo for Transifex

if [ -d tx ]; then
	rm -r tx
fi

mkdir tx
cd tx

tx init --host=www.transifex.com --no-interactive
tx set --auto-remote https://www.transifex.com/projects/p/cytopia
tx pull -a
