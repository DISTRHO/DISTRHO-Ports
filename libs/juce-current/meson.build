###############################################################################

if linux_embed
    juce_current_srcs = [
        'source/juce_audio_basics/juce_audio_basics.cpp',
        'source/juce_audio_devices/juce_audio_devices.cpp',
        'source/juce_audio_formats/juce_audio_formats.cpp',
        'source/juce_audio_processors/juce_audio_processors.cpp',
        'source/juce_audio_utils/juce_audio_utils.cpp',
        'source/juce_core/juce_core.cpp',
        'source/juce_cryptography/juce_cryptography.cpp',
        'source/juce_data_structures/juce_data_structures.cpp',
        'source/juce_dsp/juce_dsp.cpp',
        'source/juce_events/juce_events.cpp',
    ]
else
    juce_current_srcs = [
        'source/juce_audio_basics/juce_audio_basics.cpp',
        'source/juce_audio_devices/juce_audio_devices.cpp',
        'source/juce_audio_formats/juce_audio_formats.cpp',
        'source/juce_audio_processors/juce_audio_processors.cpp',
        'source/juce_audio_utils/juce_audio_utils.cpp',
        'source/juce_core/juce_core.cpp',
        'source/juce_cryptography/juce_cryptography.cpp',
        'source/juce_data_structures/juce_data_structures.cpp',
        'source/juce_dsp/juce_dsp.cpp',
        'source/juce_events/juce_events.cpp',
        'source/juce_graphics/juce_graphics.cpp',
        'source/juce_gui_basics/juce_gui_basics.cpp',
        'source/juce_gui_extra/juce_gui_extra.cpp',
        'source/juce_opengl/juce_opengl.cpp',
    ]
endif

lib_juce_current = static_library('juce-current',
    sources: juce_current_srcs,
    include_directories: [
        include_directories('.'),
        include_directories('source'),
        include_directories('..' / 'juced' / 'source' / 'dependancies' / 'ladspa_sdk' / 'src'),
    ],
    cpp_args: build_flags_cpp + ['-std=gnu++14'],
    dependencies: dependencies,
    pic: true,
    install: false,
)

###############################################################################
