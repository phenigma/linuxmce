#!/bin/bash

function XorgConfLogging() {
	local message="$1"
	local xorgLog="/var/log/pluto/xorg.conf.log"
	local xorgLines=$(cat /etc/X11/xorg.conf | wc -l)
	
	local myPid=$$

	echo "$myPid $(date -R) $message [$xorgLines]"	>> $xorgLog
}

XorgConfLogging "Starting $0 $*"

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/Utils.sh
. /usr/pluto/bin/LockUtils.sh

ConfigFile="/etc/X11/xorg.conf"
Output="CRT"

DEVICECATEGORY_Video_Cards=125
DEVICEDATA_Setup_Script=189
DEVICEDATA_Connector=68
DEVICEDATA_TV_Standard=229

resHD480=848x480
resHD720=1280x720
resHD1080=1920x1080
resHD576=720x576 # SDTV actually, EDTV with doublescan, but nVidia calls it HD so...

# Hardcoded^W Predefined modelines, found on the web: http://forums.entechtaiwan.net/viewtopic.php?t=3730
# Things to notice:
# - the dot clock value is fixed, regardless of refresh, and the other timings are elongated to compensate
# - none of the modeline generation utilities out there are able to generate these for me
# - no modeline utility is able to fix both the refresh rate and the dot clock and adjust the timings to compensate
# - these don't seem to comply to VESA GTF in any way

Modeline_1280x720_50='"1280x720" 74.250 1280 1720 1760 1980 720 741 746 750 +hsync +vsync'
Modeline_1920x540_50='"1920x540" 74.250 1920 2448 2492 2640 540 574 579 562 +hsync +vsync'
Modeline_720x576_50='"720x576" 27.000 720 732 796 864 576 581 586 625 -hsync -vsync'
Modeline_1280x720_60='"1280x720" 74.250 1280 1390 1430 1650 720 725 730 750 +hsync +vsync'
#Modeline_1280x720_60='"1280x720" 74.160 1280 1352 1392 1648 720 725 730 750 -hsync -vsync' # HDTV derived
Modeline_1920x540_60='"1920x540" 74.250 1920 2008 2052 2200 540 542 547 562 +hsync +vsync'
Modeline_720x480_60='"720x480" 27.000 720 736 798 858 480 489 495 525 -hsync -vsync'
Modeline_720x576_60='"720x576" 27.000 720 732 796 864 576 581 586 625 -hsync -vsync'
Modeline_1920x1080i_50='"1920x1080" 74.250 1920 2448 2492 2640 1080 1148 1158 1124 interlace +hsync +vsync'
Modeline_1920x1080i_60='"1920x1080" 74.250 1920 2008 2052 2200 1080 1084 1094 1124 interlace +hsync +vsync'
Modeline_1024x768_60='"1024x768" 60.80 1024 1056 1128 1272 768 768 770 796'
Modeline_640x480_60='"640x480" 25.18 640 656 752 800 480 490 492 525'

TestConfig()
{
	X :$(($Display+2)) -ignoreABI -probeonly -config "$ConfigFile" -logverbose 9 &>/dev/null
	return $?
}

GenModeline()
{
	local ResX="$1" ResY="$2" Refresh="${3:-60}" ScanType="$4"
	local Modeline
	local VarName

	VarName="Modeline_${ResX}x${ResY}"
	if [[ "$ScanType" == interlace ]]; then
		VarName="${VarName}i"
	fi
	VarName="${VarName}_${Refresh}"

	if [[ -n "${!VarName}" ]]; then
		# use a hardcoded^W predefined modline if exists
		Modeline="${!VarName}"
	else
		# generate a modeline otherwise
		Modeline=$(/usr/pluto/bin/xtiming.pl "$ResX" "$ResY" "$Refresh" "$ScanType")
		Modeline="${Modeline/@*\"/\"}"
	fi

	echo "$Modeline"
}

GetResolutionFromDB()
{
	. /usr/pluto/bin/SQL_Ops.sh
	DEVICECATEGORY_Media_Director=8
	DEVICEDATA_Video_settings=89

	ComputerDev=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Media_Director" '' 'include-parent')

	Q="SELECT IK_DeviceData FROM Device_DeviceData WHERE FK_Device='$ComputerDev' AND FK_DeviceData='$DEVICEDATA_Video_settings' LIMIT 1"
	VideoSetting=$(RunSQL "$Q")
	VideoSetting=$(Field "1" "$VideoSetting")

	ScanType=
	if [[ -n "$VideoSetting" ]]; then
		Refresh=$(echo $VideoSetting | cut -d '/' -f2)
		ResolutionInfo=$(echo $VideoSetting | cut -d '/' -f1)
		ResX=$(echo $ResolutionInfo | cut -d' ' -f1)
		ResY=$(echo $ResolutionInfo | cut -d' ' -f2)
		ResScanType=$(echo $ResolutionInfo | cut -d' ' -f3)
		if [[ -z "$Refresh" || -z "$ResX" || -z "$ResY" ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "Malformed DeviceData: VideoSetting='$VideoSetting'"
			MalformedVideoSetting='Malformed VideoSetting'
		fi
		if [[ "$ResScanType" == i ]]; then
			ScanType=interlace
		fi
	fi

	# it happened to have malformed video settings, at least once
	if [[ -n "$MalformedVideoSetting" || -z "$VideoSetting" ]]; then
		ResX='640'
		ResY='480'
		Refresh='60'
		ScanType=
	fi
	
	Resolution="${ResX}x${ResY}"

	Output=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_Connector")
	TVStandard=$(GetDeviceData "$ComputerDev" "$DEVICEDATA_TV_Standard")
}

ParseResolution()
{
	ResolutionSet=y
	Resolution="$1"
	if [[ "$Resolution" == *@* ]]; then
		Refresh=${Resolution##*@}
		Resolution=${Resolution%@*}
	else
		Refresh=60
	fi
	
	ResX=${Resolution%%x*}
	ResY=${Resolution#*x}
	if [[ "$ResY" == *i ]]; then
		ResY="${ResY%i}"
		Resolution="${Resolution%i}"
		ScanType="interlace"
	elif [[ "$ResY" == *p ]]; then
		ResY="${ResY%p}"
		Resolution="${Resolution%p}"
		ScanType=
	fi
	
	if [[ -z "$ResX" || -z "$ResY" || -z "$Refresh" || "$ResX" == *[![:digit:]]* || "$ResY" == *[![:digit:]]* || "$Refresh" == *[![:digit:]]* ]]; then
		echo "Mismatched resolution '$Resolution@$Refresh'. Use format: <ResX>x<ResY>@<Refresh>"
		exit 1
	fi
}

SetResolution()
{
	local Resolution="$1" ResX="$2" ResY="$3" Refresh="$4" ScanType="$5" Output="$6" TVStandard="$7"
	
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

	Modeline=
	# Use modeline for HD modes, non-standard VESA modes (60, 75 Hz are considered standard by X), 1024x768@60, NV: 640x480@60
	if grep -q 'Driver.*"nvidia"' "$ConfigFile"; then
		ForcedModelines="1024x768@60 640x480@60"
	else
		ForcedModelines="1024x768@60"
	fi
	if [[ -n "$nvHD" || ( -z "$nvHD" && " 60 75 " != *" $Refresh "* ) || ( " $ForcedModelines " ==  *" $Resolution@$Refresh "* ) ]]; then
		Modeline="$(GenModeline "$ResX" "$ResY" "$Refresh" "$ScanType")"
	fi

	if ! grep -q "Driver.*\"nvidia\"" "$ConfigFile"; then
		# we don't have a nVidia card; we use the nvHD variable to detect non-VESA modes too (not a perfect way to do so though)
		nvHD=
	fi
	awk -v"ResX=$ResX" -v"ResY=$ResY" -v"Refresh=$Refresh" -v"Modeline=$Modeline" -v"Force=$Force" -v"nvHD=$nvHD" -f/usr/pluto/bin/X-ChangeResolution.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}
}

XvMC_Lib()
{
	local libXvMC
	case "$DisplayDriver" in
		nvidia)
			libXvMC="libXvMCNVIDIA_dynamic.so.1"
	#		XineVideoDriver="xxmc"
		;;
	#	via)
	#		libXvMC="libviaXvMC.so.1"
	#		XineVideoDriver="xxmc"
	#	;;
		*)
			libXvMC="libXvMC.so.1"
	#		XineVideoDriver="xv"
		;;
	esac

	echo "$libXvMC"
}

CheckComponents()
{
	local -a Files=(
		/usr/pluto/bin/X-ChangeDisplayDriver.awk
		/usr/pluto/bin/X-GetDisplayDriver.awk
		/usr/pluto/bin/X-ModulesSection.awk
		/usr/pluto/bin/X-UI_Sections.awk
	)
	local File
	
	for File in "${Files[@]}"; do
		if [[ ! -f "$File" ]]; then
			Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "File not found: /usr/pluto/bin/X-ChangeDisplayDriver.awk."
			exit 1
		fi
	done
}

UpdateModules()
{
	/usr/pluto/bin/X-UpdateModules.sh --conffile "$ConfigFile" --driver "$DisplayDriver"
}

UpdateUISections()
{
	local OpenGL="$1" AlphaBlending="$2"
	
	awk -v"OpenGL=$OpenGL" -v"AlphaBlending=$AlphaBlending" -f/usr/pluto/bin/X-UI_Sections.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}
}

ScriptCustomization()
{
	local Device_Video_Card
	local DeviceData
	
	Device_Video_Card=$(FindDevice_Category "$PK_Device" "$DEVICECATEGORY_Video_Cards" "no-recursion")
	if [[ -z "$Device_Video_Card" ]]; then
		return
	fi

	DeviceData=$(GetDeviceData "$Device_Video_Card" "$DEVICEDATA_Setup_Script")
	if [[ -z "$DeviceData" ]]; then
		return
	fi

	local ScriptPath="/usr/pluto/bin/$DeviceData"
	if [[ ! -f "$ScriptPath" ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "Custom setup script not found: $DeviceData."
	fi

	. "$ScriptPath"
	GetCustomModeLines
}

ParseParameters()
{
	OrigParams=("$@")
	while [[ $# -gt 0 ]]; do
		case "$1" in
			--defaults) Defaults=y ;;
			--resolution)
				ParseResolution "$2"
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
			--keep-resolution) KeepCurrentResolution=y ;;
			--force-vesa) ForceVESA=y ;;
			--conffile) ConfigFile="$2"; shift ;;
			--skiplock) SkipLock=1 ;;
			--output) Output="$2" ; shift ;; # VGA, DVI, Component, Composite, S-Video
			--tv-standard) TVStandard="$2"; shift ;; # PAL, SECAM, NTSC
			*) echo "Unknown option '$1'"; exit 1 ;;
		esac
		shift
	done
}

EnsureResolutionVariables()
{
	if [[ ! -f "$ConfigFile" || ! -s "$ConfigFile" ]]; then
		# TODO: Detect incomplete/corrupt config files too
		Logging "$TYPE" "$SEVERITY_WARNING" "Xconfigure" "Config file '$ConfigFile' not found or empty. Forcing use of defaults."
		Defaults=y
	fi

	if [[ -z "$ResolutionSet" ]]; then
		GetResolutionFromDB
	fi

	if [[ -z "$Defaults" && -n "$KeepCurrentResolution" ]]; then
		ResX=
		ResY=
		Refresh=
		Resolution=
		ScanType=
	fi
}

wmtweaks_default()
{
	echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE mcs-option SYSTEM "mcs-option.dtd">

<mcs-option>
	<option name="Xfwm/UseCompositing" type="int" value="1"/>
</mcs-option>'
}

SetWMCompositor()
{
	local UseCompositing="$1"
	local WMTweaksFile
	local user

	for user in /home/* /root; do
		if [[ ! -d "$user" ]]; then
			continue
		fi
		WMTweaksFile="$user/.config/xfce4/mcs_settings/wmtweaks.xml"
		mkdir -p "$(dirname "$WMTweaksFile")"
		if [[ -f "$WMTweaksFile" ]]; then
			sed -i '/Xfwm\/UseCompositing/ s/value="."/value="'"$UseCompositing"'"/g' "$WMTweaksFile"
		else
			wmtweaks_default >"$WMTweaksFile"
		fi
	done
}

ScriptCustomization
CheckComponents
ParseParameters "$@"
if [[ -z "$SkipLock" ]]; then
	trap 'Unlock "Xconfigure" "Xconfigure" nolog; XorgConfLogging "Ending"' EXIT
	WaitLock "Xconfigure" "Xconfigure" nolog # don't run two copies of Xconfigure simultaneously
fi
EnsureResolutionVariables

DisplayDriver=$(GetVideoDriver)
if [[ "$DisplayDriver" == viaprop ]]; then
	DisplayDriver=via
fi

Logging "$TYPE" "$SEVERITY_STATUS" "Xconfigure" "Display Driver: $DisplayDriver" >&2
XorgConfLogging "Display Driver: $DisplayDriver"

if [[ -z "$DisplayDriver" ]]; then
	XorgConfLogging "Empty display driver. Will not continue. This shouldn't happen"
	exit 1
fi

CurrentDisplayDriver=$(awk -f/usr/pluto/bin/X-GetDisplayDriver.awk "$ConfigFile")
if [[ "$Defaults" == y ]]; then
	# write xorg.conf from scratch using the defaults
	if [[ ! -f /usr/pluto/templates/xorg.conf.in ]]; then
		Logging "$TYPE" "$SEVERITY_CRITICAL" "Xconfigure" "File not found: /usr/pluto/templates/xorg.conf.in. Can't setup defaults"
		exit 1
	fi
	cat /usr/pluto/templates/xorg.conf.in | awk -v"DisplayDriver=$DisplayDriver" -f/usr/pluto/bin/X-ChangeDisplayDriver.awk >"$ConfigFile"
elif [[ -n "$UpdateVideoDriver" && "$DisplayDriver" != "$CurrentDisplayDriver" ]]; then
	# change video driver, but only if needed
	awk -v"DisplayDriver=$DisplayDriver" -f/usr/pluto/bin/X-ChangeDisplayDriver.awk "$ConfigFile" >"$ConfigFile.$$"
	mv "$ConfigFile"{.$$,}
fi

UpdateModules
OpenGL=$(OpenGLeffects)
AlphaBlending=$(AlphaBlendingEnabled)
UpdateUISections "$OpenGL" "$AlphaBlending"
SetWMCompositor "$AlphaBlending"
libXvMC=$(XvMC_Lib)

if ! BlacklistConfFiles "/etc/X11/XvMCConfig" ;then
	if [ ! -e /etc/X11/XvMCConfig.pbackup ] && [ -e /etc/X11/XvMCConfig ] ;then
		cp /etc/X11/XvMCConfig /etc/X11/XvMCConfig.pbackup
	fi

	echo "$libXvMC" >/etc/X11/XvMCConfig
fi
#XineConfSet video.driver "$XineVideoDriver"

# Setup resolution
if [[ -n "$Resolution" ]]; then
	SetResolution "$Resolution" "$ResX" "$ResY" "$Refresh" "$ScanType" "$Output" "$TVStandard"
fi

# Test detected display driver
# Don't test if driver is vesa (assumption: always works) or current driver (assumption: already tested and works)
# Doing this test last, to test all the changes, since the video driver (e.g. nvidia) may not work without some extra options that are set together with the resolution
if [[ "$DisplayDriver" != "$CurrentDisplayDriver" && "$DisplayDriver" != vesa ]] && [[ -n "$Defaults" || -n "$UpdateVideoDriver" ]]; then
	if ! TestConfig && [[ " ${OrigParams[*]} " != *" --force-vesa "* ]]; then
		"$0" "${OrigParams[@]}" --force-vesa --skiplock
		exit $?
	fi
fi
