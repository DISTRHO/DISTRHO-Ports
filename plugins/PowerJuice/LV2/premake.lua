
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("PowerJuice")

package.files = {
  matchfiles (
    "../source/PowerJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
