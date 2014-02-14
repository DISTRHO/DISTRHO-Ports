
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("SegmentJuice")

package.files = {
  matchfiles (
    "../source/SegmentJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
