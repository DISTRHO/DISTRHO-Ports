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
    include_directories('../libs/drowaudio/source'),
    include_directories('../libs/juced/source'),
    include_directories('../libs/juce5'),
    include_directories('../libs/juce5/source'),
    include_directories('../libs/juce5/source/modules'),
    include_directories('../libs/juce-plugin'),
]

###############################################################################
# dependencies

if os_linux and not linux_embed
    dependencies_plugin = [
        dependency('x11'),
        dependency('xext'),
    ]
else
    dependencies_plugin = [
    ]
endif

###############################################################################
# build flags for plugins

build_flags_plugin = [
    '-Werror=deprecated-declarations',
]

###############################################################################
# format-specific build flags

build_flags_plugin_lv2 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=1',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=0',
    '-DJucePlugin_Build_Standalone=0',
    '-DBINTYPE=@0@LV2'.format(bintype_prefix),
    '-Wno-parentheses',
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
# format-specific link flags

link_flags_plugin_common = [
]

if os_darwin
    link_flags_plugin_common += [
        '-framework', 'Accelerate',
        '-framework', 'AudioToolbox',
        '-framework', 'AudioUnit',
        '-framework', 'Carbon',
        '-framework', 'Cocoa',
        '-framework', 'CoreAudio',
        '-framework', 'CoreAudioKit',
        '-framework', 'CoreMIDI',
        '-framework', 'IOKit',
        '-framework', 'QuartzCore',
        '-framework', 'WebKit',
    ]
    link_flags_plugin_lv2 = [
        '-Wl,-exported_symbol,_lv2_generate_ttl',
        '-Wl,-exported_symbol,_lv2_descriptor',
        '-Wl,-exported_symbol,_lv2ui_descriptor',
    ]
    link_flags_plugin_vst2 = [
        '-Wl,-exported_symbol,_VSTPluginMain',
    ]
else
    link_flags_plugin_lv2 = [
        '-Wl,--version-script=' + meson.source_root() + '/scripts/plugin-symbols-lv2.version',
    ]
    link_flags_plugin_vst2 = [
        '-Wl,--version-script=' + meson.source_root() + '/scripts/plugin-symbols-vst2.version',
    ]
endif

###############################################################################

build_flags_drowaudio = [
    '-DUSE_DROWAUDIO=1',
]

build_flags_juced = [
    '-DUSE_JUCED=1',
]

###############################################################################

if build_lv2 or build_vst2
    foreach plugin : plugins
        if plugin in get_option('plugins')
            plugin_uses_drowaudio = false
            plugin_uses_juced = false
            plugin_uses_devices = false
            plugin_uses_opengl = false
            plugin_extra_dependencies = []
            plugin_extra_include_dirs = []
            plugin_extra_build_flags = []
            plugin_extra_link_flags = []
            plugin_extra_format_specific_srcs = []

            subdir(plugin)

            plugin_extra_format_specific_srcs += files([
                '../libs/juce-plugin/JucePluginMain.cpp',
                '../libs/juce-plugin/JucePluginUtils.cpp',
            ])

            link_with_plugin = [
                lib_juce5
            ]

            if plugin_uses_drowaudio
                link_with_plugin += lib_drowaudio
            endif

            if plugin_uses_juced
                link_with_plugin += lib_juced
            endif

            if plugin_uses_devices
                link_with_plugin += lib_juce5_devices
                if os_linux
                    plugin_extra_dependencies += [
                        dependency('alsa'),
                    ]
                endif
            endif

            if plugin_uses_opengl or buildtype == 'debug'
                if os_darwin
                    plugin_extra_link_flags += [
                        '-framework', 'OpenGL',
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

            plugin_lib = static_library(plugin_name + '_lib',
                name_prefix: '',
                sources: plugin_srcs,
                include_directories: [
                    include_directories(plugin / 'source'),
                    plugin_include_dirs,
                    plugin_extra_include_dirs,
                ],
                c_args: build_flags + build_flags_plugin + plugin_extra_build_flags,
                cpp_args: build_flags_cpp + build_flags_plugin + plugin_extra_build_flags,
                dependencies: dependencies_plugin + plugin_extra_dependencies,
                pic: true,
                install: false,
            )

            link_with_plugin += plugin_lib

            if build_lv2
                plugin_lv2_lib = shared_library(plugin_name + '_lv2',
                    name_prefix: '',
                    sources: plugin_extra_format_specific_srcs,
                    include_directories: [
                        include_directories(plugin / 'source'),
                        plugin_include_dirs,
                        plugin_extra_include_dirs,
                    ],
                    c_args: build_flags + build_flags_plugin + build_flags_plugin_lv2 + plugin_extra_build_flags,
                    cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_lv2 + plugin_extra_build_flags,
                    link_args: link_flags + link_flags_plugin_common + link_flags_plugin_lv2 + plugin_extra_link_flags,
                    link_with: link_with_plugin,
                )

                plugin_lv2_dir = meson.current_build_dir() / plugin_name + '.lv2'

                plugin_lv2_ttl = custom_target(plugin_name + '_lv2-ttl',
                    output: plugin_name + '.lv2',
                    input: plugin_lv2_lib,
                    command: [
                        'mkdir', '-p', plugin_lv2_dir, '&&',
                        'cd', plugin_lv2_dir, '&&',
                        'cp', plugin_lv2_lib.full_path(), plugin_lv2_dir / plugin_name + lib_suffix, '&&',
                        (meson.is_cross_build() ? 'wine' : 'env'), lv2_ttl_generator, '.' / plugin_name + lib_suffix,
                    ],
                    install: true,
                    install_dir: lv2dir,
                )
            endif

            if build_vst2
                plugin_vst2_lib = shared_library(plugin_name,
                    name_prefix: '',
                    sources: plugin_extra_format_specific_srcs,
                    include_directories: [
                        include_directories(plugin / 'source'),
                        plugin_include_dirs,
                        plugin_extra_include_dirs,
                    ],
                    c_args: build_flags + build_flags_plugin + build_flags_plugin_vst2 + plugin_extra_build_flags,
                    cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_vst2 + plugin_extra_build_flags,
                    link_args: link_flags + link_flags_plugin_common + link_flags_plugin_vst2 + plugin_extra_link_flags,
                    link_with: link_with_plugin,
                    install: true,
                    install_dir: vst2dir,
                )
            endif
        endif
    endforeach
endif

###############################################################################
