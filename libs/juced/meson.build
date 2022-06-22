###############################################################################

lib_juced = static_library('juced',
    sources: [
        'source/juced.cpp'
    ],
    include_directories: [
        include_directories('.'),
        include_directories('../juce5'),
        include_directories('../juce5/source'),
        include_directories('../juce5/source/modules'),
    ],
    cpp_args: build_flags_cpp,
    dependencies: dependencies,
    pic: true,
    install: false,
)

###############################################################################
