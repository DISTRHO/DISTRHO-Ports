
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("StutterJuice")

package.files = {
  matchfiles (
    "../source/StutterJuiceArtwork.cpp",
    "../source/StutterJuiceUI.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
