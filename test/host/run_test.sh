#!/bin/sh
set -e
cd "$(dirname "$0")"
g++ -std=c++11 -Wall -Wextra -I. -I../../src ../../src/ARDUniaButton.cpp test_button.cpp -o test_button
./test_button
