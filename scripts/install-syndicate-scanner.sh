#!/bin/bash

set -e

SCANNER="${MESON_BUILD_ROOT}/ports-juce7/syndicate/PluginScanServer"

if [ "$(uname -s)" = "Darwin" ]; then
    VST2_PATH=Contents/Resources
else
    VST2_PATH=Resources
fi

mkdir -p ${MESON_INSTALL_DESTDIR_PREFIX}/lib/lv2/Syndicate.lv2
mkdir -p ${MESON_INSTALL_DESTDIR_PREFIX}/lib/vst/Syndicate.vst/${VST2_PATH}
mkdir -p ${MESON_INSTALL_DESTDIR_PREFIX}/lib/vst3/Syndicate.vst3/Contents/Resources

cp -v "${SCANNER}" ${MESON_INSTALL_DESTDIR_PREFIX}/lib/lv2/Syndicate.lv2/
cp -v "${SCANNER}" ${MESON_INSTALL_DESTDIR_PREFIX}/lib/vst/Syndicate.vst/${VST2_PATH}/
cp -v "${SCANNER}" ${MESON_INSTALL_DESTDIR_PREFIX}/lib/vst3/Syndicate.vst3/Contents/Resources/

# move VST2 into subdir
if [ "$(uname -s)" != "Darwin" ]; then
    mv ${MESON_INSTALL_DESTDIR_PREFIX}/lib/vst/Syndicate.so ${MESON_INSTALL_DESTDIR_PREFIX}/lib/vst/Syndicate.vst/
fi
