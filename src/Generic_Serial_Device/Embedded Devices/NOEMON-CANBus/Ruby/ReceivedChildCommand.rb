# 27-Feb-06 15:50 Receive command from child
if ( $bInit == false ) then
	log( "The initialisation is not finish yet\n")
	return
end


log( "Received command from child" + "\n" ) 
cmdId = cmd.id_
cmdTo = cmd.devidto_
cmdType= cmd.type_
childType = device_.childdevices_[cmdTo].devtemplid_
log( "CmdId:" + cmdId.to_s + "   " )
log( "CmdTo:" + cmdTo.to_s + "   " )
log( "CmdType:" + cmdType.to_s + "   " )
log( "Child type:" + childType.to_s + "\n" )

childName=String.new
deviceBranch=0
deviceUnit=1

val = device_.childdevices_[cmdTo]
#try to read name from 138 or 12
if ( val.devdata_.has_key?(138) ) then
	childName = val.devdata_[138]
else
	if ( val.devdata_.has_key?(12) ) then
		childName = val.devdata_[12]
	else
		log( "Couldn't read device id:" + "\n" )
		return
	end
end

if ( val.devdata_.has_key?(40) ) then
	deviceBranch = val.devdata_[40].to_i
else
	log( "Couldn't read device branch:" + "\n" )
	return
end

if ( val.devdata_.has_key?(147) ) then
	deviceUnit = val.devdata_[147].to_i
else
	log( "Couldn't read devide unit:" + "\n" )
	return
end

childNo= childName.to_i
log( "Child name:" + childName + "   " )
log( "Device branch:" + deviceBranch.to_s + "   " )
log( "Device unit:" + deviceUnit.to_s  + "\n" )

aux = deviceBranch * 256 + deviceUnit
idBranch = "%04u" % aux

if ( $branchList.include?(idBranch) == false ) then
	log( "New branch found" )
	$branchList.push(aux)
	NOEMONStatus($pcBranch,$pcNo,deviceBranch,deviceUnit)
end

#    38  is the device template for lights
# 1780   is the device template for StandardIrrigationSprinkler
# 1786   is the device template for StandardRelayLight 
case cmdId
	when 192      #  OFF command
	if ( childType == 38 ) then 
		NOEMONTurnLight(childNo,true,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	end
	
	# StandardSprinkler  or StandardRelayLight 
	if ( childType == 1780 ) or ( childType == 1786 ) then
		NOEMONTurnRelay(childNo,true,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	end
	when 193      #  ON command
	if ( childType == 38 ) then 
		NOEMONTurnLight(childNo,false,$pcBranch,$pcNo,deviceBranch,deviceUnit)
	end
	
	# StandardSprinkler  or StandardRelayLight 
	if ( childType == 1780 ) or ( childType == 1786 ) then
		NOEMONTurnRelay(childNo,false,$pcBranch,$pcNo,deviceBranch,deviceUnit )
	end
	
	when 184      #  SetLevel  command
	level = cmd.params_[76]
	log( "Level:" + level + "\n" )
	NOEMONLightSetLevel(childNo,level.to_i,$pcBranch,$pcNo,deviceBranch,deviceUnit)
end
