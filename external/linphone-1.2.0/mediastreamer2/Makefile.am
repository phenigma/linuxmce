

if BUILD_UGLIB
SUPPORTLIB=$(top_builddir)/support/libuglib.la
endif


noinst_LTLIBRARIES = libmediastreamer2.la 


libmediastreamer2_la_SOURCES=msfilter.c msfilter.h  \
							msAlawdec.c msAlawdec.h \
							msAlawenc.c msAlawenc.h \
							g711common.h \
							mssndread.c mssndread.h \
							mssndwrite.c mssndwrite.h \
							msutils.h waveheader.h\
							ms.c ms.h \
							msqueue.c msqueue.h \
							msfifo.c msfifo.h \
							mslink.h \
							msbuffer.c msbuffer.h \
							mssync.c mssync.h \
							sndcard.c sndcard.h \
							osscard.c osscard.h\
							hpuxsndcard.c \
							alsacard.c alsacard.h \
							jackcard.c jackcard.h \
							mscodec.h \
							msringplayer.c msringplayer.h waveheader.h 

							

libmediastreamer2_la_LIBADD= $(GLIB_LIBS) \
							../gsmlib/libgsm.la \
							../lpc10-1.5/liblpc10.la \
							../oRTP/src/libortp.la \
							$(JACK_LIBS)\
							$(SUPPORTLIB) \
							$(ALSA_LIBS) \
							$(SPEEX_LIBS)
							



AM_CFLAGS=$(GLIB_CFLAGS) -DG_LOG_DOMAIN=\"MediaStreamer\" $(TRUESPEECH_CFLAGS) $(IPV6_CFLAGS)


INCLUDES=  -I$(top_srcdir) \
						-I$(top_srcdir)/mediastreamer2 \
						-I$(top_srcdir)/oRTP/include \
						-I$(top_srcdir)/oRTP \
						-I$(top_srcdir)/gsmlib   \
						-I$(top_srcdir)/lpc10-1.5    \
						$(SPEEX_CFLAGS)
