
dofile("../../../scripts/make-project.lua")

package = make_distrho_vst_project("PingPongPan")

package.files = {
  matchfiles (
    "../source/DistrhoArtworkPingPongPan.cpp",
    "../source/DistrhoPluginPingPongPan.cpp",
    "../source/DistrhoUIPingPongPan.cpp",
    "../../../libs/distrho/DistrhoPluginMain.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
