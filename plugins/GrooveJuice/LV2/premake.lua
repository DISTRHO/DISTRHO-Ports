
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("GrooveJuice")

package.files = {
  matchfiles (
    "../source/GrooveJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
