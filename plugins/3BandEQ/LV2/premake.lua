
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("3BandEQ")

package.files = {
  matchfiles (
    "../source/DistrhoPlugin3BandEQ.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
