###############################################################################

if linux_embed
    plugin_srcs = files([
        'source/FaustUIBridge.cpp',
        'source/BinaryData.cpp',
        'source/PluginProcessor.cpp',
    ])
else
    plugin_srcs = files([
        'source/BinaryData.cpp',
        'source/FaustUIBridge.cpp',
        'source/MainComponent.cpp',
        'source/PluginEditor.cpp',
        'source/PluginProcessor.cpp',
        'source/RestrictionProcessor.cpp',
        'source/SpectroscopeComponent.cpp',
        'source/TemperLookAndFeel.cpp',
    ])
endif

plugin_name = 'Temper'
plugin_uses_opengl = true

###############################################################################
