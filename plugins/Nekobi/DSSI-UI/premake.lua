
dofile("../../../scripts/make-project.lua")

package = make_distrho_dssi_ui_project("Nekobi")

package.files = {
  matchfiles (
    "../source/DistrhoArtworkNekobi.cpp",
    "../source/DistrhoUINekobi.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
