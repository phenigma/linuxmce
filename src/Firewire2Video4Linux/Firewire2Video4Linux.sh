#!/bin/bash

# This script is creating some v4l pipes that are to be used by
# Motion_Wrapper for pipeing video1394 content into them.

PipeDevOffset=10
PipeCount=8

modprobe video1394
modprobe raw1394
modprobe vloopback pipes=$PipeCount dev_offset=$PipeDevOffset

