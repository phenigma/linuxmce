/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define this if you're running PowerPC architecture */
/* #undef ARCH_PPC */

/* Define this if you're running SPARC architecture */
/* #undef ARCH_SPARC */

/* Define this if you're running x86 architecture */
#define ARCH_X86 

/* Define this if you're running x86 architecture 32 bits */
/* #undef ARCH_X86_32 */

/* Define this if you're running x86 architecture 64 bits */
#define ARCH_X86_64 

/* define if '.align n' means alignment to (1<<n)-byte boundaries */
/* #undef ASMALIGN_1SLN */

/* Define the highest alignment supported */
#define ATTRIBUTE_ALIGNED_MAX 64

/* compiler does lsbf in struct bitfields */
#define BITFIELD_LSBF 

/* Define 1 if you are compiling using cygwin */
/* #undef CYGWIN */

/* what to put between the brackets for empty arrays */
#define EMPTY_ARRAY_SIZE 

/* Define this if you have a Motorola 74xx CPU */
/* #undef ENABLE_ALTIVEC */

/* Define this to 1 to enable font antialising. */
/* #undef ENABLE_ANTIALIASING */

/* Enable this when IPv6 is requested */
/* #undef ENABLE_IPV6 */

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#define ENABLE_NLS 1

/* Define this if you have Sun UltraSPARC CPU */
/* #undef ENABLE_VIS */

/* Dummy mark as being exported */
#define EXPORTED __attribute__((visibility("default")))

/* Get of rid system libcdio build configuration */
#define EXTERNAL_LIBCDIO_CONFIG_H 1

/* Define to select libmad fixed point arithmetic implementation */
#define FPM_64BIT 1

/* Define to select libmad fixed point arithmetic implementation */
/* #undef FPM_ARM */

/* Define to select libmad fixed point arithmetic implementation */
/* #undef FPM_DEFAULT */

/* Define to select libmad fixed point arithmetic implementation */
/* #undef FPM_INTEL */

/* Define to select libmad fixed point arithmetic implementation */
/* #undef FPM_MIPS */

/* Define to select libmad fixed point arithmetic implementation */
/* #undef FPM_PPC */

/* Define to select libmad fixed point arithmetic implementation */
/* #undef FPM_SPARC */

/* Define to 1 if you have the <a52dec/a52.h> header file. */
/* #undef HAVE_A52DEC_A52_H */

/* Define to 1 if you have the <a52dec/a52_internal.h> header file. */
/* #undef HAVE_A52DEC_A52_INTERNAL_H */

/* Define to 1 if you have the <alloca.h> header file. */
#define HAVE_ALLOCA_H 1

/* Define this if you have ALSA installed */
#define HAVE_ALSA 1

/* Define this if you have ARTS (libartsc) installed */
/* #undef HAVE_ARTS */

/* Define to 1 if you have the <asm/types.h> header file. */
#define HAVE_ASM_TYPES_H 1

/* Define to 1 if you have the `asprintf' function. */
#define HAVE_ASPRINTF 1

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if `output_muted' is member of `audio_info_t'. */
/* #undef HAVE_AUDIO_INFO_T_OUTPUT_MUTED */

/* Define to 1 if you have the `basename' function. */
#define HAVE_BASENAME 1

/* Define 1 if you have BSDI-type CD-ROM support */
/* #undef HAVE_BSDI_CDROM */

/* Define to 1 if you have the <byteswap.h> header file. */
#define HAVE_BYTESWAP_H 1

/* Define to 1 if you have the `bzero' function. */
#define HAVE_BZERO 1

/* Define this if you have CDROM ioctls */
#define HAVE_CDROM_IOCTLS 1

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define to 1 if you have the <CoreFoundation/CFBase.h> header file. */
/* #undef HAVE_COREFOUNDATION_CFBASE_H */

/* Define 1 if you have Darwin OS X-type CD-ROM support */
/* #undef HAVE_DARWIN_CDROM */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#define HAVE_DCGETTEXT 1

/* Define this if you have DirectX */
/* #undef HAVE_DIRECTX */

/* Define to 1 if you have the <dirent.h> header file. */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <dts.h> header file. */
/* #undef HAVE_DTS_H */

/* Define this if you have a suitable version of libdvdnav */
/* #undef HAVE_DVDNAV */

/* Define to 1 if you have the <dvd.h> header file. */
/* #undef HAVE_DVD_H */

/* Define to 1 if you have the environ symbol. */
/* #undef HAVE_ENVIRON */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define this if you have ESounD installed */
#define HAVE_ESD 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Define this if you have linux framebuffer support */
#define HAVE_FB 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define this if you have ffmpeg library */
/* #undef HAVE_FFMPEG */

/* Define this if you have fontconfig library */
/* #undef HAVE_FONTCONFIG */

/* Define 1 if you have FreeBSD CD-ROM support */
/* #undef HAVE_FREEBSD_CDROM */

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define this if you have freetype2 library */
/* #undef HAVE_FT2 */

/* Define to 1 if you have FusionSound. */
/* #undef HAVE_FUSIONSOUND */

/* Define this if you have gdk-pixbuf installed */
#define HAVE_GDK_PIXBUF 1

/* Define to 1 if you have the `getpwuid_r' function. */
#define HAVE_GETPWUID_R 1

/* Define if the GNU gettext() function is already present or preinstalled. */
#define HAVE_GETTEXT 1

/* Define to 1 if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <glob.h> header file. */
#define HAVE_GLOB_H 1

/* Define this if you have GLU support available */
#define HAVE_GLU 1

/* Define this if you have gnome-vfs installed */
#define HAVE_GNOME_VFS 1

/* Define to 1 if you have 'hstrerror' in <netdb.h> */
#define HAVE_HSTRERROR 1

/* Define if you have the iconv() function. */
#define HAVE_ICONV 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if the system has the type `int_fast8_t'. */
#define HAVE_INT_FAST8_T 1

/* Define to 1 if you have the <IOKit/IOKitLib.h> header file. */
/* #undef HAVE_IOKIT_IOKITLIB_H */

/* Define this if you have ip_mreqn in netinet/in.h */
#define HAVE_IP_MREQN 1

/* Define this if you have a usable IRIX al interface available */
/* #undef HAVE_IRIXAL */

/* Supports ISO _Pragma() macro */
#define HAVE_ISOC99_PRAGMA 

/* Define this if you have kernel statistics available via kstat interface */
/* #undef HAVE_KSTAT */

/* Define to 1 if you have the `a52' library (-la52). */
/* #undef HAVE_LIBA52 */

/* Define to 1 if you have the `artsc' library (-lartsc). */
/* #undef HAVE_LIBARTSC */

/* Define this if you have libfame mpeg encoder installed (fame.sf.net) */
/* #undef HAVE_LIBFAME */

/* Define to 1 if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define if external libmpc is used */
/* #undef HAVE_LIBMPC */

/* Define to 1 if you have the `posix4' library (-lposix4). */
/* #undef HAVE_LIBPOSIX4 */

/* Define this if you have librte mpeg encoder installed (zapping.sf.net) */
/* #undef HAVE_LIBRTE */

/* Define 1 if you have Linux-type CD-ROM support */
#define HAVE_LINUX_CDROM 1

/* Define to 1 if you have the <linux/cdrom.h> header file. */
#define HAVE_LINUX_CDROM_H 1

/* Define 1 if timeout is in cdrom_generic_command struct */
/* #undef HAVE_LINUX_CDROM_TIMEOUT */

/* Define to 1 if you have the <linux/version.h> header file. */
#define HAVE_LINUX_VERSION_H 1

/* Define to 1 if you have the <linux/videodev.h> header file. */
#define HAVE_LINUX_VIDEODEV_H 1

/* Define this if the 'lrintf' function is declared in math.h */
#define HAVE_LRINTF 1

/* Define to 1 if you have the `lstat' function. */
#define HAVE_LSTAT 1

/* Define to 1 if you have the <machine/soundcard.h> header file. */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define to 1 if you have the <mad.h> header file. */
/* #undef HAVE_MAD_H */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the `memalign' function. */
#define HAVE_MEMALIGN 1

/* Define to 1 if you have the `memcpy' function. */
#define HAVE_MEMCPY 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define this if you have mlib installed */
/* #undef HAVE_MLIB */

/* Define to 1 if you have the `mmap' function. */
/* #undef HAVE_MMAP */

/* Define this if you can compile MMX asm instructions */
#define HAVE_MMX 

/* define this if you have libmodplug installed */
#define HAVE_MODPLUG 1

/* Define to 1 if you have the <mpcdec/mpcdec.h> header file. */
/* #undef HAVE_MPCDEC_MPCDEC_H */

/* Define to 1 if you have the `nanosleep' function. */
#define HAVE_NANOSLEEP 1

/* Define to 1 if you have the <netdb.h> header file. */
#define HAVE_NETDB_H 1

/* Define this if you have libfame 0.8.10 or above */
/* #undef HAVE_NEW_LIBFAME */

/* Define to 1 if you have the `nl_langinfo' function. */
#define HAVE_NL_LANGINFO 1

/* Define to 1 if you have 'opendir' function */
#define HAVE_OPENDIR 1

/* Define this if you have OpenGL support available */
#define HAVE_OPENGL 1

/* Define this if you have POSIX timers. */
#define HAVE_POSIX_TIMERS 1

/* Define to 1 if you have the `readlink' function. */
#define HAVE_READLINK 1

/* Define this if the 'rintf' function is declared in math.h */
#define HAVE_RINTF 1

/* Define this if you have SDL installed */
#define HAVE_SDL 1

/* Define to 1 if you have the `setenv' function. */
#define HAVE_SETENV 1

/* Define to 1 if you have the `sigaction' function. */
#define HAVE_SIGACTION 1

/* Define to 1 if you have the `sigset' function. */
#define HAVE_SIGSET 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define 1 if you have Solaris CD-ROM support */
/* #undef HAVE_SOLARIS_CDROM */

/* Define to 1 if you have the <soundcard.h> header file. */
/* #undef HAVE_SOUNDCARD_H */

/* Define this if you have speex */
#define HAVE_SPEEX 1

/* Define to 1 if you have the <stdbool.h> header file. */
#define HAVE_STDBOOL_H 1

/* Define to 1 if you have the stderr symbol. */
#define HAVE_STDERR 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the `strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the `strpbrk' function. */
#define HAVE_STRPBRK 1

/* Define to 1 if you have the `strsep' function. */
#define HAVE_STRSEP 1

/* Define to 1 if you have the `strtok_r' function. */
#define HAVE_STRTOK_R 1

/* Define this if the 'sysi86' function is declared in sys/sysi86.h */
/* #undef HAVE_SYSI86 */

/* Define to 1 if you have the <sys/cdio.h> header file. */
/* #undef HAVE_SYS_CDIO_H */

/* Define to 1 if you have the <sys/dvdio.h> header file. */
/* #undef HAVE_SYS_DVDIO_H */

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/mixer.h> header file. */
/* #undef HAVE_SYS_MIXER_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/scsiio.h> header file. */
/* #undef HAVE_SYS_SCSIIO_H */

/* Define to 1 if you have the <sys/soundcard.h> header file. */
#define HAVE_SYS_SOUNDCARD_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/times.h> header file. */
#define HAVE_SYS_TIMES_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define this if you have theora */
#define HAVE_THEORA 1

/* Define to 1 if you have the `timegm' function. */
#define HAVE_TIMEGM 1

/* Define if struct tm has the tm_gmtoff member. */
#define HAVE_TM_GMTOFF 1

/* Define to 1 if you have the <ucontext.h> header file. */
#define HAVE_UCONTEXT_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `unsetenv' function. */
#define HAVE_UNSETENV 1

/* Define to 1 if you have the `va_copy' function. */
/* #undef HAVE_VA_COPY */

/* Define this if you use external libcdio/libvcdinfo */
/* #undef HAVE_VCDNAV */

/* Define 1 if you have vldXvMC.h */
#define HAVE_VLDXVMC 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to 1 if you have the `vsscanf' function. */
#define HAVE_VSSCANF 1

/* Define this if you have ImageMagick installed */
#define HAVE_WAND 1

/* Define 1 if you have MinGW CD-ROM support */
/* #undef HAVE_WIN32_CDROM */

/* Define this if you have X11R6 installed */
#define HAVE_X11 1

/* Define to 1 if you have the <X11/extensions/vldXvMC.h> header file. */
#define HAVE_X11_EXTENSIONS_VLDXVMC_H 1

/* Define to 1 if you have the <X11/extensions/XShm.h> header file. */
/* #undef HAVE_X11_EXTENSIONS_XSHM_H */

/* Define to 1 if you have the <X11/extensions/XvMC.h> header file. */
#define HAVE_X11_EXTENSIONS_XVMC_H 1

/* Define this if you have libXinerama installed */
#define HAVE_XINERAMA 1

/* Define this if you have libXv installed */
#define HAVE_XV 1

/* Define this if you have an XvMC library and XvMC.h installed. */
#define HAVE_XVMC 1

/* Define this if you have libXv.a */
/* #undef HAVE_XV_STATIC */

/* Define this to compile the xxmc plugin. */
#define HAVE_XXMC 1

/* Define to 1 if you have the _environ symbol. */
/* #undef HAVE__ENVIRON */

/* Define to 1 if you have the `_snprintf' function. */
/* #undef HAVE__SNPRINTF */

/* Define to 1 if you have the `_stricmp' function. */
/* #undef HAVE__STRICMP */

/* Define to 1 if you have the `_strnicmp' function. */
/* #undef HAVE__STRNICMP */

/* Define to 1 if you have the `_vsnprintf' function. */
/* #undef HAVE__VSNPRINTF */

/* Define to 1 if you have the __ctype_b symbol. */
#define HAVE___CTYPE_B 1

/* Define to 1 if you have the __environ symbol. */
#define HAVE___ENVIRON 1

/* Define to 1 if you have the __stderrp symbol. */
/* #undef HAVE___STDERRP */

/* Define to 1 if you have the ___brk_addr symbol. */
#define HAVE____BRK_ADDR 1

/* host os/cpu identifier */
#define HOST_ARCH "linux-gnu/x86_64"

/* Define this if built on Mac OS X/Darwin */
/* #undef HOST_OS_DARWIN */

/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST 

/* Type of the request parameter for ioctl() */
#define IOCTL_REQUEST_TYPE unsigned long

/* Get of rid system libcdio build configuration */
#define LIBCDIO_CONFIG_H 1

/* Define this if you have mlib installed */
/* #undef LIBMPEG2_MLIB */

/* The soname of libX11, needed for dlopen() */
#define LIBX11_SO "libX11.so.6"

/* The soname of libXv, needed for dlopen() */
#define LIBXV_SO "libXv.so.1"

/* Define 1 if you are compiling using MinGW */
/* #undef MINGW32 */

/* Define this if you want to load mlib lazily */
/* #undef MLIB_LAZYLOAD */

/* Name of package */
#define PACKAGE "xine-lib"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* Specified path for Real binary codecs */
/* #undef REAL_CODEC_PATH */

/* The size of `int', as computed by sizeof. */
/* #undef SIZEOF_INT */

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* The size of `long long', as computed by sizeof. */
/* #undef SIZEOF_LONG_LONG */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define this if the compiler supports the alias attribute */
/* #undef SUPPORT_ATTRIBUTE_ALIAS */

/* Define this if the compiler supports the format attribute */
#define SUPPORT_ATTRIBUTE_FORMAT 1

/* Define this if the compiler supports the format_arg attribute */
#define SUPPORT_ATTRIBUTE_FORMAT_ARG 1

/* Define this if the compiler supports the sentinel attribute */
#define SUPPORT_ATTRIBUTE_SENTINEL 1

/* Define this if the compiler supports __attribute__((visibility("default")))
   */
/* #undef SUPPORT_ATTRIBUTE_VISIBILITY_DEFAULT */

/* Define this if the compiler supports
   __attribute__((visibility("protected"))) */
#define SUPPORT_ATTRIBUTE_VISIBILITY_PROTECTED 1

/* Define this if the compiler supports the -fvisibility flag */
#define SUPPORT_FLAG_VISIBILITY 1

/* Version number of package */
#define VERSION "1.1.7"

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Path where aclocal m4 files will be. */
#define XINE_ACFLAGS "-I /usr/local/share/aclocal"

/* Define this to osd fonts dir location */
#define XINE_FONTDIR "/usr/local/share/xine/libxine1/fonts"

/* Path where catalog files will be. */
#define XINE_LOCALEDIR "/usr/local/share/locale"

/* xine major version number */
#define XINE_MAJOR 1

/* xine minor version number */
#define XINE_MINOR 1

/* Define this to plugins directory location */
#define XINE_PLUGINDIR "/usr/local/lib/xine/plugins/1.1.7"

/* Define this to font directory relative to prefix */
#define XINE_REL_FONTDIR "share/xine/libxine1/fonts"

/* Define this to font directory relative to prefix */
#define XINE_REL_LOCALEDIR "share/locale"

/* Define this to plugin directory relative to execution prefix */
#define XINE_REL_PLUGINDIR "lib/xine/plugins/1.1.7"

/* xine sub version number */
#define XINE_SUB 7

/* catalog message text domain */
#define XINE_TEXTDOMAIN "libxine1"

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */

/* enable warnings about being development release */
#define _DEVELOPMENT_ 

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif

/* Define this if you are ISO C9X compliant */
#define _ISOC9X_SOURCE 1

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* restrict keyword */
#define restrict __restrict__

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define the real type of socklen_t */
/* #undef socklen_t */

/* define ssize_t to __int64 if it's missing in default includes */
/* #undef ssize_t */

#ifdef ASMALIGN_1SLN
# define ASMALIGN(ZEROBITS) ".align " #ZEROBITS "\n\t"
#else
# define ASMALIGN(ZEROBITS) ".align 1<<" #ZEROBITS "\n\t"
#endif


/* include internal system specific header */
#include "os_internal.h"
