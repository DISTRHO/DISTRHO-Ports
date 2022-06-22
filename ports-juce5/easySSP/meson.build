###############################################################################

plugin_srcs = files([
    'source/AboutLayout.cpp',
    'source/BinaryData.cpp',
    'source/MainLayout.cpp',
    'source/PluginProcessor.cpp',
    'source/PreferencesLayout.cpp',
])

plugin_name = 'EasySSP'

# FIX GCC9 compiler bug, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90006
plugin_extra_build_flags = [
    '-fno-tree-slp-vectorize',
]

plugin_extra_include_dirs = include_directories([
    'source/dsp-utility',
])

###############################################################################
