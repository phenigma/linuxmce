none:
	

install:
	cp -a root/* "$(DESTDIR)"
	find "$(DESTDIR)" -type d -name .svn -exec rm -rf '{}' ';' -prune
	chmod 0766 "$(DESTDIR)/var/spool/asterisk/monitor"
	chmod 0766 "$(DESTDIR)/var/spool/asterisk/fax"

clean: none
