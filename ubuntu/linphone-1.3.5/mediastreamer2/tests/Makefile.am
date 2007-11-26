noinst_PROGRAMS=echo ring videodisplay

echo_SOURCES=echo.c
ring_SOURCES=ring.c
videodisplay_SOURCES=videodisplay.c

libexec_PROGRAMS=mediastream

mediastream_SOURCES=mediastream.c

LDADD=../libquickstream.la



INCLUDES=-I$(top_srcdir)/oRTP/include -I../

ORTP_CFLAGS=`cat $(top_builddir)/oRTP/ortp.defs`

AM_CFLAGS=$(ORTP_CFLAGS) $(STRICT_OPTIONS)
AM_LDFLAGS=-rdynamic
