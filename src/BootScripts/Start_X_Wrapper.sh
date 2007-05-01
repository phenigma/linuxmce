#!/bin/bash

. /usr/pluto/bin/X-CleanupVideo.sh

LastVar=
for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		--parms) LastVar=Parms_StartX ;;
		--client) LastVar=Parms_Client ;;
		--server) LastVar=Parms_Server ;;
		*)
			if [[ -z "$LastVar" ]]; then
				echo "ERROR: Bad parameter order"
				exit 1
			fi
			eval "$LastVar=(\${$LastVar[@]} \${!i})"
		;;
	esac
done

timestampStart=$(date +%s)
#xinit /usr/pluto/bin/XWM_Wrapper.sh "$XClient" "${XClientParm[@]}" -- "$XDisplay" -ignoreABI -ac -allowMouseOpenFail "$VT" "${XServerParm[@]}"
xinit /usr/pluto/bin/XWM_Wrapper.sh "${Parms_Client[@]}" -- "${Parms_Server[@]}"
echo "Exit code: $?"
timestampEnd=$(date +%s)

## This is vulnerable to unexpected time shifts (for example, if ntpdate runs in the meantime, it can set timestampEnd -before- timestampStart, giving a time travel effect)
echo "start: $timestampStart; end: $timestampEnd; diff: $((timestampEnd-timestampStart))"
if ((timestampEnd-timestampStart < 10)); then
	CleanupVideo
	sleep 1
	/usr/pluto/bin/Start_X.sh "${Parms_StartX[@]}"
fi
