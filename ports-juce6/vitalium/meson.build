###############################################################################

plugin_extra_build_flags = [
    '-Wno-char-subscripts',
    '-Wno-deprecated-declarations',
    '-Wno-sign-compare',
    '-DNO_AUTH=1',
]

if buildtype == 'release'
    plugin_extra_build_flags += [
        '-ftree-vectorize',
        '-ftree-slp-vectorize',
        '-funroll-loops',
        '-Ofast',
#         '-flto',
    ]
endif

if os_windows and host_machine.cpu() == 'x86'
    plugin_extra_build_flags += [
        '-O0',
    ]
endif

plugin_extra_include_dirs = include_directories([
    '.',
    'source/common',
    'source/common/wavetable',
    'source/interface/editor_components',
    'source/interface/editor_sections',
    'source/interface/look_and_feel',
    'source/interface/wavetable',
    'source/interface/wavetable/editors',
    'source/interface/wavetable/overlays',
    'source/plugin',
    'source/synthesis/synth_engine',
    'source/synthesis/effects',
    'source/synthesis/filters',
    'source/synthesis/framework',
    'source/synthesis/lookups',
    'source/synthesis/modulators',
    'source/synthesis/modules',
    'source/synthesis/producers',
    'source/synthesis/utilities',
    'third_party',
])

if linux_embed
    plugin_srcs = files([
        'BinaryData.cpp',
        'source/unity_build/common.cpp',
        'source/unity_build/plugin.cpp',
        'source/unity_build/synthesis.cpp',
    ])
else
    plugin_srcs = files([
        'BinaryData.cpp',
        'source/unity_build/common.cpp',
        'source/unity_build/interface_editor_components.cpp',
        'source/unity_build/interface_editor_sections.cpp',
        'source/unity_build/interface_editor_sections2.cpp',
        'source/unity_build/interface_look_and_feel.cpp',
        'source/unity_build/interface_wavetable.cpp',
        'source/unity_build/plugin.cpp',
        'source/unity_build/synthesis.cpp',
    ])
endif

plugin_name = 'vitalium'
plugin_uses_opengl = true

###############################################################################
