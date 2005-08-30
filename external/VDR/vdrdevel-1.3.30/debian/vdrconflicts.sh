#!/bin/sh
set -e

# Set conflicts with previous and next vdr version in control
for p in $(dh_listpackages); do
    awk -v 'FS=[ ".]+' '/define VDRVERSION / { \
        print "vdr:Conflicts=vdr (<< " $3 "." $4 "." $5   ")," \
                           " vdr (>= " $3 "." $4 "." $5+1 ")" }' \
        /usr/include/vdr/config.h >> debian/$p.substvars
done
