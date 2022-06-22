###############################################################################

if linux_embed
    plugin_srcs = files([
        'source/ADSRenv.cpp',
        'source/synth.cpp',
    ])
else
    plugin_srcs = files([
        'source/ADSRenv.cpp',
        'source/KeyboardButton.cpp',
        'source/PresetComboBox.cpp',
        'source/RotatingToggleButton.cpp',
        'source/about.cpp',
        'source/editor.cpp',
        'source/synth.cpp',
        'source/tabbed-editor.cpp',
    ])
endif

plugin_name = 'Wolpertinger'

plugin_extra_build_flags = [
    '-DWOLPVERSION=0041',
    '-DWOLPVERSIONSTRING=0.4.1',
]

if buildtype == 'debug'
    plugin_extra_build_flags += [
        '-DCONFIGURATION=Debug',
    ]
else
    plugin_extra_build_flags += [
        '-DCONFIGURATION=Release',
    ]
endif

###############################################################################
