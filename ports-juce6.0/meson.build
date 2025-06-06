###############################################################################

if linux_headless
    plugins = [
        'vitalium',
    ]
else
    plugins = [
        'vitalium',
    ]
endif

plugin_include_dirs = [
    include_directories('../libs/juce6.0'),
    include_directories('../libs/juce6.0/source'),
    include_directories('../libs/juce6.0/source/modules'),
    include_directories('../libs/juce6.0/source/modules/juce_audio_processors/format_types/VST3_SDK'),
    include_directories('../libs/juce-plugin'),
    juce60_extra_include_dirs
]

###############################################################################
# build flags for plugins

build_flags_plugin = [
    '-Werror=deprecated-declarations',
]

build_flag_plugin_cpp = [
    '-std=gnu++14',
]

###############################################################################
# format-specific build flags

build_flags_plugin_lv2 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=1',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=0',
    '-DJucePlugin_Build_VST3=0',
    '-DJucePlugin_Build_Standalone=0',
]

build_flags_plugin_vst2 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=0',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=1',
    '-DJucePlugin_Build_VST3=0',
    '-DJucePlugin_Build_Standalone=0',
]

build_flags_plugin_vst3 = [
    '-DJucePlugin_Build_AU=0',
    '-DJucePlugin_Build_LV2=0',
    '-DJucePlugin_Build_RTAS=0',
    '-DJucePlugin_Build_VST=0',
    '-DJucePlugin_Build_VST3=1',
    '-DJucePlugin_Build_Standalone=0',
    '-Wno-non-virtual-dtor',
]

if os_windows
    build_flags_plugin_vst3 += [
        '-D_NATIVE_WCHAR_T_DEFINED',
        '-D__wchar_t=wchar_t',
    ]
endif

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
    link_flags_plugin_vst3 = [
        '-Wl,-exported_symbol,_bundleEntry',
        '-Wl,-exported_symbol,_bundleExit',
        '-Wl,-exported_symbol,_GetPluginFactory',
    ]
else
    link_flags_plugin_lv2 = [
        '-Wl,--version-script=' + meson.source_root() + '/scripts/plugin-symbols-lv2.version',
    ]
    link_flags_plugin_vst2 = [
        '-Wl,--version-script=' + meson.source_root() + '/scripts/plugin-symbols-vst2.version',
    ]
    link_flags_plugin_vst3 = [
        '-Wl,--version-script=' + meson.source_root() + '/scripts/plugin-symbols-vst3.version',
    ]
endif

###############################################################################

foreach plugin : plugins
    if plugin in get_option('plugins')
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

        if plugin_uses_devices
            link_with_plugin += lib_juce60_devices
            if os_linux
                plugin_extra_dependencies += [
                    dependency('alsa')
                ]
            endif
        endif

        if plugin_uses_opengl
            if os_darwin
                plugin_extra_link_flags += [
                    '-framework', 'OpenGL',
                ]
            elif os_windows
                plugin_extra_dependencies += [
                    cc.find_library('opengl32'),
                ]
            elif not linux_headless
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
            dependencies: plugin_extra_dependencies,
            pic: true,
            install: false,
        )

        if build_lv2
            plugin_lv2_lib = shared_library(plugin_name + '-lv2',
                name_prefix: '',
                sources: plugin_extra_format_specific_srcs,
                include_directories: [
                    include_directories(plugin),
                    plugin_include_dirs,
                    plugin_extra_include_dirs,
                ],
                c_args: build_flags + build_flags_plugin + build_flags_plugin_lv2 + plugin_extra_build_flags,
                cpp_args: build_flags_cpp + build_flags_plugin + build_flags_plugin_lv2 + build_flag_plugin_cpp + plugin_extra_build_flags,
                link_args: link_flags + link_flags_plugin_common + link_flags_plugin_lv2 + plugin_extra_link_flags,
                link_with: [ lib_juce60, plugin_lib ],
            )

            custom_target(plugin_name + '-lv2-bundle',
                output: plugin_name + '.lv2',
                input: plugin_lv2_lib,
                command: [ '../scripts/generate-lv2.sh', lv2_ttl_generator, '@OUTPUT@', plugin_lv2_lib.full_path() ],
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
                link_args: link_flags + link_flags_plugin_common + link_flags_plugin_vst2 + plugin_extra_link_flags,
                link_with: [ lib_juce60, plugin_lib ],
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
                link_args: link_flags + link_flags_plugin_common + link_flags_plugin_vst3 + plugin_extra_link_flags,
                link_with: [ lib_juce60, plugin_lib ],
            )

            plugin_vst3_dir = meson.current_build_dir() / plugin_name + '.vst3' / 'Contents' / vst3_bundle_name

            plugin_vst3 = custom_target(plugin_name + '-vst3-bundle',
                output: plugin_name + '.vst3',
                input: plugin_vst3_lib,
                command: [
                    'mkdir', '-p', plugin_vst3_dir, '&&',
                    'cd', plugin_vst3_dir, '&&',
                    'cp', plugin_vst3_lib.full_path(), plugin_vst3_dir / plugin_name + lib_suffix,
                ],
                install: true,
                install_dir: vst3dir,
            )
        endif
    endif
endforeach

###############################################################################
