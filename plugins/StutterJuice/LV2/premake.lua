
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_project("StutterJuice")

package.files = {
  matchfiles (
	 "../source/CModule.hxx",
    "../source/StutterJuicePlugin.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
