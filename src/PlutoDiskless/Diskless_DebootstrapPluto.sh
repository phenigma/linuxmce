mirror_style release
download_style apt

work_out_debs () {

    required="base-files base-passwd bash bsdutils coreutils libacl1 libattr1 debconf debconf-i18n liblocale-gettext-perl libtext-wrapi18n-perl libtext-charwidth-perl debianutils diff libselinux1 libsepol1 dpkg dselect libblkid1 e2fsprogs e2fslibs libcomerr2 libss2 libuuid1 findutils grep gzip hostname libcap1 libc6 libdb1-compat libdb3 libncurses5 libnewt0.51 libpam-modules libpam-runtime libpam0g libperl5.8 libpopt0 login makedev mawk modutils mount ncurses-base ncurses-bin passwd perl-base procps sed slang1a-utf8 initscripts make sysvinit sysv-rc tar util-linux whiptail libgcc1 gcc-3.3-base libstdc++5"

    base="adduser apt apt-utils libdb4.2 libdb4.3 at base-config aptitude libsigc++-1.2-5c102 bsdmainutils console-common console-tools libconsole console-data cpio cron dhcp-client ed exim4 exim4-base exim4-config exim4-daemon-light libgcrypt11 libgnutls11 libgpg-error0 libopencdk8 libtasn1-2 fdutils gettext-base groff-base ifupdown info klogd libssl0.9.7 liblzo1 zlib1g liblockfile1 libpcre3 libwrap0 logrotate mailx man-db libgdbm3 manpages nano net-tools netbase netkit-inetd iputils-ping nvi ppp pppconfig pppoe pppoeconf libpcap0.7 sysklogd tasksel libtextwrap1 tcpd telnet libtext-iconv-perl wget $additional"

    without_package () {
        echo "$2" | tr ' ' '\n' | grep -v "^$1$" | tr '\n' ' '
    }
    subst_package () {
        echo "$3" | tr ' ' '\n' | sed "s/^$1$/$2/" | tr '\n' ' '
    }

    LIBC6=libc6
    IPFWTOOL=iptables

    case $ARCH in
        "alpha")
            required="$(subst_package "libc6" "libc6.1" "$required")"
            base="$base pciutils"
            LIBC6="libc6.1"
	    IPFWTOOL="ipchains"
            ;;
        "arm")
            base="$base libgpmg1"
	    IPFWTOOL="ipchains iptables"
            ;;
        "i386")
	    required="$(without_package "libperl5.8" "$required")"
            base="$base pciutils psmisc"
	    IPFWTOOL="ipchains iptables"
            ;;
        "ia64")
            required="$(subst_package "libc6" "libc6.1" "$required") libreadline4"
            base="$base elilo efibootmgr dosfstools libparted1.6-12 parted"
            LIBC6="libc6.1"
            ;;
        "m68k")
            required="$required libreadline4"
            required="$required libreadline4"
            base="$base atari-bootstrap atari-fdisk amiga-fdisk eject mac-fdisk pmac-fdisk-cross vmelilo"
	    IPFWTOOL="ipchains"
            ;;
        "powerpc")
            required="$required libreadline4"
            base="$base mac-fdisk amiga-fdisk psmisc powerpc-utils pciutils hfsutils"
	    IPFWTOOL="ipchains iptables"
            ;;
        "sparc")
            base="$base silo eject pciutils sparc-utils"
	    IPFWTOOL="ipchains iptables"
            ;;
        "mips")
            base="$base dvhtool pciutils"
            ;;
        "mipsel")
            base="$base pciutils"
            ;;
        "hppa")
            base="$base palo pciutils"
            required="$(without_package "libstdc++2.10-glibc2.2" "$required")"
            ;;
        s390|s390x)
            # base-config depends on console-tools and console-data
            # so we can't exclude them although they are useless on s390
            base="$base s390-tools telnetd devfsd"
            base="$(without_package "console-tools-libs" "$base")"
            base="$(without_package "fdutils" "$base")"
            base="$(without_package "ppp" "$base")"
            base="$(without_package "pppconfig" "$base")"
            base="$(without_package "pppoe" "$base")"
            base="$(without_package "pppoeconf" "$base")"
            ;;
	sh*)
	    base="$base libstdc++3 gcc-3.0-base"
	    base="$(without_package "fdutils" "$base")"
	    required="$required libgcc1"
	    required="$(without_package "libstdc++2.10-glibc2.2" "$required")"
	    ;;
        "amd64")
            required="$required libtext-iconv-perl zlib1g"
            base="$base pciutils psmisc"
	    IPFWTOOL="iptables"
            ;;
        *)
            # who knows?
            ;;
    esac

    base="$base $IPFWTOOL"

    all_debs="$required $base"

    for x in $exclude; do
      all_debs="$(without_package "$x" "$all_debs")"
    done
    
}

install_debs () {
    first_stage_install
    second_stage_install
}

first_stage_install () {
    extract $required

    mkdir -p "$TARGET/var/lib/dpkg"
    : >"$TARGET/var/lib/dpkg/status"
    : >"$TARGET/var/lib/dpkg/available"

    setup_etc
    if [ ! -e "$TARGET/etc/fstab" ]; then
        echo '# UNCONFIGURED FSTAB FOR BASE SYSTEM' > "$TARGET/etc/fstab"
        chown 0.0 "$TARGET/etc/fstab"; chmod 644 "$TARGET/etc/fstab"
    fi

    setup_proc
    setup_devices

    in_target /sbin/ldconfig

    p () {
	baseprog="$(($baseprog + ${1:-1}))"
    }

    x_feign_install () {
        local pkg="$1"
        local deb="$(debfor $pkg)"
        local ver="$(
            ar -p "$TARGET/$deb" control.tar.gz | zcat |
                tar -O -xf - control ./control 2>/dev/null |
                sed -ne 's/^Version: *//Ip' | head -n 1
        )"

        mkdir -p "$TARGET/var/lib/dpkg/info"

        echo \
"Package: $pkg
Version: $ver
Status: install ok installed" >> "$TARGET/var/lib/dpkg/status"

        touch "$TARGET/var/lib/dpkg/info/${pkg}.list"
    }

    x_feign_install dpkg
}

second_stage_install () {
    x_core_install () {
	smallyes '' | in_target dpkg --force-depends --install $(debfor "$@")
    }

    DEBIAN_FRONTEND=noninteractive
    export DEBIAN_FRONTEND

    baseprog=0
    bases=40

    p; progress $baseprog $bases INSTBASE "Installing base system" #1
    info INSTCORE "Installing core packages..."

    p; progress $baseprog $bases INSTBASE "Installing base system" #2
    rm -f "$TARGET/usr/bin/awk"
    ln -s mawk "$TARGET/usr/bin/awk"
    x_core_install base-files base-passwd
    p; progress $baseprog $bases INSTBASE "Installing base system" #3
    x_core_install dpkg

    if [ ! -e "$TARGET/etc/localtime" ]; then
        ln -sf /usr/share/zoneinfo/UTC "$TARGET/etc/localtime"
    fi

    p; progress $baseprog $bases INSTBASE "Installing base system" #4
    x_core_install $LIBC6

    p; progress $baseprog $bases INSTBASE "Installing base system" #5
    x_core_install perl-base
    p; progress $baseprog $bases INSTBASE "Installing base system" #6
    rm $TARGET/usr/bin/awk
    x_core_install mawk
    p; progress $baseprog $bases INSTBASE "Installing base system" #7
    x_core_install debconf

    info UNPACKREQ "Unpacking required packages..."

    p; progress $baseprog $bases INSTBASE "Installing base system" #8
    smallyes '' | repeat 5 in_target_failmsg UNPACK_REQ_FAIL_FIVE "Failure while unpacking required packages.  This will be attempted up to five times." "" dpkg --force-depends --unpack $(debfor $required)
    p 10; progress $baseprog $bases INSTBASE "Installing base system" #18

    info CONFREQ "Configuring required packages..."

    mv "$TARGET/sbin/start-stop-daemon" "$TARGET/sbin/start-stop-daemon.REAL"
    echo \
"#!/bin/sh
echo
echo \"Warning: Fake start-stop-daemon called, doing nothing\"" > "$TARGET/sbin/start-stop-daemon"
    chmod 755 "$TARGET/sbin/start-stop-daemon"

    setup_dselect_method apt
    #on_exit "(in_target_nofail umount /dev/pts) || true"

    p; progress $baseprog $bases INSTBASE "Installing base system" #19
    smallyes '' | in_target_failmsg CONF_REQ_FAIL "Failure while configuring required packages." "" dpkg --configure --pending --force-configure-any --force-depends
    p 10; progress $baseprog $bases INSTBASE "Installing base system" #29 

#    if [ ! -e "$TARGET/etc/exim/exim.conf" ]; then
#        mkdir -p "$TARGET/etc/exim"
#        touch "$TARGET/etc/exim/exim.conf"
#    fi

    info INSTBASE "Installing base packages..."

    p; progress $baseprog $bases INSTBASE "Installing base system" #30
    smallyes '' | repeat 5 in_target_failmsg INST_BASE_FAIL_FIVE "Failure while installing base packages.  This will be re-attempted up to five times." "" dpkg --force-auto-select --force-overwrite --force-confold --skip-same-version --unpack $(debfor $base)

    rm -f "$TARGET/usr/sbin/sendmail"
    ln -s /bin/true "$TARGET/usr/sbin/sendmail"

    smallyes '' | repeat 5 in_target_failmsg CONF_BASE_FAIL_FIVE "Failure while configuring base packages.  This will be attempted 5 times." "" dpkg  --force-confold --skip-same-version  --configure -a

    rm -f "$TARGET/usr/sbin/sendmail"
    ln -s exim4 "$TARGET/usr/sbin/sendmail"

    p 9; progress $baseprog $bases INSTBASE "Installing base system" #39

    mv "$TARGET/sbin/start-stop-daemon.REAL" "$TARGET/sbin/start-stop-daemon"

    progress $bases $bases INSTBASE "Installing base system" #40
    info BASESUCCESS "Base system installed successfully."
}
