
dofile("../../../scripts/make-project.lua")

package = make_juce_lv2_project("TAL-Dub-3")

package.includepaths = {
  package.includepaths,
  "." --fake
}

package.files = {
  matchfiles (
    "../source/*.cpp",
    "../../../libs/juce-plugin/JucePluginMain.cpp"
  )
}
