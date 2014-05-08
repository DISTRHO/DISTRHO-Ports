
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("StutterJuice")

package.files = {
  matchfiles (
    "../source/StutterJuiceArtwork.cpp",
    "../source/StutterJuicePlugin.cpp",
    "../source/StutterJuiceUI.cpp",
	 "../source/CModule.hxx",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
