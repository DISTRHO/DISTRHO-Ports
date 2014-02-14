
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("3BandSplitter")

package.files = {
  matchfiles (
    "../source/DistrhoPlugin3BandSplitter.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
