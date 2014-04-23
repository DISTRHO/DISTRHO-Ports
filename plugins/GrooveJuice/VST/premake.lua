
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("GrooveJuice")

package.files = {
  matchfiles (
    "../source/GrooveJuiceArtwork.cpp",
    "../source/GrooveJuicePlugin.cpp",
    "../source/GrooveJuiceUI.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
