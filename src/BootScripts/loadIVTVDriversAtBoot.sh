#!/bin/bash


echo "Loading ivtv drivers";
echo "Running depmod (You should not see any errors or warnings)."
depmod -a 
modprobe ivtv
echo "Drivers loaded (you might need to tweak the settings)."
