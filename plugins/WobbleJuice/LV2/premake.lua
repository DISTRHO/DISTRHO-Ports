
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("WobbleJuice")

package.files = {
  matchfiles (
    "../source/WobbleJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
