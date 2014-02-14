
dofile("../../../scripts/make-project.lua")

package = make_juce_lv2_project("drowaudio-reverb")

package.config["Release"].links = { package.config["Release"].links, "drowaudio" }
package.config["Debug"].links   = { package.config["Debug"].links, "drowaudio_debug" }

package.files = {
  matchfiles (
    "../source/*.cpp",
    "../../../libs/juce-plugin/JucePluginMain.cpp"
  )
}
