#!/bin/bash

. /usr/pluto/bin/Config_Ops.sh

ConfSet AVWizardDone 0
ConfSet AVWizardOverride 1
beep -f 1000 -l 100 -r 2
reboot
