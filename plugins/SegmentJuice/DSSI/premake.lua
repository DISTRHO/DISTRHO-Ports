
dofile("../../../scripts/make-project.lua")

package = make_distrho_dssi_project("SegmentJuice")

package.files = {
  matchfiles (
    "../source/SegmentJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
