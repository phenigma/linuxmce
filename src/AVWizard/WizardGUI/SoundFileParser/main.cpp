/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
#include <stdio.h>

#include "AMixerParser.h"

int main()
{
	AMixerParser * Parser = new AMixerParser();

	Parser->ParseConfig();

	int Lower, Upper, Value;

	Parser->GetControlOptions("Master", Lower, Upper, Value);

	delete Parser;
	return 0;
}