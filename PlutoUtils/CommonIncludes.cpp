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
		cout << Question << (bDefault ? " [Y/n] " : " [N/y] ");
		char c = getch();
		cout << endl;
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
		cout << endl;
		if( c=='\n' && cDefault)
			return cDefault;

		if( StringUtils::ToLower(Prompts).find(c)!=string::npos )
			return c;

		// In case the user entered in upper case 
		if( StringUtils::ToLower(Prompts).find(c-32)!=string::npos )
			return c-32;
	}
}
