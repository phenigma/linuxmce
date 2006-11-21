include $(top_srcdir)/misc/Makefile.common

SUBDIRS = man hackersguide faq

docs_DATA = README README.dvb README.dxr3 \
	    README.opengl README_xxmc.html 

EXTRA_DIST = $(docs_DATA)

docsdir = $(prefix)/share/doc/xine

docs clean-docs:
	@cd faq && $(MAKE) $@
	@cd hackersguide && $(MAKE) $@
