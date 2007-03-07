#!/bin/bash

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
