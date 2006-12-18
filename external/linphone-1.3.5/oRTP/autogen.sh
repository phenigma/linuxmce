set -x
rm -rf config.cache autom4te.cache
aclocal
autoheader
automake --add-missing --copy
libtoolize --copy --force
autoconf
