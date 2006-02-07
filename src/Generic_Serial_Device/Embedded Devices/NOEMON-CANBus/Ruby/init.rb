#Init 07-Feb-06 10:52
$AlarmsStatus=Hash.new
$RelaysStatus=Hash.new
$LightsStatus=Hash.new
$LedsStatus=Hash.new

$LightsNo=6       # 6
$RelaysNo=4      # 8
$AlarmsNo=4      #4

$pcBranch=0
$pcNo=3
$deviceBranch=0
$deviceNo=1

$branchList=Array.new

$eventHead
{
"AlarmOn"       => "T10C20002",
"AlarmOff"     =>  "T10820002"
}

$CmdAckHead={
"Alarms 1"    =>     "T1B810",
"Alarms 2"    =>     "T1B818",
"Relay"         =>     "T1B820",
"Lights"       =>     "T1B828",
"Leds"          =>     "T1B830"
}

$cmdState=1                      # 1 send a StatusReport, 2 wait a answer       
$cmdBuffer=Array.new
$bStartInterogate=false       # send a status report

#version
$softVersion="0"
$hardVersion="0"
$portName="0"

$bFlush = true
$waitTime = 4000      #wait time in comunication
$init=false

$line=String.new
$index=0

if ( device_.devdata_.has_key?(40)) then
	log( "Read PC branch:" + device_.devdata_[40].to_s + "\n" )
	$pcBranch = device_.devdata_[40].to_i;
end

if ( device_.devdata_.has_key?(144)) then
	log( "Read PC unit:" + device_.devdata_[144].to_s + "\n" )
	$pcNo = device_.devdata_[144].to_i;
end

if (device_.childdevices_ == nil) or (device_.childdevices_.empty?) then 
	$bInit = false
	log( "No children in database \n")
	
	createChildrenList()
else 
	$bInit = true 
	log( "Have children in database \n")
end

searchBranches()       # search all branches of childrens

CANInit()
NOEMStatus($pcBranch,$pcNo,$deviceBranch,$deviceNo)

