#
# Makefile:	hotplug-perl 1.0
# Provided by:	Anthony L. Awtrey <tony@idealcorp.com>
#
# This Makefile only is installs the Perl script and files
# since nothing needs to be made.
#

default:
	@echo Run make install or make uninstall

clean:	default

install:
	mkdir -p $(DESTDIR)/etc/hotplug-perl/blacklist.d
	mkdir -p $(DESTDIR)/etc/hotplug-perl/Hotplug
	mkdir -p $(DESTDIR)/usr/share/doc/hotplug-perl
	install -o root -g root -m 0644 Hotplug.pm $(DESTDIR)/etc/hotplug-perl
	install -o root -g root -m 0644 Hotplug/IDE.pm $(DESTDIR)/etc/hotplug-perl/Hotplug
	install -o root -g root -m 0644 Hotplug/IEEE1394.pm $(DESTDIR)/etc/hotplug-perl/Hotplug
	install -o root -g root -m 0644 Hotplug/INPUT.pm $(DESTDIR)/etc/hotplug-perl/Hotplug
	install -o root -g root -m 0644 Hotplug/ISAPNP.pm $(DESTDIR)/etc/hotplug-perl/Hotplug
	install -o root -g root -m 0644 Hotplug/PCI.pm $(DESTDIR)/etc/hotplug-perl/Hotplug
	install -o root -g root -m 0644 Hotplug/SCSI.pm $(DESTDIR)/etc/hotplug-perl/Hotplug
	install -o root -g root -m 0644 Hotplug/USB.pm $(DESTDIR)/etc/hotplug-perl/Hotplug
	install -o root -g root -m 0644 blacklist $(DESTDIR)/etc/hotplug-perl
	install -o root -g root -m 0755 start-hotplug $(DESTDIR)/etc/hotplug-perl
	install -o root -g root -m 0755 list-detected-modules $(DESTDIR)/etc/hotplug-perl
	install -o root -g root -m 0755 hotplug-perl $(DESTDIR)/sbin
	install -o root -g root -m 0644 README $(DESTDIR)/usr/share/doc/hotplug-perl/README
	pod2man Hotplug.pm $(DESTDIR)/usr/share/man/man3/Hotplug.3
	pod2man Hotplug/IDE.pm $(DESTDIR)/usr/share/man/man3/Hotplug::IDE.3
	pod2man Hotplug/IEEE1394.pm $(DESTDIR)/usr/share/man/man3/Hotplug::IEEE1394.3
	pod2man Hotplug/INPUT.pm $(DESTDIR)/usr/share/man/man3/Hotplug::INPUT.3
	pod2man Hotplug/ISAPNP.pm $(DESTDIR)/usr/share/man/man3/Hotplug::ISAPNP.3
	pod2man Hotplug/PCI.pm $(DESTDIR)/usr/share/man/man3/Hotplug::PCI.3
	pod2man Hotplug/SCSI.pm $(DESTDIR)/usr/share/man/man3/Hotplug::SCSI.3
	pod2man Hotplug/USB.pm $(DESTDIR)/usr/share/man/man3/Hotplug::USB.3

uninstall:
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug::USB.3
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug::SCSI.3
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug::PCI.3
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug::ISAPNP.3
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug::INPUT.3
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug::IEEE1394.3
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug::IDE.3
	rm -f $(DESTDIR)/usr/share/man/man3/Hotplug.3
	rm -f $(DESTDIR)/usr/share/doc/hotplug-perl/README
	rm -f $(DESTDIR)/sbin/hotplug-perl
	rm -f $(DESTDIR)/etc/hotplug-perl/list-detected-modules
	rm -f $(DESTDIR)/etc/hotplug-perl/start-hotplug
	rm -f $(DESTDIR)/etc/hotplug-perl/blacklist
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug/USB.pm
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug/SCSI.pm
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug/PCI.pm
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug/ISAPNP.pm
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug/INPUT.pm
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug/IEEE1394.pm
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug/IDE.pm
	rm -f $(DESTDIR)/etc/hotplug-perl/Hotplug.pm
	rmdir $(DESTDIR)/usr/share/doc/hotplug-perl
	rmdir $(DESTDIR)/etc/hotplug-perl/blacklist.d
	rmdir $(DESTDIR)/etc/hotplug-perl/Hotplug
	rmdir $(DESTDIR)/etc/hotplug-perl

# End
