
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("VectorJuice")

package.files = {
  matchfiles (
    "../source/VectorJuiceArtwork.cpp",
    "../source/VectorJuiceUI.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
