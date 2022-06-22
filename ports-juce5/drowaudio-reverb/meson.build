###############################################################################

if linux_embed
    plugin_srcs = files([
        'source/DRowAudioFilter.cpp',
        '../drowaudio-common/dRowAudio_AllpassFilter.cpp',
        '../drowaudio-common/dRowAudio_DelayRegister.cpp',
        '../drowaudio-common/dRowAudio_LBCF.cpp',
        '../drowaudio-common/dRowAudio_TappedDelayLine.cpp',
    ])
else
    plugin_srcs = files([
        'source/DRowAudioEditorComponent.cpp',
        'source/DRowAudioFilter.cpp',
        '../drowaudio-common/dRowAudio_AllpassFilter.cpp',
        '../drowaudio-common/dRowAudio_DelayRegister.cpp',
        '../drowaudio-common/dRowAudio_LBCF.cpp',
        '../drowaudio-common/dRowAudio_PluginLookAndFeel.cpp',
        '../drowaudio-common/dRowAudio_TappedDelayLine.cpp',
    ])
endif

plugin_name = 'drowaudio-reverb'
plugin_uses_drowaudio = true
plugin_extra_build_flags = build_flags_drowaudio
plugin_extra_include_dirs = include_directories([
    '../drowaudio-common',
])

###############################################################################
