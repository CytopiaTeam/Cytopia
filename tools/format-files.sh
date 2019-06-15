#!/usr/bin/env bash
find .. -regex '.*\.\(hxx\|cxx\)' -not -path '*/external/*' -exec clang-format -style=file -i {} \;