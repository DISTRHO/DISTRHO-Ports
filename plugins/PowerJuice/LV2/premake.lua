
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_combined_project("PowerJuice")

package.files = {
  matchfiles (
    "../source/PowerJuiceArtwork.cpp",
    "../source/PowerJuicePlugin.cpp",
    "../source/PowerJuiceUI.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
