#!/bin/bash
# ==============================================================
# make.sh
# (C) 2001 by ATI Technologies
# ==============================================================


# ==============================================================
# local variables and files
current_wd=`pwd`
logfile=$current_wd/make.sh.log	# DKMS uses name 'make.log', so that we need another name

# project name
MODULE=fglrx

FGL_PUBLIC=firegl
target_define=FGL_RX


# package defaults
#BEGIN-DEFAULT
INSTALL_MODULE_SUBDIRS=0
#END-DEFAULT

# package custom overrides, created by installer
#BEGIN-CUSTOM
#END-CUSTOM

# vendor options
DEMAND_BZIMAGE=0
CHECK_P3=0

# default options
OPTIONS_HINTS=1

# sets the GCC to use to the one required by the module (if available)
function set_GCC_version () {
  #identify GCC default version major number
  GCC_MAJOR="`gcc --version | grep -o -e "(GCC) ." | cut -d " " -f 2`"

  #identify the GCC major version that compiled the kernel
  KERNEL_GCC_MAJOR="`cat /proc/version | grep -o -e "gcc version ."  | cut -d " " -f 3`"

  #see if they don't match
  if [ ${GCC_MAJOR} != ${KERNEL_GCC_MAJOR} ]; then
       #use kernel GCC version hopefully
       KERNEL_GCC="`cat /proc/version | grep -o -e "gcc version [0-9]\.[0-9]" | cut -d " " -f 3`"
       CC="gcc-${KERNEL_GCC}"

       # check if gcc version requested exists
       GCC_AVAILABLE="`${CC} --version | grep -e "(GCC)" |cut -d " " -f 3 | cut -c-3`"
    
       if [ ${GCC_AVAILABLE} != ${KERNEL_GCC} ]; then
           echo "The GCC version that is required to compile this module is version ${KERNEL_GCC}."
           echo "Please install this GCC or recompile your kernel with ${GCC_AVAILABLE}"
           exit 1
       fi
  fi
}

if [ -z "${CC}" ]; then 
	CC=gcc
	set_GCC_version
fi

# parse options
if [ "$1" = "--nohints" ]; then
  OPTIONS_HINTS=0
fi
  

# ==============================================================
# system/kernel identification
uname_r=`uname -r`
uname_v=`uname -v`
uname_s=`uname -s`
uname_m=`uname -m`
uname_a=`uname -a`

# if you want to overload the current settings then please enter 1 below
# and then customize the overloaded strings to your specific needs
if [ "${FORCE_UNAME}" != "" ]; then
	uname_overload=1
else
	uname_overload=0
fi

if [ ${uname_overload} -gt 0 ]
then
  uname_r="2.4.7-10"
  uname_v="#1 Thu Sep 6 17:27:27 EDT 2001"
  uname_s="Linux"
  uname_m="i686"
  uname_a="${uname_s} uname_overload.ati.com ${uname_r} ${uname_v} ${uname_m} unknown"

  echo "applying uname_overload"
  echo "uname_a=${uname_a}"
fi


# ==============================================================
# xfree and system locations                                                          
XF_ROOT=/usr/X11R6                                                                    
XF_BIN=$XF_ROOT/bin
OS_MOD=/lib/modules


# ==============================================================
# check if we are running as root with typical login shell paths
if [ "${AS_USER}" != "y" ]; then
if [ `id -u` -ne 0 ]
then
  echo "You must be logged in as root to run this script."
  exit 1
fi

which depmod >/dev/null 2>&1
if [ $? -ne 0 ];
then
#  echo "(completing current path to be a root path)"
#  echo ""
  PATH=/usr/local/sbin:/usr/sbin:/sbin:${PATH}
  which depmod >/dev/null 2>&1
  if [ $? -ne 0 ];
  then
    echo "You arent running in a 'login shell'."
    echo "Please login directly from a console"
    echo "or use 'su -l' to get the same result."
    exit 1
  fi
fi

fi # build AS_USER 


# ==============================================================
# resolve commandline parameters

# (none at the moment)


# ==============================================================
# assign defaults to non-sepcified environment parameters
if [ -z "${SOURCE_PREFIX}" ]; then
  SOURCE_PREFIX=.
fi
if [ "${SOURCE_PREFIX}" != "/" ]; then
  SOURCE_PREFIX=`echo $SOURCE_PREFIX | sed -e 's,/$,,g'`
fi

if [ -z "${LIBIP_PREFIX}" ]; then
  LIBIP_PREFIX=.
fi
if [ "${LIBIP_PREFIX}" != "/" ]; then
  LIBIP_PREFIX=`echo $LIBIP_PREFIX | sed -e 's,/$,,g'`
fi


# ==============================================================
# specify defaults for include file locations

# assing default location of linux kernel headers
# *** adapt to your individual setup if needed ***
if [ -z "${KERNEL_PATH}" ]; then
#linuxincludes=/usr/include    # no config info present!
linuxincludes=/usr/src/linux/include
#linuxincludes=/usr/src/linux-2.2.14.new.iii/include
#linuxincludes=/usr/src/linux-2.4.0-test7/include

# in /lib/modules/<kernel-version> there is a symlink for latest kernel
# which calls "build" and points to the directory where modules were built.
if [ -d /lib/modules/${uname_r}/build/include ];
then
  # just comment this line out if you already set an alternative location
  linuxincludes=/lib/modules/${uname_r}/build/include
fi

else
  linuxincludes=${KERNEL_PATH}/include
fi

# ==============================================================
# print a few statistics, helpful for analyzing any build failures
echo ATI module generator V 2.0 | tee $logfile
echo ========================== | tee -a $logfile
echo initializing...            | tee -a $logfile
echo "build_date ="`date`   >>$logfile
echo "uname -a ="${uname_a} >>$logfile
echo "uname -s ="${uname_s} >>$logfile
echo "uname -m ="${uname_m} >>$logfile
echo "uname -r ="${uname_r} >>$logfile
echo "uname -v ="${uname_v} >>$logfile
id       >>$logfile
echo .   >>$logfile
ls -l -d /usr/include >>$logfile
echo .   >>$logfile
cd /usr/src
ls -l .               >>$logfile
cd $current_wd
echo .   >>$logfile


# ==============================================================
# locate and verify contents of kernel include file path

# verify match with respective line in linux/version.h
# sample: #define UTS_RELEASE "2.4.0-test7"
src_file=$linuxincludes/linux/version.h
if [ ! -e $src_file ];
then
  echo "kernel includes at $linuxincludes not found or incomplete" | tee -a $logfile
  echo "file: $src_file"                                           | tee -a $logfile
  exit 1
fi
OsRelease=${uname_r}
UTS_REL_COUNT=`cat $src_file | grep UTS_RELEASE -c`
if [ $UTS_REL_COUNT -gt 1 ];
then
  kernel_release=`cat $src_file | grep UTS_RELEASE | grep \"$OsRelease\" | cut -d'"' -f2`
else
  if [ $UTS_REL_COUNT -gt 0 ];
  then
    kernel_release=`cat $src_file | grep UTS_RELEASE | cut -d'"' -f2`
  else
    UTS_REL_COUNT=`cat $linuxincludes/linux/version-*.h | grep UTS_RELEASE -c`
    if [ $UTS_REL_COUNT -gt 0 ];
    then
        # UTS-define is in external version-*.h files, i.e. linux-2.2.14-5.0-RedHat does this flaw
        kernel_release=`cat $linuxincludes/linux/version-*.h | grep UTS_RELEASE | grep \"$OsRelease\" | cut -d'"' -f2`
    else
        # For 2.6.18 or higher, UTS-define is defined in utsrelease.h.
        kernel_release=`cat $linuxincludes/linux/utsrelease.h | grep UTS_RELEASE | grep \"$OsRelease\" | cut -d'"' -f2`
    fi
  fi
fi


if [ -z "${KERNEL_PATH}" ]; then
# compare release string of running kernel with kernel name from headers
hit=0
if [ "$OsRelease" = "$kernel_release" ];
then
  hit=1
else
  # Red Hat 7.0 source and some newer 2.4.x might not have smp suffix in UTS_RELEASE text
  if [ `echo $OsRelease | grep smp -c` -ne 0 ];
  then
    if [ "$OsRelease" = "${kernel_release}smp" ];
    then
      hit=1
    fi
  fi
fi

if [ $hit -eq 0 ]
then
  echo "Error:"                                                         | tee -a $logfile
  echo "kernel includes at $linuxincludes do not match current kernel." | tee -a $logfile
  echo "they are versioned as \"$kernel_release\""                      | tee -a $logfile
  echo "instead of \"$OsRelease\"."                                     | tee -a $logfile
  echo "you might need to adjust your symlinks:"                        | tee -a $logfile
  echo "- /usr/include"                                                 | tee -a $logfile
  echo "- /usr/src/linux"                                               | tee -a $logfile
  exit 1
fi

fi

OsVersion=${uname_v}

if [ $DEMAND_BZIMAGE -gt 0 ]
then

# verify if file linux/compile.h exists and has correct version string
# sample: #define UTS_VERSION "#2 SMP Die Sep 12 22:08:51 MEST 2000"
src_file=$linuxincludes/linux/compile.h
if [ ! -e $src_file ];
then
  # since its a compile time generated file (stamped with date and build environment)
  # we cannot rely on this file beeing always present, so we cant check this.
  echo "Warning:"                                                  >> $logfile
  echo "kernel includes at $linuxincludes not found or incomplete" >> $logfile
  echo "file: $src_file"                                           >> $logfile
  echo "Could not verify kernel reported version against source."  >> $logfile
  echo "Ignore this warning if you know you are here by intention.">> $logfile
  echo ""                                                          >> $logfile
  # regard this as a warning - distributions let user generate this file himself
  # just assume we are runnning the right kernel
  kernel_version=$OsVersion
else
  kernel_version=`cat $src_file | grep UTS_VERSION | cut -d'"' -f2`
  if [ ! "$kernel_version" = "$OsVersion" ];
  then
    echo "Warning:"                                                       >> $logfile
    echo "kernel includes at $linuxincludes do not match current kernel." >> $logfile
    echo "they are versioned as \"$kernel_version\""                      >> $logfile
    echo "instead of \"$OsVersion\"."                                     >> $logfile
    echo "you might need to adjust your symlinks:"                        >> $logfile
    echo "- /usr/include"                                                 >> $logfile
    echo "- /usr/src/linux"                                               >> $logfile
    echo ""                                                               >> $logfile
    if [ "$1" = "verbose" ]
    then
      echo "Warning:"
      echo "kernel includes at $linuxincludes do not match current kernel."
      echo "they are versioned as \"$kernel_version\""
      echo "instead of \"$OsVersion\"."
      echo "you might need to adjust your symlinks:"
      echo "- /usr/include"
      echo "- /usr/src/linux"
      echo ""
    fi
    # regard this as a warning - distributions let user generate this file himself
  fi
fi

fi

# ==============================================================
# resolve if we are running a pentium iii enabled kernel

if [ $CHECK_P3 -ne 0 ]
then

$XF_BIN/cpu_check >/dev/null                                                      
case "$?" in                                                                      
    0) iii=     ;;                                                                
    1) iii=     ;;                                                                
    2) iii=.iii ;;                                                                
    3) iii=     ;;                                                                
    4) iii=     ;;                                                                
    5) iii=.iii ;;                                                                
    6) iii=.iii ;;                                                                
    *) iii=     ;;                                                                
esac

else
  iii=
fi

# ==============================================================
# resolve if we are running an AGP capable kernel source tree.
# Hint: our custom module build simply relys on the header, 
# not on the kernel AGP caps to be enabled at all.

AGP=0

# verify if file linux/agp_backend.h exists
src_file=$linuxincludes/linux/agp_backend.h
if [ -e $src_file ];
then
  AGP=1
#  def_agp=-D__AGP__
  echo "file $src_file says: AGP=$AGP"                             >> $logfile
fi

if [ $AGP = 0 ]
then
  echo "assuming default: AGP=$AGP"                                >> $logfile
fi

# ==============================================================
# resolve if we are running a SMP enabled kernel

SMP=0

if [ $DEMAND_BZIMAGE -gt 0 ]
then

# 1.
# config/smp.h may contain this: #define CONFIG_SMP 1 | #undef  CONFIG_SMP
src_file=$linuxincludes/config/smp.h
if [ ! -e $src_file ];
then
  echo "Warning:"                                                  >> $logfile
  echo "kernel includes at $linuxincludes not found or incomplete" >> $logfile
  echo "file: $src_file"                                           >> $logfile
  echo ""                                                          >> $logfile
else
  if [ `cat $src_file | grep "#undef" | grep "CONFIG_SMP" -c` = 0 ]
  then
    SMP=`cat $src_file | grep CONFIG_SMP | cut -d' ' -f3`
    echo "file $src_file says: SMP=$SMP"                           >> $logfile
  fi
fi

fi

# 2.
# grep in OsVersion string for SMP specific keywords
if [ `echo $OsVersion | grep [sS][mM][pP] -c` -ne 0 ]
then
  SMP=1
  echo "OsVersion says: SMP=$SMP"                                  >> $logfile
fi

# 3.1
# grep in /proc/ksyms for SMP specific kernel symbols
# use triggerlevel of 10 occurences 
# (UP kernels might have 0-1, SMP kernels might have 32-45 or much more)
# 3.2
# grep in /proc/ksyms for the change_page_attr symbol
PAGE_ATTR_FIX=0
src_file=/proc/ksyms
if [ -e $src_file ]
then
  if [ `fgrep smp $src_file -c` -gt 10 ]
  then
    SMP=1
    echo "file $src_file says: SMP=$SMP"                             >> $logfile
  fi
  if [ `fgrep " change_page_attr\$" $src_file -c` -gt 0 ]
  then
    PAGE_ATTR_FIX=1
    echo "file $src_file says: PAGE_ATTR_FIX=$PAGE_ATTR_FIX"         >> $logfile
  fi
fi

src_file=/proc/kallsyms
if [ -e $src_file ]
then
  if [ `fgrep smp $src_file -c` -gt 10 ]
  then
    SMP=1
    echo "file $src_file says: SMP=$SMP"                             >> $logfile
  fi
  if [ `fgrep " change_page_attr\$" $src_file -c` -gt 0 ]
  then
    PAGE_ATTR_FIX=1
    echo "file $src_file says: PAGE_ATTR_FIX=$PAGE_ATTR_FIX"         >> $logfile
  fi
fi

# 4.
# linux/autoconf.h may contain this: #define CONFIG_SMP 1
src_file=$linuxincludes/linux/autoconf.h
if [ ! -e $src_file ];
then
  echo "Warning:"                                                  >> $logfile
  echo "kernel includes at $linuxincludes not found or incomplete" >> $logfile
  echo "file: $src_file"                                           >> $logfile
  echo ""                                                          >> $logfile
else
  if [ `cat $src_file | grep "#undef" | grep "CONFIG_SMP" -c` = 0 ]
  then
    SMP=`cat $src_file | grep CONFIG_SMP | cut -d' ' -f3`
    echo "file $src_file says: SMP=$SMP"                           >> $logfile
  fi
fi

if [ $SMP = 0 ]
then
  echo "assuming default: SMP=$SMP"                                >> $logfile
fi

# act on final result
if [ ! $SMP = 0 ]
then
  smp="-SMP"
  def_smp=-D__SMP__
fi


# ==============================================================
# resolve if we are running a MODVERSIONS enabled kernel

MODVERSIONS=0

if [ $DEMAND_BZIMAGE -gt 0 ]
then

# 1.
# config/modversions.h may contain this: #define CONFIG_MODVERSIONS 1 | #undef  CONFIG_MODVERSIONS
src_file=$linuxincludes/config/modversions.h
if [ ! -e $src_file ];
then
  echo "Warning:"                                                  >> $logfile
  echo "kernel includes at $linuxincludes not found or incomplete" >> $logfile
  echo "file: $src_file"                                           >> $logfile
  echo ""                                                          >> $logfile
else
  if [ 1 -eq 1 ]
  then
    # create a helper source file and preprocess it
    tmp_src_file=tmpsrc.c
	tmp_pre_file=tmppre.pre
    echo "#include <$src_file>"                                      > $tmp_src_file
	${CC} -E -nostdinc -dM -I$linuxincludes $tmp_src_file 			 > $tmp_pre_file

    if [ `cat $tmp_pre_file | grep "1" | grep "#define" | grep "CONFIG_MODVERSIONS" -c` = 1 ]
    then
      MODVERSIONS=`cat $tmp_pre_file | grep CONFIG_MODVERSIONS | cut -d' ' -f3`
      echo "file $src_file says: MODVERSIONS=$MODVERSIONS"           >> $logfile
    fi

    rm -f $tmp_src_file $tmp_pre_file
  else
    if [ `cat $src_file | grep "#undef" | grep "CONFIG_MODVERSIONS" -c` = 0 ]
    then
      MODVERSIONS=`cat $src_file | grep CONFIG_MODVERSIONS | cut -d' ' -f3`
      echo "file $src_file says: MODVERSIONS=$MODVERSIONS"           >> $logfile
    fi
  fi
fi

fi

# 2.
# linux/autoconf.h may contain this: #define CONFIG_MODVERSIONS 1
src_file=$linuxincludes/linux/autoconf.h
if [ ! -e $src_file ];
then
  echo "Warning:"                                                  >> $logfile
  echo "kernel includes at $linuxincludes not found or incomplete" >> $logfile
  echo "file: $src_file"                                           >> $logfile
  echo ""                                                          >> $logfile
else
  if [ `cat $src_file | grep "#undef" | grep "CONFIG_MODVERSIONS" -c` = 0 ]
  then
    MODVERSIONS=`cat $src_file | grep CONFIG_MODVERSIONS | cut -d' ' -f3`
    echo "file $src_file says: MODVERSIONS=$MODVERSIONS"           >> $logfile
  fi
fi

if [ $MODVERSIONS = 0 ]
then
  echo "assuming default: MODVERSIONS=$MODVERSIONS"                >> $logfile
fi

# act on final result
if [ ! $MODVERSIONS = 0 ]
then
  def_modversions="-DMODVERSIONS"
fi


# ==============================================================
# check for required source and lib files

file=${SOURCE_PREFIX}/${FGL_PUBLIC}_public.c
if [ ! -e $file ];
then 
  echo "$file: required file is missing in build directory" | tee -a $logfile
  exit 1
fi
file=${SOURCE_PREFIX}/${FGL_PUBLIC}_public.h
if [ ! -e $file ];
then 
  echo "$file: required file is missing in build directory" | tee -a $logfile
  exit 1
fi

# break down OsRelease string into its components
major=`echo $OsRelease | sed -n -e s/"^\([[:digit:]]*\)\.\([[:digit:]]*\)\.\([[:digit:]]*\)\(.*\)"/"\\1"/p`
minor=`echo $OsRelease | sed -n -e s/"^\([[:digit:]]*\)\.\([[:digit:]]*\)\.\([[:digit:]]*\)\(.*\)"/"\\2"/p`
patch=`echo $OsRelease | sed -n -e s/"^\([[:digit:]]*\)\.\([[:digit:]]*\)\.\([[:digit:]]*\)\(.*\)"/"\\3"/p`
extra=`echo $OsRelease | sed -n -e s/"^\([[:digit:]]*\)\.\([[:digit:]]*\)\.\([[:digit:]]*\)\(.*\)"/"\\4"/p`

if [ "$1" = "verbose" ]
then
  echo OsRelease=$OsRelease  | tee -a $logfile
  echo major=$major          | tee -a $logfile
  echo minor=$minor          | tee -a $logfile
  echo patch=$patch          | tee -a $logfile
  echo extra=$extra          | tee -a $logfile
  echo SMP=$SMP              | tee -a $logfile
  echo smp=$smp              | tee -a $logfile
  echo iii=$iii              | tee -a $logfile
  echo AGP=$AGP              | tee -a $logfile
fi

major_minor=$major.$minor.
major_minor_grep=$major[.]$minor[.]

echo .   >>$logfile

# determine compiler version
cc_version_string=`${CC} -v 2>&1 | grep -v "specs from" -v | grep -v "Thread model" | grep -v "Configured with"`
cc_version=`echo $cc_version_string | sed -e s/egcs-//g | sed -n -e 's/\(^gcc version\)[[:space:]]*\([.0123456789]*\)\(.*\)/\2/'p`
cc_version_major=`echo $cc_version | cut -d'.' -f1`
cc_version_minor=`echo $cc_version | cut -d'.' -f2`

echo CC=${CC} >> $logfile
echo cc_version=$cc_version >> $logfile
if [ "$1" = "verbose" ]
then
    echo CC=${CC}
    echo cc_version=$cc_version
fi

# try to symlink the compiler matching ip-library
lib_ip_base=${LIBIP_PREFIX}/lib${MODULE}_ip.a

# remove existing symlink first
if [ -L $lib_ip_base ];
then
  # remove that symlink to create a new one in next paragraph
  rm -f ${lib_ip_base}
else
  if [ -e $lib_ip_base ];
  then
    echo "Error: the ip-library is present as some file - thats odd!" | tee -a $logfile
    # comment out the below line if you really want to use this local file
    if [ -z "${LIBIP_PREFIX}" ]; then
	    exit 1
    fi
  fi
fi

# if there is no ip-lib file then deterimine which symlink to setup
if [ ! -e $lib_ip_base ];
then
    if [ -e ${lib_ip_base}.GCC$cc_version ];
    then
        # we do have an ip-lib that exactly matches the users compiler
        ln -s ${lib_ip_base}.GCC$cc_version ${lib_ip_base}
        echo "found exact match for ${CC} and the ip-library" >> $logfile
    else
        # there is no exact match for the users compiler
        # try if we just provide a module that matches the compiler major number
        for lib_ip_major in `ls -1 ${lib_ip_base}.GCC$cc_version_major* 2>/dev/null`;
        do
            # just the last matching library does server our purposes - ease of coding
            rm -f ${lib_ip_base}
            ln -s ${lib_ip_major} ${lib_ip_base}
        done
        
        # after the loop there should be a file or a symlink or whatever
        if [ ! -e ${lib_ip_base} ]
        then
            echo "ls -l ${lib_ip_base}*" >>$logfile
            ls -l ${lib_ip_base}* 2>/dev/null >>$logfile
            echo "Error: could not resolve matching ip-library." | tee -a $logfile
            exit 1
        else
            echo "found major but not minor version match for ${CC} and the ip-library" >> $logfile
        fi
    fi
fi

# log a few stats
echo "ls -l ${lib_ip_base}"     >> $logfile
      ls -l ${lib_ip_base}      >> $logfile

# assign result (is not really a variable in current code)
core_lib=${lib_ip_base}

#echo "lib file name was resolved to: $core_lib" >> $logfile
#if [ "$1" = "verbose" ]
#then
#  echo "lib file name was resolved to: $core_lib"
#fi
#if [ ! -e $core_lib ];
#then 
#  echo "required lib file is missing in build directory" | tee -a $logfile
#  exit 1
#fi

echo .  >> $logfile


# ==============================================================
# make clean
echo cleaning... | tee -a $logfile
if [ -e ${FGL_PUBLIC}_public.o ]
then 
  rm -f ${FGL_PUBLIC}_public.o 2>&1 | tee -a $logfile
fi
if [ -e ${MODULE}.o ]
then
  rm -f ${MODULE}.o     2>&1 | tee -a $logfile
fi

if [ -e agpgart_fe.o ]
then 
  rm -f agpgart_fe.o 2>&1 | tee -a $logfile
fi
if [ -e agpgart_be.o ]
then 
  rm -f agpgart_be.o 2>&1 | tee -a $logfile
fi
if [ -e agp3.o ]
then 
  rm -f agp3.o 2>&1 | tee -a $logfile
fi
if [ -e i7505-agp.o ]
then 
  rm -f i7505-agp.o 2>&1 | tee -a $logfile
fi
if [ -e nvidia-agp.o ]
then 
  rm -f nvidia-agp.o 2>&1 | tee -a $logfile
fi

if [ -e patch/linux ]
then
  if [ -e patch/linux/highmem.h ]
  then
    rm -f patch/linux/highmem.h
  fi
  rmdir patch/linux 2>/dev/null
fi

if [ -e patch ]
then
  rmdir patch 2>/dev/null
fi

# ==============================================================
# apply header file patches
# suppress known warning in specific header file
patch_includes=

srcfile=${linuxincludes}/linux/highmem.h
if [ -e ${srcfile} ]
then
  echo "patching 'highmem.h'..." | tee -a $logfile
  mkdir -p patch/include/linux
  cat ${srcfile} | sed -e 's/return kmap(bh/return (char*)kmap(bh/g' >patch/include/linux/highmem.h
  patch_includes="${patch_includes} -Ipatch/include"
fi

# ==============================================================
# defines for all targets
def_for_all="-DATI_AGP_HOOK -DATI -DFGL -D${target_define} -DFGL_CUSTOM_MODULE -DPAGE_ATTR_FIX=$PAGE_ATTR_FIX"

# defines for specific os and cpu platforms
if [ "${uname_m}" = "x86_64" ]; then
	def_machine="-mcmodel=kernel"
fi

if [ "${uname_m}" = "ia64" ]; then
        def_machine="-ffixed-r13 -mfixed-range=f12-f15,f32-f127"
fi

# determine which build system we should use
# note: we do not support development kernel series like the 2.5.xx tree
if [ $major -gt 2 ]; then
    kernel_is_26x=1
else
  if [ $major -eq 2 ]; then
    if [ $minor -gt 5 ]; then
        kernel_is_26x=1
    else
        kernel_is_26x=0
    fi
  else
    kernel_is_26x=0
  fi
fi

if [ $kernel_is_26x -eq 1 ]; then
    kmod_extension=.ko
else
    kmod_extension=.o
fi


# ==============================================================
# resolve if we are running a kernel with the new VMA API 
# that was introduced in linux-2.5.3-pre1
# or with the previous one that at least was valid for linux-2.4.x

if [ $kernel_is_26x -gt 0 ];
then
  echo "assuming new VMA API since we do have kernel 2.6.x..." | tee -a $logfile
  def_vma_api_version=-DFGL_LINUX253P1_VMA_API
  echo "def_vma_api_version=$def_vma_api_version"                   >> $logfile
else
  echo "probing for VMA API version..." | tee -a $logfile
  
  # create a helper source file and try to compile it into an objeckt file
  tmp_src_file=tmp_vmasrc.c
  tmp_obj_file_240=tmp_vma240.o
  tmp_obj_file_253=tmp_vma253.o
  tmp_log_file_240=tmp_vma240.log
  tmp_log_file_253=tmp_vma253.log
  cat > $tmp_src_file <<-begin_end
/* this is a generated file */
#define __KERNEL__
#include <linux/mm.h>

int probe_vma_api_version(void) {
#ifdef FGL_LINUX253P1_VMA_API
  struct vm_area_struct *vma;
#endif
  unsigned long from, to, size;
  pgprot_t prot;
  
  return (
    remap_page_range(
#ifdef FGL_LINUX253P1_VMA_API
      vma,
#endif
      from, to, size, prot)
    );
}
begin_end

  # check for 240 API compatibility
  ${CC} -I$linuxincludes $tmp_src_file                                -c -o $tmp_obj_file_240 &> $tmp_log_file_240
  cc_ret_vma_240=$?
  echo "cc_ret_vma_240 = $cc_ret_vma_240"                           >> $logfile
    
  # check for 253 API compatibility
  ${CC} -I$linuxincludes $tmp_src_file -DFGL_LINUX253P1_VMA_API       -c -o $tmp_obj_file_253 &> $tmp_log_file_253
  cc_ret_vma_253=$?
  echo "cc_ret_vma_253 = $cc_ret_vma_253"                           >> $logfile
    
  # classify and act on results
  # (the check is designed so that exactly one version should succeed and the rest should fail)
  def_vma_api_version=
  if [ $cc_ret_vma_240 -eq 0 ]
  then
    if [ $cc_ret_vma_253 -eq 0 ]
    then
      echo "check results are inconsistent!!!"                      | tee -a $logfile
      echo "exactly one check should work, but not multiple checks."| tee -a $logfile
      echo "aborting module build."                                 | tee -a $logfile
      exit 1
    else
      # the kernel tree does contain the 240 vma api version
      def_vma_api_version=-DFGL_LINUX240_VMA_API
    fi
  else
    if [ $cc_ret_vma_253 -eq 0 ]
    then
      # the kernel tree does contain the 253 vma api version
      def_vma_api_version=-DFGL_LINUX253P1_VMA_API
    else
      echo "check results are inconsistent!!!"                      | tee -a $logfile
      echo "none of the probed versions did succeed."               | tee -a $logfile
      echo "aborting module build."                                 | tee -a $logfile
      exit 1
    fi
  fi
  
  echo "def_vma_api_version=$def_vma_api_version"                   >> $logfile
    
  # cleanup intermediate files
  rm -f $tmp_src_file $tmp_obj_file_240 $tmp_obj_file_253 $tmp_log_file_240 $tmp_log_file_253
fi

# =============================================================
# Check if we're running a kernel version that has the SUSE 9.0 implementation of vmap
#
if [ $kernel_is_26x -gt 0 ]
then
  ## skip for 2.6.x and higher
  echo " Assuming default VMAP API" | tee -a $logfile
else
  echo "Probing for VMAP API version" | tee -a $logfile
  # create a helper source file and try to compile it into an object file
  tmp_src_file=tmp_vmapsrc.c
  tmp_obj_file=tmp_vmap.o
  tmp_log_file=tmp_vmap.log
# let's create the c file 
  cat > $tmp_src_file <<-begin_end
#define __KERNEL__
#include <linux/vmalloc.h>
int probe_vmap_version(void) {
    struct page** pages;
    int count= 0;
    vmap(pages,count);
    return 0;
}
begin_end
 
  # check for vmap API compatibility
  $CC -I$linuxincludes $tmp_src_file -c -o $tmp_obj_file &> $tmp_log_file
  gcc_ret_vmap=$?
    
    
# Check which VMAP API version it is and make define accordingly
  def_vmap_api_version=
  if [ $gcc_ret_vmap -eq 0 ]
    then
      echo "This is the vmap API used for SUSE 9.0 "  | tee -a $logfile
      def_vmap_api_version=-DFGL_LINUX_SUSE90_VMAP_API
    else
      echo "Use default vmap API"                     | tee -a $logfile
  fi
  
  # cleanup intermediate files
  rm -f $tmp_src_file $tmp_obj_file $tmp_log_file 
fi


# ==============================================================
# make agp kernel module (including object files) and check results

if [ $kernel_is_26x -gt 0 ]; then
    echo "doing Makefile based build for kernel 2.6.x and higher"   | tee -a $logfile
    cd 2.6.x
    V=${V:-0}
    #tlog is a temporary file that will be deleted
    make CC=${CC} V=${V} MODFLAGS="-DMODULE $def_for_all $def_smp $def_modversions" PAGE_ATTR_FIX=$PAGE_ATTR_FIX > tlog 2>&1 
    res=$?
    tee -a $logfile < tlog
    #delete tlog
    rm -f tlog
    cd ..
    if [ $res -eq 0 ]; then
        echo "build succeeded with return value $res"               | tee -a $logfile
    else
        echo "build failed with return value $res"                  | tee -a $logfile
        exit 1
    fi
    if [ -e ${MODULE}${kmod_extension} ]; then
        rm -f ${MODULE}${kmod_extension}
    fi
    ln -s 2.6.x/${MODULE}${kmod_extension}
    TERMINAL_HINT=0


#	# make fglrx_agp.ko
#    echo " compiling fglrx_agp.ko module"
#    cd firegl_agpgart
#    V=${V:-0}
#    #tlog is a tempory file that will be deleted
#    make V=${V} MODFLAGS="-DMODULE $def_for_all $def_smp $def_modversions" PAGE_ATTR_FIX=$PAGE_ATTR_FIX > tlog 2>&1 
#    res=$?
#    tee -a $logfile < tlog
#    #delete tlog
#    rm -f tlog
#    cd ..
#    if [ $res -eq 0 ]; then
#        echo "AGPGART build succeeded with return value $res"               | tee -a $logfile
#	    if [ -e ${MODULE_agpgart}${kmod_extension} ]; then
#	         rm -f ${MODULE_agpgart}${kmod_extension}
#	    fi
#	    echo " finished compiling for fglrx_agp"
#	    ln -s firegl_agpgart/${MODULE_agpgart}${kmod_extension}
#    else
#        echo "AGPGART module build failed with return value $res"                  | tee -a $logfile
#    fi
else
    echo "doing script based build for kernel 2.4.x and similar"    | tee -a $logfile
    
WARNINGS="-Wall -Wwrite-strings -Wpointer-arith -Wcast-align -Wstrict-prototypes"
if [ $cc_version_major -ge 3 ];
then
  if [ $cc_version_major -eq 3 ];
  then
    if [ $cc_version_minor -ge 3 ];
    then
      # gcc 3.3 or higher is too verbose for us when using the -Wall option
      WARNINGS="-Wwrite-strings -Wpointer-arith -Wcast-align -Wstrict-prototypes"
    fi
  else
    # gcc 3.3 or higher is too verbose for us when using the -Wall option
    WARNINGS="-Wwrite-strings -Wpointer-arith -Wcast-align -Wstrict-prototypes"
  fi
fi

#SRC=${SOURCE_PREFIX}/agpgart_fe.c
#DST=agpgart_fe.o
#echo "compiling '$SRC'..." | tee -a $logfile
#cc_cmd="${CC} ${WARNINGS} -O2 -D__KERNEL__ -DMODULE -fomit-frame-pointer $def_for_all -D$MODULE $def_smp $def_modversions $patch_includes -I$linuxincludes -c $SRC -o $DST"
#echo "$cc_cmd" >> $logfile
#$cc_cmd 2>&1 | tee -a $logfile
#if [ ! -e $DST ] ;
#then
#  echo "compiling failed - object file was not generated" | tee -a $logfile
#  exit 1
#fi

SRC=${SOURCE_PREFIX}/agpgart_be.c
DST=agpgart_be.o
echo "compiling '$SRC'..." | tee -a $logfile
cc_cmd="${CC} ${WARNINGS} -O2 -D__KERNEL__ -DMODULE -fomit-frame-pointer $def_for_all $def_machine -D$MODULE $def_smp $def_modversions $patch_includes -I$linuxincludes -c $SRC -o $DST"
echo "$cc_cmd" >> $logfile
$cc_cmd 2>&1 | tee -a $logfile
if [ ! -e $DST ] ;
then
  echo "compiling failed - object file was not generated" | tee -a $logfile
  exit 1
fi

SRC=${SOURCE_PREFIX}/agp3.c
DST=agp3.o
echo "compiling '$SRC'..." | tee -a $logfile
cc_cmd="${CC} ${WARNINGS} -O2 -D__KERNEL__ -DMODULE -fomit-frame-pointer $def_for_all $def_machine -D$MODULE $def_smp $def_modversions $patch_includes -I$linuxincludes -c $SRC -o $DST"
echo "$cc_cmd" >> $logfile
$cc_cmd 2>&1 | tee -a $logfile
if [ ! -e $DST ] ;
then
  echo "compiling failed - object file was not generated" | tee -a $logfile
  exit 1
fi

SRC=${SOURCE_PREFIX}/i7505-agp.c
DST=i7505-agp.o
echo "compiling '$SRC'..." | tee -a $logfile
cc_cmd="${CC} ${WARNINGS} -O2 -D__KERNEL__ -DMODULE -fomit-frame-pointer $def_for_all $def_machine -D$MODULE $def_smp $def_modversions $patch_includes -I$linuxincludes -c $SRC -o $DST"
echo "$cc_cmd" >> $logfile
$cc_cmd 2>&1 | tee -a $logfile
if [ ! -e $DST ] ;
then
  echo "compiling failed - object file was not generated" | tee -a $logfile
  exit 1
fi

SRC=${SOURCE_PREFIX}/nvidia-agp.c
DST=nvidia-agp.o
echo "compiling '$SRC'..." | tee -a $logfile
cc_cmd="${CC} ${WARNINGS} -O2 -D__KERNEL__ -DMODULE -fomit-frame-pointer $def_for_all $def_machine -D$MODULE $def_smp $def_modversions $patch_includes -I$linuxincludes -c $SRC -o $DST"
echo "$cc_cmd" >> $logfile
$cc_cmd 2>&1 | tee -a $logfile
if [ ! -e $DST ] ;
then
  echo "compiling failed - object file was not generated" | tee -a $logfile
  exit 1
fi

# we don't need the agpgart module - skip that thing
# echo "linking agp module..." | tee -a $logfile
# ld="ld -r agpgart_fe.po agpgart_be.po -o agpgart.o"
# echo "$ld" >> $logfile
# $ld 2>&1 | tee -a $logfile
# if [ ! -e ${MODULE}.o ] ;
# then
#  echo "linking failed - kernel module was not generated" | tee -a $logfile
#  exit 1
# fi
#
# echo .  >> $logfile


# ==============================================================
# make custom kernel module and check results

SRC=${SOURCE_PREFIX}/${FGL_PUBLIC}_public.c
DST=${FGL_PUBLIC}_public.o
echo "compiling '$SRC'..." | tee -a $logfile
cc_cmd="${CC} ${WARNINGS} -O2 -D__KERNEL__ -DMODULE -fomit-frame-pointer $def_for_all $def_machine -D${MODULE} $def_vma_api_version $def_vmap_api_version $def_smp $def_modversions $def_agp $patch_includes -I$linuxincludes -I$PWD -c $SRC -o $DST"
echo "$cc_cmd" >> $logfile
$cc_cmd 2>&1 | tee -a $logfile | grep -v "warning: pasting would not give a valid preprocessing token"
if [ ! -e $DST ] ;
then
  echo "compiling failed - object file was not generated" | tee -a $logfile
  exit 1
fi

echo "linking of ${MODULE} kernel module..." | tee -a $logfile
if [ ! -z "${MODULE_NAME}" ]; then 
	module_version=.${MODULE_NAME}
fi
ld="ld -r ${FGL_PUBLIC}_public.o agpgart_be.o agp3.o i7505-agp.o nvidia-agp.o $core_lib -o ${MODULE}${module_version}.o"
echo "$ld" >> $logfile
$ld 2>&1 | tee -a $logfile
if [ ! -e ${MODULE}${module_version}.o ] ;
then
  echo "linking failed - kernel module was not generated" | tee -a $logfile
  exit 1
fi

# end of `else` for $kernel_is_26x
fi

echo .  >> $logfile


# ==============================================================
# install generated file at required location

TERMINAL_HINT=0
if [ `pwd | grep "$OS_MOD/${MODULE}/build_mod\$" -c` -gt 0 ]
then 
  echo duplicating results into driver repository...   | tee -a $logfile
  # duplicate generated file into respective kernel module subdir
  if [ $INSTALL_MODULE_SUBDIRS -eq 1 ];
  then
    target_dir=`pwd`/../$OsRelease$iii$smp
  else
    target_dir=`pwd`/..
  fi
  target_dir=`cd $target_dir;pwd`
  echo "target location: $target_dir" >> $logfile 
  if [ ! -e $target_dir ]
  then
    echo "creating target directory" >> $logfile
    mkdir $target_dir | tee -a $logfile
  fi

  which strip > /dev/null 2>&1
  if test $? = 0; then
     strip -g ${MODULE}${kmod_extension} > /dev/null 2>&1
     if test $? = 0; then	
        echo "stripping the debug info of kernel module" >> $logfile
     else
        echo "could not strip the debug info of kernel module" >> $logfile
     fi 
  else
     echo "could not find the strip utility on your system" >> $logfile
  fi

  # for fglrx and fglrx_agp
  echo "copying ${MODULE}${kmod_extension}" >> $logfile
  if [ $INSTALL_MODULE_SUBDIRS -eq 1 ];
  then
    cp -f ${MODULE}${kmod_extension} $target_dir | tee -a $logfile
  else
    cp -f ${MODULE}${kmod_extension} $target_dir/${MODULE}.$OsRelease$iii${kmod_extension} | tee -a $logfile
  fi
  
  echo "copying logfile of build" >> $logfile
  echo "*** end of build log ***" >> $logfile
  if [ $INSTALL_MODULE_SUBDIRS -eq 1 ];
  then
    cp -f $logfile $target_dir
  else
    cp -f $logfile $target_dir/make.$OsRelease$iii.log
  fi

  # terminal hint message
  if [ $INSTALL_MODULE_SUBDIRS -eq 0 ];
  then
    TERMINAL_HINT=1
  fi
else
  # the build was done from an external location - installation not intended
  echo "duplication skipped - generator was not called from regular lib tree" | tee -a $logfile 
fi

# ==============================================================
# finale

echo done.
echo ==============================

if [ $OPTIONS_HINTS -ne 0 ]; then

if [ $TERMINAL_HINT -eq 1 ];
then
  echo "You must change your working directory to $target_dir"
  echo "and then call ./make_install.sh in order to install the built module."
  echo ==============================
fi

fi

#EOF
