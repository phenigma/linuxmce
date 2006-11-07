#!/bin/bash -x

. /usr/pluto/bin/SQL_Ops.sh

## Misc variables
DISTRIB='sarge'
MIRROR='http://deb.plutohome.com/debian/'
ARH_DIR='/usr/pluto/install'
DisklessFS="PlutoMD-pre-install.tar.bz2"

Installer_Files=(BonusCdMenu.sh BonusCdAutoInst.sh Common.sh ConfirmDependencies_Debian.sh Initial_Config.sh Initial_Config_Core.sh Initial_Config_MD.sh Initial_Config_Finish.sh Initial_DHCP_Config.sh Initial_Network_Config.sh pkgsel post-debootstrap.sh preseed.cfg PreseedStage2.sh)

## Create a temporary directory to store the diskless image into and be sure
## it gets removed after we finish
TEMP_DIR=$(mktemp -d)

function trap_EXIT() {
	umount -fl $TEMP_DIR/usr/pluto/deb-cache
	umount -fl $TEMP_DIR/proc
	umount -fl $TEMP_DIR/sys

	rm -rf $TEMP_DIR
}

trap "trap_EXIT" EXIT

## Run debootstrap to create the base debian system for our MD
#debootstrap $DISTRIB $TEMP_DIR $MIRROR

pushd "$TEMP_DIR" >/dev/null
tar -xjf "$ARH_DIR"/PlutoMD.tar.bz2
popd >/dev/null

## Preeseed the diskless root with our debconf values so the user
## won't need to interact with the installer
while read owner variable type value; do
	preseed=("${preseed[@]}" "$variable=$value")
done < /usr/pluto/install/preseed.cfg

preseed=("${preseed[@]}" "debconf/frontend=noninteractive" "debconf/priority=critical")

for setting in "${preseed[@]}"; do
	variable="${setting%%=*}"
	value="${setting#*=}"
	
	echo ". /usr/share/debconf/confmodule"		>  $TEMP_DIR/tmp/script.sh
	echo "db_set \"$variable\" \"$value\"" 		>> $TEMP_DIR/tmp/script.sh
	echo "db_fset \"$variable\" seen true"		>> $TEMP_DIR/tmp/script.sh

	chmod +x $TEMP_DIR/tmp/script.sh
	chroot $TEMP_DIR /tmp/script.sh
done

## Create the temporary sources.list file for the media director 
## FIXME: maybe we need to make it from scratch ?
cp /etc/apt/sources.list $TEMP_DIR/etc/apt

## Create a list of devices that will run on the md so we will know
## what software to preinstall there
## FIXME: get this list from the database 
DEVICE_LIST="28 62 1795 5 35 11 1825 26 1808"

## Begin installing the packages needed for the pluto devices
mkdir -p $TEMP_DIR/usr/pluto/deb-cache
mount --bind /usr/pluto/deb-cache $TEMP_DIR/usr/pluto/deb-cache
mount none -t sysfs $TEMP_DIR/sys
mount none -t proc  $TEMP_DIR/proc

chroot $TEMP_DIR apt-get -y update
chroot $TEMP_DIR apt-get -y dist-upgrade

touch "$TEMP_DIR"/etc/chroot-install
mv "$TEMP_DIR"/usr/sbin/invoke-rc.d{,.pluto-install}
mv "$TEMP_DIR"/sbin/start-stop-daemon{,.pluto-install}

echo "do_initrd = Yes" > $TEMP_DIR/etc/kernel-img.conf
chroot "$TEMP_DIR" invoke-rc.d exim4 force-stop

echo -en '#!/bin/bash\necho "WARNING: fake invoke-rc.d called"\n' >"$TEMP_DIR"/usr/sbin/invoke-rc.d
echo -en '#!/bin/bash\necho "WARNING: fake start-stop-daemon called"\n' >"$TEMP_DIR"/sbin/start-stop-daemon
chmod +x "$TEMP_DIR"/usr/sbin/invoke-rc.d
chmod +x "$TEMP_DIR"/sbin/start-stop-daemon

cp {,"$TEMP_DIR"}/etc/apt/apt.conf.d/30pluto

Branch=3
if [[ "$Branch" == 1 ]]; then # alternative selection
for device in $DEVICE_LIST; do
	Q="SELECT
		Package_Source.Name
	   FROM
		Package_Source
		JOIN DeviceTemplate on Package_Source.FK_Package = DeviceTemplate.FK_Package
	   WHERE
		PK_DeviceTemplate = $device
	   AND
		FK_RepositorySource = 2					
	"

	R=$(RunSQL "$Q")

	for Row in $R; do
		pkg_name=$(Field 1 "$Row")
	done

	echo "#### Installing $pkg_name"
	chroot $TEMP_DIR apt-get -y install $pkg_name
	if [[ "$?" == "0" ]] ;then
		echo "#### Package $pkg_name installed ok!"
	else
		echo "#### Package $pkg_name failed ($TEMP_DIR)"
		/bin/bash
	fi
done

elif [[ "$Branch" == 2 ]]; then # alternative selection

mkdir -p "$TEMP_DIR"/usr/pluto/install
for File in "${Installer_Files[@]}"; do
	cp /usr/pluto/install/"$File" "$TEMP_DIR"/usr/pluto/install
done

cp {,"$TEMP_DIR"}/usr/pluto/install/PlutoMD-activation.sh
chroot "$TEMP_DIR" bash -x /usr/pluto/install/PlutoMD-activation.sh

elif [[ "$Branch" == 3 ]]; then # alternative selection

PrioPkgs="pluto-boot-scripts"

echo $PrioPkgs | xargs chroot "$TEMP_DIR" apt-get -f -y install
cat /home/packages | xargs chroot "$TEMP_DIR" apt-get -f -y install

else

echo "WRONG"
/bin/bash

fi # alternative selection

umount $TEMP_DIR/usr/pluto/deb-cache
umount $TEMP_DIR/sys
umount $TEMP_DIR/proc

mv "$TEMP_DIR"/sbin/start-stop-daemon{.pluto-install,}
mv "$TEMP_DIR"/usr/sbin/invoke-rc.d{.pluto-install,}
rm -f "$TEMP_DIR"/etc/chroot-install

#invoke-rc.d nfs-common restart

mkdir -p "$ARH_DIR"
pushd "$TEMP_DIR" >/dev/null
tar -cjvf "$ARH_DIR/$DisklessFS" *
popd >/dev/null

rm -rf $TEMP_DIR

trap - EXIT
