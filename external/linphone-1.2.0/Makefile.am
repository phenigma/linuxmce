## Process this file with automake to produce Makefile.in

if BUILD_TRUESPEECH
  WIN32ACM_DIR = win32acm
endif


SUBDIRS = m4 support pixmaps po ipkg oRTP gsmlib lpc10-1.5 \
                $(WIN32ACM_DIR) mediastreamer mediastreamer2\
		media_api exosip coreapi console gnome share developer-docs


EXTRA_DIST =  linphone2.glade BUGS linphone.kdevprj \
				intltool-extract.in	\
				intltool-merge.in	\
				intltool-update.in \
				README.arm \
				autogen.sh
ACLOCAL_FLAGS=-I$(top_srcdir)/m4
