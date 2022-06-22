###############################################################################

lib_drowaudio = static_library('drowaudio',
    sources: [
        'source/dRowAudio/dRowAudio.cpp'
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
