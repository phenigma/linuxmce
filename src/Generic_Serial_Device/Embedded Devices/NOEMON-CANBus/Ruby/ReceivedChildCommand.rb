# 17-Feb-06 13:38 Receive command from child
if ( $bInit == false ) then
	log( "The initialisation is not finish yet\n")
	return
end


log( "Received command from child" + "\n" ) 
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

val = device_.childdevices_[cmdTo]
if (val.devdata_.has_key?(40)) and (val.devdata_.has_key?(144)) then
	log( "Id:" + val.devdata_[12] + "   " )
	log( "Branch:" + val.devdata_[40] + "   " )
	log( "Unit:" + val.devdata_[144]  + "\n" )
			
	deviceBranch = val.devdata_[40].to_i
	deviceUnit = val.devdata_[144].to_i
end

aux = deviceBranch * 256 + deviceUnit
idBranch = "%04u" % aux

if ( $branchList.include?(idBranch) == false ) then
	log( "New branch found" )
	$branchList.push(aux)
	NOEMONStatus($pcBranch,$pcNo,deviceBranch,deviceUnit)
end

case cmdId
	when 192      #  OFF
	if (childNo<=$MaxLightId) then 
		NOEMONTurnLight(childNo,true,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	else
		NOEMONTurnRelay(childNo-$MaxLightId,true,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	end
	when 193      #  ON
	if (childNo<=$MaxLightId) then 
		NOEMONTurnLight(childNo,false,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	else
		NOEMONTurnRelay(childNo-$MaxLightId,false,$pcBranch,$pcNo,deviceBranch,deviceUnit )
	end
	
	when 184      #  SetLevel
	level = cmd.params_[76]
	log( "Level:" + level + "\n" )
	NOEMONLightSetLevel(childNo,level.to_i,$pcBranch,$pcNo,deviceBranch,deviceUnit)
end
