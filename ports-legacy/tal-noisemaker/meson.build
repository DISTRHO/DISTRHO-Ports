###############################################################################

if linux_embed
    plugin_srcs = files([
        'source/TalCore.cpp',
        'source/Engine/Lfo.cpp',
    ])
else
    plugin_srcs = files([
        'source/TalComponent.cpp',
        'source/TalCore.cpp',
        'source/Engine/Lfo.cpp',
    ])
endif

plugin_name = 'TAL-NoiseMaker'
plugin_extra_include_dirs = include_directories([
    'source/Engine',
])

###############################################################################
