
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("PowerJuice")

package.files = {
  matchfiles (
    "../source/PowerJuiceArtwork.cpp",
    "../source/PowerJuiceUI.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}

package.links = {
  package.links,
  "rt"
}
