#!/bin/sh

diff -Nru -x Makefile.cvs -x README.qt-copy -x upgrade_script.qt -x .cvsignore -x CVS -x include -x debian -x doc -x examples  -x 3rdparty -x images -x mkspecs -I '^\*\*\( $\| \$Id\)' -I '^\#\( $\| \$Id\)' qt-x11-free-3.1.1.old/ qt-x11-free-3.1.1/ > qtcopy.diff
