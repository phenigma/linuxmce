#!/bin/bash

. /root/Ubuntu_Helpers/BuildUbuntu_Functions.sh

Backup_Last_Build
#Install_Build_Needed_Packages
Create_Diskless_Archive
Import_Pluto_Skins
Checkout_Pluto_Svn #branches/2.0.0.44
#Checkout_Pluto_Svn branches/Kirill/database_wrapper "src"
Build_Pluto_Replacements
Build_MakeRelease_Binary
Import_Build_Database
Create_Fake_Windows_Binaries
Build_Pluto_Stuff #-b
Create_Local_Repository
Create_ISO
Upload_Build_Archive

echo "Finished"
date -R
read

