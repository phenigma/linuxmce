KSRC ?= /usr/src/linux
KVER ?= unknown

KERNEL=$(shell uname -r)

MAINT_NAME=Pluto Inc. (www.plutohome.com)
MAINT_ADDR=office@plutohome.com

PACKAGE_NAME=ivtv-drivers-pluto
PACKAGE_VER=0.2.0.fc3

# If they didn't set $(KVERS), see if we can do it for them.
ifeq ($(KVER),unknown)
    KVER = $(shell head -1 /usr/src/linux/include/linux/version.h | sed 's/.*"\(.*\)"$$/\1/')
    endif
    
clean:
	$(MAKE) -C driver/ clean
	    
replaces:
	for i in postinst; do \
	    cat mkr_$$i.sh.in | sed  -e 's/_KVER_/$(KVER)/g'  > mkr_$$i.sh ;\
	    chmod +x mkr_$$i.sh ;\
	done
					    

modules all clean install: replaces
	make -C driver $@
	make -C i2c-drivers $@
	make -C utils $@
	make -C test $@			
