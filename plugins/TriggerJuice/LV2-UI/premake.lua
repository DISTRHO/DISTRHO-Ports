
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("TriggerJuice")

package.files = {
  matchfiles (
    "../source/TriggerJuiceArtwork.cpp",
    "../source/TriggerJuiceUI.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
