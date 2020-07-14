# DISTRHO Ports

[![Build Status](https://travis-ci.org/DISTRHO/DISTRHO-Ports.png)](https://travis-ci.org/DISTRHO/DISTRHO-Ports)

DISTRHO is an open source project with the goal of making cross-platform audio plugins and GNU/Linux ports.

<b>This repository contains the GNU/Linux plugins and LV2 ports</b>

## BUILD DEPENDENCIES

To build plugins, you first need to install the following dependencies:

All OSes:

- meson

GNU/Linux: (development versions of these)

- ALSA
- freetype2
- OpenGL/Mesa
- X11 core and extensions (XShm, XRender and XCursor)

## BUILD and INSTALL

In order to build and install the plugins, just run the usual steps for a meson project:

```
meson build --buildtype release
ninja -C build
ninja -C build install
```
