###############################################################################

if linux_embed
    plugin_srcs = files([
        'source/PluginProcessor.cpp',
    ])
else
    plugin_srcs = files([
        'source/PluginEditor.cpp',
        'source/PluginLookAndFeel.cpp',
        'source/PluginProcessor.cpp',
        'source/TremoloBufferView.cpp',
    ])
endif

plugin_name = 'drowaudio-tremolo'
plugin_uses_drowaudio = true
plugin_extra_build_flags = build_flags_drowaudio
plugin_extra_include_dirs = include_directories([
    '../drowaudio-common',
])

###############################################################################
