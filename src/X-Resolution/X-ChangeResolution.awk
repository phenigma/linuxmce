BEGIN {
	if (ResX ~ /[^0-9]|^$/ || ResY ~ /[^0-9]|^$/ || Refresh ~ /[^0-9]|^$/ || Modeline == "")
	{
		print "Error in parameters" >"/dev/stderr";
		exit;
	}
	Monitor = 0;
	Display = 0;
}
/Modeline/ || /Modes/ { next }
/Section..*"Monitor"/ { Monitor = 1; }
Monitor == 1 && (/HorizSync/ || /VertRefresh/ || /Modeline/) { next; }
Monitor == 1 && /EndSection/ {
	print "\tModeline \t" Modeline;
	if (Force != "")
	{
		print "\tHorizSync\t28-500";
	}
	else
	{
		print "\tHorizSync\t28-90";
	}
	print "\tVertRefresh\t" Refresh;
	Monitor = 0;
}
/SubSection..*"Display"/ { Display = 1; }
Display == 1 && /Modes/ { next; }
/EndSubSection/ && Display == 1 {
	print "\t\tModes\t\t\"" ResX "x" ResY "\"";
	Display = 0;
}
{ print }
