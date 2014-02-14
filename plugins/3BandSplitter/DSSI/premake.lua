
dofile("../../../scripts/make-project.lua")

package = make_distrho_dssi_project("3BandSplitter")

package.files = {
  matchfiles (
    "../source/DistrhoPlugin3BandSplitter.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
