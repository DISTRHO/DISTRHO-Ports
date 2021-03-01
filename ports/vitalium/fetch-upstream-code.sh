#!/bin/bash

set -e

cd $(dirname ${0})

# fetch git repo if not done yet
if [ ! -d vital-git ]; then
    git clone --depth=1 --recursive git@github.com:mtytel/vital.git vital-git
fi

# clean and update git repo
# pushd vital-git
# git checkout .
# git submodule update
# git pull
# git submodule update
# popd

# TODO generate binarydata ourselves
cp vital-git/plugin/JuceLibraryCode/BinaryData.{cpp,h} .

rm -rf source third_party
mkdir source
mkdir third_party
cp -r vital-git/src/{common,interface,plugin,synthesis,unity_build} source/
cp -r vital-git/third_party/{concurrentqueue,json} third_party/
