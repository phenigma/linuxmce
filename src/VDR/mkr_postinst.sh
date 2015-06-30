#!/bin/bash
/usr/pluto/bin/Debug_LogKernelModules.sh "$0" || :

# Get the device ID of this system
. /usr/pluto/bin/Config_Ops.sh
ConfEval
DCEROUTERIP=`ping $DCERouter -c1 -n | grep PING | cut -f2 -d"(" | cut -f1 -d")"`
DCENETWORK="`echo $DCEROUTERIP | cut -f-3 -d"."`.0"
# Get the video dir of VDR
VIDEO_DIR="/home/public/data/pvr"

# If the user already overwrote the default, get it.
. /etc/default/vdr || :

# Disable shutdown by VDR
if ! grep -q "^ENABLE_SHUTDOWN=0" /etc/default/vdr; then
	echo "ENABLE_SHUTDOWN=0" >> /etc/default/vdr
fi

# Set default SVDRP port to 2001
if ! grep -q "^SVDRP_PORT=2001" /etc/default/vdr; then
	echo "SVDRP_PORT=2001" >> /etc/default/vdr
fi

# Set default charset to utf-8 to permit non-english characters
if ! grep -q "^VDR_CHARSET_OVERRIDE=UTF-8" /etc/default/vdr; then
	echo "VDR_CHARSET_OVERRIDE=UTF-8" >> /etc/default/vdr
fi

# We can't have VDR running during setup file edit
invoke-rc.d vdr stop || :

# If we have an entry for svdrpservice already, we don't touch the setup.conf
if grep -q "^svdrpservice" /var/lib/vdr/setup.conf; then
	exit 0
fi

# Setup a custom nosignal image
mkdir -p /root/.xine
ln -s /usr/pluto/share/nosignal.mpg /root/.xine/nosignal.mpg || :

#
# Make a backup copy of the existing setup.conf
#
if [ -f /var/lib/vdr/setup.conf ]; then
	cp /var/lib/vdr/setup.conf `tempfile -d /var/lib/vdr -p setup -s \.conf`
fi

# The setup.conf defaults:
echo "AntiAlias = 1
AudioLanguages =
ChannelBlocker = 0
ChannelBlockerList =
ChannelBlockerMode = 0
ChannelEntryTimeout = 1000
ChannelInfoPos = 0
ChannelInfoTime = 5
CurrentChannel = 4
CurrentDolby = 0
CurrentVolume = 255
CutTimePatchEnabled = 0
DefaultLifetime = 99
DefaultPriority = 50
DelTimeshiftRec = 0
DisableVPS = 0
DiSEqC = 0
DisplaySubtitles = 0
DolbyTransferFix = 1
DoubleEpgAction = 0
DoubleEpgTimeDelta = 15
EmergencyExit = 1
EPGBugfixLevel = 3
EPGLanguages =
EPGLinger = 0
EPGScanTimeout = 5
FontFix = Courier:Bold
FontFixSize = 18
FontFixSizeP = 0.031000
FontOsd = Sans Serif:Bold
FontOsdSize = 22
FontOsdSizeP = 0.038000
FontSml = Sans Serif
FontSmlSize = 20
FontSmlSizeP = 0.035000
HardLinkCutter = 0
InitialChannel = 0
InitialVolume = -1
InstantRecordTime = 180
JumpPlay = 0
LnbFrequHi = 10600
LnbFrequLo = 9750
LnbSLOF = 11700
MarginStart = 2
MarginStop = 10
MarkInstantRecord = 1
MaxRecordingSize = 100
MaxVideoFileSize = 2000
MenuCmdPosition = 0
MenuKeyCloses = 0
MenuScrollPage = 1
MenuScrollWrap = 0
MinEventTimeout = 30
MinUserInactivity = 300
MixEpgAction = 0
MultiSpeedMode = 0
NameInstantRecord = TITLE EPISODE
NextWakeupTime = 1264359900
noEPGList =
noEPGMode = 0
NoQamOnDecoder = 0
OSDAspect = 1.422222
OSDHeight = 484
OSDHeightP = 0.840000
OSDLanguage =
OSDLeft = 58
OSDLeftP = 0.080000
OSDMessageTime = 1
OSDSkin = sttng
OSDTheme = default
OSDTop = 46
OSDTopP = 0.080000
OSDWidth = 624
OSDWidthP = 0.870000
PauseKeyHandling = 2
PauseLastMark = 0
PauseLifetime = 1
PausePriority = 10
PlayJump = 0
PrimaryDVB = 5
PrimaryLimit = 0
RecordDolbyDigital = 1
RecordingDirs = 1
ReloadMarks = 0
ResumeID = 0
SetSystemTime = 0
ShowInfoOnChSwitch = 1
ShowProgressBar = 0
ShowRecDate = 1
ShowRecLength = 0
ShowRecTime = 1
ShowReplayMode = 0
SplitEditedFiles = 0
SubtitleBgTransparency = 0
SubtitleFgTransparency = 0
SubtitleLanguages =
SubtitleOffset = 0
SVDRPTimeout = 300
TimeoutRequChInfo = 1
TimeSource = 0
TimeTransponder = 0
TurnOffPrimary = 0
UpdateChannels = 5
UseDolbyDigital = 1
UseSmallFont = 1
UseSubtitle = 1
UseSyncEarlyPatch = 0
UseVps = 0
VideoDisplayFormat = 1
VideoFormat = 0
VpsMargin = 120
ZapTimeout = 3
" > /var/lib/vdr/setup.conf

# First we add configuration settings for the plugins
#
# vdr-plugin-svdrpservice
#
echo "
svdrpservice.ConnectTimeout = 2
svdrpservice.ReadTimeout = 5
svdrpservice.ServerIp = $DCErouter
svdrpservice.ServerPort = 2001
" >> /var/lib/vdr/setup.conf
#
#
# vdr-plugin-streamdev-server
if [[ "$PK_Device" -eq "1" ]]; then
	echo "
streamdev-server.AllowSuspend = 1
streamdev-server.MaxClients = 6
streamdev-server.StartHTTPServer = 1
streamdev-server.StartServer = 1
streamdev-server.SuspendMode = 1
" >> /var/lib/vdr/setup.conf
	echo "
$DCENETWORK/24
"
>> /etc/vdr/plugins/streamdevhosts.conf

else
	echo "
streamdev-client.RemoteIp = $DCERouter
streamdev-client.RemotePort = 2004
streamdev-client.StartClient = 1
streamdev-client.StreamFilters = 1
streamdev-client.SyncEPG = 0
" >> /var/lib/vdr/setup.conf
fi
# vdr-plugin-xineliboutput
echo "
xineliboutput.OSD.AlphaCorrection = 0
xineliboutput.OSD.AlphaCorrectionAbs = 0
xineliboutput.OSD.Blending = 0
xineliboutput.OSD.BlendingLowRes = 0
xineliboutput.OSD.DvbSubtitles = 0
xineliboutput.OSD.ExtSubSize = -1
xineliboutput.OSD.LayersVisible = 4
xineliboutput.OSD.Scaling = 2
xineliboutput.Post.denoise3d.Enable = 0
xineliboutput.Post.pp.Enable = 0
xineliboutput.Post.unsharp.Enable = 0
xineliboutput.Video.AspectRatio = 0
xineliboutput.Video.AutoCrop = 0
xineliboutput.Video.Decoder.H264 = automatic
xineliboutput.Video.Decoder.H264.SkipLoopFilter = all
xineliboutput.Video.Decoder.H264.SpeedOverAccuracy = yes
xineliboutput.Video.Decoder.MPEG2 = libmpeg2
xineliboutput.Video.Deinterlace = none
xineliboutput.Video.Driver = xv
xineliboutput.Video.FieldOrder = 0
xineliboutput.Video.Overscan = 0
xineliboutput.Video.Scale = 0
xineliboutput.Video.SwScale = 0" >> /var/lib/vdr/setup.conf

#
# for the MediaDirector
#
if [[ "$PK_Device" -ne "1" ]]; then
	echo "
remotetimers.AddToRemote = 1
remotetimers.DefaultUser = 0
remotetimers.HideMainMenuEntry = 0
remotetimers.RemoteInstant = 0
remotetimers.RemotePause = 0
remotetimers.ReplaceRecordings = 0
remotetimers.ReplaceSchedule = 1
remotetimers.ReplaceTimers = 1
remotetimers.ServerDir = $VIDEO_DIR
remotetimers.ServerIp = $DCERouter
remotetimers.ServerPort = 2001
remotetimers.ShowProgressBar = 0
remotetimers.SwapOkBlue = 0
remotetimers.UserFilterRecordings = 0
remotetimers.UserFilterSchedule = 0
remotetimers.UserFilterTimers = 0
remotetimers.WatchUpdate = 1
">> /var/lib/vdr/setup.conf
fi

#
# Setup a remote.conf file, but make a backup of the original.
#
if [ -f /var/lib/vdr/remote.conf ]; then
        cp /var/lib/vdr/remote.conf `tempfile -d /var/lib/vdr -p remote -s \.conf`
fi
echo "
LIRC.Up         Xup
LIRC.Down       Xdown
LIRC.Menu       Xmenu
LIRC.Ok         Xok
LIRC.Back       Xback
LIRC.Left       Xleft
LIRC.Right      Xright
LIRC.Red        Xrot
LIRC.Green      Xgruen
LIRC.Yellow     Xgelb
LIRC.Blue       Xblau
LIRC.0          X0
LIRC.1          X1
LIRC.2          X2
LIRC.3          X3
LIRC.4          X4
LIRC.5          X5
LIRC.6          X6
LIRC.7          X7
LIRC.8          X8
LIRC.9          X9
LIRC.Info       Xtv
LIRC.Play       Xplay
LIRC.Pause      Xpause
LIRC.Stop       Xstop
LIRC.Record     Xrec
LIRC.FastFwd    Xforwd
LIRC.FastRew    Xrev
LIRC.Next       Xrskip
LIRC.Prev       Xlskip
LIRC.Channel+   Xch+
LIRC.Channel-   Xch-
LIRC.Volume+    Xvol+
LIRC.Volume-    Xvol-
LIRC.Mute       Xmute
LIRC.Schedule   Xstern
LIRC.Channels   Xprevch
LIRC.Timers     Xraute
LIRC.Recordings Xvideos
LIRC.User1      Xguide
LIRC.User2      Xmusic
LIRC.User3      Xpics
LIRC.User4      Xradio
LIRC.User5      Xpower

KBD.Up         00000000001B5B41
KBD.Down       00000000001B5B42
KBD.Menu       000000001B5B337E
KBD.Ok         000000000000000D
KBD.Back       000000000000007F
KBD.Left       00000000001B5B44
KBD.Right      00000000001B5B43
KBD.Red        000000001B5B5B41
KBD.Green      000000001B5B5B42
KBD.Yellow     000000001B5B5B43
KBD.Blue       000000001B5B5B44
KBD.0          0000000000000030
KBD.1          0000000000000031
KBD.2          0000000000000032
KBD.3          0000000000000033
KBD.4          0000000000000034
KBD.5          0000000000000035
KBD.6          0000000000000036
KBD.7          0000000000000037
KBD.8          0000000000000038
KBD.9          0000000000000039
KBD.Info       0000000000000020
KBD.Volume+    000000000000002B
KBD.Volume-    000000000000002D
KBD.Mute       000000000000006D
KBD.Schedule   0000001B5B32307E
KBD.Channels   000000001B5B347E
KBD.Timers     0000001B5B32317E
KBD.Recordings 000000001B5B5B45
KBD.Setup      0000001B5B32347E
KBD.Commands   0000001B5B32337E
KBD.User1      0000001B5B31387E
KBD.User2      0000001B5B31397E
KBD.User3      0000001B5B31377E

XKeySym.Up         Up
XKeySym.Down       Down
XKeySym.Menu       Delete
XKeySym.Ok         Return
XKeySym.Back       BackSpace
XKeySym.Left       Left
XKeySym.Right      Right
XKeySym.Red        F1
XKeySym.Green      F2
XKeySym.Yellow     F3
XKeySym.Blue       F4
XKeySym.0          0
XKeySym.1          1
XKeySym.2          2
XKeySym.3          3
XKeySym.4          4
XKeySym.5          5
XKeySym.6          6
XKeySym.7          7
XKeySym.8          8
XKeySym.9          9
XKeySym.Info       i
XKeySym.Pause      space
XKeySym.FastFwd    F6
XKeySym.FastRew    F5
XKeySym.Volume+    KP_Add
XKeySym.Volume-    KP_Subtract
" > /var/lib/vdr/remote.conf        

# We restart VDR after the install on the core
if [[ "$PK_Device" -eq "1" ]]; then
	error=0
        service vdr restart || error=1
        if [[ "$error" -eq "1" ]]; then
		cp /usr/share/doc/vdr/examples/vdr-init.d /etc/init.d/vdr || :
		chmod +x /etc/init.d/vdr || :
		/etc/init.d/vdr start || :
		error=0
	fi
fi
