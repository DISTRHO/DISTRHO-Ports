# -----------------------------------------
# Compile all the Plugins

all: libs ladspa dssi lv2 vst

plugins: ladspa dssi lv2 vst

# -----------------------------------------
# libs

libs:
	$(MAKE) -C ../libs/dgl

# -----------------------------------------
# ladspa

ladspa:
	mkdir -p ../bin/ladspa
	$(MAKE) -C 3BandEQ/LADSPA
	$(MAKE) -C 3BandSplitter/LADSPA
	$(MAKE) -C PingPongPan/LADSPA

# -----------------------------------------
# dssi

dssi: libs
	mkdir -p ../bin/dssi
	$(MAKE) -C 3BandEQ/DSSI
	$(MAKE) -C 3BandEQ/DSSI-UI
	$(MAKE) -C 3BandSplitter/DSSI
	$(MAKE) -C 3BandSplitter/DSSI-UI
	$(MAKE) -C Nekobi/DSSI
	$(MAKE) -C Nekobi/DSSI-UI
	$(MAKE) -C PingPongPan/DSSI
	$(MAKE) -C PingPongPan/DSSI-UI
	$(MAKE) -C SegmentJuice/DSSI
	$(MAKE) -C SegmentJuice/DSSI-UI

# -----------------------------------------
# lv2

lv2: libs
	mkdir -p ../bin/lv2
	$(MAKE) -C 3BandEQ/LV2
	$(MAKE) -C 3BandEQ/LV2-UI
	$(MAKE) -C 3BandSplitter/LV2
	$(MAKE) -C 3BandSplitter/LV2-UI
	$(MAKE) -C Nekobi/LV2
	$(MAKE) -C Nekobi/LV2-UI
	$(MAKE) -C PingPongPan/LV2
	$(MAKE) -C PingPongPan/LV2-UI
	$(MAKE) -C PowerJuice/LV2
	$(MAKE) -C SegmentJuice/LV2
	$(MAKE) -C SegmentJuice/LV2-UI
	$(MAKE) -C VectorJuice/LV2
	$(MAKE) -C VectorJuice/LV2-UI
	$(MAKE) -C WobbleJuice/LV2
	$(MAKE) -C WobbleJuice/LV2-UI
	$(MAKE) -C GrooveJuice/LV2
	$(MAKE) -C GrooveJuice/LV2-UI

# -----------------------------------------
# vst

vst: libs
	mkdir -p ../bin/vst
	$(MAKE) -C 3BandEQ/VST
	$(MAKE) -C 3BandSplitter/VST
	$(MAKE) -C Nekobi/VST
	$(MAKE) -C PingPongPan/VST
	$(MAKE) -C PowerJuice/VST
	$(MAKE) -C SegmentJuice/VST
	$(MAKE) -C VectorJuice/VST
	$(MAKE) -C WobbleJuice/VST
	$(MAKE) -C GrooveJuice/VST

# -----------------------------------------
# clean

clean:
	# LADSPA
	$(MAKE) clean -C 3BandEQ/LADSPA
	$(MAKE) clean -C 3BandSplitter/LADSPA
	$(MAKE) clean -C PingPongPan/LADSPA

	# DSSI
	$(MAKE) clean -C 3BandEQ/DSSI
	$(MAKE) clean -C 3BandEQ/DSSI-UI
	$(MAKE) clean -C 3BandSplitter/DSSI
	$(MAKE) clean -C 3BandSplitter/DSSI-UI
	$(MAKE) clean -C Nekobi/DSSI
	$(MAKE) clean -C Nekobi/DSSI-UI
	$(MAKE) clean -C PingPongPan/DSSI
	$(MAKE) clean -C PingPongPan/DSSI-UI
	$(MAKE) clean -C SegmentJuice/DSSI
	$(MAKE) clean -C SegmentJuice/DSSI-UI

	# LV2
	$(MAKE) clean -C 3BandEQ/LV2
	$(MAKE) clean -C 3BandEQ/LV2-UI
	$(MAKE) clean -C 3BandSplitter/LV2
	$(MAKE) clean -C 3BandSplitter/LV2-UI
	$(MAKE) clean -C Nekobi/LV2
	$(MAKE) clean -C Nekobi/LV2-UI
	$(MAKE) clean -C PingPongPan/LV2
	$(MAKE) clean -C PingPongPan/LV2-UI
	$(MAKE) clean -C PowerJuice/LV2
	$(MAKE) clean -C SegmentJuice/LV2
	$(MAKE) clean -C SegmentJuice/LV2-UI
	$(MAKE) clean -C VectorJuice/LV2
	$(MAKE) clean -C VectorJuice/LV2-UI
	$(MAKE) clean -C WobbleJuice/LV2
	$(MAKE) clean -C WobbleJuice/LV2-UI
	$(MAKE) clean -C GrooveJuice/LV2
	$(MAKE) clean -C GrooveJuice/LV2-UI

	# VST
	$(MAKE) clean -C 3BandEQ/VST
	$(MAKE) clean -C 3BandSplitter/VST
	$(MAKE) clean -C Nekobi/VST
	$(MAKE) clean -C PingPongPan/VST
	$(MAKE) clean -C PowerJuice/VST
	$(MAKE) clean -C SegmentJuice/VST
	$(MAKE) clean -C VectorJuice/VST
	$(MAKE) clean -C WobbleJuice/VST
	$(MAKE) clean -C GrooveJuice/VST

	rm -rf */LADSPA/intermediate
	rm -rf */DSSI/intermediate
	rm -rf */DSSI-UI/intermediate
	rm -rf */LV2/intermediate
	rm -rf */LV2-UI/intermediate
	rm -rf */VST/intermediate

distclean: clean
	rm -rf */LV2/*.lv2
	rm -rf */LV2-UI/*.lv2
	rm -f */*/Makefile
	rm -f */*/*.make
