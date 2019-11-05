#!/usr/bin/env bash

if ! [ -x "$(command -v clang-format)" ]; then
  printf "\e[31mError: the command clang-format was not found\e[0m"
  exit
fi

find .. -regex '.*\.\(hxx\|cxx\)' -not -path '*/external/*' -exec clang-format -style=file -i {} \;
