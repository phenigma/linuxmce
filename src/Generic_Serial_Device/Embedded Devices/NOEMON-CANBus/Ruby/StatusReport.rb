#StatusReport 17-Feb-06 12:15
log( "Call Status Report:" + arguments + "  " )

if (arguments.empty? == false) 
	res=arguments.split( "," )
	if( res.size > 2 ) then
		deviceBranch = res[0].to_i
		deviceNo      = res[1].to_i
		
		log( "Device branch:" + deviceBranch.to_s + "  " )
		log( "Device no:" + deviceNo.to_s + "\n" )
		logReport()
	else
		logReport()
	end
	
end



