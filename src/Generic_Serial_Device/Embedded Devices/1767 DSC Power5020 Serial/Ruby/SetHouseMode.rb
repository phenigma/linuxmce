#SetHouseMode  09-Jun-06 11:40  ApexDestiny 6100
#log parameters
log( "SetHouseMode:" + "  " )
log( "Value:"         +   value_to_assign           + "  " )
log( "User:"           +   pk_users                    + "  " )
log( "Password:"     +  password                    + "  " )
log( "Device:"        +  pk_devicegroup            + "  "  )
log( "Handling:"     +  handling_instructions     + "\n" )

#check parameters
if ($partMapping.has_key?(pk_devicegroup) == false) and (pk_devicegroup != "0") then 
	badParam("SetHouseMode" ,"Bad device group")  
	return
end

valueNo=value_to_assign.to_i
if (valueNo < 1) or (valueNo > 6)  then 
	badParam("SetHouseMode" ,"Value to assign") 
	return
end

partList=Array.new
#construct list of partition
if (pk_devicegroup == "0") then 
	log( "All partition" + "\n" )
	partList = $partStatus.keys
else 
	log( "Only a partition" + "\n" )
	partList.push( $partMapping[pk_devicegroup] ) 
end

log( "Size:" + partList.size.to_s + " Partition list: " )
partList.each { |i| log( i.to_s + " ") }
log( "\n" )

case valueNo
when 1      #unarmed at home
	ApexDisarmPartition("1",password) 
when 2      # armed away
	ApexArmPartition("AWAY","1",password ) 
when 3      # armed at home
	DSCArmPartition("HOME","1",password) 
when 4      # sleeping
	log( "Case 4:\n" )
when 5      # entertaining
	log( "Case 5:\n" )
when 6      # armed extended away
	ApexArmPartition("AWAY","1",password ) 
end

end