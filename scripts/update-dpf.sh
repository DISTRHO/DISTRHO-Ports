#!/bin/bash

set -e

if [ ! -d bin ]; then
  echo "Please run this script from the distrho root folder"
  exit
fi

DPF_SOURCE_DIR="/home/falktx/Personal/FOSS/GIT/falktx/DPF"

make clean -C $DPF_SOURCE_DIR/dgl

cp -v    $DPF_SOURCE_DIR/distrho/*.{c,h}pp ./libs/distrho/
cp -v -r $DPF_SOURCE_DIR/distrho/src/*     ./libs/distrho/src/

cp -v $DPF_SOURCE_DIR/dgl/*.hpp      ./libs/dgl/
cp -v $DPF_SOURCE_DIR/dgl/src/*.cpp  ./libs/dgl/src/
cp -v $DPF_SOURCE_DIR/dgl/src/pugl/* ./libs/dgl/src/pugl/

cp -v $DPF_SOURCE_DIR/utils/png2rgba.py ./scripts/
