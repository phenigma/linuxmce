#!/bin/bash
. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh
. /usr/pluto/bin/pluto.func

Device=$2

echo "$(date -R) ConfigureHDHomeRun start"

Q="SELECT IPaddress FROM Device WHERE PK_Device='$Device'"
IPAddress=$(RunSQL "$Q")

DeviceID=$(/usr/pluto/bin/hdhomerun_config discover "$IPAddress" | grep "$IPAddress" | cut -d' ' -f3)

echo "$(date -R) HDHomeRun ID $DeviceID IP $IPAddress"

Q="SELECT PK_Device FROM Device WHERE FK_Device_ControlledVia='$Device'"
R=$(RunSQL "$Q")
for i in $R; do
	/usr/pluto/bin/MessageSend dcerouter -targetType template -r 0 27 1 246 2 $i 52 152 5 "$DeviceID"
	# hardcode the id until this is fixed
done

/usr/pluto/bin/MessageSend $DCERouter -targetType template -r 0 36 1 824  # Force the new device id to be written into mythconverg setup

/usr/pluto/bin/hdhomerun_config $IPAddress upgrade /usr/pluto/bin/hdhomerun_firmware_20080212.bin
sleep 30  # give it time to recover after the firmware upgrade

echo "$(date -R) ConfigureHDHomeRun done"

#./hdhomerun_config 192.168.80.252 set /lineup/location us:94005  # opt-in to do channel scanning  /hdhomerun_config 192.168.80.252 set /lineup/location disabled
