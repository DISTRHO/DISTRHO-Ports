
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("3BandEQ")

package.files = {
  matchfiles (
    "../source/DistrhoArtwork3BandEQ.cpp",
    "../source/DistrhoPlugin3BandEQ.cpp",
    "../source/DistrhoUI3BandEQ.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
