#!/bin/bash

DevEntry="$1" #/dev/videoX
Input="$2"

# hardcoded input values as they are in the database
# also, positions in the Inputs array
# Note, it seems SVideo is b&w on #1, color on #2, and I can't get composite to work at all
Input_SVideo=1
Input_Composite=2
Input_Antenna=3

# Inputs=("" SVideo Composite Antenna)
Inputs=("" 1 2 0)

v4l2-ctl --device="$DevEntry" --set-input="${Inputs[$Input]}"
