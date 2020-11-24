#!/bin/bash

set -e

# nothing to do for macOS native
if [ "${TARGET}" = "macos" ]; then
    exit 0
fi

sudo add-apt-repository -y ppa:kxstudio-debian/kxstudio
sudo add-apt-repository -y ppa:kxstudio-debian/toolchain

if [ "${TARGET}" = "win32" ]; then
    sudo dpkg --add-architecture i386
fi

sudo apt-get update -qq
sudo apt-get install kxstudio-repos
sudo apt-get update -qq
