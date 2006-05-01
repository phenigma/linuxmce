#!/bin/bash

BaseDir=/usr/pluto/bin

Wiz="$BaseDir"/AVWizard
Resolutions_VESA="640x480 800x600 1024x768 1280x1024 1600x1200"
Resolutions_HDTV="480p=848x480 720p=1280x720 1080i=1920x1080i 1080p=1920x1080"
Refresh_Rates="50 60 65 72 75 80 85 100 120"

XF86Config="/etc/X11/XF86Config-4"

StartX()
{
	"$BaseDir"/Start_X.sh
}

StopX()
{
	"$BaseDir"/Stop_X.sh
}

WizSet()
{
	local Var="$1" Value="$2"
	
	[[ -z "$Var" ]] && return 1
	SDL_VIDEODEVICE=dummy "$Wiz" -set "$Var" "$Value" 2>/dev/null
}

WizSet()
{
	local Var="$1"
	
	[[ -z "$Var" ]] && return 1
	SDL_VIDEODEVICE=dummy "$Wiz" -get "$Var" 2>/dev/null
}
