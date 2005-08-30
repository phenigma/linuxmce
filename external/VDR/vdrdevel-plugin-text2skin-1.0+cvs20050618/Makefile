# exchange the comments on the following to lines if you would like to use
# Imlib2 for loading images. BEWARE that you can not use GraphTFT together with
# Text2Skin if you use Imlib2! (That's why I actually implemented ImageMagick)

HAVE_IMAGEMAGICK=1
#HAVE_IMLIB2=1

# comment this out if you don't want to use FreeType font rendering

HAVE_FREETYPE=1


# DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU'RE DOING
# -------------------------------------------------------------
#
# $Id: Makefile,v 1.4 2005/01/27 17:32:07 lordjaxom Exp $
#

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = text2skin

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'const char \*cText2SkinPlugin::VERSION *=' $(PLUGIN).c | awk '{ print $$5 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The object files (add further files here):

OBJS = $(PLUGIN).o loader.o display.o render.o common.o bitmap.o \
       file.o i18n.o theme.o cache.o setup.o status.o scroller.o screen.o \
       menu.o font.o quantize.o \
       \
       xml/skin.o xml/parser.o xml/string.o xml/object.o xml/function.o \
       xml/type.o xml/display.o xml/xml.o

### Includes and Defines (add further entries here):

ifdef HAVE_IMLIB2
	DEFINES += -DHAVE_IMLIB2
	LIBS += -lImlib2
endif

ifdef HAVE_IMAGEMAGICK
	DEFINES += -DHAVE_IMAGEMAGICK
	LIBS += -lMagick -lMagick++
#	LIBS += $(shell Magick++-config --ldflags --libs)
endif

ifdef HAVE_FREETYPE
	ifneq ($(shell which freetype-config),)
		INCLUDES += $(shell freetype-config --cflags)
		LIBS += $(shell freetype-config --libs)
	else
		INCLUDES += -I/usr/include/freetype -I/usr/local/include/freetype
		LIBS += -lfreetype
	endif
	DEFINES += -DHAVE_FREETYPE
	OBJS += graphtft/font.o
endif

ifdef DEBUG
	CXXFLAGS += -g -fno-inline
	DEFINES += -DDEBUG
else
	CXXFLAGS += -O2 -g
ifdef BENCH
	DEFINES += -DBENCH
endif
endif

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/linux/include -I$(DVBDIR)/include -I.

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)
ifndef DEBUG
	strip $(LIBDIR)/$@.$(VDRVERSION)
endif

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@ln -s $(ARCHIVE) $(TMPDIR)/$(PLUGIN)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE) $(PLUGIN)
	@-rm -rf $(TMPDIR)/$(ARCHIVE) $(TMPDIR)/$(PLUGIN)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~ SKINS SKINS.de
