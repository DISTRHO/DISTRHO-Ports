#!/bin/bash

set -e

if [ x"${1}" = x"" ]; then
    echo "usage: $0 <target>"
    exit 1
fi

function get_linux_deb_arch() {
    case "${1}" in
        "linux-aarch64") echo "arm64" ;;
        "linux-armhf") echo "armhf" ;;
        "linux-i686") echo "i386" ;;
        "linux-riscv64") echo "riscv64" ;;
        "linux-x86_64") echo "amd64" ;;
    esac
}

function install_compiler() {
    local arch="${1}"
    local release="${2}"

    apt-get install -yqq g++

    if [ -n "${GITHUB_ENV}" ] && [ "${release}" = "bionic" ]; then
        apt-get install -yqq g++-11
        update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60 --slave /usr/bin/g++ g++ /usr/bin/g++-11
    fi

    case "${arch}" in
        "linux-aarch64")
            if [ "$(uname -m)" != "aarch64" ]; then
                apt-get install -yqq g++-aarch64-linux-gnu
            elif [ -n "${GITHUB_ENV}" ] && [ "${release}" = "bionic" ]; then
                update-alternatives --install /usr/bin/aarch64-linux-gnu-gcc aarch64-linux-gnu-gcc /usr/bin/gcc-11 60 --slave /usr/bin/aarch64-linux-gnu-g++ aarch64-linux-gnu-g++ /usr/bin/g++-11
            fi
        ;;
        "linux-armhf")
            if [ "$(uname -m)" != "armhf" ]; then
                apt-get install -yqq g++-arm-linux-gnueabihf
            elif [ -n "${GITHUB_ENV}" ] && [ "${release}" = "bionic" ]; then
                update-alternatives --install /usr/bin/arm-linux-gnueabihf-gcc arm-linux-gnueabihf-gcc /usr/bin/gcc-11 60 --slave /usr/bin/arm-linux-gnueabihf-g++ arm-linux-gnueabihf-g++ /usr/bin/g++-11
            fi
        ;;
        "linux-i686")
            if [ "$(uname -m)" != "i686" ]; then
                apt-get install -yqq g++-i686-linux-gnu
            elif [ -n "${GITHUB_ENV}" ] && [ "${release}" = "bionic" ]; then
                update-alternatives --install /usr/bin/i686-linux-gnu-gcc i686-linux-gnu-gcc /usr/bin/gcc-11 60 --slave /usr/bin/i686-linux-gnu-g++ i686-linux-gnu-g++ /usr/bin/g++-11
            fi
        ;;
        "linux-riscv64")
            if [ "$(uname -m)" != "riscv64" ]; then
                apt-get install -yqq g++-riscv64-linux-gnu
            fi
        ;;
        "linux-x86_64")
            if [ "$(uname -m)" != "x86_64" ]; then
                apt-get install -yqq g++-x86_64-linux-gnu
            elif [ -n "${GITHUB_ENV}" ] && [ "${release}" = "bionic" ]; then
                update-alternatives --install /usr/bin/x86_64-linux-gnu-gcc x86_64-linux-gnu-gcc /usr/bin/gcc-11 60 --slave /usr/bin/x86_64-linux-gnu-g++ x86_64-linux-gnu-g++ /usr/bin/g++-11
            fi
        ;;
        "win32")
            apt-get install -yqq binutils-mingw-w64-i686 g++-mingw-w64-i686
        ;;
        "win64")
            apt-get install -yqq binutils-mingw-w64-x86-64 g++-mingw-w64-x86-64
        ;;
    esac
}

case "${1}" in
    "macos"|"macos-universal"|"macos-universal-10.15")
        brew install autoconf automake cmake coreutils gawk git gnu-sed jq make meson

        [ -n "${GITHUB_ENV}" ] && echo "PAWPAW_PACK_NAME=${1}-$(sw_vers -productVersion | cut -d '.' -f 1)" >> "${GITHUB_ENV}"
    ;;
    *)
        apt-get update -qq
        apt-get install -yqq autoconf automake build-essential curl cmake file git jq libglib2.0-dev-bin libtool lsb-release make meson gperf patchelf uuid-dev zlib1g-dev

        linux_arch=$(get_linux_deb_arch "${1}")
        release=$(lsb_release -cs 2>/dev/null)

        if [ -n "${linux_arch}" ]; then
            if [ "$(lsb_release -si 2>/dev/null)" = "Ubuntu" ]; then
                sed -i "s/deb http/deb [arch=i386,amd64] http/" /etc/apt/sources.list
                sed -i "s/deb mirror/deb [arch=i386,amd64] mirror/" /etc/apt/sources.list
                if [ "${linux_arch}" != "amd64" ] && [ "${linux_arch}" != "i386" ]; then
                    echo "deb [arch=${linux_arch}] http://ports.ubuntu.com/ubuntu-ports ${release} main restricted universe multiverse" | tee -a /etc/apt/sources.list
                    echo "deb [arch=${linux_arch}] http://ports.ubuntu.com/ubuntu-ports ${release}-updates main restricted universe multiverse" | tee -a /etc/apt/sources.list
                    echo "deb [arch=${linux_arch}] http://ports.ubuntu.com/ubuntu-ports ${release}-backports main restricted universe multiverse" | tee -a /etc/apt/sources.list
                fi
                if [ -n "${GITHUB_ENV}" ] && [ "${release}" = "bionic" ]; then
                    apt-get install -yqq --no-install-recommends software-properties-common
                    add-apt-repository -yn ppa:ubuntu-toolchain-r/test
                fi
            fi
            dpkg --add-architecture ${linux_arch}
            apt-get update -qq
            apt-get install -yqq \
                binfmt-support \
                qemu-user-static \
                qtbase5-dev-tools \
                libasound2-dev:${linux_arch} \
                libdbus-1-dev:${linux_arch} \
                libgl1-mesa-dev:${linux_arch} \
                libglib2.0-dev:${linux_arch} \
                libpcre2-dev:${linux_arch} \
                libpcre3-dev:${linux_arch} \
                libqt5svg5-dev:${linux_arch} \
                libx11-dev:${linux_arch} \
                libxcb1-dev:${linux_arch} \
                libxcursor-dev:${linux_arch} \
                libxext-dev:${linux_arch} \
                libxfixes-dev:${linux_arch} \
                libxrandr-dev:${linux_arch} \
                libxrender-dev:${linux_arch} \
                qtbase5-dev:${linux_arch} \
                uuid-dev:${linux_arch}
        elif [ "${1}" = "win32" ] || [ "${1}" = "win64" ]; then
            dpkg --add-architecture i386
            apt-get update -qq
            apt-get install -yqq autopoint libffi-dev libreadline-dev mingw-w64
            if [ "$(lsb_release -si 2>/dev/null)" = "Debian" ]; then
                apt-get install -yqq wine wine32 wine64
            else
                apt-get install -yqq wine-stable
            fi
        fi

        case "${release}" in
            "bionic"|"bullseye"|"focal")
                apt-get install -yqq --no-install-recommends python3-pip
                pip3 install --isolated --upgrade --no-cache-dir --force-reinstall pip
                pip3 install --isolated --upgrade --no-cache-dir --force-reinstall cmake meson
            ;;
        esac

        install_compiler "${1}" "${release}"

        [ -n "${GITHUB_ENV}" ] && echo "PAWPAW_PACK_NAME=${1}-${release}" >> "${GITHUB_ENV}"
    ;;
esac
