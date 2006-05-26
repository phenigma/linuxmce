#Process IDLE  26-May-06 17:29 Power5020
myTime = Time.now
timeStr = myTime.strftime("%d-%m-%Y  %H:%M:%S  ")
diffTime = myTime - $lastCmdTime

if (  diffTime > 5 ) then
	log( "No answer from previous command" )
	log( "Start:" + $lastCmdTime.strftime("%d-%m-%Y  %H:%M:%S  ") + "Finish:" + timeStr)
	$cmdState = 2
	#$cmdBuffer=Array.new
end
end
