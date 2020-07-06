###############################################################################

lib_juced = static_library('juced',
    sources: [
        'source' / 'juced.cpp'
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
