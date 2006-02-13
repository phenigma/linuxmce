#!/bin/bash

# replaceVar $varName
# Replaces a template variable with it's value in PlutoXConf.template file
function replaceVar() {
	local VarName=$1
	local VarValue=''
	eval  VarValue='$'$VarName

	sed "s%#$VarName#%$VarValue%g" /tmp/PlutoXConf.template.$$ > /tmp/PlutoXConf.template.$$.sed
	mv  /tmp/PlutoXConf.template.$$.sed /tmp/PlutoXConf.template.$$
}

# Set up the DefaultKeyboard
function setupKeyboard() {
	#Keyboard_Layout
	if [[ $Keyboard_Layout == "" ]]; then
		Keyboard_Layout="us"
	fi
	replaceVar "Keyboard_Layout"
	
	#Keyboard_Variant
	if [[ $Keyboard_Variant == "" ]]; then
		Keyboard_Variant="basic"
	fi
	replaceVar "Keyboard_Variant"

	#Keyboard_Model
	if [[ $Keyboard_Model == "" ]]; then
		Keyboard_Model="pc105"
	fi
	replaceVar "Keyboard_Model"
}

function setupMouse() {
	if [[ $Mouse_Protocol == "" ]]; then
		Mouse_Prococol="auto"
	fi
	replaceVar "Mouse_Protocol"

	if [[ $Mouse_Device == "" ]]; then
		Mouse_Device="/dev/input/mice"
	fi
	replaceVar "Mouse_Device"

	if [[ $Mouse_ZAxisMapping != "" ]]; then
		Mouse_ZAxisMapping="Option \"ZAxisMapping\" \"$Mouse_ZAxisMapping\""
		replaceVar "Mouse_ZAxisMapping"
	fi
}

function setupMonitor() {
	# Run edid and save the ouput to a file
	get-edid 2>/dev/null | parse-edid 2>/dev/null 1>/tmp/edid.$$
	
	# Monitor_Vendor
	if [[ $Monitor_Vendor == "" ]]; then
		Monitor_Vendor=$(cat /tmp/edid.$$ | grep VendorName | cut -d\" -f2)
		if [[ $Monitor_Vendor == "" ]]; then
			Monitor_Vendor="Unknown Monitor Vendor"
		fi
	fi
	replaceVar "Monitor_Vendor"

	# Monitor_ModelName
	if [[ $Monitor_ModelName == "" ]]; then
		Monitor_ModelName=$(cat /tmp/edid.$$ | grep ModelName | cut -d\" -f2)
		if [[ $Monitor_ModelName == "" ]]; then
			Monitor_ModelName="Unknown Monitor Model"
		fi
	fi
	replaceVar "Monitor_ModelName"

	# Monitor_HorizSync
	if [[ $Monitor_HorizSync == "" ]]; then
		Monitor_HorizSync=$(cat /tmp/edid.$$ | grep HorizSync | cut -d' ' -f2)
	fi	
	if [[ $Monitor_HorizSync != "" ]]; then
		Monitor_HorizSync="HorizSync $Monitor_HorizSync"
		replaceVar "Monitor_HorizSync"
	fi
	
	# Monitor_VertRefresh
	if [[ $Monitor_VertRefresh == "" ]]; then
		Monitor_VertRefresh=$(cat /tmp/edid.$$ | grep VertRefresh | cut -d' ' -f2)
	fi
	if [[ $Monitor_VertRefresh != "" ]]; then
		Monitor_VertRefresh="VertRefresh $Monitor_VertRefresh"
		replaceVar "Monitor_VertRefresh"
	fi
	
	# Delete the edid output as is no loger needed
	rm -f /tmp/edid.$$
}

function setupVideoCard() {
	# Run lshwd -xo to generate /tmp/xinfo file
	lshwd -ox  2>/dev/null 1>/dev/null

	#VideoCard_Driver
	if [[ $VideoCard_Driver == "" ]]; then
		VideoCard_Driver=$( cat /tmp/xinfo | grep Driver | cut -d\" -f2 )
		if [[ $VideoCard_Driver == "" ]]; then
			VideoCard_Driver="vesa"
		fi
	fi
	replaceVar "VideoCard_Driver"

	#VideoCard_VendorName
	if [[ $VideoCard_VendorName == "" ]]; then
		VideoCard_VendorName=$( cat /tmp/xinfo | grep VendorName | cut -d\" -f2 )
		if [[ $VideoCard_VendorName == "" ]]; then
			VideoCard_VendorName="Unknown Vendor Name"
		fi
	fi
	replaceVar "VideoCard_VendorName"
	
	#VideoCard_BoardName
	if [[ $VideoCard_BoardName == "" ]]; then
		VideoCard_BoardName=$( cat /tmp/xinfo | grep BoardName | cut -d\" -f2 )
		if  [[ $VideoCard_BoardName == "" ]]; then
			VideoCard_BoardName="Unknown Board Name"
		fi
	fi
	replaceVar "VideoCard_BoardName"
	
	# Remove /tmp/xinfo as is no loger needed
	rm /tmp/xinfo
}

function setupScreen() {
	#Screen_DefaultDepth
	if [[ $Screen_DefaultDepth != "" ]]; then
		Screen_DefaultDepth="DefaultDepth $DefaultDepth"
		replaceVar "DefaultDepth"
	fi
}

function setupServerFlags() {
	replaceVar "ServerFlags_AllowMouseOpenFail"
}

# Read the default vaues
. PlutoXConf.vars

# Create the temporary file that we will populate
rm -f /tmp/PlutoXConf.template.*
cp PlutoXConf.template /tmp/PlutoXConf.template.$$

setupKeyboard
setupMouse
setupMonitor
setupVideoCard
setupScreen
setupServerFlags

