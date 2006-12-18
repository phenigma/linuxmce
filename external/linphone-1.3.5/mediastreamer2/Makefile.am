SUBDIRS=. tests

BUILT_SOURCES=alldescs.h

CLEANFILES=alldescs.h

noinst_LTLIBRARIES=libmediastreamer2.la libquickstream.la


libmediastreamer2_la_SOURCES=	mscommon.c	mscommon.h allfilters.h\
				msfilter.c	msfilter.h msvideo.h \
				msqueue.c	msqueue.h \
				msticker.c	msticker.h \
				alaw.c 		g711common.h \
				ulaw.c \
				mssndcard.c	mssndcard.h \
				msfileplayer.c 	msfileplayer.h waveheader.h \
				msrtp.c		msrtp.h \
				dtmfgen.c	dtmfgen.h \
				msfilerec.c	msfilerec.h\
				msspeex.c \
				gsm.c 


libquickstream_la_SOURCES=audiostream.c mediastream.h

if BUILD_ALSA
libmediastreamer2_la_SOURCES+=alsa.c
endif

if BUILD_OSS
libmediastreamer2_la_SOURCES+=oss.c
endif

if BUILD_ARTS
libmediastreamer2_la_SOURCES+=arts.c
endif

if BUILD_VIDEO
libmediastreamer2_la_SOURCES+=	msv4l.c msv4l.h \
														sdlout.c \
														videoenc.c \
														videodec.c rfc2429.h

libquickstream_la_SOURCES+=videostream.c

endif

alldescs.h:	Makefile.am $(libmediastreamer2_la_SOURCES)
	awk 'BEGIN { FS="[()]" ; }; /^MS_FILTER_DESC_EXPORT/{ printf("%s\n", $$2) } '  > filterdescs.txt $(libmediastreamer2_la_SOURCES)
	awk 'BEGIN { print("#include \"msfilter.h\"\n") } { printf("extern MSFilterDesc %s;\n",$$1) } ' filterdescs.txt > $@
	awk 'BEGIN { print("MSFilterDesc * ms_filter_descs[]={") } { printf("&%s,\n",$$1) } END{ print("NULL\n};\n") } ' filterdescs.txt >> $@
	rm -f filterdescs.txt

libmediastreamer2_la_LIBADD=	$(top_builddir)/oRTP/src/libortp.la \
				$(ALSA_LIBS) \
				$(ARTS_LIBS) \
				$(SPEEX_LIBS) \
				$(top_builddir)/gsmlib/libgsm.la \
				-lrt

if BUILD_VIDEO
libmediastreamer2_la_LIBADD+=$(VIDEO_LIBS)
endif

libquickstream_la_LIBADD=libmediastreamer2.la

INCLUDES=-I$(top_srcdir)/oRTP/include -I$(top_srcdir)/gsmlib

ORTP_CFLAGS=`cat $(top_builddir)/oRTP/ortp.defs`

AM_CFLAGS=$(ORTP_CFLAGS) $(SPEEX_CFLAGS) $(STRICT_OPTIONS)
