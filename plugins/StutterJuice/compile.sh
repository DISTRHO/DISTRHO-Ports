cd ./LV2/
make
cd ..
cd ./LV2-UI/
make
cd ..

#sudo cp ../../bin/vst/PowerJuice.so /usr/lib/vst/
#sudo cp -r ../../bin/lv2/GrooveJuice.lv2 /usr/lib/lv2
export LV2_PATH="/home/andre/Documents/DISTRHO/bin/lv2"
#exec lv2ls
exec jalv.gtk urn:distrho:StutterJuice&
#sudo cp -r ../../bin/lv2/GrooveJuice.lv2/ /usr/lib/lv2/
sleep 1
jack_connect "PulseAudio JACK Sink:front-left" "StutterJuice:lv2_audio_in_1"
jack_connect "PulseAudio JACK Sink:front-right" "StutterJuice:lv2_audio_in_2"
jack_connect "StutterJuice:lv2_audio_out_1" "system:playback_1"
jack_connect "StutterJuice:lv2_audio_out_2" "system:playback_2"

