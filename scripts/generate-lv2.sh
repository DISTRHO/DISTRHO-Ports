#!/bin/bash

set -e

gen="${1}"
dir="${2}"
lib="${3}"

# meson does not support MESON_EXE_WRAPPER for post-build commands, only for tests
# do a whole dance to try to replicate expected behaviour
if [ -z "${MESON_EXE_WRAPPER}" ]; then
    fileout="$(file "${gen}")"
    if echo "${fileout}" | grep -q "Mach-O"; then
        MESON_EXE_WRAPPER=""

    elif echo "${fileout}" | grep -q "PE32 executable.*Intel 80386"; then
        MESON_EXE_WRAPPER="wine"

    elif echo "${fileout}" | grep -q "PE32+ executable.*x86-64"; then
        MESON_EXE_WRAPPER="wine64"

    elif echo "${fileout}" | grep -q "ARM aarch64"; then
        if [ "$(uname -m)" != "aarch64" ]; then
            MESON_EXE_WRAPPER="qemu-aarch64-static"
        fi

    elif echo "${fileout}" | grep -q "ARM"; then
        if [ "$(uname -m)" != "arm" ] && [ "$(uname -m)" != "armv7l" ]; then
            MESON_EXE_WRAPPER="qemu-arm-static"
        fi

    elif echo "${fileout}" | grep -q "Intel 80386"; then
        if [ "$(uname -m)" != "i386" ] && [ "$(uname -m)" != "i686" ] && [ "$(uname -m)" != "x86_64" ]; then
            MESON_EXE_WRAPPER="qemu-i386-static"
        fi

    elif echo "${fileout}" | grep -q "RISC-V"; then
        if [ "$(uname -m)" != "riscv64" ]; then
            MESON_EXE_WRAPPER="qemu-riscv64-static"
        fi

    elif echo "${fileout}" | grep -q "x86-64"; then
        if [ "$(uname -m)" != "x86_64" ]; then
            MESON_EXE_WRAPPER="qemu-x86_64-static"
        fi

    elif echo "${fileout}" | grep -q "64-bit LSB.*PowerPC"; then
        if [ "$(uname -m)" != "ppc64le" ]; then
            MESON_EXE_WRAPPER="qemu-ppc64le-static"
        fi

    elif echo "${fileout}" | grep -q "64-bit MSB.*PowerPC"; then
        if [ "$(uname -m)" != "ppc64" ]; then
            MESON_EXE_WRAPPER="qemu-ppc64-static"
        fi

    elif echo "${fileout}" | grep -q "32-bit MSB.*PowerPC"; then
        if [ "$(uname -m)" != "ppc" ] && [ "$(uname -m)" != "ppc64" ]; then
            MESON_EXE_WRAPPER="qemu-ppc-static"
        fi

    else
        echo "unrecognized file output: ${fileout}"
        exit 1
    fi
fi

mkdir -p "${dir}"
cd "${dir}"
cp -v "${lib}" .
exec ${MESON_EXE_WRAPPER} "${gen}" "${lib}"
