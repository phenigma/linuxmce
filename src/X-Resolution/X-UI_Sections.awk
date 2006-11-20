BEGIN {
	SectionExtension = 0;
	SectionScreenOrDevice = 0;
	if (OpenGL == "")
		OpenGL = "0";
	if (OpenGL != "0" && OpenGL != "1")
	{
		print "Invalid OpenGL variable value: " OpenGL;
		exit 1;
	}
}

# output the part that interests us in the Screen/Device section
function outputScreenOrDevice()
{
	if (OpenGL == 0)
		return;
	print "\tOption \"XvmcUsesTextures\" \"true\"";
	print "\tOption \"AllowGLXWithComposite\" \"true\"";
	print "\tOption \"AddARGBGLXVisuals\" \"true\"";
	print "\tOption \"renderAccel\" \"true\"";
#<-mkr_b_via_b->
	# http://wiki.openchrome.org/tikiwiki/tiki-index.php?page=EXAAcceleration
	print "\tOption \"AccelMethod\" \"exa\"";
	print "\tOption \"ExaScratchSize\" \"8192\"";
	print "\tOption \"MaxDRIMem\" \"16384\"";
	print "\tOption \"MigrationHeuristic\" \"greedy\"";
#<-mkr_b_via_e->
}

function outputExtensions()
{
	if (OpenGL == 0)
		return;
	print "Section \"Extensions\"";
	print "\tOption \"Composite\" \"true\"";
	print "\tOption \"RENDER\" \"true\"";
	print "EndSection";
}

# update Screen and Device sections
/Section..*"Screen"/ || /Section..*"Device"/ { SectionScreenOrDevice = 1; print; next; }
SectionScreenOrDevice == 1 && (/Option..*"XvmcUsesTextures"/ || /Option..*"AllowGLXWithComposite"/ || /Option..*"AddARGBGLXVisuals"/ || /Option..*"renderAccel"/ \
	|| /Option..*"AccelMethod"/ || /Option..*"ExaScratchSize"/ || /Option..*"MaxDRIMem"/ || /Option..*"MigrationHeuristic"/ \
) { next; }
SectionScreenOrDevice == 1 && /EndSection/ { SectionScreenOrDevice = 0; outputScreenOrDevice(); print; next; }
SectionScreenOrDevice == 1 { print; next; }

# update (as in "delete+rewrite") Extensions section
/Section..*"Extensions"/ { SectionExtension = 1; next; }
SectionExtension == 1 && /EndSection/ { SectionExtension = 0; next; }
SectionExtension == 1 { next; }
{ print; }

END { outputExtensions(); }
