

AM_CFLAGS=  $(PTHREAD_CFLAGS)  -I$(top_srcdir) $(TRUESPEECH_CFLAGS)
AM_LDFLAGS= $(PTHREAD_LDFLAGS)

INCLUDES=-I$(top_srcdir)/include/

lib_LTLIBRARIES = libortp.la

libortp_la_SOURCES=	str_utils.c 	\
			port.c \
			rtpmod.c rtpmod.h	\
			rtpparse.c  \
			rtpsession.c \
			jitterctl.c jitterctl.h \
			rtpsignaltable.c  \
			rtptimer.c	rtptimer.h \
			posixtimer.c \
			ortp.c \
			scheduler.c scheduler.h \
			avprofile.c  \
			sessionset.c  \
			telephonyevents.c  \
			payloadtype.c \
			rtcp.c  \
			utils.c utils.h \
			rtcpparse.c \
			rtpsession_ext.c 


libortp_la_LIBADD= $(PTHREAD_LIBS) -lm

libortp_la_LDFLAGS= -version-info $(LIBORTP_SO_VERSION)


SUBDIRS= . tests
