###############################################################################

plugin_srcs = files([
    'source/BinaryData.cpp',
    'source/PluginEditor.cpp',
    'source/PluginProcessor.cpp',
    'source/dsp/BandLimit.cpp',
    'source/dsp/delaytabdsp.cpp',
    'source/dsp/pitcheddelay.cpp',
    'source/dsp/simpledetune.cpp',
    'source/gui/DelayGraph.cpp',
    'source/gui/pitcheddelaytab.cpp',
])

plugin_name = 'PitchedDelay'

if host_machine.cpu_family() == 'arm'
    plugin_extra_build_flags = [
        '-mfpu=neon',
    ]
endif

###############################################################################
