#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

resHD480=848x480
resHD720=1280x720
resHD1080=1920x1080
resHD576=720x576 # SDTV actually, EDTV with doublescan, but nVidia calls it HD so...

GetVideoDriver()
{
	if [[ -n "$ForceVESA" ]]; then
		echo vesa
		return 0
	fi
	
	lshwd -ox >/dev/null
	local VideoDriver="$(grep 'Driver' /tmp/xinfo | awk -F'"' '{print $2}')"
	rm /tmp/xinfo
	case "$VideoDriver" in
		nv) PackageIsInstalled nvidia-glx && VideoDriver="nvidia" ;;
		radeon|ati) PackageIsInstalled fglrx-driver && VideoDriver="fglrx" ;;
		"") VideoDrivers="vesa" ;; # just-in-case default
	esac
	echo "$VideoDriver"
}

TestConfig()
{
	X :2 -probeonly -xf86config "$ConfigFile" &>/dev/null
	return $?
}

OrigParams=("$@")
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
			if [[ -z "$ResX" || -z "$ResY" || -z "$Refresh" || "$ResX" == *[![:digit:]]* || "$ResY" == *[![:digit:]]* || "$Refresh" == *[![:digit:]]* ]]; then
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
		--update-video-driver) UpdateVideoDriver=y ;;
		--force-vesa) ForceVESA=y ;;
		--conffile) ConfigFile="$2"; shift ;;
		*) echo "Unknown option '$1'"; exit 1 ;;
	esac
	shift
done

[[ -n "$Defaults" || -n "$UpdateVideoDriver" ]] && DisplayDriver=$(GetVideoDriver)
if [[ "$Defaults" == y ]]; then
	cat /usr/pluto/templates/XF86Config-4.in | awk -v"DisplayDriver=$DisplayDriver" -f/usr/pluto/bin/X-ChangeDisplayDriver.awk >"$ConfigFile"
elif [[ -n "$UpdateVideoDriver" ]]; then
	awk -v"DisplayDriver=$DisplayDriver" -f/usr/pluto/bin/X-ChangeDisplayDriver.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}
fi

if [[ -n "$Defaults" || -n "$UpdateVideoDriver" ]]; then
	if ! TestConfig && [[ " ${OrigParams[*]} " != *" --force-vesa "* ]]; then
		exec "$0" "${OrigParams[@]}" --force-vesa
	fi
fi

if [[ -n "$Resolution" ]]; then
	for Var in ${!resHD*}; do
		if [[ "$Resolution" == "${!Var}" ]]; then
			nvHD=${Var#res}
			if [[ "$ScanType" == interlace && "$Var" != *720* ]]; then
				nvHD="${nvHD}i"
			else
				nvHD="${nvHD}p"
			fi
			break
		fi
	done
	Modeline="$(/usr/pluto/bin/xtiming.pl "$ResX" "$ResY" "$Refresh" "$ScanType")"
	Modeline="${Modeline/@*\"/\"}"
	if [[ -z "$nvHD" && " 60 75 " == *" $Refresh "* ]]; then
		# don't use a modeline for standard refresh rates
		# (X seems to know only 60 and 75 as "standard", all the others either not working, or giving different resolutions)
		Modeline=
	fi
	if ! grep -q "Driver.*\"nvidia\"" "$ConfigFile"; then
		# we don't have a nVidia card; we use the nvHD variable to detect non-VESA modes too (not a perfect way to do so though)
		nvHD=
	fi
	awk -v"ResX=$ResX" -v"ResY=$ResY" -v"Refresh=$Refresh" -v"Modeline=$Modeline" -v"Force=$Force" -v"nvHD=$nvHD" -f/usr/pluto/bin/X-ChangeResolution.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}
fi
