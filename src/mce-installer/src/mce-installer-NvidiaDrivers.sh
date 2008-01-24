#!/bin/bash

. ./mce-installer-common.sh

#Setup_Apt_Conffiles
if [[ ! -f /etc/X11/xorg.conf.pbackup ]] ;then
	cp /etc/X11/xorg.conf /etc/X11/xorg.conf.pbackup
fi

nvidia_glx_installed="false"
linux_restricted_modules_generic_installed="false"

mount /dev/cdrom /media/cdrom || :

if [[ "$(aptitude show nvidia-glx | grep State | cut -d ' ' -f2)" == "installed" ]] ;then
	nvidia_glx_installed="true"
fi

if [[ "$nvidia_glx_installed" == "false" ]] ;then
	dpkg -i /media/cdrom/deb-cache/nvidia-glx_*.deb && nvidia_glx_installed="true"
fi

if [[ "$nvidia_glx_installed" == "false" ]] ;then
	pushd /tmp
		if [[ "$(uname -m)" == "i686" ]] ;then
			wget http://deb.linuxmce.org/install/nvidia-glx_1.0.9639+2.6.22.4-14.9+lmce3_i386.deb
			dpkg -i nvidia-glx_1.0.9639+2.6.22.4-14.9+lmce3_i386.deb && nvidia_glx_installed="true"
		else
			wget http://deb.linuxmce.org/install/nvidia-glx_1.0.9639+2.6.22.4-14.10+lmce3_amd64.deb
			dpkg -i nvidia-glx_1.0.9639+2.6.22.4-14.10+lmce3_amd64.deb && nvidia_glx_installed="true"
		fi
	popd
fi

if [[ "$(aptitude show linux-restricted-modules-generic | grep State | cut -d ' ' -f2)" == "installed" ]] ;then
	linux_restricted_modules_generic_installed="true"
fi

if [[ "$linux_restricted_modules_generic_installed" == "false" ]] ;then
	if [[ "$(uname -m)" == "i686" ]] ;then
		wget http://deb.linuxmce.org/install/linux-restricted-modules-2.6.22-14-generic_2.6.22.4-14.9+lmce3_i386.deb
		dpkg -i linux-restricted-modules-2.6.22-14-generic_2.6.22.4-14.9+lmce3_i386.deb && linux_restricted_modules_generic_installed="true"
	else
		wget http://deb.linuxmce.org/install/linux-restricted-modules-2.6.22-14-generic_2.6.22.4-14.10+lmce3_amd64.deb
		dpkg -i linux-restricted-modules-2.6.22-14-generic_2.6.22.4-14.10+lmce3_amd64.deb && linux_restricted_modules_generic_installed="true"
	fi
fi

#if [[ "$linux_restricted_modules_generic_installed" == "false" ]] ;then
#	dpkg -i 
#fi

if [[ "$linux_restricted_modules_generic_installed" == "false" || "$nvidia_glx_installed" == "false" ]] ;then
	echo "ERROR: Some of the pacakges could not be installed"
	exit 1
fi

#apt-get install nvidia-glx
#apt-get install linux-restricted-modules-generic
sed -i 's/Driver.*"nv"/Driver "nvidia"/gi' /etc/X11/xorg.conf
nvidia-xconfig --no-use-edid
nvidia-xconfig --exact-mode-timings-dvi
nvidia-xconfig --no-logo
nvidia-xconfig --no-bandwidth-test
nvidia-xconfig --no-dynamic-twinview
nvidia-xconfig --xvmc-uses-textures
nvidia-xconfig --allow-glx-with-composite
nvidia-xconfig --add-argb-glx-visuals
nvidia-xconfig --render-accel

awk '
BEGIN { Section = ""; }
/Section.*"Screen"/ { Section = "Screen"; print; next; }
Section == "Screen" && /Option..*"ModeValidation"/ { next; }
Section == "Screen" && /EndSection/ {
	Section = "";
	print "\tOption \"ModeValidation\" \"NoDFPNativeResolutionCheck, NoEdidMaxPClkCheck, NoMaxPClkCheck\"";
	print;
	next;
}
{ print; }
' /etc/X11/xorg.conf >/etc/X11/xorg.conf.$$
mv /etc/X11/xorg.conf{.$$,}

reboot
