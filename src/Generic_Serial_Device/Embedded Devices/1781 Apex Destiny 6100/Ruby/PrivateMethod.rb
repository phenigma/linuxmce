#PrivateMethod  09-Jun-06 15:20  ApexDestiny 6100
def logString(word)
log( "Line size:" + word.size.to_s + "  String:" )
word.each_byte{ |i| 
	log( i.to_s + "  " )
}

log( "\n" )
end

def badParam(funcName,paramName)
log( "Bad parameter in: " + funcName + " Parmeter:" + paramName + "\n" )
end

def logState(bSensors)
logTime = Time.now
timeStr = logTime.strftime("%d-%m-%Y  %H:%M:%S  ")

log( "PC5401 state:" + "\n" )
log( "Machine time:" + timeStr + "\n" )
log( "Max partition:" + $MaxPartition.to_s + "   Max sensor:" + $MaxSensor.to_s + "\n" )
log( "Max line length:" + $MaxLineLength.to_s + "   "  )
log( "Min line length:" + $MinLineLength.to_s + "\n" )

log( "PC5401 State:" + $panelState.to_s + " Init:" + $bInit.to_s + "\n"  )
log( "TimeStamp:" + $bTimeStamp.to_s + " TimeBroadcast:" + $bTimeBroadcast.to_s + "   " )
log( "Descriptive arm:" + $bDescArm.to_s )

if (bSensors==true) then
	#log zone and partition
	log( "Partitions list: " + $partStr + "\n" )
	log( "Sensors list: " + $zoneStr + "\n" )

	#log open zone
	log( "Sensor open:" )
	$sensorStatus.each{ |key, value|
		log( key.to_s + " -> " + value.to_s )
		if $sensorType.has_key?(key) then log( " -> " + $sensorType[key].to_s ) end
	}
end

log( "\n" )
end

def getCmdType(code)
cmdNo=code.to_i
if (cmdNo < 500) then return "Command:" end
if (cmdNo >= 500) and (cmdNo <= 502) then return "Cmd_ack:"  end
if (cmdNo > 502) then return "Event:"  end
end

def logCmd(code)
logState(false)

log( getCmdType(code) + code.to_s + "  " )
if $cmdCode.has_key?(code) then 
	log( $cmdCode[code] )
else 
	log( "Unknown code" )
end
log( "\n\n" )
end

def mapp(zones)      #mapping Partition -> Zones
list=zones.split(",")
list.each{ |i|
	list2=i.split("=")

	if (list2.size != 2) then 
		badParam( "Mapp", "Zones" ) 
		next 
	end      #bad format string

	# add information
	part=list2[1] 
	zone=list2[0]
	$partMapping[zone]=part
}

end

def buildMess(buff)
len = "%02X" % (buff.size() + 6)
buff = len + buff + "00"
buff += checkSumProc(buff) + "\r" + "\n"
end

def checkSumProc(buff)
count=0   
buff.each_byte{ |i|
	count += i
	#log( i.to_s + ":" + count.to_s + "\n" )
}
#calculate checksum
count=256-(count%256)
return "%02X" % count
end

#IO operation
def readLine
$index=0
$line=String.new

cod=conn_.Recv(1, $waitTime)
$line+=cod
while cod[0,1] != "\n" do
	if (conn_ != nil) then 
		cod=conn_.Recv(1, $waitTime)
		$line += cod
	else
		log( "Can not read from serial " + "\n" )
	end
end
log( "Read line " )
logString($line)

#check if line is well formated		      
#if ($line.size() >$MaxLineLength) or ($line.size() < $MinLineLength) then
if($line.size() > 106 ) or ($line.size() < 8) then
	log( "Error reading line. Line size to short or long:" + $line.size.to_s + "\n" )
	#send error ...
end	

len=String.new
len=$line[0..1]
chkSum=$line[$line.size()-2,2]
$line=$line[0,$line.size()-2]
log( "Line size:" + len.to_s + "\n" )
log( "CheckSum:" + chkSum + "\n" )

if( chkSum != checkSumProc($line) ) then
	log( "Error reading line. Bad checksum" +  "\n" )
end
if( len.hex != ($line.size()+2) ) then
	log( "Error reading line. Bad size" +  "\n" )
end

#removed "00" reserved and length
$line=$line[2..$line.size()-3]
log( $line )
end

def readByte(val)
valNo=val.to_i
if (valNo + $index <= $line.size) then
	buff = $line[$index,valNo]
	$index+=valNo
else 
	log( "Error reading   " )
	log( "Try to read:" + valNo.to_s + " bytes from index:" + $index.to_s + "   " )
	log( "Total size:" + $line.size.to_s + "\n" )
	buff=String.new
end

return buff
end

def send(buff)
if conn_.nil? then
	log( "Problem with sending data to serial!!! \n" )
else
	conn_.Send(buff)
end
end

def sendCmd(buff)
buff2=checkSumProc(buff)

if ($panelState == 3) then    #wait to finish another command
	$cmdBuffer.push(buff2)
	log( "Adding cmd to buffer" + "   " )
else                                #ready to process another command
	send( buff2 )
	$panelState=3
	log( "Send comand" + "   " )
	$lastCmdTime=Time.now()
end

logString(buff2)
end

def sendCmd2()
log( "Try to send next command from buffer." )

if $cmdBuffer.empty? then      #execute first command from buffer
	log( "Buffer is empty\n" )
	if ($panelState == 3) then $panelState=2 end 
else                                    #buffer empty
	log( "Sending comand from buffer.\n" )
	send( $cmdBuffer.first )
	logString($cmdBuffer[0])

	$cmdBuffer.delete_at(0)
end

#return buff2
end

#status true if tripped
def addSens(val,status,type)
if $sensorStatus.has_key?(val) then 
	log( "Already exist " + val.to_s + "\n" )
else 
	$sensorStatus[val]=status 
	$sensorType[val]=type 

	#add to zone string
	#[internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
	if ($bPartDetect == false ) then       #more than 8 sensor
		auxStr=String.new
		auxStr += device_.devid_.to_s + "\t" + val.to_s + "\t" + "room1" + "\t" + type.to_s + "\t" + "3" + "\n" 
		$zoneStr+= auxStr

		log( "Aux:"        + auxStr     +  "\n"  )
		log( "zoneStr:"  + $zoneStr  +  "\n" )
	end
end      #end new sensor found  

if ($bInit == true) then        #fire sensor trip only after adding child
	valNo=val.to_i
	idNo=-1

	log( "Log children list:" + "\n" )
	children=device_.childdevices_
	children.each{ |key, value|
		strAux=value.devdata_[12]
		log( "Child id:" + key.to_s + "  " )
		log( "Child name:" +  strAux + "  "  )
		log( "Search name:" + valNo.to_s + "\n" )

		if (valNo.to_s == strAux) then 
			log( "Found child with name:" + val.to_s + "  Id:" + key.to_s + "\n" )
			idNo = key.to_i 
		end
	}
	log( "\n" )
	log( "Fire 9 event with " + idNo.to_s + " " + status.to_s + "\n" )

	if (idNo != -1) then 
		tripEv= Command.new(idNo, -1001, 1, 2, 9);      #9 sensor tripp   key.to_i		
		if (status.to_s == "true") then 
			tripEv.params_[25] = "1"
		else
			tripEv.params_[25] = "0"
		end
		SendCommand(tripEv)
	else
		log( "Didn't find the child with name" + val.to_s + "\n" )
	end
end

end

def addPart(val,status)
#check to see if there is some mapping
if $partMapping.has_key?(val) == false then  
	$partMapping[val] = val 
	log( "Add mapping:" + val.to_s + "->" + val.to_s + "\n" )
end

if $partStatus.has_key?(val) then 
	bFind = false
	log( "Already exist " + val.to_s + "\n" )
else 
	bFind = true
	$partStatus[val]=status 

	#[internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
	auxStr=String.new
	auxStr += val.to_s + "\t" + val.to_s + "\t" + "room1" + "\t" + "56" + "\t" + "3" + "\n" 
	$partStr+= auxStr
	log( "Aux:"        + auxStr     +  "\n" )
	log( "partStr:"  + $partStr  +  "\n" )
end  #found end

end

def addChildDevices      #add to data base
if $partStr.empty? then 
	log( "Child list is empty not fire event" + "\n" )
else
	if ($bInit == true) then 
		log( "Already initialized not fire event." + "\n" )
	else
		$bInit=true
		
		reportEv= Command.new(device_.devid_, -1001, 1, 2, 54);      #54 Reporting
		reportEv.params_[12] = ""                                                 # error 
		reportEv.params_[13] = $partStr                                         # text
		SendCommand(reportEv)
		
		log( "Fire 54 event:" + $partStr + "\n" )
		logState(true)
	end	
end

end

def DSCError(err)
log( "System error: " + param + " " )
if $errCode.has_key?(param) then 
	log( $errCode[param]  + "\n" ) 
else 
	log( "Unknown error" + "\n" )
end
#if error on arm/disarm send a message
if( err==23 || err==24) then
changeStateEv= Command.new(devid_, -1001, 1, 2, 67);      #67 PanelChangeState		

if(err==23) then 
	changeStateEv.params_[30] = "0," +$errCode[err] 
end
if(err==24)
	changeStateEv.params_[30] = "1," +$errCode[err] 
end

changeStateEv.params_[47]="1"
SendCommand(changeStateEv)

log("Fire 67 event" + devid_.to + "-1001, 1, 2")
end


end

#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  Apex function
# Command for security panel

def ApexArmPartition(type,user,password)
buff="a"
#check param
if (type != "AWAY") and (type != "HOME")  then 
	badParam(cmdCode[buff],"arm Type") 
	return
end
if ( user.size()  !=2 )  then 
	badParam(cmdCode[buff], "User number") 
	return
end

if (password.size != 4)  then 
	badParam($cmdCode[buff],"Password to long/short") 
	return
end

#command code
case type
when "AWAY"       
	buff += "a"       
when "HOME" 
	buff += "h" 
end	

buff += user.to_s + password.to_s
sendCmd(buff)
end

def ApexDisarmPartition(user,password)
f ( user.size()  !=2 )  then 
	badParam(cmdCode[buff], "User number") 
	return
end

if (password.size != 4)  then 
	badParam($cmdCode[buff],"Password to long/short") 
	return
end

buff="aa"
buff += value + password
sendCmd(buff)
end

def ApexArmingRequest()
sendCmd( "as" )
end

def ApexStatusRequest()
sendCmd( "zs" )
end

def ApexZoneRequest()
sendCmd( "zp" )
end