
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("GrooveJuice")

package.files = {
  matchfiles (
    "../source/GrooveJuiceArtwork.cpp",
    "../source/GrooveJuiceUI.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
