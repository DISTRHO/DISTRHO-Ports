
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("SegmentJuice")

package.files = {
  matchfiles (
    "../source/SegmentJuiceArtwork.cpp",
    "../source/SegmentJuicePlugin.cpp",
    "../source/SegmentJuiceUI.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
