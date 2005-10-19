#!/bin/bash
mkdir -p /home/mirrors/replacements/main/binary-i386
cd /home/mirrors/replacements/main/binary-i386
rm -f Packages.gz
rm -f kernel-image-2.6.8-2-686_2.6.8-12_i386.deb
rm -f lirc-2.6.10-1-686_0.7.1+0.7.0-2_i386.deb
rm -f lirc_0.7.1pre2-2_i386.deb
rm -f msttcorefonts_1.2.pluto.3_all.deb
# add Packages.gz
# add libxine1_1.0.2-0vdr1+net1_i386.deb
# add lirc-pluto_0.7.2r+1modules2.6.13.2-5_i386.deb
# add msttcorefonts_1.2.pluto.4_all.deb
# add vdr-addon-noad_0.5.2pluto1-1_i386.deb
# add vdrdevel-addon-noad_0.5.2+1_all.deb
# add vdrdevel-channellogos_0.0.3-2_all.deb
# add vdrdevel-dev_1.3.34-1_all.deb
# add vdrdevel-plugin-mp3_0.9.13-4_i386.deb
# add vdrdevel-plugin-mplayer_0.9.13-4_i386.deb
# add vdrdevel-plugin-osdteletext_0.5.1-4_i386.deb
# add vdrdevel-plugin-streamdev-client_0.3.1+cvs20050522-9_i386.deb
# add vdrdevel-plugin-streamdev-server_0.3.1+cvs20050522-9_i386.deb
# add vdrdevel-plugin-text2skin_1.0+cvs20050618-8_i386.deb
# add vdrdevel-plugin-tvonscreen_0.7.0-21_i386.deb
# add vdrdevel-plugin-xine_0.7.6-1net3_i386.deb
# add vdrdevel-skin-elchi_0.0.0+0.0.1pre6a+1.0rc1-1_all.deb
# add vdrdevel-xpmlogos_0.0.1-2_all.deb
# add vdrdevel_1.3.34-1_i386.deb
tar -xzvf /home/uploads/replacements.tar.gz
