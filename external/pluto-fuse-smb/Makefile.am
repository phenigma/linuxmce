AM_CPPFLAGS=-D_REENTRANT -D_FILE_OFFSET_BITS=64
bin_PROGRAMS=fusesmb
fusesmb_SOURCES=fusesmb.c smbctx.c
noinst_HEADERS = smbctx.h debug.h

#  uncomment the following if fusesmb requires the math library

#EXTRA_DIST=fusesmb.lsm.in fusesmb.spec.in fusesmb.texinfo
EXTRA_DIST= debian fusesmb.conf.ex fusesmb.1 fusesmb.conf.5

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk

#  build and install the .info pages
#info_TEXINFOS = fusesmb.texinfo
#fusesmb_TEXINFOS = gpl.texinfo

#  install the man pages
man_MANS=fusesmb.1 fusesmb.conf.5
