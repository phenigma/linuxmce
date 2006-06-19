BEGIN {
	SectionExtension = 0;
	SectionScreenOrDevice = 0;
	if (UI == "")
		UI = "1";
	if (UI != "1" && UI != "2")
	{
		print "UI version " UI " not supported";
		exit 1;
	}
}

# output the part that interests us in the Screen/Device section
function outputScreenOrDevice()
{
	if (UI == 1)
		return;
	print "\tOption \"XvmcUsesTextures\" \"true\"";
	print "\tOption \"AllowGLXWithComposite\" \"true\"";
	print "\tOption \"AddARGBGLXVisuals\" \"true\"";
	print "\tOption \"renderAccel\" \"true\"";
}

function outputExtensions()
{
	if (UI == 1)
		return;
	print "Section \"Extensions\"";
	print "\tOption \"Composite\" \"true\"";
	print "\tOption \"RENDER\" \"true\"";
	print "EndSection";
}

# update Screen and Device sections
/Section..*"Screen"/ || /Section..*"Device"/ { SectionScreenOrDevice = 1; print; next; }
SectionScreenOrDevice == 1 && (/Option..*"XvmcUsesTextures"/ || /Option..*"AllowGLXWithComposite"/ || /Option..*"AddARGBGLXVisuals"/ || /Option..*"renderAccel"/) { next; }
SectionScreenOrDevice == 1 && /EndSection/ { SectionScreenOrDevice = 0; outputScreenOrDevice(); print; next; }
SectionScreenOrDevice == 1 { print; next; }

# update (as in "delete+rewrite") Extensions section
/Section..*"Extensions"/ { SectionExtension = 1; next; }
SectionExtension == 1 && /EndSection/ { SectionExtension = 0; next; }
SectionExtension == 1 { next; }
{ print; }

END { outputExtensions(); }
