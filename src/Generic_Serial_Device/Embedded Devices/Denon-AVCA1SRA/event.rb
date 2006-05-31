#event  28 April 06 18:15 Denon AVC 
buff = readLine()
cmd=buff[0..1]
param=buff[2..buff.size()-1]

log( "AVR Received  " )
log( "Cmd:" + cmd.to_s + "   " )
log( "Param:" + param )

case cmd

	when "SI"
	log( "It's an input: command" + pram.to_s + "\n" )
	case param
		when "VDP"
			input_param = 176
		when "VCR-3"
			input_param = 284
		when "VCR-2"
			input_param = 283
		when "VCR-1"
			input_param = 282
		when "V.AUX"
			input_param = 285
		when "TV"
			input_param = 161
		when "TUNER"
			input_param = 166
		when "PHONO"
			input_param = 163
		when "MD/TAPE2"
			input_param = 174
		when "DVD"
			input_param = 165
		when "DBS/SAT"
			input_param = 281
		when "CD"
			input_param = 162
		when "CDR/TAPE1"
			input_param = 420
			
		else
			log( "Unknown parameter\n" )
	end # case

	if( param.size() !=0 ) then
		log( "Changing to input " +  param + "\n" )
		cmd = Command.new(device_.devid_, -1001, 1, 2, 49);
		cmd.params_[41] = input_param.to_s()
		SendCommand(cmd);
	end
	
	when "PW"
	log( "It's a power command:" + param.to_s + "\n" )
		if( param =="STANDBY" ) then
			log( "Turning off:", param , "\n" )
			cmd = Command.new(device_.devid_, -1001, 1, 2, 48);
			cmd.params_[10] = "0";
			SendCommand(cmd);
		else 
			if ( param =="ON" ) then
				plog( "Turning on\n" )
				cmd = Command.new(device_.devid_, -1001, 1, 2, 48);
				cmd.params_[10] = "1";
				SendCommand(cmd);
			end
		end
		
	when "MV"
		newlevel = buff[2..buff.size()-1]
	
		log( "It is a volume command" + param.to_s + "\n" );
		cmd = Command.new(device_.devid_, -1000, 1, 3, 0);
		cmd.params_[158] = newlevel.to_s;
		SendCommand(cmd);
		
	else
		log( "Unknown comman")
 end   #case
 
 