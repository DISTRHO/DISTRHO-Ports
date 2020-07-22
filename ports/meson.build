###############################################################################

if linux_embed
    # TODO deal with this later
    plugins = []
else
    plugins = [
        'chow',
    ]
endif

plugin_include_dirs = [
    include_directories('../libs/juce-current'),
    include_directories('../libs/juce-current/source'),
    include_directories('../libs/juce-current/source/modules'),
    include_directories('../libs/juce-current/source/modules/juce_audio_processors/format_types/VST3_SDK'),
    include_directories('../libs/juce-plugin'),
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

build_flag_plugin_cpp = [
    '-std=gnu++14',
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
        '-Wl,-export-symbols=' + meson.source_root() + '/scripts/plugin-symbols.txt',
    ]
else
    link_flags_plugin += [
        '-Wl,--version-script=' + meson.source_root() + '/scripts/plugin-symbols.version',
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
    '-DJucePlugin_Build_VST3=0',
    '-DJucePlugin_Build_Standalone=0',
    '-DBINTYPE=@0@LV2'.format(bintype_prefix),
    '-Wno-parentheses',
    '-Wno-deprecated-declarations', # FIXME
]

build_flags_plugin_vst2 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=0',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=1',
    '-DJucePlugin_Build_VST3=0',
    '-DJucePlugin_Build_Standalone=0',
    '-DBINTYPE=@0@VST2'.format(bintype_prefix),
    '-Wno-write-strings',
]

build_flags_plugin_vst3 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=0',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=0',
    '-DJucePlugin_Build_VST3=1',
    '-DJucePlugin_Build_Standalone=0',
    '-DBINTYPE=@0@VST3'.format(bintype_prefix),
    '-Wno-non-virtual-dtor',
]

if os_windows
    build_flags_plugin_vst3 += [
        '-D_NATIVE_WCHAR_T_DEFINED',
        '-D__wchar_t=wchar_t',
    ]
endif

###############################################################################

foreach plugin : plugins
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

    plugin_lib = static_library(plugin_name + '_lib',
        name_prefix: '',
        sources: plugin_srcs,
        include_directories: [
            include_directories(plugin),
            plugin_include_dirs,
            plugin_extra_include_dirs,
        ],
        c_args: build_flags + build_flags_plugin + plugin_extra_build_flags,
        cpp_args: build_flags_cpp + build_flags_plugin + build_flag_plugin_cpp + plugin_extra_build_flags,
        dependencies: dependencies_plugin + plugin_extra_dependencies,
        pic: true,
        install: false,
    )

    if build_lv2
        plugin_lv2_lib = shared_library(plugin_name + '_lv2',
            name_prefix: '',
            sources: plugin_extra_format_specific_srcs,
            include_directories: [
                include_directories(plugin),
                plugin_include_dirs,
                plugin_extra_include_dirs,
            ],
            c_args: build_flags + build_flags_plugin + build_flags_plugin_lv2 + plugin_extra_build_flags,
            cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_lv2 + build_flag_plugin_cpp + plugin_extra_build_flags,
            link_args: link_flags + link_flags_plugin,
            link_with: [ lib_juce_current, plugin_lib ],
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
        plugin_vst2 = shared_library(plugin_name,
            name_prefix: '',
            sources: plugin_extra_format_specific_srcs,
            include_directories: [
                include_directories(plugin),
                plugin_include_dirs,
                plugin_extra_include_dirs,
            ],
            c_args: build_flags + build_flags_plugin + build_flags_plugin_vst2 + plugin_extra_build_flags,
            cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_vst2 + build_flag_plugin_cpp + plugin_extra_build_flags,
            link_args: link_flags + link_flags_plugin,
            link_with: [ lib_juce_current, plugin_lib ],
            install: true,
            install_dir: vst2dir,
        )
    endif

    if build_vst3
        plugin_vst3_lib = shared_library(plugin_name + '-vst3',
            name_prefix: '',
            sources: plugin_extra_format_specific_srcs,
            include_directories: [
                include_directories(plugin),
                plugin_include_dirs,
                plugin_extra_include_dirs,
            ],
            c_args: build_flags + build_flags_plugin + build_flags_plugin_vst3 + plugin_extra_build_flags,
            cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_vst3 + build_flag_plugin_cpp + plugin_extra_build_flags,
            link_args: link_flags + link_flags_plugin,
            link_with: [ lib_juce_current, plugin_lib ],
        )

        plugin_vst3 = custom_target(plugin_name + '.vst3',
            output: plugin_name + '.vst3',
            input: plugin_vst3_lib,
            command: [
                'mv', plugin_vst3_lib.full_path(), meson.current_build_dir() / plugin_name + '.vst3'
            ],
            install: true,
            install_dir: vst3dir,
        )
    endif
endforeach

###############################################################################
