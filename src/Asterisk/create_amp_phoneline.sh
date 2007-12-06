#!/bin/bash

Type="$1"
shift

case "$Type" in
	"broadvoice")
		AmpScript="/usr/pluto/bin/create_amp_broadvoice.pl"
	;;
	"freeworlddialup")
		AmpScript="/usr/pluto/bin/create_amp_fwd.pl"
	;;
	"inphonex")
		AmpScript="/usr/pluto/bin/create_amp_inphonex.pl"
	;;
	"efon")
		AmpScript="/usr/pluto/bin/create_amp_efon.pl"
	;;
	"teliax")
		AmpScript="/usr/pluto/bin/create_amp_teliax.pl"
	;;
	"nufone")
		AmpScript="/usr/pluto/bin/create_amp_nufone.pl"
	;;
	"voiceeclipse")
		AmpScript="/usr/pluto/bin/create_amp_voiceeclipse.pl"
	;;
	"sipgate")
		AmpScript="/usr/pluto/bin/create_amp_sipgate.pl"
	;;
esac

"$AmpScript" "$@"
/usr/pluto/bin/create_pluto_dialplan.pl
