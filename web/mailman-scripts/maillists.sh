#!/bin/bash

if [[ "$#" -lt 2 ]]; then
	echo "Syntax: $(basename $0) <command> <parameter>"
	echo "Commands: find <e-mail>, new <list> [Description], delete <list>, view lists, view members <list> subscribe <list> <e-mail list>, unsubscribe <list> <e-mail list>"
	exit
fi

MMdir="/var/mailman/bin"
QMSdir="/usr/pluto/bin"
Cmd="$1"; shift
Parameter="$1"; shift
Parameters=("$@")

Settings="
reply_goes_to_list = 1
send_welcome_msg = False
send_goodbye_msg = False
welcome_msg = \"You have been subscribed to the %(real_name)s@%(host_name)s mailing list\"
#goodbye_msg = \"You have been removed from the %(real_name)s@%(hostname)s mailing list\"
msg_footer = \"\"\"--
Go to http://plutohome.com/ for more info\"\"\"
digest_footer = \"\"\"--
Go to http://plutohome.com/ for more info\"\"\"
bounce_matching_headers = ''
description = '''${Parameters[*]}'''
"

case "$Cmd" in
	find)
		$MMdir/find_member "${Parameter//./\.}" | tail +2
	;;

	new)
		$MMdir/newlist -q "$Parameter" "lists@plutohome.com" qaz123 &>/dev/null
		echo "$Settings" >list.$$
		$MMdir/config_list -i list.$$ "$Parameter"
		rm -f ./list.$$
		$QMSdir/qmail.sh add "$Parameter"
	;;

	delete)
		$MMdir/rmlist "$Parameter" &>/dev/null
		$QMSdir/qmail.sh del "$Parameter"
	;;

	view)
		case "$Parameter" in
			lists)
				$MMdir/list_lists | tail +2
			;;

			members)
				$MMdir/list_members $Parameters
			;;
		esac
	;;

	subscribe)
		echo "${Parameters[@]}" | tr ' ' '\n' >members.$$
		$MMdir/add_members -r members.$$ "$Parameter" &>/dev/null
		rm -f members.$$
	;;

	unsubscribe)
		echo "${Parameters[@]}" | tr ' ' '\n' >members.$$
		$MMdir/remove_members -f members.$$ "$Parameter" &>/dev/null
		rm -f members.$$
	;;
esac
