###############################################################################

if linux_embed
    plugin_srcs = files([
        'source/DRowAudioFilter.cpp',
    ])
else
    plugin_srcs = files([
        'source/DRowAudioEditorComponent.cpp',
        'source/DRowAudioFilter.cpp',
        '../drowaudio-common/dRowAudio_PluginLookAndFeel.cpp',
    ])
endif

plugin_name = 'drowaudio-flanger'
plugin_uses_drowaudio = true
plugin_extra_build_flags = build_flags_drowaudio
plugin_extra_include_dirs = include_directories([
    '../drowaudio-common',
])

###############################################################################
