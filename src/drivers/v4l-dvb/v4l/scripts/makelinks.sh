#!/bin/sh
# link drivers sources from CVS or release tarball into your 2.6.x kernel sources;

if test -z $1 || ! test -d $1 ; then
	echo
	echo "  usage: $0 <path_to_kernel_to_patch>"
	echo
	exit 
fi

echo "patching $1..."

cd linux
PWD=`pwd`

for x in `find drivers -type d | grep -v CVS` ; do
	mkdir -p -v $1/$x 
done

for x in `find Documentation -type d | grep -v CVS` ; do
	mkdir -p -v $1/$x 
done

for x in `find include -type d | grep -v CVS` ; do
	mkdir -p -v $1/$x 
done

for x in `find Documentation -type f | grep -v CVS | grep -v .cvsignore` ; do
	ln -v -f -s $PWD/$x $1/$x
done

for x in `find drivers -type f | grep -v CVS | grep -v .cvsignore` ; do
	ln -v -f -s $PWD/$x $1/$x
done

for x in `find include -type f | grep -v CVS | grep -v .cvsignore` ; do
	ln -v -f -s $PWD/$x $1/$x
done

cd ..

ln -v -f -s $PWD/v4l/compat.h $1/include/media/compat.h
ln -v -f -s $PWD/v4l/compat.h $1/include/linux/compat.h
ln -v -f -s $PWD/v4l/compat.h $1/drivers/media/common/compat.h
ln -v -f -s $PWD/v4l/compat.h $1/drivers/media/dvb/bt8xx/compat.h
ln -v -f -s $PWD/v4l/compat.h $1/drivers/media/dvb/cinergyT2/compat.h
ln -v -f -s $PWD/v4l/compat.h $1/drivers/media/dvb/dvb-core/compat.h
ln -v -f -s $PWD/v4l/compat.h $1/drivers/media/video/compat.h
patch -p0 <<'DIFF'
diff -u -p videodev.h
--- linux/include/linux/videodev.h
+++ linux/include/linux/videodev.h
@@ -1,6 +1,7 @@
 #ifndef __LINUX_VIDEODEV_H
 #define __LINUX_VIDEODEV_H

+#include "compat.h"
 #include <linux/types.h>

 #define HAVE_V4L1 1
diff -up v4l-kernel.orig/v4l/Makefile v4l-kernel/v4l/Makefile
--- v4l/Makefile	2005-10-10 00:57:41.000000000 -0400
+++ v4l/Makefile	2005-10-10 00:58:26.000000000 -0400
@@ -407,6 +407,7 @@ clean::
 	@find . -name '*.c' -type l -exec rm '{}' \;
 	@find . -name '*.h' -type l -exec rm '{}' \;
 	-rm -f *~ *.o *.ko *.mod.c
+	@cd ..; patch -p0 -s -R -N < v4l/scripts/makelinks.sh
 
 distclean:: clean
 	-rm -f .version .*.o.flags .*.o.d .*.o.cmd .*.ko.cmd
