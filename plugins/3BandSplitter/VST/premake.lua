
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("3BandSplitter")

package.files = {
  matchfiles (
    "../source/DistrhoArtwork3BandSplitter.cpp",
    "../source/DistrhoPlugin3BandSplitter.cpp",
    "../source/DistrhoUI3BandSplitter.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
