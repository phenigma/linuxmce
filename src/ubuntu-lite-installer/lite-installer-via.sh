#!/bin/bash
set -e

Debug=0
FromHdd=0
grep -q "from_hdd_recovery" /proc/cmdline && FromHdd="1"

TargetHdd=
BootUUID=
SwapUUID=
RootUbuntuUUID=
RootDebianUUID=

/usr/bin/lite-installer-via_functions.sh

GetConsole
GetHddToUse
PartitionHdd
FormatPartitions
MountPartitions
CopyDVD
ExtractArchives
PrepareFirstBoot
SetupFstab
InstallGrub
TargetCleanup
UnmountPartitions
Reboot
