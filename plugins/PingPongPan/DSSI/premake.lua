
dofile("../../../scripts/make-project.lua")

package = make_distrho_dssi_project("PingPongPan")

package.files = {
  matchfiles (
    "../source/DistrhoPluginPingPongPan.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp"
  )
}
