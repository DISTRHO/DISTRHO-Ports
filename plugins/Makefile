# -----------------------------------------
# Compile all the Plugins

all: libs dssi lv2 vst

plugins: dssi lv2 vst

# -----------------------------------------
# libs

libs:
	$(MAKE) -C ../libs/dgl

# -----------------------------------------
# dssi

dssi: libs
	mkdir -p ../bin/dssi
	$(MAKE) -C SegmentJuice/DSSI
	$(MAKE) -C SegmentJuice/DSSI-UI

# -----------------------------------------
# lv2

lv2: libs
	mkdir -p ../bin/lv2
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
	$(MAKE) -C PowerJuice/VST
	$(MAKE) -C SegmentJuice/VST
	$(MAKE) -C VectorJuice/VST
	$(MAKE) -C WobbleJuice/VST
	$(MAKE) -C GrooveJuice/VST

# -----------------------------------------
# clean

clean:
	$(MAKE) clean -C SegmentJuice/DSSI
	$(MAKE) clean -C SegmentJuice/DSSI-UI

	# LV2
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
	$(MAKE) clean -C PowerJuice/VST
	$(MAKE) clean -C SegmentJuice/VST
	$(MAKE) clean -C VectorJuice/VST
	$(MAKE) clean -C WobbleJuice/VST
	$(MAKE) clean -C GrooveJuice/VST

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
