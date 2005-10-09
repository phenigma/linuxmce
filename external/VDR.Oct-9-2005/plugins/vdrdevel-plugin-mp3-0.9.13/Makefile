#
# MP3/MPlayer plugin to VDR
#
# (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
#
# This code is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# Or, point your browser to http://www.gnu.org/copyleft/gpl.html

# You can change the compile options here or create a Make.config
# in the VDR directory an set them there.

### uncomment one of these lines, if you don't want one of the plugins
#WITHOUT_MP3=1
#WITHOUT_MPLAYER=1

### uncomment the following line, if you don't have libsndfile installed
#WITHOUT_LIBSNDFILE=1

### uncomment the following line, if you don't have libvorbisfile installed
#WITHOUT_LIBVORBISFILE=1

### uncomment the following line, if you want OSS sound output
#WITH_OSS_OUTPUT=1

### uncomment the following line, if you want to include debug symbols
#DBG=1

### The C++ compiler and options:
CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Woverloaded-virtual

###############################################
###############################################
#
# no user configurable options below this point
#
###############################################
###############################################

### The directory environment:

DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN  = mp3
PLUGIN2 = mplayer

ifndef WITHOUT_MP3
  ALL += libvdr-$(PLUGIN).so
endif
ifndef WITHOUT_MPLAYER
  ALL += libvdr-$(PLUGIN2).so
endif

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'define PLUGIN_VERSION' version.h | awk '{ print $$3 }' | sed -e 's/[";]//g')

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')
VDRVERSNUM = $(shell grep 'define VDRVERSNUM ' $(VDRDIR)/config.h | awk '{ print $$3 }')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include
DEFINES  += -D_GNU_SOURCE -DVDRVERSNUM=$(VDRVERSNUM)

### The object files (add further files here):

COM_OBJS = i18n.o data.o menu.o

OBJS     = $(PLUGIN).o $(COM_OBJS)\
            data-mp3.o setup-mp3.o player-mp3.o stream.o network.o\
            decoder.o decoder-mp3.o decoder-mp3-stream.o decoder-snd.o \
            decoder-ogg.o
LIBS     = -lz -lmad -lid3tag

ifndef WITHOUT_LIBSNDFILE
  LIBS    += -lsndfile
  DEFINES += -DHAVE_SNDFILE
endif
ifndef WITHOUT_LIBVORBISFILE
  LIBS    += -lvorbisfile -lvorbis
  DEFINES += -DHAVE_VORBISFILE
endif
ifdef WITH_OSS_OUTPUT
  DEFINES += -DWITH_OSS
endif
ifdef BROKEN_PCM
  DEFINES += -DBROKEN_PCM
endif

OBJS2    = $(PLUGIN2).o $(COM_OBJS)\
            setup-mplayer.o player-mplayer.o
LIBS2    = 

ifeq ($(shell test -f $(VDRDIR)/fontsym.h ; echo $$?),0)
  DEFINES += -DHAVE_BEAUTYPATCH
endif  

ifdef DBG
CXXFLAGS += -g
endif

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) $(OBJS2:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: $(ALL)

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)

libvdr-$(PLUGIN2).so: $(OBJS2)
	$(CXX) $(CXXFLAGS) -shared $(OBJS2) $(LIBS2) -o $@
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)

i18ntest: i18ntest.c i18n.c i18n.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $< -o $@

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tar.gz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tar.gz

clean:
	@-rm -f $(OBJS) $(OBJS2) $(DEPFILE) i18ntest libvdr-*.so $(PACKAGE).tar.gz core* *~
