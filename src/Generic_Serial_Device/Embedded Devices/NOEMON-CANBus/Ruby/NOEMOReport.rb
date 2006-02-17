#NOEMOReport 15-Feb-06 17:25
log( "Call NOEMON status" + arguments + "  " )

if (arguments.empty? == false) 
	res=arguments.split( "," )
	if( res.size > 2 ) then
		deviceBranch = res[1].to_i
		deviceNo      = res[2].to_i
		
		log( "Unit branch:" + deviceBranch.to_s + "  " )
		log( "Unit no:" + deviceNo.to_s + "\n" )
	end
	NOEMStatus($pcBranch,$pcNo,deviceBranch,deviceNo)
end
logReport()
