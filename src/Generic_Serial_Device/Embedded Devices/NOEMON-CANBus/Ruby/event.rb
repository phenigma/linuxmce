#event 07-Feb-06 10:52
buff=readLine()
bFind=false

$eventHead.each{ |key,value|
	if( buff.size >= value.size ) then 
		head=buff[0..value.size-1]
		#log( "Head:" + head + "   " )
		#log( "Value:" + value + "\n" )
		if (head == value) then
			bFind=true
			NOEMEvent( buff , key )
			break
		end
	end
}

$cmdAckHead.each{ |key,value|
	if( buff.size >= value.size ) then 
		head=buff[0..value.size-1]
		#log( "Head:" + head + "   " )
		#log( "Value:" + value + "\n" )
		if (head == value) then
			bFind=true
			NOEMCmdAck( buff , key )
			break
		end
	end
}

if (bFind==false) then
	log( "Could not interpret received:" + "\n" ) 
end
log( "\n" )
