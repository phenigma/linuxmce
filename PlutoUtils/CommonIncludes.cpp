/* 
	CommonIncludes
	
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation
	
	www.plutohome.com		
	
	Phone: +1 (877) 758-8648
	
	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
	
	See the GNU General Public License for more details.
*/

#include "PlutoUtils/CommonIncludes.h"	 
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#ifdef WIN32
#include <conio.h>
#endif

bool AskYNQuestion(string Question,bool bDefault)
{
	while(true)
	{
		fflush(stdin);
		cout << Question << (bDefault ? " [Y/n] " : " [N/y] ");
		char c = getch();
		cout << endl;
		if( c==3 )
			exit(1);
		if( c=='y' || c=='Y' )
			return true;
		if( c=='n' || c=='N' )
			return false;
		if( c=='\n' )
			return bDefault;
	}
}

char AskMCQuestion(string Question,string Prompts)
{
	char cDefault=0;
	while(true)
	{
		fflush(stdin);
		cout << Question << " [";
		bool bFirst=true;
		for(size_t s=0;s<Prompts.length();++s)
		{
			string Char = " ";
			Char[0] = Prompts[s];
			Char = StringUtils::ToLower(Char); // Just to see if it was uppercase
			if( Char[0] != Prompts[s] ) // It was uppercase
				cDefault = Char[0];
			cout << (bFirst ? "" : "/") << Prompts[s];
			bFirst=false;
		}
		cout << "] ";
		char c = (char) getch();
//#pragma warning("need something unbuffered && need to clear the buffer, otherwise it uses old keystrokes")
//		cin.read( &c[0], 1 );
		if( c==3 )
			exit(1);
		cout << endl;
		if( c=='\n' && cDefault)
			return cDefault;

		if( StringUtils::ToLower(Prompts).find(c)!=string::npos )
			return c;

		// In case the user entered in upper case 
		if( StringUtils::ToLower(Prompts).find(c+32)!=string::npos )
			return c+32;
	}
}
