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

if libdir.contains('/')
    libdir = 'lib'
endif

build_lv2 = get_option('build-lv2')
build_vst2 = get_option('build-vst2')
build_vst3 = get_option('build-vst3')
build_legacy_only = get_option('build-legacy-only')
linux_embed = get_option('linux-embed')
optimizations = get_option('optimizations') and host_machine.cpu_family().contains('x86')

###############################################################################
# set paths

lv2dir = prefix / libdir / 'lv2'
vst2dir = prefix / libdir / 'vst'
vst3dir = prefix / libdir / 'vst3'

###############################################################################
# set target OS

os_darwin = false
os_linux = false
os_windows = false

if host_machine.system() == 'darwin'
    os_darwin = true
elif host_machine.system() == 'windows'
    os_windows = true
elif host_machine.system() == 'linux'
    os_linux = true
else
    error('unsupported system')
endif

###############################################################################
# set OS-specific details

if os_darwin
    bin_suffix = ''
    lib_suffix = '.dylib'
    bintype_prefix = 'Mac-'
elif os_windows
    bin_suffix = '.exe'
    lib_suffix = '.dll'
    bintype_prefix = 'Windows-'
else
    bin_suffix = ''
    lib_suffix = '.so'
    bintype_prefix = 'Linux-'
endif

###############################################################################
# base compiler details

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')

###############################################################################
# dependencies

dependencies = [
]

if os_linux
    dependencies += [
        cc.find_library('dl'),
        cc.find_library('rt'),
    ]
endif

if os_linux and not linux_embed
    dependencies += [
        dependency('alsa'),
        dependency('freetype2'),
        dependency('x11'),
        dependency('xext'),
    ]
endif

###############################################################################
# build flags

build_flags = [
    '-DJUCE_APP_CONFIG_HEADER="AppConfig.h"',
    '-pthread',
    '-Wall',
    '-Wno-multichar',
    '-Wno-strict-overflow',
    '-Wno-unused-but-set-variable',
    '-Wno-unused-function',
]

build_flags_cpp = [
    '-Wno-class-memaccess',
    '-std=gnu++11',
]

build_flags_debug = [
    '-O0', '-ggdb',
    '-DDEBUG=1', '-D_DEBUG=1',
]

build_flags_debug_cpp = [
]

build_flags_release = [
    '-O3', '-fvisibility=hidden',
    '-DNDEBUG=1',
]

build_flags_release_cpp = [
    '-fvisibility-inlines-hidden',
]

if optimizations and not linux_embed
    build_flags_release += [
        '-mtune=generic', '-msse', '-msse2',
    ]
endif

if not os_darwin
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
        '-mstackrealign',
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
endif

###############################################################################
# link flags

link_flags = [
    '-pthread',
]

link_flags_debug = [
]

link_flags_release = [
]

if os_windows
    link_flags += [
        '-static',
    ]
endif

if not os_darwin
    link_flags += [
        '-Wl,--no-undefined',
    ]
    link_flags_release += [
        '-fdata-sections', '-ffunction-sections', '-Wl,--gc-sections',
        '-Wl,--as-needed',
        '-Wl,--strip-all',
    ]
endif

###############################################################################
# combine flags depending on build type

if buildtype == 'debug'
    build_flags     += build_flags_debug
    build_flags_cpp += build_flags_debug_cpp + build_flags
    link_flags      += link_flags_debug
else
    build_flags     += build_flags_release
    build_flags_cpp += build_flags_release_cpp + build_flags
    link_flags      += link_flags_release
endif

###############################################################################
# go into subdir to build libraries and plugins

subdir('libs')
subdir('ports')
subdir('ports-legacy')

###############################################################################
# extra files to install

extra_lv2_preset_bundles = [
    'TAL-NoiseMaker-Noise4U.lv2',
]

foreach bundle : extra_lv2_preset_bundles
    install_data([
            'static-lv2-ttl/@0@/manifest.ttl'.format(bundle),
            'static-lv2-ttl/@0@/presets.ttl'.format(bundle),
        ],
        install_dir: lv2dir / bundle,
    )
endforeach

###############################################################################
