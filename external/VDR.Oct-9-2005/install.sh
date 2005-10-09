#!/bin/bash

# dirty installing all *.debs :

dpkg -i vdrdevel_1.*.deb
pushd plugins
dpkg -i vdrdevel-plugin-*.deb
popd
