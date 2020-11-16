#!/bin/bash

set -e

if [ ! -f meson.build ]; then
    echo "run from top dir"
    exit 1
fi

# legacy/compat repo
pushd libs/juce-legacy

if [ -d source ]; then
    mv source source.bak
fi

git clone git@github.com:DISTRHO/juce.git -b distrho-ports-compat source

popd

# main/juce6 repo
pushd libs/juce-current

if [ -d source ]; then
    mv source source.bak
fi

git clone git@github.com:DISTRHO/juce.git -b main source

popd
