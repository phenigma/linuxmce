KERNEL=2.6.10-1-686

MAINT_NAME=Pluto Inc. (www.plutohome.com)
MAINT_ADDR=office@plutohome.com

PACKAGE_NAME=ivtv-drivers-pluto
PACKAGE_VER=0.2.0.fc3


clean:
	$(MAKE) -C driver/ clean

modules: FORCE
	cd driver/ && KDIR=/lib/modules/$(KERNEL)/build $(MAKE) -e 

deb: modules 
	rm -rf $(PACKAGE_NAME)-$(PACKAGE_VER);
	mkdir $(PACKAGE_NAME)-$(PACKAGE_VER)
	cd $(PACKAGE_NAME)-$(PACKAGE_VER) && echo | DEBFULLNAME="$(MAINT_NAME)" dh_make -s -e "$(MAINT_ADDR)"
	mkdir -p $(PACKAGE_NAME)-$(PACKAGE_VER)/lib/modules/$(KERNEL)/extra
	cp driver/*.ko $(PACKAGE_NAME)-$(PACKAGE_VER)/lib/modules/$(KERNEL)/extra/
	cp postinst $(PACKAGE_NAME)-$(PACKAGE_VER)/debian/
	cp Makefile.debian $(PACKAGE_NAME)-$(PACKAGE_VER)/Makefile
	cd $(PACKAGE_NAME)-$(PACKAGE_VER) && dpkg-buildpackage -b 

FORCE:
