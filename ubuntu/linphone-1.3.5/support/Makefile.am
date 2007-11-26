
if BUILD_UGLIB
noinst_LTLIBRARIES= libuglib.la
endif

linphone_includedir=$(includedir)/linphone

linphone_include_HEADERS=uglib.h glist.h gspawn.h gshell.h


libuglib_la_SOURCES=			glist.c glist.h \
					uglib.c uglib.h \
					gspawn.c gspawn.h \
					gshell.c gshell.h

INCLUDES=-I$(top_srcdir)/oRTP/include