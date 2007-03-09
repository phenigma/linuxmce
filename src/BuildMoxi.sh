#!/bin/bash

set -e
export SNR_CPPFLAGS="-DMOXI"

## SerializeClass
pushd SerializeClass
make -f Makefile.Moxi clean
make -f Makefile.Moxi
popd

## DCE
pushd DCE
make -f Makefile.Moxi clean
make -f Makefile.Moxi
popd

## PlutoUtils
pushd PlutoUtils
make -f Makefile.Moxi clean
make -f Makefile.Moxi
popd

## MessageSend
pushd MessageSend
make -f Makefile.Moxi clean
make -f Makefile.Moxi
popd

sudo cp lib/* /export/obj/via-mc.limonite-dev/apps/aaa/build/mc/lib
sudo cp bin/* /export/obj/via-mc.limonite-dev/apps/aaa/build/mc/bin
