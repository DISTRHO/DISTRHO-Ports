###############################################################################

project('DISTRHO-Ports',
    'c', 'cpp',
    license : 'GPLv3'
)

###############################################################################
# get options

buildtype = get_option('buildtype')
prefix = get_option('prefix')
bindir = get_option('bindir')
libdir = get_option('libdir')

linux_embed = get_option('linux-embed')
optimizations = get_option('optimizations')

###############################################################################
# set target OS

os_darwin = false
os_linux = false
os_windows = false

if host_machine.system() == 'darwin'
    os_darwin = true
elif host_machine.system() == 'windows'
    os_windows = true
else
    os_linux = true
endif

###############################################################################
# set OS-specific details

if os_darwin
    bin_suffix = ''
    lib_suffix = '.dylib'
elif os_windows
    bin_suffix = '.exe'
    lib_suffix = '.dll'
else
    bin_suffix = ''
    lib_suffix = '.so'
endif

###############################################################################
# base compiler details

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

build_flags = [
    '-DJUCE_APP_CONFIG_HEADER="AppConfig.h"',
    '-pthread',
    '-Wall', '-Wno-multichar', '-Wno-unused-but-set-variable', '-Wno-unused-function', '-Wno-strict-overflow',
]

build_flags_cpp = [
]

build_flags_release = [
    '-O3', '-fvisibility=hidden', '-DNDEBUG=1',
]

build_flags_release_cpp = [
    '-fvisibility-inlines-hidden',
]

build_flags_debug = [
    '-O0', '-ggdb', '-DDEBUG=1', '-D_DEBUG=1',
]

build_flags_debug_cpp = [
]

if optimizations and not linux_embed
    build_flags_release += [ 
        '-mtune=generic', '-msse', '-msse2',
    ]
endif

if not os_darwin
    build_flags_cpp += [
        '-std=gnu++11',
    ]
    build_flags_release += [
        '-fdata-sections', '-ffunction-sections',
    ]
endif

if os_darwin
    build_flags += [
        '-DMAC=1',
    ]
    build_flags_cpp += [
        '-ObjC++',
    ]
elif os_windows
    build_flags += [
        '-DWINDOWS=1',
    ]
    build_flags_cpp += [
        '-fpermissive',
    ]
elif linux_embed
    build_flags += [
        '-DLINUX=1',
    ]
    build_flags_cpp += [
        '-DJUCE_AUDIOPROCESSOR_NO_GUI=1',
    ]
else
    build_flags += [
        '-DLINUX=1',
    ]
    build_flags_cpp += [
        '`pkg-config --cflags alsa freetype2 x11 xext`',
    ]
endif

###############################################################################

if buildtype == 'debug'
    build_flags = build_flags + build_flags_debug
    build_flags_cpp = build_flags + build_flags_cpp + build_flags_debug_cpp
else
    build_flags = build_flags + build_flags_release
    build_flags_cpp = build_flags + build_flags_cpp + build_flags_release_cpp
endif

###############################################################################
# go into subdir to build libraries and plugins

subdir('libs')
# subdir('ports')

###############################################################################
