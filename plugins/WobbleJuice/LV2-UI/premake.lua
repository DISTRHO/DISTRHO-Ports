
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("WobbleJuice")

package.files = {
  matchfiles (
    "../source/WobbleJuiceArtwork.cpp",
    "../source/WobbleJuiceUI.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
