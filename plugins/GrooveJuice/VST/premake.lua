
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("VectorJuice")

package.files = {
  matchfiles (
    "../source/VectorJuiceArtwork.cpp",
    "../source/VectorJuicePlugin.cpp",
    "../source/VectorJuiceUI.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
