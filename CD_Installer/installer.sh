#!/bin/bash

required="base-files base-passwd bash bsdutils coreutils libacl1 libattr1 debconf-i18n debconf liblocale-gettext-perl libtext-wrapi18n-perl libtext-charwidth-perl debianutils diff dpkg dselect libblkid1 e2fsprogs e2fslibs libcomerr2 libss2 libuuid1 findutils grep gzip hostname libcap1 libc6 libdb1-compat libdb3 libncurses5 libnewt0.51 libpam-modules libpam-runtime libpam0g libperl5.8 libpopt0 login makedev mawk modutils mount ncurses-base ncurses-bin passwd perl-base procps sed slang1a-utf8 initscripts sysvinit sysv-rc tar util-linux whiptail libgcc1 gcc-3.3-base libstdc++5 zlib1g libtext-iconv-perl"

base="adduser apt apt-utils libdb4.2 at base-config aptitude libsigc++-1.2-5c102 bsdmainutils console-common console-tools libconsole console-data cpio cron dhcp-client ed exim4 exim4-base exim4-config exim4-daemon-light libgcrypt11 libgnutls10 libgnutls11 libgcrypt7 libgpg-error0 libopencdk8 libtasn1-2 fdutils gettext-base groff-base ifupdown info klogd libssl0.9.7 liblzo1 liblockfile1 libpcre3 libwrap0 logrotate mailx man-db libgdbm3 manpages nano net-tools netbase netkit-inetd iputils-ping nvi ppp pppconfig pppoe pppoeconf libpcap0.7 sysklogd tasksel libtextwrap1 tcpd telnet wget libdevmapper1.00 lilo fileutils kernel-image-2.4.22 $additional"

debfor()
{
	PREFIX="$1"
	if [ "$PREFIX" == "/" ]; then
		CHROOT=1
	fi
	shift
	for i in "$@"; do
		if [ -z "$CHROOT" ]; then
			file=$(bash -c "ls "$PREFIX"/Packages_Base/${i}_*.deb")
		else
			file=$(chroot /mnt bash -c "ls "$PREFIX"/Packages_Base/${i}_*.deb")
		fi
		file_list="$file_list $file"
	done
	echo "$file_list"
	echo "debfor: File list: $file_list" >&2
}

install_dpkg()
{
	echo "Phase 1"
	for i in $(debfor /mnt $required); do
		(cd /mnt; ar -p $i data.tar.gz | tar -xzf -) || exit
	done

	mkdir -p /mnt/var/lib/dpkg
	: > /mnt/var/lib/dpkg/status
	: > /mnt/var/lib/dpkg/available

	mkdir -p /mnt/etc
	cp /etc/resolv.conf /mnt/etc
	cp /etc/hostname /mnt/etc
	mkdir -p /mnt/etc/apt
	
	echo "# Not configured" > /mnt/etc/fstab
	chown 0.0 /mnt/etc/fstab
	chmod 644 /mnt/etc/fstab
	
	mkdir -p /mnt/proc
	chroot /mnt /bin/mount -t proc proc /proc
	(cd /mnt; tar -xzf /usr/lib/debootstrap/devices.tar.gz)
	chroot /mnt /sbin/ldconfig
	mkdir -p /mnt/var/lib/dpkg/info
	
	local version="$(
		ar -p "/mnt/Packages_Base/dpkg_1.10.23_i386.deb" control.tar.gz |
		tar -O -xzf - control ./control 2>/dev/null |
		sed -ne 's/^Version: *//Ip' | head -n 1
	)"
	echo \
"Package: dpkg
Version: $version
Status: install ok installed" >> "/mnt/var/lib/dpkg/status"
	touch /mnt/var/lib/dpkg/info/dpkg.list
}

small_yes()
{
	YES="${1-y}"
	while echo "$YES" ; do : ; done
}

force_install()
{
	for i in $(debfor / "$@"); do
		small_yes '' | chroot /mnt dpkg --force-depends --install $i
	done
}

install_rest()
{
	echo "Phase 2"
	export DEBIAN_FRONTEND=noninteractive
	ln -s mawk /mnt/usr/bin/awk

	force_install base-passwd base-files
	force_install dpkg

    if [ ! -e "/mnt/etc/localtime" ]; then
		ln -sf /usr/share/zoneinfo/UTC "/mnt/etc/localtime"
	fi

	force_install libc6
	force_install perl-base
	rm /mnt/usr/bin/awk
	force_install mawk
	force_install debconf

	small_yes '' | chroot /mnt dpkg --force-depends --unpack $(debfor / $required)

	mv "/mnt/sbin/start-stop-daemon" "/mnt/sbin/start-stop-daemon.REAL"
	echo \
"#!/bin/sh
echo
echo \"Warning: Fake start-stop-daemon called, doing nothing\"" > "/mnt/sbin/start-stop-daemon"
	chmod 755 "/mnt/sbin/start-stop-daemon"

	mkdir -p /mnt/var/lib/dpkg
	echo "apt apt" > "/mnt/var/lib/dpkg/cmethopt"
	chmod 644 "/mnt/var/lib/dpkg/cmethopt"
	
	chroot /mnt dpkg --configure --pending --force-configure-any --force-depends
	small_yes '' | chroot /mnt dpkg --force-auto-select --force-overwrite --force-confold --skip-same-version --unpack $(debfor / $base)

	rm -f "/mnt/usr/sbin/sendmail"
	ln -s /bin/true "/mnt/usr/sbin/sendmail"
	small_yes '' | chroot /mnt dpkg --force-confold --skip-same-version  --configure -a

	rm -f "/mnt/usr/sbin/sendmail"
	ln -s exim4 "/mnt/usr/sbin/sendmail"
}

install_extra()
{
	Files=$(chroot /mnt bash -c "ls /Packages_Extra/*.deb")
	chroot /mnt dpkg -i $Files
}

install_pluto()
{
	Files=$(chroot /mnt bash -c "ls /Packages_Pluto/*.deb")
	chroot /mnt dpkg -i $Files
}

# should we delete these after installing?
cp -a /cdrom/Packages_Base /mnt
cp -a /cdrom/Packages_Extra /mnt
cp -a /cdrom/Packages_Pluto /mnt

echo "Installing base system"
install_dpkg
install_rest

echo "Installing extra packages"
install_extra

echo "Installing Pluto packages"
install_pluto

mv "/mnt/sbin/start-stop-daemon.REAL" "/mnt/sbin/start-stop-daemon"

if [ -e "/mnt/etc/apt/sources.list" ]; then
	rm -f "/mnt/etc/apt/sources.list"
fi

