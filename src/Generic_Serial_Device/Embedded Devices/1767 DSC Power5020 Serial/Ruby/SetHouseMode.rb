def SetHouseMode(value_to_assign, pk_users, password, pk_devicegroup, handling_instructions)
#SetHouseMode 03-Ian-06 12:10
#log parameters
$logFile.print "SetHouseMode:" + "  "
$logFile.print "Value:"         +   value_to_assign          + "  "
$logFile.print "User:"           +   pk_users                    + "  " 
$logFile.print "Password:"     +  password                    + " "
$logFile.print "Device:"        +  pk_devicegroup            + "  " 
$logFile.print "Handling:"     +  handling_instructions     + "\n"

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
if pk_devicegroup == "0" then 
	$logFile.print "All partition" + "\n"
	partList = $partStatus.keys
else 
	$logFile.print "Only a partition" + "\n"
	partList.push( $partMapping[pk_devicegroup] ) 
end

$logFile.print "Size:" + partList.size.to_s + " Partition list: "
partList.each { |i| $logFile.print i.to_s + " " }
$logFile.print "\n"

case valueNo
when 1      #unarmed at home
	partList.each { |i| 
	DSCDisarmPartition(i.to_s ,password) }
when 2      # armed away
	partList.each { |i| 
	DSCArmPartition("CODE",i.to_s,password ) }
when 3      # armed at home
	partList.each { |i| 
	DSCArmPartition("STAY",i.to_s,password) }
when 4      # sleeping
	$logFile.print "Case 4:\n"
when 5      # entertaining
	$logFile.print "Case 5:\n"
when 6      # armed extended away
	partList.each { |i| 
	DSCArmPartition("AWAY",i.to_s,password) }
end

if $bFlush then $logFile.flush end
end