// UI1 Screen Game Atari 2600 (295)
// Created using structure-qml.php from Peer Oliver Schmidt\m// based on the LinuxMCE database
import QtQuick 1.1
import "../components"
Item {
	Image {
		source: "skins/Basic/Media/PVRremote.png"
		width: 1920
		height: 1080
	}
	function onActivate1785() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17174
		setCurrentScreen("Screen_1.qml")
	}
	StyledText {
		x: 1680
		y: 0
		height: 1
		width: 1
		textLabel: "Main menu"
	}
	ButtonSq {
		id: button1785_3082
		x: 1680
		y: 0
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/TabletControls/home.png"
		buttontext: "Main menu"
		clickHandler.onClicked: onActivate1785()
	}
	function onActivate1569() 
	{
	 // Calling Command 44 (MH Stop Media) with CommandGroup_Parameters from 13739
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 44 45 \"<%=E%>\" ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18035
		setCurrentScreen("Screen_1.qml")
	}
	ButtonSq {
		id: button1569_3083
		x: 1680
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/power.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1569()
	}
	function onActivate2441() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14732
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"+10\" ")
	}
	ButtonSq {
		id: button2441_3084
		x: 960
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2441()
	}
	function onActivate2443() 
	{
	 // Calling Command 408 (Show Floorplan) with CommandGroup_Parameters from 15042
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 408 11 \"100\" 12 \"100\" 14 \"light\" ")
	}
	StyledText {
		x: 960
		y: 540
		height: 5
		width: 3
		textLabel: "Lights"
	}
	ButtonSq {
		id: button2443_3085
		x: 960
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_map.png"
		buttontext: "Lights"
		clickHandler.onClicked: onActivate2443()
	}
	function onActivate2442() 
	{
	 // Calling Command 184 (Set Level) with CommandGroup_Parameters from 14733
		manager.sendDceMessage(srouterip + " "+deviceid + " -104 1 184 76 \"-10\" ")
	}
	ButtonSq {
		id: button2442_3086
		x: 960
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/lights_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate2442()
	}
	function onActivate2289() 
	{
	 // Calling Command 97 (Mute) with CommandGroup_Parameters from 13657
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 97 ")
	}
	StyledText {
		x: 720
		y: 540
		height: 240
		width: 180
		textLabel: "Mute"
	}
	ButtonSq {
		id: button2289_3087
		x: 720
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_mute.png"
		buttontext: "Mute"
		clickHandler.onClicked: onActivate2289()
	}
	function onActivate1850() 
	{
	 // Calling Command 89 (Vol Up) with CommandGroup_Parameters from 13671
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 89 ")
	}
	ButtonSq {
		id: button1850_3088
		x: 720
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_up.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1850()
	}
	function onActivate1851() 
	{
	 // Calling Command 90 (Vol Down) with CommandGroup_Parameters from 13672
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 90 ")
	}
	ButtonSq {
		id: button1851_3089
		x: 720
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/vol_down.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1851()
	}
	StyledText {
		x: 729
		y: 7
		height: 208
		width: 346
		textLabel: "<%=NP%>"
	}
	StyledText {
		x: 720
		y: 169
		height: 164
		width: 360
		textLabel: "<%=NP_SEC%>"
	}
	StyledText {
		x: 720
		y: 292
		height: 90
		width: 271
		textLabel: "<%=NP_TIME_SHORT%>"
	}
	StyledText {
		x: 1081
		y: 292
		height: 90
		width: 89
		textLabel: "<%=NP_SPEED%>"
	}
	ButtonSq {
		id: button2455_3090
		x: 720
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/532"
		buttontext: "<%=NP_SPEED%>"
		clickHandler.onClicked: onActivate2455()
	}
	ButtonSq {
		id: button3368_3091
		x: 1200
		y: 0
		width: 480
		height: 360
		buttonbackground: "skins/Basic/Backgrounds/logo.png"
		buttontext: ""
		clickHandler.onClicked: onActivate3368()
	}
	function onActivate2338() 
	{
	 // Calling Command 65 (Jump Position In Playlist) with CommandGroup_Parameters from 13915
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 65 5 \"<%=35%>\" ")
	}
	function onActivate2458() 
	{
	 // Calling Command 401 (Show File List) with CommandGroup_Parameters from 15962
		manager.sendDceMessage(srouterip + " "+deviceid + " -300 1 401 29 \"<%=MT%>\" ")
	}
	StyledText {
		x: 1680
		y: 180
		height: 240
		width: 180
		textLabel: "file
list"
	}
	ButtonSq {
		id: button2458_3092
		x: 1680
		y: 180
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "file
list"
		clickHandler.onClicked: onActivate2458()
	}
	ButtonSq {
		id: button5774_3093
		x: 1200
		y: 360
		width: 720
		height: 540
		buttonbackground: "skins/Basic/798"
		buttontext: ""
		clickHandler.onClicked: onActivate5774()
	}
	function onActivate1582() 
	{
	 // Calling Command 39 (Pause Media) with CommandGroup_Parameters from 14163
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 39 ")
	}
	ButtonSq {
		id: button1582_3094
		x: 1440
		y: 540
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/enter.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1582()
	}
	function onActivate5616() 
	{
	 // Calling Command 943 (Game 1P Start) with CommandGroup_Parameters from 17659
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 943 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17884
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	StyledText {
		x: 1200
		y: 360
		height: 240
		width: 180
		textLabel: "P1"
	}
	ButtonSq {
		id: button5616_3095
		x: 1200
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "P1"
		clickHandler.onClicked: onActivate5616()
	}
	function onActivate5775() 
	{
	 // Calling Command 950 (Game Select) with CommandGroup_Parameters from 17865
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 950 ")
	}
	StyledText {
		x: 1200
		y: 720
		height: 240
		width: 180
		textLabel: "Select"
	}
	ButtonSq {
		id: button5775_3096
		x: 1200
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/bl.png"
		buttontext: "Select"
		clickHandler.onClicked: onActivate5775()
	}
	function onActivate5876() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 18228
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"2\" ")
	}
	StyledText {
		x: 1440
		y: 720
		height: 240
		width: 180
		textLabel: "Options"
	}
	ButtonSq {
		id: button5876_3097
		x: 1440
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Options"
		clickHandler.onClicked: onActivate5876()
	}
	function onActivate5396() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 17707
		setCurrentScreen("Screen_257.qml")
	}
	StyledText {
		x: 1680
		y: 360
		height: 240
		width: 180
		textLabel: "Thumbnail"
	}
	ButtonSq {
		id: button5396_3098
		x: 1680
		y: 360
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/tl.png"
		buttontext: "Thumbnail"
		clickHandler.onClicked: onActivate5396()
	}
	function onActivate1566() 
	{
	 // Calling Command 102 (Record) with CommandGroup_Parameters from 13636
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 102 ")
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 18381
		setCurrentScreen("Screen_<%=NP_R%>.qml")
	}
	ButtonSq {
		id: button1566_3099
		x: 1680
		y: 720
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/record.png"
		buttontext: ""
		clickHandler.onClicked: onActivate1566()
	}
	function onActivate3285() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 14143
		setCurrentScreen("Screen_20.qml")
	}
	StyledText {
		x: 1440
		y: 900
		height: 240
		width: 180
		textLabel: "Manage
playlist"
	}
	ButtonSq {
		id: button3285_3100
		x: 1440
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Manage
playlist"
		clickHandler.onClicked: onActivate3285()
	}
	function onActivate2028() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 9488
		setCurrentScreen("Screen_52.qml")
	}
	StyledText {
		x: 1200
		y: 900
		height: 240
		width: 180
		textLabel: "Resend
A/V
codes"
	}
	ButtonSq {
		id: button2028_3101
		x: 1200
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/standard.png"
		buttontext: "Resend
A/V
codes"
		clickHandler.onClicked: onActivate2028()
	}
	function onActivate3547() 
	{
	 // Calling Command 741 (Goto Screen) with CommandGroup_Parameters from 15050
		setCurrentScreen("Screen_87.qml")
	}
	StyledText {
		x: 960
		y: 900
		height: 240
		width: 180
		textLabel: "Book
marks"
	}
	ButtonSq {
		id: button3547_3102
		x: 960
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Book
marks"
		clickHandler.onClicked: onActivate3547()
	}
	function onActivate5623() 
	{
	 // Calling Command 87 (Goto Media Menu) with CommandGroup_Parameters from 17666
		manager.sendDceMessage(srouterip + " "+deviceid + " -106 1 87 64 \"1\" ")
	}
	StyledText {
		x: 720
		y: 900
		height: 240
		width: 180
		textLabel: "Monitor Mode"
	}
	ButtonSq {
		id: button5623_3103
		x: 720
		y: 900
		width: 240
		height: 180
		buttonbackground: "skins/Basic/Buttons/Media/adv_2.png"
		buttontext: "Monitor Mode"
		clickHandler.onClicked: onActivate5623()
	}
}