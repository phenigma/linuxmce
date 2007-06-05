BEGIN {
	if (ResX ~ /[^0-9]|^$/ || ResY ~ /[^0-9]|^$/ || Refresh ~ /[^0-9]|^$/)
	{
		print "Error in parameters" >"/dev/stderr";
		exit;
	}
	Monitor = 0;
	Display = 0;
	Screen = 0;
	IGNORECASE = 1;
}
/Modeline/ { next }
/Section..*"Monitor"/ { Monitor = 1; }
Monitor == 1 && (/HorizSync/ || /VertRefresh/ || /Modeline/) { next; }
Monitor == 1 && /EndSection/ {
	if (Modeline != "")
		print "\tModeline \t" Modeline;
	if (Force != "")
	{
		print "\tHorizSync\t28-500";
	}
	else
	{
		print "\tHorizSync\t28-500";
	}
	print "\tVertRefresh\t" Refresh-1 "-" Refresh+1;
	Monitor = 0;
}
/SubSection..*"Display"/ { Display = 1; }
Display == 1 && /Modes[^"]/ { next; }
Display == 1 && /Virtual/ { next; }
/EndSubSection/ && Display == 1 {
	print "\t\tModes\t\t\"" ResX "x" ResY "\"";
	print "\t\tVirtual\t\t" ResX " " ResY;
	Display = 0;
}
/Section..*"Screen"/ { Screen = 1; }
Screen == 1 && /Option.*"TVStandard"/ { next }
Screen == 1 && /EndSection/ && nvHD != "" {
	print "\tOption\t\"TVStandard\"\t\"" nvHD "\"";
	Screen = 0;
}
{ print }
