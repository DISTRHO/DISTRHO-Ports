
dofile("../../../scripts/make-project.lua")

package = make_library_project("juce")

package.includepaths = {
  ".",
  "../source",
  "../../../sdks/vstsdk2.4/"
}

package.files = {
  matchfiles (
    "../source/modules/juce_audio_basics/juce_audio_basics.cpp",
    "../source/modules/juce_audio_devices/juce_audio_devices.cpp",
    "../source/modules/juce_audio_formats/juce_audio_formats.cpp",
    "../source/modules/juce_audio_processors/juce_audio_processors.cpp",
    "../source/modules/juce_audio_utils/juce_audio_utils.cpp",
    "../source/modules/juce_core/juce_core.cpp",
    "../source/modules/juce_data_structures/juce_data_structures.cpp",
    "../source/modules/juce_events/juce_events.cpp",
    "../source/modules/juce_graphics/juce_graphics.cpp",
    "../source/modules/juce_gui_basics/juce_gui_basics.cpp",
    "../source/modules/juce_gui_extra/juce_gui_extra.cpp"
  )
}
