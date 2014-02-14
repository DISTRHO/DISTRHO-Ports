
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("WobbleJuice")

package.files = {
  matchfiles (
    "../source/WobbleJuiceArtwork.cpp",
    "../source/WobbleJuicePlugin.cpp",
    "../source/WobbleJuiceUI.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
