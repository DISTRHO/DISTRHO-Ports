
dofile("../../../scripts/make-project.lua")

package = make_juce_lv2_project("EQinox")

package.config["Release"].links = { package.config["Release"].links, "juced" }
package.config["Debug"].links   = { package.config["Debug"].links, "juced_debug" }

package.files = {
  matchfiles (
    "../source/*.cpp",
    "../source/Components/*.cpp",
    "../source/Filters/*.cpp",
    "../../../libs/juce-plugin/JucePluginMain.cpp"
  )
}
