

## Process this file with automake to produce Makefile.in
linphone_includedir=$(includedir)/linphone

linphone_include_HEADERS=linphonecore.h ../config.h ../mediastreamer/sndcard.h lpconfig.h

INCLUDES = \
	-I$(top_srcdir)\
	-I$(top_srcdir)/exosip\
	-I$(top_srcdir)/oRTP/include \
	-I$(top_builddir)/oRTP/ \
	-I$(top_srcdir)/oRTP/ \
	-I$(top_srcdir)/mediastreamer \
	$(SPEEX_CFLAGS) \
	-I$(osip_prefix)/include/ 


lib_LTLIBRARIES=liblinphone.la

liblinphone_la_SOURCES=\
	linphonecore.c linphonecore.h private.h\
	exevents.c exevents.h \
	misc.c misc.h \
	arts.c \
	enum.c enum.h \
	sdphandler.c sdphandler.h \
	presence.c \
	proxy.c \
	friend.c \
	authentication.c \
	lpconfig.c lpconfig.h \
	chat.c


liblinphone_la_LDFLAGS= -version-info $(LIBLINPHONE_SO_VERSION)

liblinphone_la_LIBADD=	$(top_builddir)/mediastreamer/libmediastreamer.la \
						$(top_builddir)/exosip/libeXosip.la 


AM_CFLAGS=$(STRICT_OPTIONS) $(GLIB_CFLAGS) \
					-DENABLE_TRACE -DG_LOG_DOMAIN=\"LinphoneCore\" $(IPV6_CFLAGS) -DORTP_INET6 $(VIDEO_CFLAGS)
