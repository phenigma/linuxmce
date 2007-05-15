#!/bin/bash

. /tmp/mce_wizard_data.sh
. ./mce-installer-common.sh

#Setup_Apt_Conffiles
if [[ ! -f /etc/X11/xorg.conf.pbackup ]] ;then
	cp /etc/X11/xorg.conf /etc/X11/xorg.conf.pbackup
fi

apt-get install nvidia-glx
apt-get install linux-restricted-modules-generic
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
