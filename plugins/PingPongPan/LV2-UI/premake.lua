
dofile("../../../scripts/make-project.lua")

package = make_distrho_lv2_ui_project("PingPongPan")

package.files = {
  matchfiles (
    "../source/DistrhoArtworkPingPongPan.cpp",
    "../source/DistrhoUIPingPongPan.cpp",
    "../../../libs/distrho/DistrhoUIMain.cpp"
  )
}
