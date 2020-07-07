###############################################################################

if linux_embed
    plugin_srcs = files([
        'source/TalCore.cpp',
        'source/engine/vocoder/Fft.cpp',
    ])
else
    plugin_srcs = files([
        'source/TalComponent.cpp',
        'source/TalCore.cpp',
        'source/engine/vocoder/Fft.cpp',
    ])
endif

plugin_name = 'TAL-Vocoder-2'
plugin_extra_include_dirs = include_directories([
    'source/engine',
])

###############################################################################
