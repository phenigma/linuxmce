AC_INIT
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(libXvMCW,0.9.3)
SOVERSION=1:0:0
AC_PROG_CC
AC_PATH_X
AC_PROG_LIBTOOL
AC_ARG_WITH(config-path, AC_HELP_STRING([--with-config-path],[The path to the desired
	X config directory. Defaults to /etc/X11]),
	config_path="$withval", config_path="/etc/X11")
AC_DEFINE_UNQUOTED(XVMC_CONFIGDIR,$config_path,[The X config directory path])
dnl
dnl Check for Xv version.
dnl
AC_CHECK_FILE($x_libraries/libXv.so.1,xv_version=".1",xv_version="no")
if test x$xv_version == "xno"; then
  AC_CHECK_FILE($x_libraries/libXv.so.1.0,xv_version=".1.0",xv_version="no")
fi
if test x$xv_version == "xno"; then
  xv_version=""
  echo "*** Warning. Could not find a dynamic libXv.so library."
  echo "*** This may cause run-time failures when the hardware-specific"
  echo "*** library is loaded. I'm continuing anyway."
fi
AC_DEFINE_UNQUOTED(XV_SOVERSION,"$xv_version",[The version number of libXv.so])
dnl
dnl Check for XvMC version.
dnl
AC_CHECK_FILE($x_libraries/libXvMC.so.1,xvmc_version=".1",xvmc_version="no")
if test x$xvmc_version == "xno"; then
  AC_CHECK_FILE($x_libraries/libXvMC.so.1.0,xvmc_version=".1.0",xv_version="no")
fi	
if test x$xvmc_version == "xno"; then
  AC_MSG_ERROR(Could not find a libXvMC.so dynamic library. Aborting.)
fi
AC_DEFINE_UNQUOTED(XVMC_SOVERSION,"$xvmc_version",[The version number of libXvMC.so])
dnl
dnl
dnl
WINCLUDES="-I$x_includes/X11/extensions"
XLIB=$x_libraries
XEXTINCLUDE=$x_includes/X11/extensions
XCONFIG=$config_path
AC_SUBST(SOVERSION)
AC_SUBST(WINCLUDES)
AC_SUBST(XEXTINCLUDE)
AC_SUBST(XLIB)
AC_SUBST(XCONFIG)
AC_OUTPUT(Makefile)
