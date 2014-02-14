
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("Nekobi")

package.links = {
  package.links,
  "pthread"
}

package.files = {
  matchfiles (
    "../source/DistrhoPluginNekobi.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
