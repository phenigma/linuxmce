#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/LockUtils.sh
. /usr/pluto/bin/Utils.sh

NeedConfigure()
{
	local Device NeedConfigure PK_Device
	local i

	Device="$1"

	Q="SELECT NeedConfigure FROM Device WHERE PK_Device='$Device'"
	R=$(RunSQL "$Q")
	NeedConfigure=$(Field 1 "$R")

	if [ "$NeedConfigure" -eq 1 ]; then
		return 0
	fi

	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Device'"
	R=$(RunSQL "$Q")
	for i in $R; do
		if NeedConfigure $i; then
			return 0
		fi
	done
	
	return 1
}

Unset_NeedConfigure_Children()
{
	local Device
	local i

	Device="$1"
#	Q="UPDATE Device SET NeedConfigure=0 WHERE PK_Device='$Device'" # this line resets the orbiters and OrbiterGen doesn't run anymore
	Q="UPDATE Device
		JOIN DeviceTemplate ON FK_DeviceTemplate=PK_DeviceTemplate
		JOIN DeviceCategory ON FK_DeviceCategory=DeviceCategory.PK_DeviceCategory
		LEFT JOIN DeviceCategory AS DeviceCategory2 ON DeviceCategory2.PK_DeviceCategory=DeviceCategory.FK_DeviceCategory_Parent
		LEFT JOIN DeviceCategory AS DeviceCategory3 ON DeviceCategory3.PK_DeviceCategory=DeviceCategory2.FK_DeviceCategory_Parent
		SET NeedConfigure=0
		WHERE (DeviceCategory.PK_DeviceCategory<>5 OR DeviceCategory.PK_DeviceCategory IS NULL)
		AND (DeviceCategory2.PK_DeviceCategory<>5 OR DeviceCategory2.PK_DeviceCategory IS NULL)
		AND (DeviceCategory3.PK_DeviceCategory<>5 OR DeviceCategory3.PK_DeviceCategory IS NULL)
		AND (DeviceCategory3.FK_DeviceCategory_Parent<>5 OR DeviceCategory3.FK_DeviceCategory_Parent IS NULL)
		AND PK_Device='$Device'"
	RunSQL "$Q"

	Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Device'"
	R=$(RunSQL "$Q")
	for i in $R; do
		Unset_NeedConfigure_Children $i
	done
}

# Clean up video driver packages
# When a video card is removed/replaced, remove its packages
# The new drivers for video cards are installed later in this script, not in this function
CleanupVideo()
{
	# Check for video card changes and update system accordingly
	local Pkgs_nVidia="nvidia-driver nvidia-glx nvidia-kernel-* pluto-nvidia-video-drivers"
	local Pkgs_ATI="fglrx-driver pluto-ati-video-drivers"
	local nV_inst ATI_inst nV_dev ATI_dev
	local DEVICECATEGORY_Video_Cards=125

	nV_inst="$(echo "$Pkgs_nVidia" | COLUMNS=1024 xargs dpkg -l | grep ^ii | awk '{ print $2 }')"
	ATI_inst="$(echo "$Pkgs_ATI" | COLUMNS=1024 xargs dpkg -l | grep ^ii | awk '{ print $2 }')"
	nV_dev="$(FindDevice_Category $PK_Device $DEVICECATEGORY_Video_Cards)"
	ATI_dev="$(FindDevice_Category $PK_Device $DEVICECATEGORY_Video_Cards)"

	if [[ -n "$nV_inst" && -z "$nV_dev" ]]; then
		apt-get -y remove --purge $nV_inst
	elif [[ -n "$ATI_inst" && -z "$ATI_dev" ]]; then
		apt-get -y remove --purge $ATI_inst
	fi

	/usr/pluto/bin/Xconfigure.sh --update-video-driver
}

for ((i = 1; i <= "$#"; i++)); do
	case "${!i}" in
		F) Force=y ;;
		StartLocalDevice) StartLocalDevice=y ;;
		NoVideo) NoVideo=y ;;
		*) echo "Unrecognized parameter '${!i}' skipped." ;;
	esac
done

if [[ "$NoVideo" != "y" ]]; then
	CleanupVideo
fi

if [[ "$Force" != "y" ]]; then
	NeedConfigure "$PK_Device" || exit 0
fi

[ -n "$MySqlPassword" ] && Pass="-p$MySqlPassword"
CUsh="/usr/pluto/install/config_update.sh"

echo /usr/pluto/bin/ConfirmDependencies -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device install
/usr/pluto/bin/ConfirmDependencies -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device install >"$CUsh.$$"
linecount=$(cat "$CUsh.$$" | wc -l)
awk "NR<$linecount-8" "$CUsh.$$" >"$CUsh"
rm "$CUsh.$$"

chmod +x "$CUsh"
WaitLock "InstallNewDevice" "Confirm_Device_Changes" # don't step on InstallNewDevices scripts that may be running in the background
if bash -x "$CUsh" &> >(tee /var/log/pluto/Config_Device_Changes.newlog); then
	Unset_NeedConfigure_Children "$PK_Device"
fi
Unlock "InstallNewDevice" "Confirm_Device_Changes"
#rm "$CUsh"

echo /usr/pluto/bin/ConfirmDependencies -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device buildall
mkdir -p /usr/pluto/sources

: >"/usr/pluto/sources/buildall.sh"
echo '#!/bin/bash' >>"/usr/pluto/sources/buildall.sh"
echo "cd /usr/pluto/sources" >>"/usr/pluto/sources/buildall.sh"
/usr/pluto/bin/ConfirmDependencies -n -h $MySqlHost -u $MySqlUser $Pass -d $PK_Device buildall >>"/usr/pluto/sources/buildall.sh"
rm -f "/usr/pluto/install/compile.sh" # old version mistake precaution
ln -sf "/usr/pluto/sources/buildall.sh" "/usr/pluto/install/compile.sh"
chmod +x "/usr/pluto/sources/buildall.sh"

if [[ "$StartLocalDevice" == "y" ]]; then
        echo "Starting local devices"
        /usr/pluto/bin/Start_LocalDevices.sh
fi
