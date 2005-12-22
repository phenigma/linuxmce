#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

ConfSet UseVideoWizard 1
ConfSet ReplaceSkin_1 7
ConfSet ReplaceSkin_3 10
ConfSet ReplaceSkin_6 11
ConfSet UseSkinSettings_7 1
ConfSet UseSkinSettings_10 3
ConfSet UseSkinSettings_11 6

mv /var/www/pluto-admin/include/images/{logo_pluto,monster_logo}.jpg
