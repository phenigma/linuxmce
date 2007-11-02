MODULE_NAME=zapauto
MODULES_DIR=/var/www/html/admin/modules/
BIN_DIR=/var/lib/asterisk/bin
SHARE_FILES=detect_zap fix_ast_db zap2amp zap.template
DESTDIR=

SUDOERS=/etc/sudoers
LINE=asterisk ALL=NOPASSWD:$(BIN_DIR)/detect_zap
LINE2=asterisk ALL=NOPASSWD:/etc/init.d/asterisk


############

.PHONY: install uninstall forceuninstall remove_share_files

all:
	@echo "freepbx-module-zapauto installer"
	@echo " "
	@echo "usage: make install - install the module"
	@echo "       make uninstall - remove the module"
	@echo " "
	@echo "The uninstall target will not remove the PHP files used"
	@echo "by the web interface. You should remove the module from"
	@echo "freepbx gui manually."
	@echo " "
	@echo " "
	@echo "For more information, please contact support@xorcom.com"
	@echo " "
	@echo " "

installfiles:
	mkdir -p $(DESTDIR)/$(BIN_DIR)
	mkdir -p $(DESTDIR)/$(MODULES_DIR)$(MODULE_NAME)
		
#	install support files
	for i in $(SHARE_FILES); do cp -a share/$$i $(DESTDIR)/$(BIN_DIR); done
	
#	install the main module
	for j in php sql sqlite html xml; do \
		for i in *.$$j; do if [ -f $$i ]; then cp -a $$i $(DESTDIR)/$(MODULES_DIR)$(MODULE_NAME); fi done; \
	done

patchfiles:
#	patch the installed files
	sed -i -e 's|/usr/share/freepbx-common|/var/lib/asterisk/bin|g' $(DESTDIR)/$(BIN_DIR)/detect_zap
	sed -i -e 's|/usr/share/asterisk/bin|/var/lib/asterisk/bin|g'   $(DESTDIR)/$(BIN_DIR)/detect_zap
	sed -i -e 's|/usr/share/freepbx-common|/var/lib/asterisk/bin|g' $(DESTDIR)/$(BIN_DIR)/zap2amp
	sed -i -e 's|/usr/share/asterisk/bin|/var/lib/asterisk/bin|g'   $(DESTDIR)/$(BIN_DIR)/zap2amp
	sed -i -e 's|/usr/share/freepbx|/var/www/html|g'                $(DESTDIR)/$(BIN_DIR)/fix_ast_db
	sed -i -e 's|/usr/share/freepbx-common|/var/lib/asterisk/bin|g' $(DESTDIR)/$(MODULES_DIR)/$(MODULE_NAME)/*.php

install: installfiles patchfiles
#	setup sudo
	yum install sudo
	if ! fgrep -q "$(LINE)"  $(SUDOERS); then echo "$(LINE)"  >> $(SUDOERS); fi
	if ! fgrep -q "$(LINE2)" $(SUDOERS); then echo "$(LINE2)" >> $(SUDOERS); fi
	


uninstall: remove_share_files
	echo "Please remove the module from freePBX gui"

forceuninstall: remove_share_files
	rm -fr $(DESTDIR)/$(MODULES_DIR)/$(MODULE_NAME)

REVISION=$(shell svnversion -c . | cut -d: -f2)
FILENAME=freepbx-module-zapauto-r$(REVISION).tar.gz
#TMPDIR=$(shell mktemp -d -p /tmp/ freepbx-module-zapauto-XXXXXX)
TMPDIR=/tmp/freepbx-module/

dist:
	rm -fr $(TMPDIR)
	mkdir -p $(TMPDIR)
	svn export . $(TMPDIR)/freepbx-module-zapauto-r$(REVISION)/
	sed -i 's/^Release:.*/Release: $(REVISION)/'  $(TMPDIR)/freepbx-module-zapauto-r$(REVISION)/freepbx-module-zapauto.spec
	tar -czf  $(FILENAME) -C $(TMPDIR)/ .
	rm -fr $(TMPDIR)

####################################################3
$(DESTDIR)/$(MODULES_DIR)/$(MODULE_NAME):
	mkdir -p $(DESTDIR)/$(MODULES_DIR)/$(MODULE_NAME)

remove_share_files:
	# clean up sudo
	fgrep -v "$(LINE)"  /etc/sudoers > /etc/sudoers.tmp && cp /etc/sudoers.tmp /etc/sudoers && rm /etc/sudoers.tmp
	fgrep -v "$(LINE2)" /etc/sudoers > /etc/sudoers.tmp && cp /etc/sudoers.tmp /etc/sudoers && rm /etc/sudoers.tmp
	
	# remove support scripts
	#mv /usr/local/sbin/genzaptelconf.trixbox /usr/local/sbin/genzaptelconf
	for i in $(SHARE_FILES); do rm -f  $(DESTDIR)/$(BIN_DIR)/$$i; done
