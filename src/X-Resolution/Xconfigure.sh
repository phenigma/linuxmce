#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

ConfigFile="/etc/X11/XF86Config-4"
while [[ $# -gt 0 ]]; do
	case "$1" in
		--defaults) Defaults=y ;;
		--resolution)
			Resolution="$2"
			if [[ "$Resolution" == *@* ]]; then
				Refresh=${Resolution##*@}
				Resolution=${Resolution%@*}
			else
				Refresh=60
			fi
			ResX=${Resolution%%x*}
			ResY=${Resolution#*x}
			if [[ "$ResX" == *[![:digit:]]* || "$ResY" == *[![:digit:]]* || "$Refresh" == *[![:digit:]]* ]]; then
				echo "Mismatched resolution. Use format: <ResX>x<ResY>@<Refresh>"
				exit 1
			fi
			shift
		;;
		--scantype)
			case "$2" in
				progressive) ;;
				interlace|doublescan) ScanType="$2"; shift ;;
				*) echo "Unknown scan type '$2'"; exit 1 ;;
			esac
		;;
		--force) Force=y ;;
		--conffile) ConfigFile="$2"; shift ;;
		*) echo "Unknown option '$1'"; exit 1 ;;
	esac
	shift
done

if [[ "$Defaults" == y ]]; then
	lshwd -ox >/dev/null
	DisplayDriver="$(grep 'Driver' /tmp/xinfo | awk -F'"' '{print $2}')"
	rm /tmp/xinfo
	case "$DisplayDriver" in
		nv) PackageIsInstalled pluto-nvidia-video-drivers && DisplayDriver="nvidia" ;;
		radeon|ati) PackageIsInstalled fglrx-driver && DisplayDriver="fglrx" ;;
		"") DisplayDrivers="vesa" ;; # just-in-case default
	esac
	cat /usr/pluto/templates/XF86Config-4.in | awk -v"DisplayDriver=$DisplayDriver" -f/usr/pluto/bin/X-ChangeDisplayDriver.awk >"$ConfigFile"
fi

if [[ -n "$Resolution" ]]; then
	Modeline="$(/usr/pluto/bin/xtiming.pl "$ResX" "$ResY" "$Refresh" "$ScanType")"
	Modeline="${x/@*\"/\"}"
	awk -v"ResX=$ResX" -v"ResY=$ResY" -v"Refresh=$Refresh" -v"Modeline=$Modeline" -v"Force=$Force" -f/usr/pluto/bin/X-ChangeResolution.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}
fi
