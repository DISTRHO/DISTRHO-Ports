#!/bin/bash

set -e

# common
sudo apt-get install -y build-essential meson

if [ "${TARGET}" = "linux" ]; then
    sudo apt-get install -y pkg-config libfreetype6-dev libx11-dev libxcomposite-dev libxrender-dev libxcursor-dev libasound2-dev libgl1-mesa-dev libglu1-mesa-dev

elif [ "${TARGET}" = "win32" ]; then
    sudo apt-get install -y mingw-w64 binutils-mingw-w64-i686 g++-mingw-w64-i686 g++-multilib wine32-development-tools

elif [ "${TARGET}" = "win64" ]; then
    sudo apt-get install -y mingw-w64 binutils-mingw-w64-x86-64 g++-mingw-w64-x86-64 wine64-development-tools

fi
