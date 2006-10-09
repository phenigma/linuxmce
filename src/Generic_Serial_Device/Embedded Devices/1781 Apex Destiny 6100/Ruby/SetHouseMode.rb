
case value_to_assign.to_i
when 1      #unarmed at home
	ApexDisarmPartition("1",password) 
when 2      # armed away
	ApexArmPartition("AWAY","1",password) 
when 3      # armed at home
	ApexArmPartition("HOME","1",password) 
when 4      # sleeping
	log( "Case 4:\n" )
when 5      # entertaining
	log( "Case 5:\n" )
when 6      # armed extended away
	ApexArmPartition("AWAY","1",password)
end
