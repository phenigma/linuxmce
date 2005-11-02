VERS26=$(findstring 2.6,$(shell uname -r))
MAKEDIR?=$(shell pwd)
export MAKEDIR

ifeq ($(VERS26),2.6)
include $(MAKEDIR)/Makefile2.6

all: ivtv-svnversion.h
else
include $(MAKEDIR)/Makefile2.4
endif

clean::
	rm -f ivtv-svnversion.h

ivtv-svnversion.h:
	@echo created $@
	@if [ -f ivtv-svnrelease.h ]; then \
		echo '#include "ivtv-svnrelease.h"' >$@; \
	elif [ -d .svn ]; then \
		echo -n '#define IVTV_DRIVER_VERSION_COMMENT "(development svn snapshot revision ' >$@; \
		svnversion -n . >>$@; \
		echo ')"' >>$@; \
	else date +'#define IVTV_DRIVER_VERSION_COMMENT "(development snapshot compiled on %c)"' >$@; fi
		 

.PHONY: ivtv-svnversion.h
