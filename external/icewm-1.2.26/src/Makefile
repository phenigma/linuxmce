PACKAGE = icewm
VERSION = 1.2.26
HOSTOS  = Linux 2.6.12-10-686-smp
HOSTCPU = i686

LIBDIR = /usr/share/icewm
CFGDIR = /etc/icewm
LOCDIR = /usr/share/locale
DOCDIR = /usr/share/doc

################################################################################

CXX =           g++
LD =            g++
EXEEXT =        

DEBUG =         
GCCDEP =        
DEFS =          -DHAVE_CONFIG_H \
                -DLIBDIR='"$(LIBDIR)"' \
                -DCFGDIR='"$(CFGDIR)"' \
                -DLOCDIR='"$(LOCDIR)"' \
                -DKDEDIR='"$(KDEDIR)"' \
                -DPACKAGE='"icewm"' \
                -DVERSION='"$(VERSION)"' \
                -DHOSTOS='"$(HOSTOS)"' \
                -DHOSTCPU='"$(HOSTCPU)"' \
                -DEXEEXT='"$(EXEEXT)"' \
                -DICEWMEXE='"icewm$(EXEEXT)"' \
                -DICEWMTRAYEXE='"icewmtray$(EXEEXT)"' \
                -DICEWMBGEXE='"icewmbg$(EXEEXT)"' \
                -DICESMEXE='"icewm-session$(EXEEXT)"' \
                -DICEHELPEXE='"icehelp$(EXEEXT)"' \
                -DICEHELPIDX='"$(DOCDIR)/icewm-$(VERSION)/icewm.html"'

CXXFLAGS =      -fpermissive -Wall -Wpointer-arith -Wconversion -Wwrite-strings -Winline -Woverloaded-virtual -W -fno-exceptions -fno-rtti -g -O2 $(DEBUG) $(DEFS) \
	         -I/usr/include -I/usr/include/freetype2   # `fc-config --cflags`
LFLAGS =	
LIBS =          

CORE_LIBS =     -lXinerama  -lSM -lICE  -lX11  -L/usr/X11R6/lib  -lXrandr -lXrender -lXext -lXft -lX11 -lfreetype -lz -lfontconfig -lXrender -lX11 -ldl # `fc-config --libs`
IMAGE_LIBS =    -L/usr/lib -lImlib -ljpeg -ltiff -lungif -lpng10 -lz -lm -lXext -L/usr/X11R6/lib -lSM -lICE -lXext -lX11
AUDIO_LIBS =    
GNOME1_LIBS =    
GNOME2_LIBS =    

################################################################################

libice_OBJS = ref.o \
        yapp.o yxapp.o ytimer.o ywindow.o ypaint.o ypopup.o \
        misc.o ycursor.o ysocket.o \
        ypaths.o ypixbuf.o ylocale.o yarray.o ypipereader.o \
        yxembed.o yconfig.o yprefs.o \
        yfont.o yfontcore.o yfontxft.o \
	yimage.o \
        ytooltip.o  # FIXME
        
libitk_OBJS = \
	ymenu.o ylabel.o yscrollview.o \
        ymenuitem.o yscrollbar.o ybutton.o ylistbox.o yinput.o \
        wmconfig.o yicon.o # FIXME

genpref_OBJS = \
	genpref.o

icewm_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icewm_OBJS = \
        ymsgbox.o ydialog.o yurl.o \
        wmsession.o wmwinlist.o wmtaskbar.o wmwinmenu.o \
        wmdialog.o wmabout.o wmswitch.o wmstatus.o \
        wmoption.o wmaction.o \
        wmcontainer.o wmclient.o \
        wmmgr.o wmapp.o \
        wmframe.o wmbutton.o wmminiicon.o wmtitle.o movesize.o \
        themes.o decorate.o browse.o \
        wmprog.o \
        atasks.o aworkspaces.o amailbox.o aclock.o acpustatus.o \
	apppstatus.o aaddressbar.o objbar.o aapm.o atray.o ysmapp.o \
        yxtray.o \
        $(libitk_OBJS) $(libice_OBJS)

icesh_LIBS = \
	$(CORE_LIBS)
icesh_OBJS = \
	icesh.o misc.o

icewm-session_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icewm-session_OBJS = \
	icesm.o $(libice_OBJS)

icewmhint_LIBS = \
	$(CORE_LIBS)
icewmhint_OBJS = \
	icewmhint.o

icewmbg_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icewmbg_OBJS = \
	icewmbg.o $(libice_OBJS)

icesound_LIBS = \
	$(CORE_LIBS) $(AUDIO_LIBS)
icesound_OBJS = \
	icesound.o misc.o ycmdline.o

icewm-menu-gnome1_LIBS = \
	$(CORE_LIBS) $(GNOME1_LIBS)
icewm-menu-gnome1_OBJS = \
	gnome.o misc.o ycmdline.o yarray.o

icewm-menu-gnome2_LIBS = \
	$(CORE_LIBS) $(GNOME2_LIBS)
icewm-menu-gnome2_OBJS = \
	gnome2.o misc.o ycmdline.o yarray.o

icehelp_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icehelp_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icehelp.o

iceclock_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceclock.o aclock.o
iceclock_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icebar_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) \
        wmtaskbar.o \
        wmprog.o browse.o themes.o wmaction.o \
	amailbox.o aclock.o acpustatus.o apppstatus.o aaddressbar.o objbar.o
icewmtray_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) yxtray.o icetray.o
icewmtray_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icesame_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icesame.o
icesame_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icelist_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icelist.o
icelist_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
iceview_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceview.o
iceview_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
iceicon_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceicon.o
iceicon_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
icerun_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) icerun.o
icerun_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
iceskt_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) iceskt.o
testmenus_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS) 
testmenus_OBJS = \
	$(libitk_OBJS) $(libice_OBJS) testmenus.o wmprog.o wmaction.o themes.o browse.o
testwinhints_OBJS= \
        testwinhints.o
testwinhints_LIBS = \
        $(CORE_LIBS) $(IMAGE_LIBS)

testnetwmhints_OBJS= \
        testnetwmhints.o
testnetwmhints_LIBS = \
        $(CORE_LIBS) $(IMAGE_LIBS)

testmap_OBJS = \
        testmap.o
testmap_LIBS = \
        $(CORE_LIBS) $(IMAGE_LIBS)

testlocale_OBJS = \
	testlocale.o ylocale.o misc.o
testdesktop_OBJS = \
	testdesktop.o ydesktop.o yparser.o ylocale.o misc.o
testdesktop_LIBS = \
	$(CORE_LIBS) $(IMAGE_LIBS)
testarray_OBJS = \
	testarray.o yarray.o misc.o

################################################################################

APPLICATIONS = icewm icewm-session icesh icewmhint icewmbg icewmtray icehelp
TESTCASES = testarray testdesktop testlocale testmap testmenus testnetwmhints testwinhints iceview icesame iceicon icerun icelist
OBJECTS =  $(icewm_OBJS) $(icewm-session_OBJS) $(icesh_OBJS) $(icewmhint_OBJS) $(icewmbg_OBJS) $(icewmtray_OBJS) $(icehelp_OBJS)  $(testarray_OBJS) $(testdesktop_OBJS) $(testlocale_OBJS) $(testmap_OBJS) $(testmenus_OBJS) $(testnetwmhints_OBJS) $(testwinhints_OBJS) $(iceview_OBJS) $(icesame_OBJS) $(iceicon_OBJS) $(icerun_OBJS) $(icelist_OBJS)
BINARIES =  icewm icewm-session icesh icewmhint icewmbg icewmtray icehelp  testarray testdesktop testlocale testmap testmenus testnetwmhints testwinhints iceview icesame iceicon icerun icelist

################################################################################

all: base
base:  icewm icewm-session icesh icewmhint icewmbg icewmtray icehelp genpref ../lib/preferences
tests:  testarray testdesktop testlocale testmap testmenus testnetwmhints testwinhints iceview icesame iceicon icerun icelist
clean:
	rm -f genpref$(EXEEXT) *.o *.d *~
	for bin in $(BINARIES) ; do rm -f $$bin$(EXEEXT) ; done

.PHONY: all base tests clean

################################################################################

%.o: %.cc
	@echo "  CXX   " $@
	@$(CXX) $(CXXFLAGS) $(GCCDEP) -c $<
	
genpref $(BINARIES):
	@echo "  LD    " $@
	@$(LD) -o $@$(EXEEXT) $($@_OBJS) $(LFLAGS) $($@_LFLAGS) $(LIBS) $($@_LIBS)

################################################################################

gnome.o: gnome.cc
	@echo "  CXX   " $@
	@$(CXX) $(CXXFLAGS)   $(GCCDEP) -c $<

gnome2.o: gnome2.cc
	@echo "  CXX   " $@
	@$(CXX) $(CXXFLAGS)   $(GCCDEP) -c $<

################################################################################

#libice.so: $(libice_OBJS)
#	-@rm -f $@
#	ld -shared -o $@ $(libice_OBJS)

wmabout.o: ../VERSION

../lib/preferences: genpref
	./genpref$(EXEEXT) >../lib/preferences

genpref: $(genpref_OBJS)

################################################################################

check: 	all tests
	./icewm --help >/dev/null	
	./testarray
	./testlocale

################################################################################
#END

icewm: $(icewm_OBJS)
icewm-session: $(icewm-session_OBJS)
icesh: $(icesh_OBJS)
icewmhint: $(icewmhint_OBJS)
icewmbg: $(icewmbg_OBJS)
icewmtray: $(icewmtray_OBJS)
icehelp: $(icehelp_OBJS)
testarray: $(testarray_OBJS)
testdesktop: $(testdesktop_OBJS)
testlocale: $(testlocale_OBJS)
testmap: $(testmap_OBJS)
testmenus: $(testmenus_OBJS)
testnetwmhints: $(testnetwmhints_OBJS)
testwinhints: $(testwinhints_OBJS)
iceview: $(iceview_OBJS)
icesame: $(icesame_OBJS)
iceicon: $(iceicon_OBJS)
icerun: $(icerun_OBJS)
icelist: $(icelist_OBJS)
