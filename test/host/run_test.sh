#!/bin/sh
set -eu

SCRIPT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
REPO_ROOT="$(CDPATH= cd -- "$SCRIPT_DIR/../.." && pwd)"
BINARY="$SCRIPT_DIR/test_button"

cleanup() {
  rm -f "$BINARY"
}
trap cleanup EXIT HUP INT TERM

cd "$REPO_ROOT"
g++ -std=c++11 -Wall -Wextra -Werror \
  -Itest/host -Isrc \
  src/ARDUniaButton.cpp test/host/test_button.cpp \
  -o "$BINARY"

"$BINARY"
