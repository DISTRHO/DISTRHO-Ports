# -----------------------------------------
# Compile all the Plugins and Ports

PREFIX = /usr/local

# -----------------------------------------
# all

all:
	$(MAKE) -C libs/dgl
	$(MAKE) -C libs/drowaudio
	$(MAKE) -C libs/juce
	$(MAKE) -C libs/juced
	$(MAKE) -C libs/lv2-ttl-generator
	$(MAKE) -C plugins plugins
	$(MAKE) -C ports plugins
	$(MAKE) gen

# -----------------------------------------
# install

install:
	install -d $(DESTDIR)$(PREFIX)/lib/ladspa/
	install -d $(DESTDIR)$(PREFIX)/lib/dssi/
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/
	install -d $(DESTDIR)$(PREFIX)/lib/vst/
	cp -r bin/ladspa/* $(DESTDIR)$(PREFIX)/lib/ladspa/
	cp -r bin/dssi/*   $(DESTDIR)$(PREFIX)/lib/dssi/
	cp -r bin/vst/*    $(DESTDIR)$(PREFIX)/lib/vst/
	cp -r bin/lv2/*.lv2/ static-lv2-ttl/*.lv2/ $(DESTDIR)$(PREFIX)/lib/lv2/

# -----------------------------------------
# gen

gen: gen_lv2 gen_vst

gen_lv2:
	$(MAKE) -C libs/lv2-ttl-generator
	@./scripts/generate-cabbage-lv2.sh
	@./scripts/generate-ttl.sh

gen_vst:
	@./scripts/generate-cabbage-vst.sh

# -----------------------------------------
# clean

clean:
	$(MAKE) clean -C libs/dgl
	$(MAKE) clean -C libs/drowaudio
	$(MAKE) clean -C libs/juce
	$(MAKE) clean -C libs/juced
	$(MAKE) clean -C libs/lv2-ttl-generator
	$(MAKE) clean -C plugins
	$(MAKE) clean -C ports
	rm -rf bin/lv2/*.lv2/

distclean: clean
	$(MAKE) distclean -C libs/dgl
	$(MAKE) distclean -C libs/drowaudio
	$(MAKE) distclean -C libs/juce
	$(MAKE) distclean -C libs/juced
	$(MAKE) distclean -C plugins
	$(MAKE) distclean -C ports

# -----------------------------------------
# mingw

mingw:
	$(MAKE) -C libs/lv2-ttl-generator mingw

# -----------------------------------------
# Custom build types

ladspa:
	$(MAKE) -C plugins ladspa

dssi:
	$(MAKE) -C libs/dgl
	$(MAKE) -C plugins dssi

lv2:
	$(MAKE) -C libs/dgl
	$(MAKE) -C libs/drowaudio
	$(MAKE) -C libs/juce
	$(MAKE) -C libs/juced
	$(MAKE) -C libs/lv2-ttl-generator
	$(MAKE) -C plugins lv2
	$(MAKE) -C ports lv2
	$(MAKE) gen_lv2

vst:
	$(MAKE) -C libs/dgl
	$(MAKE) -C libs/drowaudio
	$(MAKE) -C libs/juce
	$(MAKE) -C libs/juced
	$(MAKE) -C plugins vst
	$(MAKE) -C ports vst
	$(MAKE) gen_vst
