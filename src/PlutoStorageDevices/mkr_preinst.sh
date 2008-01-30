#!/bin/bash

dpkg-divert --add --rename --package pluto-storage-devices --divert /bin/umount.wraped /bin/umount
