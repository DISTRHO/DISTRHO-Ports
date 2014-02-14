
dofile("../../../scripts/make-project.lua")

package = make_distrho_ladspa_project("PingPongPan")

package.files = {
  matchfiles (
    "../source/DistrhoPluginPingPongPan.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
