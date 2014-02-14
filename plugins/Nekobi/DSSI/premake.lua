
dofile("../../../scripts/make-project.lua")

package = make_distrho_dssi_project("Nekobi")

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
