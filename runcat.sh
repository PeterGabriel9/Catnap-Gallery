#!/usr/bin/env bash
# Build and run Catnap Gallery.
# Forces X11 on Wayland so GLEW can initialise.
set -e

cd "$(dirname "$0")"

cmake -S . -B build
cmake --build build

cd build/Bin
env -u WAYLAND_DISPLAY XDG_SESSION_TYPE=x11 ./ACG_Lab4
