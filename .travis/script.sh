#!/bin/bash

set -e

export DESTDIR=/tmp/distrho-ports

if [ "${TARGET}" = "win32" ]; then
    EXTRA_ARGS="--cross-file scripts/meson/win32.ini"
elif [ "${TARGET}" = "win64" ]; then
    EXTRA_ARGS="--cross-file scripts/meson/win64.ini"
fi

echo "meson build --buildtype=release ${EXTRA_ARGS}"
meson build --buildtype=release --prefix=/usr ${EXTRA_ARGS}
ninja -v -C build
ninja -C build install

if [ "${TARGET}" = "linux" ]; then
    export LV2_PATH=${DESTDIR}/usr/lib/lv2:/usr/lib/lv2
    for uri in $(lv2ls); do lv2lint -W lv2_generate_ttl -Snowarn ${uri}; done
fi
