#!/bin/sh
set -eu

# Set's up local repo for Transifex
mkdir tx;
cd tx
tx init --host=www.transifex.com --no-interactive
tx set --auto-remote https://www.transifex.com/projects/p/cytopia

