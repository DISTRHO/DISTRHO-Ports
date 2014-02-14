
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("3BandEQ")

package.files = {
  matchfiles (
    "../source/DistrhoArtwork3BandEQ.cpp",
    "../source/DistrhoUI3BandEQ.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
