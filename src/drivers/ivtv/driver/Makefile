VERS26=$(findstring 2.6,$(shell uname -r))
MAKEDIR?=$(shell pwd)
export MAKEDIR

ifeq ($(VERS26),2.6)
include $(MAKEDIR)/Makefile2.6
else
include $(MAKEDIR)/Makefile2.4
endif
