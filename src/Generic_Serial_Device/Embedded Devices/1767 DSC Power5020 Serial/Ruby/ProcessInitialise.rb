#ProcessInitialise  09-Jun-06 11:40  ApexDestiny 6100

#create log file
#$logFile = File.new("/var/log/pluto/DSC_Power.log", "w")
$logFile = File.new("/var/log/pluto/ApexDestiny6100.log", "w")
#global variables
$waitTime = 400      #wait time in comunication
$bFlush = true        #log imediatly every event,comand

#panel settings
$MaxSensor=96
$MaxPartition=8
$MinLineLength=8
$MaxLineLength=106
$bDescArm=false
$bTimeStamp=false
$bTimeBroadcast=false

$bPartDetect=true    #?

#buffer for reading from serial
$line=String.new
$lineIndex=0

#sensor and partition state
$sensorStatus=Hash.new
$sensorType=Hash.new
$partStatus=Hash.new
#partition mapping
$partMapping=Hash.new
#description of partition and zone
$partStr=String.new
$zoneStr=String.new

#state of panel  0-start,1-init,2-ready to receive command,3 analize to receivce 
$panelState=0
$bInit=false
#command buffer and 
$cmdBuffer=Array.new
$bCmdSuccess=true
$timeNo=0
$lastCmdTime=Time.now()

#$myIo = MyIO.new

$cmdCode = { 
"001"    =>    "cmd"
}

$errCode = {
"aa"     =>  "",
"bb"     =>  ""
}

#if device has childs already initialise
$partStatus[1]=true
if (device_.childdevices_ == nil) or (device_.childdevices_.empty?) then 
	$bInit = false
else 
	$bInit = true 
end

log( "Starting Apex Destiny 6100\n" )
logState( true )

if (device_.devdata_ != nil) then 
	if (device_.devdata_[135] != nil) and ( device_.devdata_[135].empty? == false) then
		$logFile.print "Mapping:" + device_.devdata_[135] + "\n"
		mapp( device_.devdata_[135] )
	else
		$logFile.print "Mapping with empty string" + "\n"
		mapp("")
	end
else
	$logFile.print "Run without devicedata " + "\n"
	mapp( "" )	
end
#mapp("")

log("Start interogation" + "\n")
ApexArmingRequest()
ApexStatusRequest()
ApexZoneRequest()

log( "\n\n" )


