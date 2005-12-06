#!/bin/bash

. /usr/pluto/bin/pluto.func
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

DEVICEDATA_Type=47
DEVICEDATA_TCP_Address=117
DEVICEDATA_Share_Name=126
DEVICEDATA_Username=127
DEVICEDATA_Password=128
DEVICEDATA_Mount_Point=129
DEVICEDATA_Use_Pluto_Directory_Structure=130

# Syntax: NAS_Init <samba|nfs> "$@"
NAS_Init()
{
	NAS_ShareType="$1"
	shift
	
	case "$NAS_ShareType" in
		samba|nfs) : ;;
		*) Logging "NAS" $SEVERITY_CRITICAL "share mount" "Unknown mount type '$NAS_ShareType'"; exit 1 ;;
	esac
	Logging "NAS" $SEVERITY_NORMAL "share mount: $NAS_ShareType" "NAS_Init"
	
	while [[ "$#" > 0 ]]; do
		case "$1" in
			-d) myPK_Device="$2"; shift ;;
			*) Logging "NAS" $SEVERITY_WARNING "share mount: $NAS_ShareType" "Unknown parameter '$1'" ;;
		esac
		shift
	done

	if [[ -z "$myPK_Device" ]]; then
		Logging "NAS" $SEVERITY_CRITICAL "share mount: $NAS_ShareType" "I didn't receive my device number as a parameter. Ending"
		exit 1
	fi
}

NAS_GetData()
{
	local FK_DeviceData IK_DeviceData
	
	if [[ -z "$myPK_Device" ]]; then
		Logging "NAS" $SEVERITY_CRITICAL "NAS_GetData ($NAS_ShareType)" "Please call NAS_Init"
		exit 1
	fi

	SQL="SELECT FK_DeviceData,IK_DeviceData FROM Device_DeviceData
		WHERE FK_Device=$myPK_Device AND FK_DeviceData IN ($DEVICEDATA_Type,$DEVICEDATA_TCP_Address,$DEVICEDATA_Share_Name,$DEVICEDATA_Username,$DEVICEDATA_Password,$DEVICEDATA_Mount_Point,$DEVICEDATA_Use_Pluto_Directory_Structure)"
	R=$(RunSQL "$SQL")

	for Row in $R; do
		FK_DeviceData=$(Field 1 "$Row")
		IK_DeviceData=$(Field 2 "$Row")

		case "$FK_DeviceData" in
			$DEVICEDATA_Type) NAS_Type=$IK_DeviceData ;;
			$DEVICEDATA_TCP_Address) NAS_IP_Address=$IK_DeviceData ;;
			$DEVICEDATA_Share_Name) NAS_Share_Name=$IK_DeviceData ;;
			$DEVICEDATA_Username) NAS_Username=$IK_DeviceData ;;
			$DEVICEDATA_Password) NAS_Password=$IK_DeviceData ;;
			$DEVICEDATA_Mount_Point) NAS_Mount_Point=$IK_DeviceData ;;
			$DEVICEDATA_Use_Pluto_Directory_Structure) NAS_Use_Pluto_Directory_Structure=$IK_DeviceData ;;
			*) Logging "NAS" $SEVERITY_WARNING "NAS_GetData ($NAS_ShareType)" "Unknown DeviceData '$FK_DeviceData=$IK_DeviceData'" ;;
		esac
	done

	SQL="SELECT Description FROM Device WHERE PK_Device=$myPK_Device"
	R=$(RunSQL "$SQL")
	NAS_Description=$(RunSQL "$R")

	local Msg="Description: $NAS_Description; Type: $NAS_Type; IP: $NAS_IP_Address; Share name: $NAS_Share_Name; Username: $Username; Mount point: $NAS_MOUNT_Point; Pluto directory structure: $DEVICEDATA_Use_Pluto_Directory_Structure"
	Logging "NAS" $SEVERITY_NORMAL "NAS_GetData ($NAS_ShareType)" "$Msg"

	if [[ "$NAS_Type" != "$NAS_ShareType" ]]; then
		Logging "NAS" $SEVERITY_CRITICAL "NAS_GetData ($NAS_ShareType)" "Share type in database doesn't match my known share type. This shouldn't happen. Bailing out"
		exit 1
	fi

	if [[ -z "$NAS_IP_Address" || -z "$NAS_Share_Name" ]]; then
		Logging "NAS" $SEVERITY_CRITICAL "NAS_GetData ($NAS_ShareType)" "Missing important mount parameters"
		exit 1
	fi
}

NAS_Mount()
{
	local Type="$1" Src="$2" Dst="$2" Opts="$3"

	mkdir -p "$Dst"
	if [[ -z "$Opts" ]]; then
		mount -t "$Type" "$Src" "$Dst"
	else
		mount -t "$Type" -o "$Opts" "$Src" "$Dst"
	fi
	
	local Ret="$?"
	if [[ "$Ret" != 0 ]]; then
		Logging "NAS" $SEVERITY_CRITICAL "NAS_Mount ($NAS_ShareType)" "Mount failed"
	else
		Logging "NAS" $SEVERITY_CRITICAL "NAS_Mount ($NAS_ShareType)" "Mount succeeded"
	fi

	return "$Ret"
}
