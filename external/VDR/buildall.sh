#!/bin/bash

# vdrdevel
pushd vdrdevel-*
dpkg-buildpackage -tc
popd

# mplayer
pushd vdrdevel-plugin-mp3-*
dpkg-buildpackage -tc
popd

# streamdev server
pushd vdrdevel-plugin-streamdev-*
dpkg-buildpackage -tc
popd

# text2skin server
pushd vdrdevel-plugin-text2skin-*
dpkg-buildpackage -tc
popd

# tvonscreen
pushd vdrdevel-plugin-tvonscreen-*
dpkg-buildpackage -tc
popd

# xine pluto powered!
# got to fix 00_patches
pushd vdrdevel-plugin-xine-*
dpkg-buildpackage -tc
popd
