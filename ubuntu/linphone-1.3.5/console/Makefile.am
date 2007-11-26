## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I$(top_srcdir)\
	-I$(top_srcdir)/coreapi\
	-I$(top_srcdir)/osipua/\
	-I$(top_srcdir)/osipua/src \
	-I$(top_srcdir)/oRTP/include \
	-I$(top_builddir)/oRTP/ \
	-I$(top_srcdir)/oRTP/ \
	-I$(top_srcdir)/mediastreamer \
	$(SPEEX_CFLAGS)\
	-I$(osip_prefix)/include \
	-I$(top_srcdir)/exosip



bin_PROGRAMS =  linphonec sipomatic

linphonec_SOURCES = linphonec.c linphonec.h commands.c

linphonec_LDADD = $(top_builddir)/coreapi/liblinphone.la $(READLINE_LIBS)

sipomatic_SOURCES=\
	sipomatic.c sipomatic.h

sipomatic_LDADD= 	$(INTLLIBS)  \
					$(top_builddir)/coreapi/liblinphone.la

AM_CFLAGS=$(STRICT_OPTIONS) $(GLIB_CFLAGS) -DENABLE_TRACE -D_ORTP_SOURCE $(VIDEO_CFLAGS) $(READLINE_CFLAGS)

#in order to workaround a arm-linux cross-compilation libtool bug
#we need to explicit again the dependencies of the executables:
if ARMBUILD
AM_LDFLAGS= 	$(top_builddir)/oRTP/src/libortp.la \
				$(SPEEX_LIBS) \
				$(OSIP_LIBS)
endif
