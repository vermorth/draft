#!/bin/bash

find ./ -name '*.cpp' -o -name '*.hpp' -o -name "*.cc" -o -name "*.h" | xargs clang-format -i --style=file
