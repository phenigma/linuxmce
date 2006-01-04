def ReportChildDevice
#ReportChildDevice 03-Ian-06 17:38
$sensorStatus=Hash.new
$sensorType=Hash.new
$partStatus=Hash.new

$panelStatus=2

$logFile.print "ReportChildDevice\n"

#DSCSetDescArm($bDescArm)
DSCSetTimeStamp(true)
DSCSetTimeBroadcast(true)

$panelStatus=1	#??
if $bInit == true then 
	$logFile.print "Already intit. Not send StatusReport" + "\n"
else
	DSCStatusReport2()
end

if $bFlush then $logFile.flush end
end