# Copyright (C) 2020 Alexandros Theodotou <alex at zrythm dot org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

project (
  'DISTRHO-ports', ['cpp'],
  version: '2018-04-16',
  license: 'GPL-2-or-later',
  meson_version: '>= 0.53.0',
  default_options: [
    'warning_level=2',
    ],
  )

cxx = meson.get_compiler ('cpp')
release = get_option ('buildtype') == 'release'
debug = not release

prefix = get_option ('prefix')
bindir = prefix / get_option('bindir')
libdir = prefix / get_option ('libdir')
lv2dir = libdir / 'lv2'
vstdir = libdir / 'vst'

os_darwin = false
os_linux = false
os_freebsd = false
os_windows = false
bin_suffix = ''
lib_suffix = '.so'

if host_machine.system() == 'darwin'
  os_darwin = true
  lib_suffix = '.dylib'
elif host_machine.system() == 'linux'
  os_linux = true
elif host_machine.system() == 'freebsd'
  os_freebsd = true
elif host_machine.system() == 'windows'
  os_windows = true
  bin_suffix = '.exe'
  lib_suffix = '.dll'
endif

lib_deps = []
lib_defines = []
lib_cflags = [
  '-DPIC',
  '-Wall',
  '-DJUCE_APP_CONFIG_HEADER="AppConfig.h"',
  '-Wno-multichar',
  '-Wno-unused-but-set-variable',
  '-Wno-unused-function',
  '-Wno-strict-overflow',
  ]

if (get_option ('buildtype') == 'release')
  lib_cflags += [
    '-O3',
    '-fvisibility=hidden',
    '-fvisibility-inlines-hidden',
    ]
  lib_defines += [
    '-DNDEBUG=1',
    ]
else
  lib_cflags += [
    '-O0',
    '-ggdb',
    ]
  lib_defines += [
    '-DDEBUG=1',
    '-D_DEBUG=1',
    ]
endif

if os_windows
  lib_cflags += [
    '-fpermissive',
    '-std=c++0x',
    ]
  lib_defines += [
    '-DWINDOWS=1',
    ]
elif os_darwin
  lib_cflags += [
    '-ObjC++',
    ]
  lib_defines += [
    '-DMAC=1',
    ]
elif get_option ('linux-embed')
  lib_cflags += [
    '-DJUCE_AUDIO_PROCESSOR_NO_GUI=1',
    '-std=c++0x',
    ]
  lib_defines += [
    '-DLINUX=1',
    ]
else
  lib_cflags += [
    '-std=c++0x',
    ]
  lib_defines += [
    '-DLINUX=1',
    ]
  lib_deps += [
    dependency ('alsa'),
    dependency ('freetype2'),
    dependency ('x11'),
    dependency ('xext'),
    ]
endif

if get_option ('optimizations') or not get_option ('linux-embed')
  lib_cflags += [
    '-mtune=generic',
    '-msse',
    '-msse2',
    ]
endif

if not os_darwin
  lib_cflags += [
    '-fdata-sections',
    '-ffunction-sections',
    ]
endif

# FIXME not sure what the difference between defines/cflags is
lib_cflags += lib_defines

lib_cflags = cxx.get_supported_arguments (lib_cflags)

# ------------------------------------------

plugin_cflags = []
plugin_defines = []
plugin_ldflags = []

plugin_cflags += [
  '-Wall', '-Werror=deprecated-declarations',
  '-pthread',
  '-DJUCE_APP_CONFIG_HEADER="AppConfig.h"',
  ]
plugin_ldflags += [
  '-pthread',
  ]

if release
  plugin_defines += [
    '-DNDEBUG=1', '-DCONFIGURATION="Release"' ]
  plugin_cflags += [
    '-O3', '-ffast-math', '-fomit-frame-pointer', '-fvisibility=hidden', '-fvisibility-inlines-hidden' ]
endif

if get_option ('optimizations') or not get_option ('linux-embed')
  plugin_cflags += [
    '-mtune=generic', '-msse', '-msse2', '-mfpmath=sse' ]
endif

if not os_darwin
  plugin_ldflags += [
    '-Wl,--no-undefined',
    ]
  if get_option ('buildtype') == 'release'
    plugin_cflags += [
      '-fdata-sections', '-ffunction-sections' ]
    plugin_ldflags += [
      '-fdata-sections', '-ffunction-sections',
      '-Wl,--gc-sections', '-Wl,--strip-all' ]
  endif
endif

if not release
  plugin_defines += [
    '-DDEBUG=1', '-D_DEBUG=1', '-DCONFIGURATION="Debug"' ]
endif

plugin_include_dirs = [
  include_directories ('libs' / 'juce' / 'source'),
  include_directories ('libs' / 'juce' / 'source' / 'modules'),
  include_directories ('libs' / 'drowaudio' / 'source'),
  include_directories ('libs' / 'juced' / 'source'),
  include_directories ('libs' / 'juce-plugin'),
  ]

if os_windows
  plugin_defines += [
    '-DWINDOWS=1',
    ]
  plugin_cflags += [
    '-fpermissive',
    '-std=c++0x',
    ]
elif os_darwin
  plugin_defines += [
    '-DMAC=1',
    ]
  plugin_cflags += [
    '-ObjC++',
    ]
  plugin_ldflags += [
    '-dynamiclib',
    ]
else
  plugin_defines += [
    '-DLINUX=1',
    ]
  plugin_cflags += [
    '-std=c++0x',
    ]
endif

if get_option ('linux-embed')
  plugin_cflags += [
    '-DJUCE_AUDIO_PROCESSOR_NO_GUI=1', ]
endif

plugin_ldflags = cxx.get_supported_link_arguments (
  plugin_ldflags)

# ------------------------------------------

lv2_cflags = plugin_cflags
lv2_ldflags = plugin_ldflags
lv2_defines = plugin_defines
lv2_deps = []

if release
  lv2_ldflags += [ '-ljuce' ]
else
  lv2_ldflags += [ '-ljuce_debug' ]
endif

if os_windows
  lv2_ldflags += [
    'comdlg32', 'gdi32', 'imm32', 'ole32', 'oleaut32', 'shlwapi', 'uuid', 'version', 'winmm', 'wininet', 'ws2_32' ]
elif os_darwin
  lv2_ldflags += [
    '-framework Accelerate', '-framework AudioToolbox', '-framework AudioUnit', '-framework Carbon', '-framework Cocoa',
    '-framework CoreAudio', '-framework CoreAudioKit', '-framework CoreMIDI', '-framework IOKit', '-framework QuartzCore', '-framework WebKit' ]
elif get_option ('linux-embed')
  lv2_ldflags += ['-ldl', '-lrt']
else
  lv2_ldflags += ['-ldl', '-lrt']
  lv2_deps += [
    dependency ('freetype2'),
    dependency ('x11'),
    dependency ('xext') ]
endif

lv2_defines += [
  '-DDISTRHO_PLUGIN_TARGET_LV2=1', '-DJucePlugin_Build_AU=0', '-DJucePlugin_Build_LV2=1', '-DJucePlugin_Build_RTAS=0', '-DJucePlugin_Build_VST=0', '-DJucePlugin_Build_Standalone=0' ]

if os_windows
  lv2_defines += [
    '-DBINTYPE=Windows-LV2',
    ]
elif os_darwin
  lv2_defines += [
    '-DBINTYPE=Mac-LV2',
    ]
else
  lv2_defines += [
    '-DBINTYPE=Linux-LV2',
    ]
endif

# FIXME not sure what the difference between defines/cflags is
lv2_cflags += lv2_defines

# ------------------------------------------

vst_cflags = plugin_cflags
vst_ldflags = plugin_ldflags
vst_defines = plugin_defines
vst_deps = []

if release
  vst_ldflags += [ '-ljuce' ]
else
  vst_ldflags += [ '-ljuce_debug' ]
endif

vst_cflags += [
  '-Wno-multichar',
  '-Wno-write-strings',
  ]

if os_windows
  vst_ldflags += [
    'comdlg32', 'gdi32', 'imm32', 'ole32', 'oleaut32', 'shlwapi', 'uuid', 'version', 'winmm', 'wininet', 'ws2_32' ]
elif os_darwin
  vst_ldflags += [
    '-framework Accelerate', '-framework AudioToolbox', '-framework AudioUnit', '-framework Carbon', '-framework Cocoa',
    '-framework CoreAudio', '-framework CoreAudioKit', '-framework CoreMIDI', '-framework IOKit', '-framework QuartzCore', '-framework WebKit' ]
else
  vst_ldflags += ['-ldl', '-lrt']
  vst_deps += [
    dependency ('freetype2'),
    dependency ('x11'),
    dependency ('xext') ]
endif

vst_defines += [
  '-DDISTRHO_PLUGIN_TARGET_VST=1', '-DJucePlugin_Build_AU=0', '-DJucePlugin_Build_LV2=0', '-DJucePlugin_Build_RTAS=0', '-DJucePlugin_Build_VST=1', '-DJucePlugin_Build_Standalone=0' ]

if os_windows
  vst_defines += [
    '-DBINTYPE=\'Windows-VST\'',
    ]
elif os_darwin
  vst_defines += [
    '-DBINTYPE=\'Mac-VST\'',
    ]
else
  vst_defines += [
    '-DBINTYPE=\'Linux-VST\'',
    ]
endif

# FIXME not sure what the difference between defines/cflags is
vst_cflags += vst_defines

# ---------------------------------------------

subdir ('libs')
subdir ('ports')
