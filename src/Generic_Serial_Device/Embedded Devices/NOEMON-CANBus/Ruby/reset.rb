#Reset 16-Feb-06 18:04
log( "\n\n" + "Reinit:" + arguments + "\n\n" )

case arguments
	when "NOEMON"
	NOEMONInit()
	when "CANBUS"
	CANInit()
end
