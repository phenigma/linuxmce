#!/bin/bash
set -e

Debug=0

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
