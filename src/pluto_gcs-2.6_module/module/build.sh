#!/bin/bash
make -C /usr/src/kernel-source-2.6.8/ SUBDIRS=$(pwd) modules
