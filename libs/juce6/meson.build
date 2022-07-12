###############################################################################

if linux_embed
    juce6_srcs = [
        'source/modules/juce_audio_basics/juce_audio_basics.cpp',
        'source/modules/juce_audio_formats/juce_audio_formats.cpp',
        'source/modules/juce_audio_processors/juce_audio_processors.cpp',
        'source/modules/juce_audio_utils/juce_audio_utils.cpp',
        'source/modules/juce_core/juce_core.cpp',
        'source/modules/juce_cryptography/juce_cryptography.cpp',
        'source/modules/juce_data_structures/juce_data_structures.cpp',
        'source/modules/juce_dsp/juce_dsp.cpp',
        'source/modules/juce_events/juce_events.cpp',
    ]
else
    juce6_srcs = [
        'source/modules/juce_audio_basics/juce_audio_basics.cpp',
        'source/modules/juce_audio_formats/juce_audio_formats.cpp',
        'source/modules/juce_audio_processors/juce_audio_processors.cpp',
        'source/modules/juce_audio_utils/juce_audio_utils.cpp',
        'source/modules/juce_core/juce_core.cpp',
        'source/modules/juce_cryptography/juce_cryptography.cpp',
        'source/modules/juce_data_structures/juce_data_structures.cpp',
        'source/modules/juce_dsp/juce_dsp.cpp',
        'source/modules/juce_events/juce_events.cpp',
        'source/modules/juce_graphics/juce_graphics.cpp',
        'source/modules/juce_gui_basics/juce_gui_basics.cpp',
        'source/modules/juce_gui_extra/juce_gui_extra.cpp',
        'source/modules/juce_opengl/juce_opengl.cpp',
    ]
endif

juce6_devices_srcs = [
    'source/modules/juce_audio_devices/juce_audio_devices.cpp',
]

juce6_extra_cpp_args = [
    '-std=gnu++14',
    '-Wno-non-virtual-dtor',
]

if os_windows
    juce6_extra_cpp_args += [
        '-D_NATIVE_WCHAR_T_DEFINED',
        '-D__wchar_t=wchar_t',
        '-Wno-unknown-pragmas',
    ]
    if host_machine.cpu() == 'x86'
        juce6_extra_cpp_args += [
            '-mpreferred-stack-boundary=2',
        ]
    endif
endif

lib_juce6 = static_library('juce6',
    sources: juce6_srcs,
    include_directories: [
        include_directories('.'),
        include_directories('source'),
        include_directories('source/modules'),
        include_directories('../juced/source/dependancies/ladspa_sdk/src'),
    ],
    cpp_args: build_flags_cpp + juce6_extra_cpp_args,
    dependencies: dependencies,
    pic: true,
    install: false,
)

lib_juce6_device = static_library('juce6-devices',
    sources: juce6_devices_srcs,
    include_directories: [
        include_directories('.'),
        include_directories('source'),
        include_directories('source/modules'),
    ],
    cpp_args: build_flags_cpp + juce6_extra_cpp_args + ['-DJUCE_MODULE_AVAILABLE_juce_audio_devices=1'],
    dependencies: dependencies_devices,
    pic: true,
    install: false,
)

###############################################################################
