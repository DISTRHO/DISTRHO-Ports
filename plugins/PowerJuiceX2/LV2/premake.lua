
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_combined_project("PowerJuiceX2")

package.files = {
  matchfiles (
    "../source/PowerJuiceX2Artwork.cpp",
    "../source/PowerJuiceX2Plugin.cpp",
    "../source/PowerJuiceX2UI.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
