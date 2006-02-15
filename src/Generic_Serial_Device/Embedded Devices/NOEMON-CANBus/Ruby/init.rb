#Init 15-Feb-06 17:20
#$logFile = File.new("g:/CanBUS.log","w")
$logFile = File.new("/var/log/pluto/CanBus.log", "w")
$bFlush = true
$waitTime = 4000      #wait time in comunication

$line=String.new
$index=0

$cmdState = 1                      # 1 send a StatusReport, 2 wait a answer       
$cmdBuffer = Array.new

#NOEMON 
$AlarmsStatus=Hash.new
$RelaysStatus=Hash.new
$LightsStatus=Hash.new
$LedsStatus=Hash.new

$LightsNo=6       # 6
$RelaysNo=4      # 8
$AlarmsNo=4      #4
$MaxLightId=200     #used to detect type of child not very well

#hard coded version. 
$pcBranch=0
$pcNo=3
$deviceBranch=0
$deviceNo=1

$branchList=Array.new
$currentBranch=0
$startTime=Time.new
$waitTime2=45        #time in second for wait 

$eventHead={
"AlarmOn"       => "T10C20002",
"AlarmOff"     =>  "T10820002"
}

$cmdAckHead={
"Alarms 1"    =>     "T1B810",
"Alarms 2"    =>     "T1B818",
"Relay"         =>     "T1B820",
"Lights"       =>     "T1B828",
"Leds"          =>     "T1B830"
}

#version
$softVersion="0"
$hardVersion="0"
$portName="0"

$bInit = false
$bHaveChilds = false
$bStartInterogate = false       # send a status report ??

if ( device_.devdata_.has_key?(40)) then
	log( "Read PC branch:" + device_.devdata_[40].to_s + "\n" )
	$pcBranch = device_.devdata_[40].to_i;
else
	log( "Couldn't read PC branch:" + "\n" )
end

if ( device_.devdata_.has_key?(144)) then
	log( "Read PC unit:" + device_.devdata_[144].to_s + "\n" )
	$pcNo = device_.devdata_[144].to_i;
else
	log( "Couldn't read PC unit:" + "\n" )
end

if (device_.childdevices_ == nil) or (device_.childdevices_.empty?) then 
	$bHaveChilds = false
	log( "No children in database \n")
	
	createChildrenList()
else 
	$bHaveChilds = true 
	log( "Have children in database \n")
end

CANInit()
NOEMONInit()

