# Compile the dgl library

all: build


build: dgl


dgl:
	$(MAKE) -C build


clean:
	$(MAKE) clean -C build
	rm -rf build-*/intermediate

distclean: clean
	rm -f build-*/Makefile
	rm -f build-*/*.make
	rm -f build-*/*.sln
	rm -f build-*/*.vcproj
