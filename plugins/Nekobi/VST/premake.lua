
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("Nekobi")

package.links = {
  package.links,
  "pthread"
}

package.files = {
  matchfiles (
    "../source/DistrhoArtworkNekobi.cpp",
    "../source/DistrhoPluginNekobi.cpp",
    "../source/DistrhoUINekobi.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
