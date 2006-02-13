# 13-Feb-06 13:11 Receive command from child
log( "received command from child" + "\n" ) 
cmdId = cmd.id_
cmdTo = cmd.devidto_
cmdType= cmd.type_
childName=device_.childdevices_[cmdTo].devdata_[12]
childNo= childName.to_i
log( "CmdId:" + cmdId.to_s + "   " )
log( "CmdTo:" + cmdTo.to_s + "   " )
log( "CmdType:" + cmdType.to_s + "   " )
log( "Child name:" + childName + "\n" )

deviceBranch=0
deviceUnit=1

children=device_.childdevices_
children.each { |key,val| 
	#log( key.to_s + "   " + val.to_s + "\n" ) 
	
	if (childName == val.devdata_[12] ) then      #found the child
		
		if (val.devdata_.has_key?(40)) and (val.devdata_.has_key?(139)) then
			log( "Id:" + val.devdata_[12] + "   " )
			log( "Branch:" + val.devdata_[40] + "   " )
			log( "Unit:" + val.devdata_[139]  + "\n" )
			
			deviceBranch = val.devdata_[40].to_i
			deviceUnit = val.devdata_[139].to_i
		end
	end
	}
	
aux = deviceBranch * 256 + deviceUnit
if ( $branchList.include?(aux) == false ) then
	log( "New branch found" )
	$branchList.push(aux)
	NOEMStatus($pcBranch,$pcNo,deviceBranch,deviceUnit)
end

case cmdId
	when 192      #  OFF
	if (childNo<=$MaxLightId) then 
		NOEMTurnLight(childNo,true,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	else
		NOEMTurnRelay(childNo-$MaxLightId,true,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	end
	when 193      #  ON
	if (childNo<=$MaxLightId) then 
		NOEMTurnLight(childNo,false,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	else
		NOEMTurnRelay(childNo-$MaxLightId,false,$pcBranch,$pcNo,deviceBranch,deviceUnit )
	end
	
	when 184      #  SetLevel
	level = cmd.params_[76]
	log( "Level:" + level + "\n" )
	NOEMLightSetLevel(childNo,level.to_i,$pcBranch,$pcNo,deviceBranch,deviceUnit)
end
