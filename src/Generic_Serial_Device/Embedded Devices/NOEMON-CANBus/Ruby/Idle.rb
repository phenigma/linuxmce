#Idle 15-Feb-06 16:27
myTime = Time.new
strTime = myTime.strftime("%H:%M:%S  ")
diff = myTime - $startTime

if ( $cmdState == 2 ) then      #wait a answer
	log( "Wait answer:" + strTime + "   " )
	wait = diff.to_i
	log( "Diff:" + diff.to_s + "  " + wait.to_s + "\n" )

	if (  wait > $waitTime2 ) then
		log( "No answer from " )
		log( "Branch:" + $branchList[$currentBranch] + "\n\n" )
		
		$cmdState = 1
		$cmdBuffer=Array.new
		finishBranchInt()  
	end
end
