###############################################################################

plugin_extra_include_dirs = include_directories([
    'source',
])

if linux_embed
    plugin_srcs = files([
        'source/PluginProcessor.cpp',
    ])
else
    plugin_srcs = files([
        'source/PluginEditor.cpp',
        'source/PluginProcessor.cpp',
        'source/Gui/BinaryData.cpp',
    ])
endif

plugin_name = 'Obxd'

###############################################################################
