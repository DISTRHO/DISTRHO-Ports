###############################################################################

if linux_embed
    plugins = [
        'arctican-function',
        'arctican-pilgrim',
        'drowaudio-distortion',
        'drowaudio-distortionshaper',
        'drowaudio-flanger',
        'drowaudio-reverb',
        'drowaudio-tremolo',
        'luftikus',
        'obxd',
        'tal-dub-3',
        'tal-filter',
        'tal-filter-2',
        'tal-noisemaker',
        'tal-reverb',
        'tal-reverb-2',
        'tal-reverb-3',
        'tal-vocoder-2',
        'temper',
        'vex',
        'wolpertinger',
    ]
else
    plugins = [
        'arctican-function',
        'arctican-pilgrim',
        'dexed',
        'drowaudio-distortion',
        'drowaudio-distortionshaper',
        'drowaudio-flanger',
        'drowaudio-reverb',
        'drowaudio-tremolo',
        'drumsynth',
        'easySSP',
        'eqinox',
        'HiReSam',
        'juce-opl',
        'klangfalter',
        'LUFSMeter',
        'LUFSMeter-Multi',
        'luftikus',
        'obxd',
        'pitchedDelay',
        'refine',
        'stereosourceseparation',
        'tal-dub-3',
        'tal-filter',
        'tal-filter-2',
        'tal-noisemaker',
        'tal-reverb',
        'tal-reverb-2',
        'tal-reverb-3',
        'tal-vocoder-2',
        'temper',
        'vex',
        'wolpertinger',
    ]
endif

plugin_include_dirs = [
    '../../libs/drowaudio/source',
    '../../libs/juced/source',
    '../../libs/juce-legacy',
    '../../libs/juce-legacy/source',
    '../../libs/juce-legacy/source/modules',
    '../../libs/juce-plugin',
    '../../libs/juce-plugin/juce-legacy',
]

###############################################################################
# dependencies

dependencies_plugin = [
]

if os_windows
    dependencies_plugin += [
        cc.find_library('comdlg32'),
        cc.find_library('gdi32'),
        cc.find_library('imm32'),
        cc.find_library('ole32'),
        cc.find_library('oleaut32'),
        cc.find_library('shlwapi'),
        cc.find_library('uuid'),
        cc.find_library('version'),
        cc.find_library('winmm'),
        cc.find_library('wininet'),
        cc.find_library('ws2_32'),
    ]
endif

###############################################################################
# build flags for plugins

build_flags_plugin = [
    '-Werror=deprecated-declarations',
]

build_flags_plugin_debug = [
]

build_flags_plugin_release = [
    '-ffast-math',
]

if optimizations and not linux_embed
    build_flags_plugin_release += [
        '-mfpmath=sse',
    ]
endif

###############################################################################
# link flags for plugins

link_flags_plugin = [
]

if os_darwin
    link_flags_plugin += [
        '-Wl,-export-symbols=../scripts/plugin-symbols.txt',
    ]
else
    link_flags_plugin += [
        '-Wl,--version-script=../scripts/plugin-symbols.version',
    ]
endif

if os_darwin
    link_flags_plugin += [
        '-framework Accelerate',
        '-framework AudioToolbox',
        '-framework AudioUnit',
        '-framework Carbon',
        '-framework Cocoa',
        '-framework CoreAudio',
        '-framework CoreAudioKit',
        '-framework CoreMIDI',
        '-framework IOKit',
        '-framework QuartzCore',
        '-framework WebKit',
    ]
endif

###############################################################################
# combine flags depending on build type

if buildtype == 'debug'
    build_flags_plugin += build_flags_plugin_debug
else
    build_flags_plugin += build_flags_plugin_release
endif

###############################################################################
# format-specific flags

build_flags_plugin_lv2 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=1',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=0',
    '-DJucePlugin_Build_Standalone=0',
    '-DBINTYPE=@0@LV2'.format(bintype_prefix),
]

build_flags_plugin_vst2 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=0',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=1',
    '-DJucePlugin_Build_Standalone=0',
    '-DBINTYPE=@0@VST2'.format(bintype_prefix),
    '-Wno-write-strings',
]

###############################################################################

build_flags_drowaudio = [
    '-DUSE_DROWAUDIO=1',
]

build_flags_juced = [
    '-DUSE_JUCED=1',
]

###############################################################################

foreach plugin : plugins
    plugin_uses_drowaudio = false
    plugin_uses_juced = false
    plugin_uses_opengl = false
    plugin_extra_dependencies = []
    plugin_extra_include_dirs = []
    plugin_extra_build_flags = []
    plugin_extra_link_flags = []
    subdir(plugin)

    link_with_plugin = [
        lib_juce_legacy
    ]

    if plugin_uses_drowaudio
        link_with_plugin += lib_drowaudio
    endif

    if plugin_uses_juced
        link_with_plugin += lib_juced
    endif

    if plugin_uses_opengl
        if os_darwin
          plugin_extra_link_flags += [
              '-framework OpenGL',
          ]
        elif os_windows
            plugin_extra_dependencies += [
                cc.find_library('opengl32'),
            ]
        elif not linux_embed
            plugin_extra_dependencies += [
                dependency('gl')
            ]
        endif
    endif

    if build_lv2
        plugin_lv2_lib = shared_library(plugin_name + '_lv2',
            name_prefix: '',
            sources: plugin_srcs,
            include_directories: [
                include_directories(plugin / 'source'),
                plugin_include_dirs,
                plugin_extra_include_dirs,
            ],
            c_args: build_flags + build_flags_plugin + build_flags_plugin_lv2 + plugin_extra_build_flags,
            cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_lv2 + plugin_extra_build_flags,
            link_args: link_flags + link_flags_plugin,
            link_with: link_with_plugin,
            dependencies: dependencies_plugin + plugin_extra_dependencies,
        )

        plugin_lv2_dir = meson.current_build_dir() / plugin_name + '.lv2'

        plugin_lv2_ttl = custom_target(plugin_name + '_lv2-ttl',
            output: plugin_name + '.lv2',
            input: plugin_lv2_lib,
            command: [
                'mkdir', '-p', plugin_lv2_dir, '&&',
                'cd', plugin_lv2_dir, '&&',
                'cp', plugin_lv2_lib.full_path(), plugin_lv2_dir / plugin_name + lib_suffix, '&&',
                lv2_ttl_generator, '.' / plugin_name + lib_suffix,
            ],
            install: true,
            install_dir: lv2dir,
        )
    endif

    if build_vst2
        plugin_vst2_lib = shared_library(plugin_name,
            name_prefix: '',
            sources: plugin_srcs,
            include_directories: [
                include_directories(plugin / 'source'),
                plugin_include_dirs,
                plugin_extra_include_dirs,
            ],
            c_args: build_flags + build_flags_plugin + build_flags_plugin_vst2 + plugin_extra_build_flags,
            cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_vst2 + plugin_extra_build_flags,
            link_args: link_flags + link_flags_plugin,
            link_with: link_with_plugin,
            dependencies: dependencies_plugin + plugin_extra_dependencies,
            install: true,
            install_dir: vst2dir,
        )
    endif
endforeach

###############################################################################
