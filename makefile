CC=g++
PKG_CONFIG=pkg-config
SHARE_FOLDER=share/noise-free-cnv/

CFLAGS=-c -Wall -O3 \
	$(shell $(PKG_CONFIG) --cflags gtkmm-2.4 fftw3) \
	-DNFCNV_VERSION_MAJOR=1 \
	-DNFCNV_VERSION_MINOR=13 \
	-DNFCNV_SHARE_FOLDER='"$(SHARE_FOLDER)"'
LDFLAGS1=-s -O3 \
	$(shell $(PKG_CONFIG) --libs gtkmm-2.4 fftw3)
LDFLAGS2=-s -O3 \
	$(shell $(PKG_CONFIG) --libs glibmm-2.4 cairomm-1.0 fftw3)
SOURCES1=$(shell ls src/[CGP]*.cc)
SOURCES2=$(shell ls src/[CP]*.cc)
OBJECTS1=$(SOURCES1:.cc=.o)
OBJECTS2=$(SOURCES2:.cc=.o)

build: $(SOURCES1) bin/noise-free-cnv-gtk bin/noise-free-cnv-filter

bin/noise-free-cnv-gtk: $(OBJECTS1) src/noise-free-cnv-gtk.o
	$(CC) -o $@ $(OBJECTS1) src/noise-free-cnv-gtk.o $(LDFLAGS1)

bin/noise-free-cnv-filter: $(OBJECTS2) src/noise-free-cnv-filter.o
	$(CC) -o $@ $(OBJECTS2) src/noise-free-cnv-filter.o $(LDFLAGS2)

src/%.o: src/%.cc
	$(CC) -o $@ $< $(CFLAGS)

install: bin/noise-free-cnv-gtk bin/noise-free-cnv-filter
	install -d $(DESTDIR)/share
	install -d $(DESTDIR)/share/noise-free-cnv
	install -m 0644 share/noise-free-cnv/icon3.png $(DESTDIR)/share/noise-free-cnv/icon3.png
	install -m 0644 share/noise-free-cnv/icon1.png $(DESTDIR)/share/noise-free-cnv/icon1.png
	install -m 0644 share/noise-free-cnv/icon6.png $(DESTDIR)/share/noise-free-cnv/icon6.png
	install -m 0644 share/noise-free-cnv/icon4.png $(DESTDIR)/share/noise-free-cnv/icon4.png
	install -m 0644 share/noise-free-cnv/icon5.png $(DESTDIR)/share/noise-free-cnv/icon5.png
	install -m 0644 share/noise-free-cnv/icon2.png $(DESTDIR)/share/noise-free-cnv/icon2.png
	install -d $(DESTDIR)/share/applications
	install -m 0644 share/applications/noise-free-cnv.desktop $(DESTDIR)/share/applications/noise-free-cnv.desktop
	install -d $(DESTDIR)/share/doc
	install -d $(DESTDIR)/share/doc/noise-free-cnv
	install -m 0644 share/doc/noise-free-cnv/changelog.gz $(DESTDIR)/share/doc/noise-free-cnv/changelog.gz
	install -m 0644 share/doc/noise-free-cnv/readme $(DESTDIR)/share/doc/noise-free-cnv/readme
	install -m 0644 share/doc/noise-free-cnv/copyright $(DESTDIR)/share/doc/noise-free-cnv/copyright
	install -d $(DESTDIR)/bin

	install -m 0755 bin/* $(DESTDIR)/bin/

clean:
	rm -f src/*.o
	rm -f bin/*

.PHONY: build install clean
