################################################################################
# GNU make should only be needed in maintainer mode now
################################################################################
# Please run 'configure' first (generate it with autogen.sh)
################################################################################

srcdir     = .
top_srcdir = .

PACKAGE = icewm
VERSION = 1.2.26

PREFIX = /usr
BINDIR = /usr/X11R6/bin
LIBDIR = /usr/share/icewm
CFGDIR = /etc/icewm
LOCDIR = /usr/share/locale
KDEDIR = /usr/share
DOCDIR = /usr/share/doc
MANDIR = /usr/man
GWMDIR = 

INSTALL    = /usr/bin/install -c
INSTALLDIR = /usr/bin/install -c -m 755 -d
INSTALLBIN = ${INSTALL}
INSTALLLIB = ${INSTALL} -m 644
INSTALLETC = ${INSTALL} -m 644
INSTALLMAN = ${INSTALL} -m 644
MKFONTDIR  = /usr/bin/mkfontdir

DESTDIR     =

################################################################################

BINFILES    = $(top_srcdir)/src/icewm $(top_srcdir)/src/icewm-session $(top_srcdir)/src/icesh $(top_srcdir)/src/icewmhint $(top_srcdir)/src/icewmbg $(top_srcdir)/src/icewmtray $(top_srcdir)/src/icehelp 
LIBFILES    = lib/preferences lib/winoptions lib/keys \
              lib/menu lib/toolbar # lib/programs
DOCFILES    = README BUGS CHANGES COPYING AUTHORS INSTALL VERSION icewm.lsm
MANFILES    = icewm.1
XPMDIRS     = icons ledclock taskbar mailbox cursors
THEMES      = nice motif win95 warp3 warp4 metal2 gtk2 Infadel2 nice2 \
              icedesert yellowmotif

GNOMEFILES  = lib/IceWM.desktop

all:		base nls
install:	install-base install-nls 

base icesound icehelp:
	@cd src; $(MAKE) $@

docs:
	@cd doc; $(MAKE) all

nls:
	@cd po; $(MAKE) all

gnome:

srcclean:
	@cd src; $(MAKE) clean

clean:  srcclean
	@cd doc; $(MAKE) clean

distclean: clean
	rm -f *~ config.cache config.log config.status install.inc \
	sysdep.inc src/config.h \
	lib/preferences \
	lib/menu lib/programs lib/keys lib/winoptions lib/toolbar

maintainer-clean: distclean
	rm -f icewm.spec icewm.lsm Makefile configure src/config.h.in
	@cd doc; $(MAKE) maintainer-clean

check:
	@cd src ; $(MAKE) check >/dev/null

dist:	distclean docs configure

# Makefile TABS *SUCK*
install-base: base
	@echo ------------------------------------------
	@echo "Installing binaries in $(DESTDIR)$(BINDIR)"
	@$(INSTALLDIR) "$(DESTDIR)$(BINDIR)"
	@for bin in $(BINFILES); do \
             $(INSTALLBIN) "$${bin}" "$(DESTDIR)$(BINDIR)"; \
         done
	
	@echo "Installing presets and icons in $(DESTDIR)$(LIBDIR)"
	@$(INSTALLDIR) "$(DESTDIR)$(LIBDIR)"
	#-@$(INSTALLDIR) "$(DESTDIR)$(CFGDIR)"
	@for lib in $(LIBFILES); do \
             $(INSTALLLIB) "$${lib}" "$(DESTDIR)$(LIBDIR)"; \
         done

	@for xpmdir in $(XPMDIRS); do \
	     if test -d "lib/$${xpmdir}"; then \
		$(INSTALLDIR) "$(DESTDIR)$(LIBDIR)/$${xpmdir}"; \
		for pixmap in "lib/$${xpmdir}/"*.xpm; do \
		    $(INSTALLLIB) "$${pixmap}" "$(DESTDIR)$(LIBDIR)/$${xpmdir}"; \
		done; \
	    fi; \
        done
	
	@echo ------------------------------------------
	@for theme in $(THEMES); do \
	     SRCDIR="$(top_srcdir)" \
	     DESTDIR="$(DESTDIR)" \
	     LIBDIR="$(LIBDIR)" \
	     XPMDIRS="$(XPMDIRS)" \
	     INSTALLDIR="$(INSTALLDIR)" \
	     INSTALLLIB="$(INSTALLLIB)" \
	     MKFONTDIR="$(MKFONTDIR)" \
	     $(top_srcdir)/utils/install-theme.sh "$${theme}"; \
	done
	@#for a in $(ETCFILES) ; do $(INSTALLETC) "$$a" $(CFGDIR) ; done
	@echo ------------------------------------------

install-docs: docs
	@echo ------------------------------------------
	@rm -fr "$(DESTDIR)$(DOCDIR)/icewm-$(VERSION)"
	@$(INSTALLDIR) "$(DESTDIR)$(DOCDIR)/icewm-$(VERSION)"
	@echo "Installing documentation in $(DESTDIR)$(DOCDIR)"
	@$(INSTALLLIB) $(DOCFILES) "$(DESTDIR)$(DOCDIR)/icewm-$(VERSION)"
	@$(INSTALLLIB) "$(top_srcdir)/doc/"*.sgml "$(DESTDIR)$(DOCDIR)/icewm-$(VERSION)"
	@$(INSTALLLIB) "$(top_srcdir)/doc/"*.html "$(DESTDIR)$(DOCDIR)/icewm-$(VERSION)"
	@echo ------------------------------------------

install-nls: nls
	@echo ------------------------------------------
	@cd po; $(MAKE) install
	@echo ------------------------------------------

install-man:
	@for man in $(MANFILES); do \
		$(INSTALLMAN) doc/$$man.man $(DESTDIR)$(MANDIR)/man1/$$man; \
	done

install-gnome: gnome
	@echo ------------------------------------------
	@echo Installing icewm-gnome in $(DESTDIR)$(GWMDIR)
	@$(INSTALLDIR) "$(DESTDIR)$(GWMDIR)"
	@$(INSTALLLIB) "$(top_srcdir)/lib/IceWM.desktop" "$(DESTDIR)$(GWMDIR)"
	@echo ------------------------------------------
