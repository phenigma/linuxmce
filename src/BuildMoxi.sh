#!/bin/bash

set -e

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

## MessageSend
pushd MessageSend
make -f Makefile.Moxi clean
make -f Makefile.Moxi
popd
