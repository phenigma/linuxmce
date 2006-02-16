#Reset 15-Feb-06 17:40 
log( "\n\n" + "Reinit:" + arguments + "\n\n" )

case arguments
	when "NOEMON"
	CANInit()
	when "CANBUS"
	NOEMONInit()
end


