#!/bin/bash

DevEntry="$1" #/dev/videoX
Input="$2"

# hardcoded input values as they are in the database
# also, positions in the Inputs array
Input_SVideo=1
Input_Composite=2
Input_Antenna=3

# Inputs=("" SVideo Composite Antenna)
Inputs=("" 1 2 0)

ivtvctl -d "$DevEntry" -p "${Inputs[$Input]}"
