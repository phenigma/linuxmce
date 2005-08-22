#!/bin/bash
CD_FRAMESIZE_RAW=2352
CD_FRAMEWORDS=$((CD_FRAMESIZE_RAW/2))

LastSector=0
Percentage=0
LastPercentage=0

while read line; do
	if [[ "$line" == *"sector"* ]]; then
		value=${line##*sector}
		value=${value%%(track*}
		value=${value// }
		if [[ "$line" == *from* ]]; then
			StartSector="$value"
		else
			EndSector="$value"
		fi
		if [[ -n "$StartSector" && -n "$EndSector" ]]; then
			((TrackLength = EndSector - StartSector))
		fi
	elif [[ "$line" == "##:"*read* ]]; then
		CurrentSector=${line##*@ }
		((CurrentSector /= CD_FRAMEWORDS))
		((CurrentSector -= StartSector))
		if ((CurrentSector < LastSector)); then
			continue
		fi
		((Percentage = CurrentSector * 100 / TrackLength))
		if ((Percentage != LastPercentage)); then
			echo "$Percentage%"
			LastPercentage="$Percentage"
		fi
		LastSector="$CurrentSector"
	fi
done
