#include "PlutoUtils/CommonIncludes.h"	 
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#include <iostream>
#ifdef WIN32
#include <conio.h>
#endif

bool AskQuestion(string Question,bool bDefault)
{
	while(true)
	{
		cout << Question << (bDefault ? " [Y/n] " : " [N/y] ");
#ifdef WIN32
		char c = getch();
#else
		char c = getchar();
#endif
		if( c=='y' || c=='Y' )
			return true;
		if( c=='n' || c=='N' )
			return false;
		if( c=='\n' )
			return bDefault;
	}
}

char AskQuestion(string Question,string Prompts)
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
#ifdef WIN32
		char c = (char) getch();
#else
		char c = getchar();
#endif
//#pragma warning("need something unbuffered && need to clear the buffer, otherwise it uses old keystrokes")
//		cin.read( &c[0], 1 );
		if( c=='\n' && cDefault)
			return cDefault;

		if( StringUtils::ToLower(Prompts).find(c)!=string::npos )
			return c;

		// In case the user entered in upper case 
		if( StringUtils::ToLower(Prompts).find(c-32)!=string::npos )
			return c-32;
	}
}
