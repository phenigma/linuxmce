# 02-March-06 16:31 Receive command from child
if ( $bInit == false ) then
	log( "The initialisation is not finish yet\n")
	return
end

log( "Received command from child" + "   " ) 
logTime = Time.now
timeStr = logTime.strftime("%d-%m-%Y  %H:%M:%S  ")
log( "Time:" + timeStr.to_s + "\n" )


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
if ( val.devdata_.has_key?(12) ) then
	childData = val.devdata_[12]
	childList = childData.split( "," )
	if ( childList.size() < 3 ) then
		log( "Wrong format " + childData + "\n" )
		return
	end
	
	log( "Read child data" + "   " )
	deviceBranch  =  childList[0].to_i 
	deviceUnit      =  childList[1].to_i
	childNo          =  childList[2].to_i
else
	log( "Couldn't read child data id:" + "\n" )
	return
end

log( "Child name:" + childNo.to_s + "   " )
log( "Device branch:" + deviceBranch.to_s + "   " )
log( "Device unit:" + deviceUnit.to_s  + "\n" )

aux = deviceBranch * 256 + deviceUnit
idBranch = "%04u" % aux

if ( $branchList.include?(idBranch) == false ) then
	log( "New branch found" + idBranch.to_s + "\n" )
	return 
	#$branchList.push(aux)
	#NOEMONStatus($pcBranch,$pcNo,deviceBranch,deviceUnit)
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
