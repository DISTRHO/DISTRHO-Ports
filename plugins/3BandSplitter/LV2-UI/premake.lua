
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("3BandSplitter")

package.files = {
  matchfiles (
    "../source/DistrhoArtwork3BandSplitter.cpp",
    "../source/DistrhoUI3BandSplitter.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
