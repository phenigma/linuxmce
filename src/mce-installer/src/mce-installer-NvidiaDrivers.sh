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
	dpkg -i /media/cdrom/cachecd1-cache/nvidia-glx_*.deb && nvidia_glx_installed="true"
fi

if [[ "$nvidia_glx_installed" == "false" ]] ;then
	apt-get -f -y install nvidia-glx && nvidia_glx_installed="true"
fi

if [[ "$(aptitude show linux-restricted-modules-generic | grep State | cut -d ' ' -f2)" == "installed" ]] ;then
	linux_restricted_modules_generic_installed="true"
fi

if [[ "$linux_restricted_modules_generic_installed" == "false" ]] ;then
	apt-get -f -y install linux-restricted-modules-generic && linux_restricted_modules_generic_installed="true"
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
