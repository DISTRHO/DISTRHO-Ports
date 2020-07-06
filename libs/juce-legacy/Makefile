# Compile the juce shared lib

all:
	$(MAKE) -C build-juce-legacy

clean:
	$(MAKE) clean -C build-juce-legacy
	rm -rf build-*/intermediate

distclean: clean
	rm -f build-*/Makefile
	rm -f build-*/*.make
	rm -f build-*/*.sln
	rm -f build-*/*.vcproj
