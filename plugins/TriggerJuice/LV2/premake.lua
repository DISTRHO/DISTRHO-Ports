
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("TriggerJuice")

package.files = {
  matchfiles (
    "../source/TriggerJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
