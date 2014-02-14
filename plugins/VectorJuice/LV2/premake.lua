
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("VectorJuice")

package.files = {
  matchfiles (
    "../source/VectorJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
