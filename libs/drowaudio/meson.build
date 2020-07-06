###############################################################################

lib_drowaudio = static_library('drowaudio',
    sources: [
        'source' / 'dRowAudio' / 'dRowAudio.cpp'
    ],
    include_directories: [
        include_directories('.'),
        include_directories('..' / 'juce-legacy'),
        include_directories('..' / 'juce-legacy' / 'source'),
        include_directories('..' / 'juce-legacy' / 'source' / 'modules'),
    ],
    cpp_args: build_flags_cpp,
    pic: true,
    install: false,
)

###############################################################################
