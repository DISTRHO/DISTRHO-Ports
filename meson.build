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

linux_embed = get_option('linux-embed')
build_lv2 = get_option('build-lv2')
build_vst2 = get_option('build-vst2') and not linux_embed
build_vst3 = get_option('build-vst3') and not linux_embed
build_juce5_only = get_option('build-juce5-only')
build_juce6_only = get_option('build-juce6-only')
build_juce7_only = get_option('build-juce7-only')
build_universal = get_option('build-universal')
optimizations = get_option('optimizations') and buildtype != 'debug'
lto_optimizations = get_option('lto-optimizations') and buildtype != 'debug'
sse_optimizations = get_option('sse-optimizations') and buildtype != 'debug'

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
    bintype_prefix = 'macOS-'
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

if os_linux
    dependencies = [
        cc.find_library('dl'),
        cc.find_library('rt'),
        dependency('fftw3f').partial_dependency(link_args: false, links: false),
    ]
    dependencies_devices = [
        dependency('alsa'),
    ]
    if not linux_embed
        dependencies += [
            dependency('freetype2'),
            dependency('x11').partial_dependency(link_args: false, links: false),
            dependency('xext').partial_dependency(link_args: false, links: false),
        ]
    endif
elif os_windows
    dependencies = [
        cc.find_library('comdlg32'),
        cc.find_library('gdi32'),
        cc.find_library('imm32'),
        cc.find_library('ole32'),
        cc.find_library('oleaut32'),
        cc.find_library('shlwapi'),
        cc.find_library('uuid'),
        cc.find_library('version'),
        cc.find_library('wininet'),
        cc.find_library('winmm'),
        cc.find_library('ws2_32'),
    ]
    dependencies_devices = [
    ]
else
    dependencies = [
    ]
    dependencies_devices = [
    ]
endif

###############################################################################
# build flags

build_flags = [
    '-DJUCE_APP_CONFIG_HEADER="AppConfig.h"',
    '-fno-common',
    '-pthread',
    '-Wall',
    '-Wno-multichar',
    '-Wno-strict-overflow',
    '-Wno-unused-function',
]

build_flags_cpp = [
    '-std=gnu++11',
]

if not os_darwin
    build_flags += [
        '-Wno-unused-but-set-variable',
    ]
    build_flags_cpp += [
        '-Wno-class-memaccess',
    ]
endif

if not (os_darwin or os_windows)
    build_flags += [
        '-fno-gnu-unique'
    ]
endif

###############################################################################
# build flags (debug)

build_flags_debug = [
    '-O0',
    '-ggdb',
    '-DDEBUG=1',
    '-D_DEBUG=1',
]

build_flags_debug_cpp = [
]

###############################################################################
# build flags (release)

build_flags_release = [
    '-O3',
    '-fvisibility=hidden',
    '-DNDEBUG=1',
    '-fdata-sections',
    '-ffunction-sections',
]

build_flags_release_cpp = [
    '-fvisibility-inlines-hidden',
]

if optimizations or sse_optimizations
    build_flags_release += [
        '-ffast-math',
    ]
endif

if optimizations
    build_flags_release += [
        '-fomit-frame-pointer', '-ftree-vectorize', '-funroll-loops',
    ]
    if not os_darwin
        build_flags_release += [
            '-fprefetch-loop-arrays',
        ]
    endif
endif

if lto_optimizations
    build_flags_release += [
        '-fno-strict-aliasing', '-flto',
    ]
endif

if sse_optimizations or build_universal
    build_flags_release += [
        '-mtune=generic', '-msse', '-msse2',
    ]
    if not build_universal
        build_flags_release += [
            '-mfpmath=sse',
        ]
    endif
endif

###############################################################################
# build flags (per-OS macros and setup)

if os_darwin
    build_flags += [
        '-DMAC=1',
        '-DGL_SILENCE_DEPRECATION=1',
    ]
    build_flags_cpp += [
        '-ObjC++',
    ]
    if build_universal
        build_flags += [
            '-arch', 'x86_64',
            '-arch', 'arm64',
        ]
    endif
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
    '-fdata-sections', 
    '-ffunction-sections',
]

if lto_optimizations
    link_flags_release += [
        '-Werror=odr',
        '-Werror=lto-type-mismatch',
    ]
endif

if os_darwin
    if build_universal
        link_flags += [
            '-arch', 'x86_64',
            '-arch', 'arm64',
        ]
    endif
    link_flags_release += [
        '-Wl,-dead_strip,-dead_strip_dylibs,-x',
    ]
elif os_windows
    link_flags += [
        '-static',
    ]
else
    link_flags += [
        '-Wl,--as-needed,--no-undefined',
    ]
    link_flags_release += [
        '-Wl,-O1,--gc-sections,--strip-all',
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

if not (build_juce6_only or build_juce7_only)
    subdir('ports-juce5')
endif

if not (build_juce5_only or build_juce7_only)
    subdir('ports-juce6')
endif

if not (build_juce5_only or build_juce6_only)
    subdir('ports-juce7')
endif

###############################################################################
# extra files to install

if 'tal-noisemaker' in get_option('plugins')
    extra_lv2_preset_files = [
        'TAL-NoiseMaker-Noise4U.lv2/manifest.ttl',
        'TAL-NoiseMaker-Noise4U.lv2/presets.ttl',
    ]
    foreach preset_file : extra_lv2_preset_files
        install_data([ 'static-lv2-ttl/@0@'.format(preset_file) ],
            install_dir: lv2dir / 'TAL-NoiseMaker-Noise4U.lv2',
        )
    endforeach
endif

if 'vitalium' in get_option('plugins')
    extra_lv2_preset_files = [
        'Vitalium-unfa.lv2/manifest.ttl',
        'Vitalium-unfa.lv2/Analog_Brass.ttl',
        'Vitalium-unfa.lv2/Combat.ttl',
        'Vitalium-unfa.lv2/Dark_Ambient.ttl',
        'Vitalium-unfa.lv2/Dark_Bouncy_Groove.ttl',
        'Vitalium-unfa.lv2/Hardcore_Kick.ttl',
        'Vitalium-unfa.lv2/Kickbass.ttl',
        'Vitalium-unfa.lv2/Koto.ttl',
        'Vitalium-unfa.lv2/Nasty_Growl.ttl',
        'Vitalium-unfa.lv2/Pianium.ttl',
        'Vitalium-unfa.lv2/Power_Lead.ttl',
        'Vitalium-unfa.lv2/Retro_Ambient_Pluck.ttl',
        'Vitalium-unfa.lv2/Sci_Fi_Computer.ttl',
        'Vitalium-unfa.lv2/Sci_Fi_Piano.ttl',
        'Vitalium-unfa.lv2/Sparkly_Dreamy_Pad.ttl',
        'Vitalium-unfa.lv2/Supersaw.ttl',
        'Vitalium-unfa.lv2/Trance_Pluck.ttl',
        'Vitalium-unfa.lv2/Vitalium_Groove.ttl',
    ]
    foreach preset_file : extra_lv2_preset_files
        install_data([ 'static-lv2-ttl/@0@'.format(preset_file) ], 
             install_dir: lv2dir / 'Vitalium-unfa.lv2',
         )
    endforeach
endif

###############################################################################
