cd ./LV2/
make
cd ..
#sudo cp ../../bin/vst/PowerJuice.so /usr/lib/vst/
sudo cp -r ../../bin/lv2/PowerJuice.lv2 /usr/lib/lv2
#export LV2_PATH="/home/andre/Documents/DISTRHO/bin/lv2"
#exec jalv.gtk urn:distrho:PowerJuice&
#sleep 1
#jack_connect "PulseAudio JACK Sink:front-left" "PowerJuice:lv2_audio_in_1"
#jack_connect "PowerJuice:lv2_audio_out_1" "system:playback_1"
#jack_connect "PowerJuice:lv2_audio_out_1" "system:playback_2"

