###############################################################################

plugin_srcs = files([
    'source/ChannelButtonLookAndFeel.cpp',
    'source/DROMultiplexer.cpp',
    'source/EnumFloatParameter.cpp',
    'source/FloatParameter.cpp',
    'source/IntFloatParameter.cpp',
    'source/OPLLookAndFeel.cpp',
    'source/PluginGui.cpp',
    'source/PluginProcessor.cpp',
    'source/SbiLoader.cpp',
    'source/dbopl.cpp',
    'source/hiopl.cpp',
    'source/nkopl3.cpp',
    'source/tests.cpp',
    'source/zdopl.cpp',
])

plugin_name = 'JuceOPL'

if not os_windows
    plugin_extra_include_dirs = include_directories([
        'source/compat',
    ])
endif

###############################################################################
