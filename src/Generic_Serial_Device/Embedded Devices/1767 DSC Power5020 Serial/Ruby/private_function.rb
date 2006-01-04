#private functions  03-Ian-06 17:30
def readLine
$index=0
$line=String.new

cod=conn_.Recv(1, $waitTime)
$line+=cod
while cod[0,1] != "\n" do
	if conn_ != nil then 
		cod=conn_.Recv(1, $waitTime)
		$line += cod
	else
		$logFile.print "Can not read from serial " + "\n"
	end
		
	if ($line.size>$MaxLineLength) then
		$logFile.print "Comunication error. Line size" + $line.size.to_s + "\n"
		#send error ...
	end	
end

logString($line)
end

def readByte(val)
valNo=val.to_i
if valNo + $index <= $line.size then
	buff = $line[$index,valNo]
	$index+=valNo
else 
	$logFile.print "Error reading   "	
	$logFile.print "Try to read:" + valNo.to_s + " bytes from index:" + $index.to_s + "   "
	$logFile.print "Total size:" + $line.size.to_s + "\n"
	buff=String.new
end

if $bFlush then $logFile.flush end
return buff
end

def logString(word)
$logFile.print "Line read Size:" + word.size.to_s + "  String:"
word.each_byte{ |i| 
	#if i >='0' and i<= '9' then $logFile.print l= end
	$logFile.print i.to_s + "  "
}

$logFile.print "\n"
if $bFlush then $logFile.flush end
end

def logState()
logTime = Time.now
timeStr = logTime.strftime("%d-%m-%Y  %H:%M:%S  ")

$logFile.print "PC5401 state:" + "\n"
$logFile.print "Machine time:" + timeStr + "\n"
$logFile.print "Max partition:" + $MaxPartition.to_s + "   Max sensor:" + $MaxSensor.to_s + "   "
$logFile.print "Max line length:" + $MaxLineLength.to_s + "\n"

$logFile.print "PC5401 State:" + $panelState.to_s + " Init:" + $bInit.to_s + "\n"  
$logFile.print "TimeStamp:" + $bTimeStamp.to_s + " TimeBroadcast:" + $bTimeBroadcast.to_s + "   "
$logFile.print "Descriptive arm:" + $bDescArm.to_s + "\n"

$logFile.print "Partitions list: " + $partStr + "\n" 
$logFile.print "Sensors list: " + $zoneStr + "\n"

#log open zone
$logFile.print "Sensor open:"
$sensorStatus.each{ |key, value|
	$logFile.print key.to_s + " -> " + value.to_s
	if $sensorType.has_key?(key) then $logFile.print " -> " + $sensorType[key].to_s end
	$logFile.print "\n"
}

$logFile.print "\n"
if $bFlush then $logFile.flush end
end

def logCmd(code)
#logState()

logTime = Time.now
timeStr = logTime.strftime("%d-%m-%Y  %H:%M:%S  ")

cmdNo=code.to_i

$logFile.print "Machine time:" + timeStr + "\n"
$logFile.print "PC5401 State:" + $panelState.to_s + " Init:" + $bInit.to_s + "\n"
$logFile.print "TimeStamp:" + $bTimeStamp.to_s + " TimeBroadcast:" + $bTimeBroadcast.to_s
$logFile.print "\n"

if (cmdNo < 500) then $logFile.print "Command:" end
if (cmdNo >= 500) and (cmdNo <= 502) then $logFile.print "Cmd_ack:" end
if (cmdNo > 502) then $logFile.print "Event:" end

$logFile.print code.to_s + "  " 
if $cmdCode.has_key?(code) then 
	$logFile.print $cmdCode[code] 
else 
	$logFile.print "Unknown code" 
end
$logFile.print "\n\n"

if $bFlush then $logFile.flush end
end

def mapp(zones)      #mapping Partition -> Zones
list=zones.split(",")
list.each{ |i|
	list2=i.split("=")

	if list2.size != 2 then 
		print "Wrong\n" 
		next 
	end      #bad format string

	# add information
	part=list2[1] 
	zone=list2[0]
	$partMapping[zone]=part
}

end

def addSens(val,status,type)
if $sensorStatus.has_key?(val) then 
	$logFile.print "Already exist " + val.to_s + "\n" 
else 
	$sensorStatus[val]=status 
	$sensorType[val]=type 

	#add to zone string
	#[internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
	if ($bPartDetect == false ) then       #more than 8 sensor
		auxStr=String.new
		auxStr += device_.devid_.to_s + "\t" + val.to_s + "\t" + "room1" + "\t" + type.to_s + "\t" + "3" + "\n" 
		$zoneStr+= auxStr

		$logFile.print "Aux:"        + auxStr     +  "\n"   
		$logFile.print "zoneStr:"  + $zoneStr  +  "\n"
	end
end      #end new sensor found  

if $bInit == true then        #fire sensor trip only after adding child
	valNo=val.to_i
	idNo=-1

	$logFile.print "Log children list:" + "\n"
	children=device_.childdevices_
	children.each{ |key, value|
		strAux=value.devdata_[12]
		$logFile.print "Child id:" + key.to_s + "  "
		$logFile.print "Child name:" +  strAux + "  "  
		$logFile.print "Search name:" + valNo.to_s + "\n" 

		if valNo.to_s == strAux then 
			$logFile.print "Found child with name:" + val.to_s + "  Id:" + key.to_s + "\n"
			idNo = key.to_i 
		end
	}
	$logFile.print "\n"
	$logFile.print "Fire 9 event with " + idNo.to_s + " " + status.to_s + "\n"
	$logFile.flush

	if idNo != -1 then 
		tripEv= Command.new(idNo, -1001, 1, 2, 9);      #9 sensor tripp   key.to_i
		tripEv.params_[25] = status.to_s                     #25 state
		SendCommand(tripEv)
	else
		$logFile.print "Didn't find the child with name" + val.to_s + "\n"
	end
end

if $bFlush then $logFile.flush end
end

def addPart(val,status)
#check to see if there is some mapping
if $partMapping.has_key?(val) == false then  
	$partMapping[val] = val 
	$logFile.print "Add mapping:" + val.to_s + "->" + val.to_s + "\n"
end

if $partStatus.has_key?(val) then 
	bFind = false
	print "Already exist " + val.to_s + "\n" 
else 
	bFind = true
	$partStatus[val]=status 

	#[internal id] \t [description] \t [room name] \t [device template] \t [floorplan id] \n
	auxStr=String.new
	auxStr += val.to_s + "\t" + val.to_s + "\t" + "room1" + "\t" + "56" + "\t" + "3" + "\n" 
	$partStr+= auxStr
	$logFile.print "Aux:"        + auxStr     +  "\n"   
	$logFile.print "partStr:"  + $partStr  +  "\n"
end  #found end

if $bFlush then $logFile.flush end
end

def addChildDevices      #add to data base
if $partStr.empty? then 
	$logFile.print "Child list is empty not fire event" + "\n"
else
	if $bInit == true then 
		$logFile.print "Already initialized." + "\n"
	else
		$bInit=true
		
		reportEv= Command.new(device_.devid_, -1001, 1, 2, 54);      #54 Reporting
		reportEv.params_[12] = ""                                                 # error 
		reportEv.params_[13] = $partStr                                         # text
		SendCommand(reportEv)
		
		$logFile.print "Fire 54 event:" + $partStr + "\n"
		logState()
	end	
end

if $bFlush then $logFile.flush end
end

def badParam(funcName,paramName)
$logFile.print "Bad parameter in: " + funcName + " Parmeter:" + paramName + "\n"
if $bFlush then $logFile.flush end
end

def checkSumProc(buff)
count=0   
chkStr=""
print "\nCheckSum call:\n"
print "Size:" + buff.size.to_s + "  " + buff + "\n"
for i in 0..buff.size - 1  
count=count+buff[i,1].to_i+48
print buff[i,1] + ":" + count.to_s + "\n" 
end
#calculate checksum
count=count%256
chkStr= "%X" % count
print count.to_s + " " + chkStr + "  " + chkStr[0,1] + " " + chkStr[1,1]  + "\n"
#show list before
print "Size: " + buff.size.to_s + " " + buff + "\n"
#add checksum
buff += chkStr
#add crl/lf   
buff +="\r" + "\n"
#show list after
print "Size: " + buff.size.to_s + " " + buff + "\n"
return buff
end

def sendCmd(buff)
buff2=checkSumProc(buff)
logString(buff2)

if $panelState == 3 then    #wait to finish another command
	$cmdBuffer.push(buff2)
	$logFile.print "Adding cmd to buffer" + "\n\n"
else                                #ready to process another command
	conn_.Send(buff2)
	$panelState=3
	$logFile.print "Send cmd" + "\n\n"
end

if $bFlush then $logFile.flush end
end

def sendCmd2()
$logFile.print "Start cmd2: "
$logFile.flush

if $cmdBuffer.empty? then      #execute first command from buffer
	if $panelState == 3 then $panelState=2 end 
else                                    #buffer empty
	$logFile.print "Send cmd2: "
	conn_.Send($cmdBuffer.first)
	logString($cmdBuffer[0])

	$cmdBuffer.delete_at(0)
end

if $bFlush then $logFile.flush end
#return buff2
end

#  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#  DSC function

def DSCPoll
buff="000"
logCmd(buff)
$logFile.print "\n"
sendCmd(buff)
end

def DSCStatusReport2
buff="001"
logCmd(buff)
$logFile.print "\n"
sendCmd(buff)
end

def DSCSetDescArm(bState)
buff="050"
logCmd(buff)

$bTimeStamp=bState
# add data
if bState == true then 
	buff += '1' 
else 
	buff += '0' 
end

sendCmd(buff)
end

def DSCSetTimeStamp(bState)
buff="055"
logCmd(buff)

$bTimeStamp=bState
# add data
if bState == true then 
	buff += '1' 
else 
	buff += '0' 
end

sendCmd(buff)
end

def DSCSetTimeBroadcast(bState)
buff="056"
logCmd(buff)
$bTimeBroadcast=bState
# add data
if bState == true then 
	buff += '1' 
else 
	buff += '0' 
end

sendCmd(buff)
end

def DSCSimulateAlarmCmd(value)
buff="060"
logCmd(buff)

$logFile.print "Value: " + value.to_s +  "\n"
#check param
chkValue=value.to_i
if (chkValue<1) and (chkValue>3)  then 
	badParam($cmdCode[buff],"Wrong value") 
	return
end

# add data
buff += value.to_s      #time

sendCmd(buff)
end

#command for security panel
def DSCArmPartition(type,value,password)
buff="03"
logCmd(buff)

$logFile.print value.to_s + " armType: " + type + "\n"
#check param
if (type != "AWAY") and (type != "STAY") and (type != "NODELAY") and (type != "CODE") then 
	badParam(cmdCode[buff],"arm Type") 
	return
end

partNo=value.to_i
if (partNo < 0) or (partNo > 8) then 
	badParam(cmdCode[buff],"Partition number") 
	return
end

if (password.size < 4) or (password.size > 6) then 
	badParam($cmdCode[buff],"Password to long/short") 
	return
end

#command code
case type
when "AWAY"       
	buff += "0"       
when "STAY" 
	buff += "1" 
when "NODELAY"  
	buff += "2"                
when "CODE"	
	buff += "3"
end	
	
#data - add the partition number
buff += partNo.to_s
if type == "CODE"       then buff += password           end

sendCmd(buff)
end

def DSCDisarmPartition(value,password)
buff="040"
logCmd(buff)

$logFile.print "Partition:" + value.to_s + " Code:" + password + "\n"
#check param
if (password.size < 4) or (password.size > 6) then 
	badParam($cmdCode[buff],"Password to long/short") 
	return
end

partNo=value.to_i
if (partNo < 0) or (partNo > 8) then 
	badParam($cmdCode[buff],"Partition number") 
	return
end

#data
buff += value
buff += password

sendCmd(buff)
end
