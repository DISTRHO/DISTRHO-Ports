#!/bin/bash

set -e

# needed homebrew packages
if [ "${TARGET}" = "macos" ]; then
    HOMEBREW_NO_AUTO_UPDATE=1 brew install meson
    exit 0
fi

# common
sudo apt-get install -y build-essential meson

if [ "${TARGET}" = "linux" ]; then
    sudo apt-get install -y \
        pkg-config \
        libasound2-dev libfftw3-dev libfreetype6-dev libgl1-mesa-dev libglu1-mesa-dev \
        libx11-dev libxcomposite-dev libxrender-dev libxcursor-dev \
        lilv-utils lv2lint kxstudio-lv2-extensions lv2-dev

elif [ "${TARGET}" = "win32" ]; then
    sudo apt-get install -y mingw-w64 binutils-mingw-w64-i686 g++-mingw-w64-i686 wine-stable wine32

elif [ "${TARGET}" = "win64" ]; then
    sudo apt-get install -y mingw-w64 binutils-mingw-w64-x86-64 g++-mingw-w64-x86-64 wine-stable

fi
