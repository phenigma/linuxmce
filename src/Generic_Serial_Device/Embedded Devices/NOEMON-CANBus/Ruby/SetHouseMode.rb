#SetHouseMode 25-Ian-06 16:34
#log parameters
log( "SetHouseMode:" + "  " )
log( "Value:"         +   value_to_assign           + "  " )
log( "User:"           +   pk_users                    + "  " )
log( "Password:"     +  password                    + "  " )
log( "Device:"        +  pk_devicegroup            + "  "  )
log( "Handling:"     +  handling_instructions     + "\n" )

valueNo=value_to_assign.to_i
if (valueNo < 1) or (valueNo > 6)  then 
	#badParam("SetHouseMode" ,"Value to assign") 
	return
end

log( "\n" )

case valueNo
when 1      #unarmed at home
	for i in 1..5
		NOEMONArmInput(i)
	end	
when 2      # armed away
	for i in 1..5
		NOEMONArmInput(i)
	end
	
when 3      # armed at home
	log( "Case 3:\n" )
when 4      # sleeping
	log( "Case 4:\n" )
when 5      # entertaining
	log( "Case 5:\n" )
when 6      # armed extended away
	log( "Case 5:\n" )
end
