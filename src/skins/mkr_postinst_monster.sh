#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh
. /usr/pluto/bin/SQL_Ops.sh

ConfSet UseVideoWizard 1
ConfSet ReplaceSkin_1 7
ConfSet ReplaceSkin_3 10
ConfSet ReplaceSkin_6 11
ConfSet UseSkinSettings_7 1
ConfSet UseSkinSettings_10 3
ConfSet UseSkinSettings_11 6

mv /var/www/pluto-admin/include/images/{monster_logo,logo_pluto}.jpg
cp /usr/pluto/orbiter/skins/Monster/bootsplash/*.cfg /etc/bootsplash/themes/pluto/config
cp /usr/pluto/orbiter/skins/Monster/bootsplash/*.jpg /etc/bootsplash/themes/pluto/images
/var/lib/dpkg/info/bootsplash.postinst configure

# Get rid of the default room / user since we'll do this in the wizard
Q="DELETE FROM Room WHERE PK_Room=1"
RunSQL "$Q"

Q="UPDATE Device SET FK_Room=NULL WHERE FK_Room=1"
RunSQL "$Q"

Q="DELETE FROM Users WHERE PK_Users=1"
RunSQL "$Q"

Q="DELETE FROM Installation_Users WHERE FK_Users=1"
RunSQL "$Q"
