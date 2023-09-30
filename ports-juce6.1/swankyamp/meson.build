###############################################################################

plugin_extra_include_dirs = include_directories([
    'source',
    'thirdparty/optional-lite/include',
])

if linux_embed
    plugin_srcs = files([
        'source/PluginProcessor.cpp',
        'source/Utils.cpp',
    ])
else
    plugin_srcs = files([
        'source/BinaryData.cpp',
        'source/NewComponent.cpp',
        'source/PluginEditor.cpp',
        'source/PluginProcessor.cpp',
        'source/PresetManager.cpp',
        'source/SwankyAmpLAF.cpp',
        'source/TooltipsData.cpp',
        'source/Utils.cpp',
        'source/Components/AmpGroup.cpp',
        'source/Components/CabGroup.cpp',
        'source/Components/LevelMeter.cpp',
        'source/Components/LevelsGroup.cpp',
        'source/Components/ParameterGroup.cpp',
        'source/Components/PowerAmpGroup.cpp',
        'source/Components/PreAmpGroup.cpp',
        'source/Components/PresetGroup.cpp',
        'source/Components/RButton.cpp',
        'source/Components/RSlider.cpp',
        'source/Components/RSliderLabel.cpp',
        'source/Components/StagingGroup.cpp',
        'source/Components/ToneStackGroup.cpp',
    ])
endif

plugin_name = 'SwankyAmp'

###############################################################################
