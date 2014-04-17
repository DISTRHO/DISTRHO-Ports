
dofile("../../../scripts/make-project.lua")

package = make_distrho_dssi_ui_project("SegmentJuice")

package.files = {
  matchfiles (
    "../source/SegmentJuiceArtwork.cpp",
    "../source/SegmentJuiceUI.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
