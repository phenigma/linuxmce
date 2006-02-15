#event 15-Feb-06 17:50
buff=readLine()
bFind=false

$eventHead.each{ |key,value|
	if( buff.size >= value.size ) then 
		head=buff[0..value.size-1]
		#log( "Head:" + head + "   " )
		#log( "Value:" + value + "\n" )
		if (head == value) then
			bFind=true
			NOEMONEvent( buff , key )
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
			NOEMONCmdAck( buff , key )
			break
		end
	end
}

if (bFind==false) then
	log( "Could not interpret received:" + "\n" ) 
end
log( "\n" )
