
dofile("../../../scripts/make-project.lua")

package = make_juce_vst_project("EasySSP")

-- FIX GCC9 compiler bug, see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90006
package.config["Release"].buildoptions = { "-fno-tree-slp-vectorize" }

package.files = {
  matchfiles (
    "../source/*.cpp",
    "../../../libs/juce-plugin/JucePluginMain.cpp"
  )
}

package.includepaths = {
  package.includepaths,
  "../source/dsp-utility"
}
