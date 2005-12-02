BUILD_DIR := $(shell pwd)/v4l

ifeq ($(EDITOR),)
  ifeq ($(VISUAL),)
	EDITOR := vi
  else
	EDITOR := $(VISUAL)
  endif
endif

all:

install:
	$(MAKE) -C $(BUILD_DIR) install

%::
	$(MAKE) -C $(BUILD_DIR) $(MAKECMDGOALS)

changelog:: whitespace
	cd $(BUILD_DIR); scripts/cardlist; cd ..
	cvs update
	cd $(BUILD_DIR); scripts/prepare-ChangeLog.pl; cd ..
	$(EDITOR) v4l/ChangeLog

whitespace:
	@v4l/scripts/strip-trailing-whitespaces.sh | patch -p0

