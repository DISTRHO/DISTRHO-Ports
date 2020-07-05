# -----------------------------------------
# Compile all the Ports

all: libs lv2 vst

plugins: lv2 vst

# -----------------------------------------
# libs

libs:
	$(MAKE) -C ../libs/juce
	$(MAKE) -C ../libs/lv2-ttl-generator

# -----------------------------------------
# lv2

lv2: libs
	install -d ../bin/lv2

# -----------------------------------------
# vst

vst: libs
	install -d ../bin/vst

# -----------------------------------------
# clean

clean:
	# LV2

	# VST

	rm -rf */LV2/intermediate
	rm -rf */VST/intermediate

distclean: clean
	rm -rf */LV2/*.lv2
	rm -f */*/Makefile
	rm -f */*/*.make
