#Private functions  15-Feb-06 18:00

def log( buff )
if  $logFile.nil? then
	print( buff )
else
	$logFile.print( buff )
	if $bFlush then $logFile.flush() end
end
end

def logString(word)
log( "Size:" + word.size.to_s + "  String:" )
word.each_byte{ |i| 
	#if i >='0' and i<= '9' then log( l= end
	log( i.to_s + "  " )
}

log( "\n" )
end

def logStringA(word)
log( word.size.to_s + " bytes " + "  Values:" )

for i in 0..word.size-1 

	aux=word[i,1]
	case  aux
		when "\r"
		log( "0x0D(13)" )
		when "\n"
		log( "0x0A(10)" )
		when "\7"
		log( "0x07" )
		else
		log( aux  )
	end
	
	if (i < word.size-1) then
		log( "," )
	end
	
	end
log( "\n" )
end

def logReport()
log( "Report: \n" )

log( "Branches lists:" + "   " )
$branchList.each{ |i| log( i.to_s + "," ) }
log( "\n" )

log( "CmdState:" +$cmdState.to_s + "   "  )
log( "CmdBuffer size:" + $cmdBuffer.size.to_s + "\n" )

log( "Alarms  value: ")
if ($AlarmsStatus.size==0) then log( "No alarms")  end
$AlarmsStatus.each{ |key,value|
	log( key.to_s + "," + value.to_s + "   " )
}
log( "\n" )

log( "Relays  value: ")
if ($RelaysStatus.size==0) then log( "No relays")  end
$RelaysStatus.each{ |key,value|
	log( key.to_s + "," + value.to_s + "   " )
}
log( "\n" )

log( "Lights  value: ")
if ($LightsStatus.size==0) then log( "No lights")  end
$LightsStatus.each{ |key,value|
	log( key.to_s + "," + value.to_s + "   " )
}
log( "\n" )

log( "Leds  value: ")
if ($LedsStatus.size==0) then log( "No leds")  end
$LedsStatus.each{ |key,value|
	log( key.to_s + "," + value.to_s + "   " )
}
log( "\n\n" )
end

def extractData(data,noBytes)
	rez=Array.new
	len = (data.size() / noBytes) - 1
	for i in 0..len
		start=noBytes*i
		finish=noBytes*(i+1)
		finish-=1
		rez[i]=data[start..finish].to_i
	end
	
	return rez
end

def extractDataBits(data,noBits)
	rez=Array.new
	noBits-=1
	for i in 0..noBits
		aux = 1 << i
		val = data & aux 
		#log( i.to_s + " " + aux.to_s + "  " + val.to_s + "\n" )
		
		if ( val > 0 ) then
			rez[noBits-i]=1
		else
			rez[noBits-i]=0
		end
	end
	
	return rez
end

def send(buff)
if ( (defined? conn_) == nil) then
	log( "Conn_ not defined\n" )
else
	if conn_.nil? then
		log( "Conn_ is NULL\n" )
	else
		conn_.Send(buff)
	end
end
logStringA( buff )
log( "\n" )
end

def sendCmd(word,waitAnswer)

if ( waitAnswer == true ) then 
	log( "Sending:")
	send(word) 
	else
		if ( $cmdState==1 ) then
			log( "Sending:")
			send(word)
			$cmdState = 2
		else 
			log( "Adding to buffer:" )
			logStringA( word )
			$cmdBuffer.push( word )
	end
end

end

def sendCmd2()
if ($cmdBuffer.empty?) then      #buffer empty
	log( "Buffer is empty:" + "\n" )
	$cmdState=1
	if ($bStartInterogate == true) then
		finishBranchInt()
	end
else                                    #execute first command from buffer
	log( "Sending comand from buffer:\n" )
	send( $cmdBuffer.first )
	$cmdBuffer.delete_at(0)
end

#return buff2
end

def searchBranches()
	log( "Call search branches:\n" )	
	children=device_.childdevices_
	children.each { |key,val| 
		if (val.devdata_.has_key?(40)) and (val.devdata_.has_key?(144)) then
			deviceBranch = val.devdata_[40].to_i
			deviceUnit = val.devdata_[144].to_i
			aux = deviceBranch * 256 + deviceUnit			
			idBranch = "%04u" % aux
			 
			if ( $branchList.include?( idBranch ) == false ) then
				$branchList.push( idBranch )
				log( "Found:" + idBranch.to_s + " branch\n" )
			end
		end
	}
	log( "Found:" + $branchList.size.to_s + " branches" + "\n")
	$branchList.each(){ |i| 
		log( i + "  " )
	}
	log( "\n" )
end

def readLine()
	$line=String.new
	$index=0
	
	cod=conn_.Recv(1, $waitTime)
	$line+=cod
while (cod[0,1] != "\r") and ( cod[0,1] != "\7")  do
	if (conn_ != nil) then 
		cod=conn_.Recv(1, $waitTime)
		$line += cod
	else
		log( "Can not read from serial " + "\n" )
	end
end	
	log( "Received:" )
	logStringA( $line )
	if ( cod == "\7" ) then
		log( "Failed:" + "\n" )
	end
	
	if  ( cod == "\r" ) then
		log( "Success:" + "\n" )
	end
	return $line
end

def createChildrenList()
	auxStr=String.new
	totalIndex=1
	
	#[internal id] \t [description] \t [room name] \t 
	#[device template] \t [floorplan id] \n
	#lights
	
	for index in 1..$LightsNo
	auxStr += totalIndex.to_s + "\t" + index.to_s + "\t" + "room1" 
	auxStr += "\t" + "38" + "\t" + "3" + "\n" 
	
	totalIndex +=1
	end
	
	totalIndex=$MaxLightId
	#relays
	for index in 1..$RelaysNo        #8   
	auxStr += totalIndex.to_s + "\t" + index.to_s + "\t" + "room1" 
	auxStr += "\t" + "1780" + "\t" + "3" + "\n" 
	
	totalIndex +=1
	end
	
	#alarms
	#for index in 1..$AlarmsNo        #12   
	#auxStr += index.to_s + "\t" + index.to_s + "\t" + "room1" 
	#auxStr += "\t" + "1780" + "\t" + "3" + "\n" 
	
	#totalIndex +=1
	#end
	
	addChildDevices( auxStr )
end

def addChildDevices( childList )
if (childList.empty?) then 
	log( "Child list is empty not fire event" + "\n" )
else
	if ($bHaveChilds == true) then 
		log( "Already initialized not fire event." + "\n" )
	else
		$bHaveChilds = true
		
		reportEv= Command.new(device_.devid_, -1001, 1, 2, 54);      #54 Reporting
		reportEv.params_[12] = ""                                                 # error 
		reportEv.params_[13] = childList                                         # text
		SendCommand(reportEv)
		
		log( "Fire 54 event:" + "\n" + childList + "\n" )
	end	
end

end

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#                                                                 CAN

def CANInit()
log( "Start CAN Init:" + "\n" )

CANClose()

CANGetVersion()
#CANGetSerial()

CANSetAutoPoll( true )
CANSetUARTSpeed(2)
CANSetBit(4)
CANSetBTR("171C")

CANOpen()
CANReadFlag()

log( "\n" )
end

def CANOpen()
	outStr = "O" + "\r"
	log( "CAN open" + "   " )

	send(outStr)
	readLine()
end

def CANClose()
	outStr = "C" + "\r"
	log( "CAN close" + "   " )
	
	send(outStr)
	readLine()
end

def CANGetVersion()
	log( "Can get version" + "   " )
	outStr = "V" + "\r"
	send(outStr)
	
	buff=readLine()
	if (buff.size == 6 ) then 
		$hardVersion=buff[1..2]
		$softVersion=buff[3..4]
		
		log( "Version:" + buff[0..4] + "   " )
		log( "Hard version:" + $hardVersion + "   " )
		log( "Soft version:" + $softVersion + "\n" )
	else
		log( "Failed to read version" )
	end
end

def CANGetSerial()
	log( "CAN get serial" + "   " )
	outStr = "N" + "\r"
	
	send(outStr)
	readLine()
end


def CANReadFlag()
	log( "CAN read flag" + "   " )
	outStr = "F" + "\r"

	send(outStr)
	readLine()
end

def CANSetAutoPoll(state)
	log( "CAN set auto poll:" + state.to_s + "   " )
	outStr="X"
	if ( state == false ) then 
		outStr += "0" 
	else 
		outStr += "1" 
	end
	outStr += "\r"
	
	send(outStr)
	readLine()
end

def CANSetBit(no)
	log( "CAN set bit rates:" + no.to_s + "   " )
	outStr="S" + no.to_s + "\r"
	
	send(outStr)
	readLine()
end

def CANSetUARTSpeed(no)
	log( "CAN set UART speed:" + no.to_s + "   " )
	outStr="U" + no.to_s + "\r"
	
	send(outStr)
	readLine()
end

def CANSetBTR(no)
	log( "CAN set BTR:" + no.to_s + "   " )
	outStr="s" + no.to_s + "\r"
	
	send(outStr)
	readLine()
end

def CANSend11(header,data)
	outStr = "t"

	log( "Call CANSend11 \n" )
	log( "Header:"  + "   " )
	header.each_byte { |i|
		str = "%02X" % i
		log( str + " " )
		outStr += str
	}
	log( "\n" )
	
	log( "Data:" + "   " )
	outStr += data.size.to_s	
	data.each_byte { |i|
		str = "%02X" % i
		log( str + " " )
		outStr += str
	}
	log( "\n" )
		
	outStr += "\r"
	
	send( outStr )
end

def CANSend29( header,data,waitAnswer)
	outStr = "T"

	log( "Call CANSend29 \n" )
	log( "Header:"  + "   " )
	header.each_byte { |i|
		str = "%02X" % i
		log( str + " " )
		outStr += str
	}
	log( "\n" )
	
	log( "Data:" + "   " )
	outStr += data.size.to_s	
	data.each_byte { |i|
		str = "%02X" % i
		log( str + " " )
		outStr += str
	}
	log( "\n" )
		
	outStr += "\r"
	
	if ( waitAnswer == true )
		sendCmd( outStr, waitAnswer )
		readLine()
	else
		sendCmd( outStr, waitAnswer )
	end
end

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#                                                                 NOEMON function

def NOEMONInit()
	log( "Call NOEMON init\n")
	$bInit = false
	$bStartInterogate = false      #??
	
	$AlarmsStatus=Hash.new
	$RelaysStatus=Hash.new
	$LightsStatus=Hash.new
	$LedsStatus=Hash.new
	
	$branchList=Array.new
	$currentBranch=0
	
	searchBranches()       # search all branches of childrens
	startBranchInt()        # status for all branches
end

def NOEMONSendCmd(pcBranch,pcNo,deviceBranch,deviceNo,code)
	header = String.new
	data = String.new
	
	header << 0x10 << 0x20
	#branch Number of the P.C., next 7 bits is the Unit Number of the P.C
	pcValue = pcNo * 2 + pcBranch * 256
	header << ( pcValue / 256 )
	header << ( pcValue % 256 )
	
	log( "PC branch:" + pcBranch.to_s + "  " )
	log( "PC no" + pcNo.to_s +  "   " )
	log( "PC value:" + pcValue.to_s + "  " )
	log( "Device branch:" + deviceBranch.to_s + "   " )
	log( "Device no:" + deviceNo.to_s + "\n" )
	
	log( "Code:" )
	logString( code.to_s )
	 
	data << 0x00 << 0x8a   
	data << deviceBranch << deviceNo
	
	code.each_byte { |i| 
		data << i
	}
	CANSend29( header , data , true )
	log( "\n\n" )
end

def startBranchInt()  
	if ($branchList.size <= $currentBranch ) then 
		log( "Finish to interogate all branches\n\n" )
		$bStartInterogate = false;
		$bInit = true;
		return 
	end
	
	val = $branchList[$currentBranch].to_i
	deviceBranch = val / 256
	deviceUnit = val % 256 
	
	$bStartInterogate = true
	$startTime=Time.now
	
	log( "Start interogate\n" )	
	log( "Device branch:"  + deviceBranch.to_s + "   " )
	log( "Device unit:"  + deviceUnit.to_s + "   " )
	log( "Id:" + $branchList[$currentBranch] + " " )
	log( "Time:" + $startTime.strftime("%H:%M:%S  ") + "\n" )
	
	NOEMONStatus($pcBranch,$pcNo,deviceBranch,deviceUnit)  
end

#end a branch interogation
def finishBranchInt
	myTime=Time.new
	strTime = myTime.strftime("%H:%M:%S")
	diff = (myTime - $startTime).to_i
	
	log( "Finish to interogate: " )
	log( "Time:" + strTime + "   " )
	log( "In:" + diff.to_s + "   sec\n" ) 
		
	logReport();
	$bStartInterogate = false;
	$currentBranch += 1
	startBranchInt()
end

def NOEMONStatus(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONAlarmsStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONRelaysStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONLightsStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONLedsStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	
	#small bug last send twice
	NOEMONLedsStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	
	$bStartInterogate=true
	#logReport()
end

def NOEMONStatusReport(pcBranch,pcNo,branchNo,chipNo,reportType)
	data = String.new
	header = String.new
	
	header << 0x10 << 0x20
	pcValue = pcNo * 2 + pcBranch * 256
	header << ( pcValue / 256 )
	header << ( pcValue % 256 )
	
	log( "NOEMON Status Report:" + $cmdState.to_s + "\n" )
	log( "Branch no:" + branchNo.to_s + "  " )
	log( "Chip no" + chipNo.to_s + "\n" )
	log( "PC branch:" + pcBranch.to_s + "  " )
	log( "PC no:" + pcNo.to_s + "   " )
	log( "Value:" + pcValue.to_s + "\n" )
	
	data << 0x00 << 0x89
	data << branchNo << chipNo << reportType
	
	CANSend29( header , data , false )
	log( "\n\n" )
end

def NOEMONEvent(code,type)
	len=code.size()-2      #without 0x0A
	data=code[10..len]    #T+8+1(length) 
	
	log( "Event:" + type + "\n" )
	log( "Data:" + data + "\n" )

	case type
	when "AlarmOn"
	when "AlarmOff"
	end
end

def NOEMONCmdAck(code,type)
	len=code.size()-2      #without 0x0A
	data=code[14..len]    #T+8+1(length) + 4 first 2bytes of data   
	res=Array.new
	count=1
	log( "Command ack:" + type + "\n" )
	log( "Data:" + data + "\n" )
	
	if ( $branchList.size> $currentBranch ) then 
		id = $branchList[$currentBranch] 
		log( "Id:"  + id + "\n" )
	else
		log( "Current branch " + $currentBranch.to_s + " to big\n" )
		return
	end

	case type
	
		when "Alarms 1"
		res=extractData(data,2)
		res.each{ |i| 
			idTotal = id + ("%02u" % count)
			$AlarmsStatus[idTotal] = i 
			count += 1
		}
		
		log( "Alarms value: ")
		$AlarmsStatus.each{ |key,value|
			log( key.to_s + "," + value.to_s + "   " )
		}
		
		when "Alarms 2"
		res=extractData(data,2)
		count = 7
		res.each{ |i| 
			idTotal = id + ("%02u" % count)
			$AlarmsStatus[idTotal] = i 
			count += 1
		}
		
		log( "Alarms value: ")
		$AlarmsStatus.each{ |key,value|
			log( key.to_s + "," + value.to_s + "   " )
		}
		
		
		when "Relay"
		res=extractDataBits(data.to_i,8)
		res.each{ |i| 
			idTotal = id + ("%02u" % count)
			$RelaysStatus[idTotal] = i 
			count += 1
		}
		
		log( "Relays  value: ")
		$RelaysStatus.each{ |key,value|
			log( key.to_s + "," + value.to_s + "   " )
		}
	
		when "Lights"
		res=extractData(data,2)
		res.each{ |i| 
			idTotal = id + ("%02u" % count)
			$LightsStatus[idTotal] = i 
			count += 1
		}
		
		log( "Lights  value: ")
		$LightsStatus.each{ |key,value|
			log( key.to_s + "," + value.to_s + "   " )
		}
		
		when "Leds"  
		res=extractData(data,2)
		res.each{ |i| 
			idTotal = id + ("%02u" % count)
			$LedsStatus[idTotal] = i 
			count += 1
		}
		
		log( "Leds  value: ")
		$LedsStatus.each{ |key,value|
			log( key.to_s + "," + value.to_s + "   " )
		}
	end	
	
	log( "\n\n")
	sendCmd2()
	log( "\n" )
end

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# NOEMON command
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#!!!!!! Lights
def NOEMONLightsStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo,4)
end

def NOEMONTurnLight(no,state,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "Turn light " + no.to_s + "   " + state.to_s + "\n" )
	aux = deviceBranch * 256 + deviceUnit	
	id = "%04u" % aux
	id += "%02u" % no

	if ( state == true ) then
		NOEMONSetUpLightLevel(no,100,pcBranch,pcUnit,deviceBranch,deviceUnit)      # 100 level    
		NOEMONSetDimmUpRate(no,2,pcBranch,pcUnit,deviceBranch,deviceUnit)     # 1 s
		NOEMONStopDownDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
		NOEMONStartUpDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
		
		$LightsStatus[id] = 100
	else
		NOEMONSetDownLightLevel(no,0,pcBranch,pcUnit,deviceBranch,deviceUnit)          # 0 level    
		NOEMONSetDimmDownRate(no,2,pcBranch,pcUnit,deviceBranch,deviceUnit)     # 1 s
		NOEMONStopUpDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
		NOEMONStartDownDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
		
		$LightsStatus[id] = 0
	end
end

def NOEMONLightSetLevel(no,level,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "Light set level " + no.to_s + "   " + level.to_s + "\n" )
	aux = deviceBranch * 256 + deviceUnit	
	id = "%04u" % aux
	id += "%02u"  % no 
	
	NOEMONSetUpLightLevel(no,level,pcBranch,pcUnit,deviceBranch,deviceUnit)     
	NOEMONSetDimmUpRate(no,2,pcBranch,pcUnit,deviceBranch,deviceUnit)     # 1 s
	
	log( "Ligths is:" + id.to_s + "   " )
	lastVal=$LightsStatus[id].to_i
	log( lastVal.to_s )
	if ( lastVal < level ) then                                    #is lower then  
		log( "Previous value was lower\n" )
		NOEMONStopDownDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
		NOEMONStartUpDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
	else
		log( "Previous value was higher\n" )
		NOEMONStopUpDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
		NOEMONStartDownDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
	end
	
	$LightsStatus[id] = level
end

def NOEMONSetUpLightLevel(no,level,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "SetUpLightLevel" + "\n" )

	outStr=String.new
	outStr << 3 << (no-1) << 0 << level
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

def NOEMONSetDownLightLevel(no,level,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "SetDownLightLevel" + "\n" )

	outStr=String.new
	outStr << 4 << (no-1) << 0 << level
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

def NOEMONSetDimmUpRate(no,rate,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "SetDimmUpRate" + "\n" )
	outStr=String.new
	outStr << 5 << (no-1) << 0 << rate
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

def NOEMONSetDimmDownRate(no,rate,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "SetDimmDownRate" + "\n" )

	outStr=String.new
	outStr << 6 << (no-1) << 0 << rate
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

def NOEMONStartUpDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "StartUpDimming" + "\n" )
	
	outStr=String.new
	outStr << 7 << (no-1) << 0 << 1
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

def NOEMONStopUpDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "StopUpDimming" + "\n" )

	outStr=String.new
	outStr << 7 << (no-1) << 0 << 0
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

def NOEMONStartDownDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "StartDownDimming" + "\n" )

	outStr=String.new
	outStr << 8 << (no-1) << 0 << 1
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

def NOEMONStopDownDimming(no,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "StopDownDimming" + "\n" )

	outStr=String.new
	outStr << 8 << (no-1) << 0 << 0
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

#!!!!!! Alarm
def NOEMONAlarmsStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo,2)
end

def NOEMONArmInput(no,pcBranch,pcUnit,deviceBranch,deviceNo)
	log( "ArmInput" + "\n" )
	outStr = String.new
	outStr << 14 << (no-1) << 0 << 1
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceNo,outStr )
end

def NOEMONDisarmInput(no,pcBranch,pcUnit,deviceBranch,deviceNo)
	log( "DisarmInput" + "\n" )
	outStr = String.new
	outStr << 14 << (no-1) << 0 << 0
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceNooutStr )
end

def NOEMONSetAlarmInput(no,state,pcBranch,pcUnit,deviceBranch,deviceNo)      #0 bypass,1 not bypass
	log( "SetAlarmInput" + "\n" )
	outStr << 13 << (no-1)
	case state 
	when 0
		outStr << 0 << 1
	when 1
		outStr << 0 << 0
	end
	
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceNo,outStr )
end

#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  Turn Relay on/off
def NOEMONRelaysStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo,3)
end

def NOEMONTurnRelay(no,state,pcBranch,pcUnit,deviceBranch,deviceUnit)
	log( "TurnOnRelay:" + no.to_s + "   " + state.to_s + "\n" )
	
	outStr = String.new
	outStr << 0 << (no-1) << 0
	if (state==true) then
		outStr << 1
	else
		outStr << 0
	end			
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceUnit,outStr )
end

#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  Leds
def NOEMONLedsStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo)
	NOEMONStatusReport(pcBranch,pcUnit,deviceBranch,deviceNo,5)
end

def NOEMONSetLed(no,state,pcBranch,pcUnit,deviceBranch,deviceNo)
	outStr = String.new
	
	case state
	when 1 				# continous mode
		outStr << 1 << (no-1) << 0 << 0
	when 2			        # blink mode	
		outStr << 1 << (no-1) << 0 << 1
	when 3				# slow blink
		outStr << 2 << (no-1) << 0 << 1
	when 4				# fast blink
		outStr << 2 << (no-1) << 0 << 0
	end
	
	NOEMONSendCmd( pcBranch,pcUnit,deviceBranch,deviceNo,outStr )
end

#other

def NOEMONSetBeeper(mode)
	case mode
	when "discontinuous" 
		outStr << 9 << 0 << 0 << 1
	when "continuous"
		outStr << 9 << 0 << 0 << 0
	when "on"
		outStr << 10 << 0 << 0 << 1
	when "off"
		outStr << 10 << 0 << 0 << 0
	end
end

def NOEMONSetKeyboard( pass )
	outStr << 50 << 0 << 0 << pass
	NOEMONSendCmd( $pcBranch,$pcNo,$branchNo,$chipNo,outStr )
end
