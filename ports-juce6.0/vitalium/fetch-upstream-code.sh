#!/bin/bash

set -e

cd $(dirname ${0})

# build binarybuilder tool
make -C ../../libs/juce-current/source/extras/BinaryBuilder/Builds/LinuxMakefile/

# fetch git repo if not done yet
if [ ! -d vital-git ]; then
    git clone --depth=1 --recursive git@github.com:mtytel/vital.git vital-git
fi

# clean and update git repo
pushd vital-git
git checkout .
git submodule update
git pull
git submodule update
popd

../../libs/juce-current/source/extras/BinaryBuilder/Builds/LinuxMakefile/build/BinaryBuilder resources/ resources BinaryData "*.*"
sed -i \
  -e "s/ 5_Limit_scl/ _5_Limit_scl/g" \
  -e "s/ 7_Limit_scl/ _7_Limit_scl/g" \
  -e "s/:5_Limit_scl/:_5_Limit_scl/g" \
  -e "s/:7_Limit_scl/:_7_Limit_scl/g" \
  resources/BinaryData.*

# TODO finish this script, needs to take into account custom DISTRHO fork changes
exit 0

rm -rf source third_party
mkdir source
mkdir third_party
cp -r vital-git/src/{common,interface,plugin,synthesis,unity_build} source/
cp -r vital-git/third_party/{concurrentqueue,json} third_party/
