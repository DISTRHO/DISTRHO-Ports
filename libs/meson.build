###############################################################################

subdir('lv2-ttl-generator')

if not (build_juce6_only or build_juce7_only)
    subdir('drowaudio')
    subdir('juced')
    subdir('juce5')
endif

if not (build_juce5_only or build_juce7_only)
    subdir('juce6')
endif

if not (build_juce5_only or build_juce6_only)
    subdir('juce7')
endif

###############################################################################
