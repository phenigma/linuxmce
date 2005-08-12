/*
    StringUtils

    Copyright (C) 2004 Pluto, Inc., a Florida Corporation

    www.plutohome.com

    Phone: +1 (877) 758-8648

    This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
    This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.
*/

/**
 *
 * @file StringUtils.cpp
 * @brief source file for the StringUtils
 * @author
 * @todo notcommented
 *
 */


#include "FileUtils.h"
#include "StringUtils.h"
#include "Other.h"

#ifndef SYMBIAN
    #include <stdio.h>
    #include <time.h>
    #include <cctype>
    #include <algorithm>
    #include <locale>
    #include <stdarg.h>
    #include <iostream>
    #include <iomanip>
    #ifdef WIN32
        #ifndef WINCE
            #include <direct.h>
            #include <conio.h>
        #else
            #include _STLP_NATIVE_C_HEADER(time.h)
            #include _STLP_NATIVE_C_HEADER(stdio.h)
            #include "wince.h"
        #endif
    #else
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <dirent.h>
        #define stricmp(x, y) strcasecmp(x, y)
        #include <signal.h>
    #endif
#endif //#ifndef SYMBIAN

string StringUtils::Tokenize( string &sInput, string sToken, string::size_type &CurPos )
{
    if ( CurPos >= sInput.length() ) // allready extracted all the pices
	{
        return "";
	}

    string::size_type TokenPos;

    if ( sToken.length() > 1 )
    {
        TokenPos = sInput.length() + 1;

        for( string::size_type i=0; i < sToken.length(); i++ )
        {
            string::size_type NewPos = CurPos;
            Tokenize( sInput, sToken.substr(i, 1), NewPos );
            if ( NewPos < TokenPos )
                TokenPos = NewPos;
        }
        TokenPos--;
    }
    else
    {
        TokenPos = sInput.find( sToken, CurPos );
        if ( TokenPos == string::npos )
        {
            TokenPos = sInput.length();
        }
    }

	string sReturnValue = sInput.substr( CurPos, (TokenPos - CurPos) );
    CurPos = TokenPos + 1;
    return sReturnValue;
}

string StringUtils::ToUpper(string sInput)
{
#ifndef SYMBIAN
    for (string::iterator i = sInput.begin(); i != sInput.end(); i++)
    {
        *i = toupper(*i);
    }
//  transform( sInput.begin(), sInput.end(), sInput.begin(), toupper );
    return sInput;
#else
    string sOutput( sInput );
    sOutput.toupper();
    return sOutput;
#endif
}

string StringUtils::ToLower(string sInput)
{
#ifndef SYMBIAN
    for (string::iterator i = sInput.begin(); i != sInput.end(); i++)
    {
        *i = tolower(*i);
    }
//  transform( sInput.begin(), sInput.end(), sInput.begin(), tolower );
    return sInput;
#else
    string sOutput(sInput);
    sOutput.tolower();
    return sOutput;
#endif
}

string StringUtils::itos( int iNum )
{
#ifndef SYMBIAN
    char acS[81];
    sprintf( acS, "%d", iNum);
    return string( acS );
#else
    return string::IntegerToString( iNum );
#endif
}

string StringUtils::Replace( string sInput, const string &sSearch, const string &sReplace )
{
    string::size_type s=0;

    while( ( s=sInput.find( sSearch, s ) ) != string::npos ) // search for the first apperence of the sSearch string
    {
        sInput.replace( s, sSearch.length(), sReplace );
        s += sReplace.length();
    }

    return sInput;
}

string StringUtils::Replace( string *sInput, const string &sSearch, const string &sReplace )
{
	return (*sInput) = StringUtils::Replace( (*sInput),sSearch,sReplace );	
}

#ifndef SYMBIAN

string StringUtils::ltos( long lNum )
{
    char acS[81];
    sprintf( acS, "%ld", lNum );
    return string( acS );
}

void StringUtils::Tokenize(string &Input, string Tokens, vector<string> &vect_strings)
{
	const char *pTokens = Tokens.c_str();
	int Size=Tokens.size();
	if( Size<1 || Size>3 )
		return; // Not supported no tokens or more than 3

	const char *pcPtr = Input.c_str();
	char *pPtr_dup = strdup(pcPtr);
	char *pPtr = pPtr_dup;
	const char *pPtr_Start = pPtr;

	while( *pPtr )
    {
		while( *pPtr && *pPtr!=pTokens[0]
			&& (Size<2 || *pPtr!=pTokens[1])
			&& (Size<3 || *pPtr!=pTokens[2]) )
				pPtr++;

		// We're stopped on a token, terminate this string and skip over any more tokens
		while( *pPtr && 
			( *pPtr==pTokens[0] ||
			  (Size>1 && *pPtr==pTokens[1]) ||
			  (Size>2 && *pPtr==pTokens[2]) ) )
		{
			(*pPtr)=0;
			pPtr++;
		}

		if( *pPtr_Start && pPtr_Start!=pPtr )
            vect_strings.push_back(pPtr_Start);
		pPtr_Start=pPtr;
    }
	free(pPtr_dup);
}

void StringUtils::Tokenize(string &Input, string Tokens, deque<string> &deque_strings, bool bPushToFront)
{
    string::size_type pos=0;
    string sToken;
    while( (sToken=StringUtils::Tokenize(Input,Tokens,pos)).length() )
    {
        if( bPushToFront )
            deque_strings.push_front(sToken);
        else
            deque_strings.push_back(sToken);
    }
}


string StringUtils::RepeatChar(char c,int count)
{
    if( count<1 )
        return "";

    char *cBuffer = new char[count+1];
    for(int i=0;i<count;++i)
        cBuffer[i]=c;
    cBuffer[count]=0;
    return cBuffer;
}

string StringUtils::TrimSpaces( string &sInput )
{
    while( sInput.length() && sInput[0] == ' ' ) // eliminate spaces from the begining
        sInput = sInput.substr(1);

    while( sInput.length() && sInput[ sInput.length()-1 ] == ' ' ) // eliminate spaces from the end
        sInput = sInput.substr( 0, sInput.length()-1 );

    return sInput;
}

int StringUtils::CompareNoCase( string sFirst, string sSecond )
{
#ifdef UNDER_CE
    // @todo CE doesn't have a stricmp function
    if ( sFirst > sSecond )
        return 1;
    if ( sFirst < sSecond )
        return -1;
    return 0;
#else
    char acFirst[256], acSecond[256];

    // convert booth strings to char arrays and then use the stricmp function
    strncpy( acFirst, sFirst.c_str(), sizeof( acFirst ) );
    strncpy( acSecond, sSecond.c_str(), sizeof( acSecond ) );
    return stricmp( acFirst, acSecond );

#endif
}

bool StringUtils::Replace( string sInputFile, string sOutputFile, string sSearch, string sReplace )
{
    size_t size;
    char *buffer = FileUtils::ReadFileIntoBuffer( sInputFile, size );

    if( !buffer ) // file is empty
        return false;

    string sInput( buffer );
    delete[] buffer; // freeing the buffer allocated by ReadFileIntoBuffer

    StringUtils::Replace( &sInput, sSearch, sReplace );

#ifndef WIN32
    system(("mkdir -p \"" + FileUtils::BasePath(sOutputFile) + "\"").c_str());
#endif

    FILE *pFile = fopen( sOutputFile.c_str(), "wb" );
    if( !pFile )
        return false; // file couldn't be written

    if( fwrite( sInput.c_str(), 1, sInput.size(), pFile ) != sInput.size() )
    {
        fclose( pFile );
        return false; // couldn't write all the content to the file
    }

    fclose( pFile );
    return true;
}

string StringUtils::ParseValueFromString( string sParameterList, string sParameterName, string sDefaultValue, const char *acDelimiters )
{
    string::size_type CurPos = 0;
    string sToken;

    if ( acDelimiters == NULL )
        acDelimiters = ";= "; // default delimiters

    while( ( sToken = StringUtils::Tokenize( sParameterList, acDelimiters, CurPos )) != "" ) // takes the first token from the parameter list
    {
        if ( StringUtils::CompareNoCase( sToken, sParameterName ) == 0 ) // if it is equal to the parameter name
            return StringUtils::Tokenize( sParameterList, acDelimiters, CurPos ); // returns the next token after the parameter name
    }
    return sDefaultValue; // the parameter name was not found in the parameter list
}

int StringUtils::ParseIntFromString( string sParameterList, string sParameterName, int iDefaultValue )
{
    string::size_type CurPos = 0;
    string sToken;

    while( ( sToken = StringUtils::Tokenize( sParameterList, ";= ", CurPos ) ) != "" ) // the first token from the parameter list
    {
        if ( StringUtils::CompareNoCase( sToken, sParameterName ) == 0 ) // if it is equal to the parameter name
            return atoi( StringUtils::Tokenize( sParameterList, ";= ", CurPos ).c_str() ); // returns the next token after the parameter name converted to int
    }
    return iDefaultValue; // the parameter name was not found in the parameter list
}

string StringUtils::Format( const char *pcFormat, ... )
{
    char acS[256];

    va_list argList;
    va_start( argList, pcFormat );
#ifdef WIN32
    _vsnprintf( acS, sizeof(acS), pcFormat, argList );
#else
    vsnprintf( acS,sizeof(acS), pcFormat, argList );
#endif
    va_end( argList );
    return string( acS );
}

string StringUtils::EscapeChars( string sInput )
{
    string sReturnValue;

    string::size_type s;
    // Replace the \ with a special sequence so that \\\\to do doesn't get escaped as a tab
    while( ( s = sInput.find( "\\\\" ) ) != string::npos )
        sInput.replace( s, 2, "~\t`" );
    while( ( s = sInput.find( "\\t" ) ) != string::npos )
        sInput.replace( s, 2, "\t" );
    while( ( s = sInput.find( "\\n" ) ) != string::npos )
        sInput.replace( s,2,"\n" );
    while( ( s = sInput.find( "~\t`" ) ) != string::npos )
        sInput.replace( s, 3, "\\" );

    string::size_type pos=0, last=0;
    while ( ( pos = sInput.find( "\\x", last ) ) != string::npos )
    {
        sReturnValue += sInput.substr( last, pos-last );
        last = pos + 2;
        string Hex;
        char ch;
        int i;
        while( last < sInput.length() && Hex.length() < 2 )
        {
            char ch = sInput[last];
            if ( ( ch >= '0' && ch <= '9' ) || ( ch >= 'a' && ch <= 'f' ) )
            {
                Hex += ch;
                last++;
            }
            else break;
        }
        sscanf( Hex.c_str(), "%x", &i );
        ch = (char)i;
        sReturnValue += ch;
    }
    sReturnValue += sInput.substr( last );
    return sReturnValue;
}

// This will put tabs in every x characters.  output can be null, meaning put it in the input again
void StringUtils::BreakIntoLines( string sInput, vector<string> *vectStrings, int iNumChars)
{
    int iNumCharsSoFar=0;
    string::size_type lastSpace = string::npos;

	int iCurrentIndex = 0;
    for( int i=0; i< (int)sInput.length() + 1; ++i )
    {
        if( iNumCharsSoFar++ >= iNumChars && ( ((int)sInput.length()) == i || sInput[i] == ' ') )
        {
            if( lastSpace != string::npos)
            {
				if(lastSpace - ( i + 1 - iNumCharsSoFar ) > iNumChars)
				{
					lastSpace = iNumChars + i + 1 - iNumCharsSoFar;
				}

				vectStrings->push_back( sInput.substr( i + 1 - iNumCharsSoFar, lastSpace - ( i + 1 - iNumCharsSoFar ) ) );
				iCurrentIndex = lastSpace;
				iNumCharsSoFar = (int)( i - lastSpace );
				while( iNumCharsSoFar > 0 && sInput[ i + 1 - iNumCharsSoFar ] == ' ' )
					--iNumCharsSoFar;
            }
            lastSpace=i;
        }
        else if ( (int)sInput.length() > i && sInput[i] == ' ' )
            lastSpace = i;
	}

	int iFirst = iCurrentIndex;
    --iNumCharsSoFar; // we'll be 1 too many since we looped for the final position too
    if( iNumCharsSoFar > 0 )
	{
		if((int)sInput.length() - iCurrentIndex > iNumChars)
		{
			while( iNumCharsSoFar > 0 )
			{
				int iLineLength = (iNumChars < int(sInput.length() - iFirst) ? iNumChars : int(sInput.length() - iFirst));
				vectStrings->push_back( sInput.substr(iFirst, iLineLength));
				iFirst += iLineLength;
				iNumCharsSoFar -= iLineLength;
			}
		}
		else
			vectStrings->push_back( sInput.substr( sInput.length() - iNumCharsSoFar));
	}
}

char StringUtils::HexByte2Num( char* pcPtr )
{
    char buf[3];
    // take the first 2 letters of the prameter and copy them to buf
    buf[0] = pcPtr[0];
    buf[1] = pcPtr[1];
    buf[2] = 0; // add the final character

    return (unsigned char) strtol( buf, NULL, 16 ); // convert to long, considering the number to be stored in hex and return it as a char
}

void StringUtils::AddressAndPortFromString( string sInput, int iDefaultPort, string &sAddress, int &iPort )
{
    string::size_type CurPos = 0;

    sAddress = Tokenize( sInput, ":", CurPos); // the address should be before the ':' character
    if ( CurPos >= sInput.length() )
    {
        iPort = iDefaultPort; // no port given
    }
    else
    {
        iPort = atoi( sInput.substr(CurPos).c_str() ); // port follows address
    }
}

string StringUtils::URLEncode( string sInput )
{
    char hexValues[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    const char *inBuffer = sInput.c_str();
    char *outBuffer = new char[(sInput.length() * 3) + 1];
    char *baseOutBuffer = outBuffer;

    while ( *inBuffer != 0 )
    {
        switch ( *inBuffer )
        {
            case '\"': case '<': case '>': case '%': case '\\':
            case  '^': case '[': case ']': case '`': case '+':
            case  '$': case ',': case '@': case ';': case '/':
            case  '!': case '#': case '?': case '=': case '&':
            case  ':':
                *outBuffer++ = '%';
                *outBuffer++ = hexValues[(*inBuffer >> 4) & 0x0F];
                *outBuffer++ = hexValues[*inBuffer & 0x0F];
                break;
            default:
                if ( *inBuffer > 32 && *inBuffer < 123 )
                    *outBuffer++ = *inBuffer;
                else
                {
                    *outBuffer++ = '%';
                    *outBuffer++ = hexValues[(*inBuffer >> 4) & 0x0F];
                    *outBuffer++ = hexValues[*inBuffer & 0x0F];
                }
        }

        inBuffer++;
    }
    *outBuffer = '\0';

    string result(baseOutBuffer);
    delete baseOutBuffer;
    return result;
}

string StringUtils::URLDecode( string sInput )
{
    const char *inBuffer = sInput.c_str();
    char *outBuffer = new char[sInput.length() + 1];
    char *baseOutBuffer = outBuffer;

    while ( *inBuffer != 0 )
    {
        if ( *inBuffer != '%' ) // if not special just copy it and continue.
            *outBuffer = *inBuffer;
        else
        {
            inBuffer++; // skip the %;

            // this routine looks to be too complicated. But i don't have time to try to fix it.
            if ( *inBuffer && *(inBuffer + 1) && // make sure we aren't past the end of string.
                ( ('0' <= *inBuffer && *inBuffer <= '9') ||
                    ('A' <= *inBuffer && *inBuffer <= 'F') ||
                    ('a' <= *inBuffer && *inBuffer <= 'f') ) && // check the range of the first character
                ( ('0' <= *(inBuffer + 1) && *(inBuffer + 1) <= '9') ||
                    ('A' <= *(inBuffer + 1) && *(inBuffer + 1) <= 'F') ||
                    ('a' <= *(inBuffer + 1) && *(inBuffer + 1) <= 'f') ) ) // check the range of the second character
            {
                if ( '0' <= *inBuffer && *inBuffer <= '9' )
                    *outBuffer = *inBuffer - '0';
                else if ('A' <= *inBuffer && *inBuffer <= 'F')
                    *outBuffer = *inBuffer - ('A' - 10);
                else if ('A' <= *inBuffer && *inBuffer <= 'F')
                    *outBuffer = *inBuffer - ('a' - 10);
                *outBuffer <<= 4;

                inBuffer++;
                if ( '0' <= *inBuffer && *inBuffer <= '9' )
                    *outBuffer |= (*inBuffer - '0');
                else if ('A' <= *inBuffer && *inBuffer <= 'F')
                    *outBuffer |= (*inBuffer - ('A' - 10));
                else if ('A' <= *inBuffer && *inBuffer <= 'F')
                    *outBuffer |= (*inBuffer - ('a' - 10));
            }
#ifndef WINCE
            else
                cerr << "Invalid string passed (found % followed by something which is not a hex digit) to URLDecode: " << sInput << endl;
#endif

        }

        outBuffer++;
        inBuffer++; // advance the source
    }


    *outBuffer = '\0';

    string result(baseOutBuffer);
    delete baseOutBuffer;
    return result;
}

unsigned int StringUtils::CalcChecksum( unsigned int iSize, char *pData )
{
    unsigned int iCS = 0;
    for( unsigned int i=0; i < iSize; ++i )
        iCS += pData[i]; // add data to checksum
    return iCS;
}

time_t StringUtils::SQLDateTime( string sSQLDate )
{
    tm t;
    if( sSQLDate.length() == 14 && sSQLDate.find( '-' ) == string::npos ) // the YYYYMMDDHHMMSS format of a timestamp
    {
        t.tm_year = atoi( sSQLDate.substr(0,4).c_str() )-1900;
        t.tm_mon = atoi( sSQLDate.substr(4,2).c_str() )-1;
        t.tm_mday = atoi( sSQLDate.substr(6,2).c_str() );
        t.tm_hour = atoi( sSQLDate.substr(8,2).c_str() );
        t.tm_min = atoi( sSQLDate.substr(10,2).c_str() );
        t.tm_sec = atoi( sSQLDate.substr(12,2).c_str() );
    }
	else if( sSQLDate.find('-')!=string::npos && sSQLDate.find(':')!=string::npos ) // yyyy-mm-dd hh:mm:ss
	{
		string::size_type p1=0,p2;
		p2 = sSQLDate.find('-',p1+1);
		t.tm_year = atoi(sSQLDate.substr(p1,p2-p1).c_str())-1900;

		p1=p2+1;
		p2 = sSQLDate.find('-',p1+1);
		if( p2 )
			t.tm_mon = atoi(sSQLDate.substr(p1,p2-p1).c_str())-1;

		p1=p2+1;
		p2 = sSQLDate.find(' ',p1+1);
		if( p2 )
			t.tm_mday = atoi(sSQLDate.substr(p1,p2-p1).c_str());

		p1=p2+1;
		p2 = sSQLDate.find(':',p1+1);
		if( p2 )
			t.tm_hour = atoi(sSQLDate.substr(p1,p2-p1).c_str());

		p1=p2+1;
		p2 = sSQLDate.find(':',p1+1);
		if( p2 )
			t.tm_min = atoi(sSQLDate.substr(p1,p2-p1).c_str());

		t.tm_sec = atoi(sSQLDate.substr(p2+1).c_str());
	}
	else if( sSQLDate.find('/')!=string::npos && sSQLDate.find(':')!=string::npos ) // yyyy-mm-dd hh:mm:ss
	{
		string::size_type p1=0,p2;
		p2 = sSQLDate.find('/',p1+1);
		t.tm_year = atoi(sSQLDate.substr(p1,p2-p1).c_str())-1900;

		p1=p2+1;
		p2 = sSQLDate.find('/',p1+1);
		if( p2 )
			t.tm_mon = atoi(sSQLDate.substr(p1,p2-p1).c_str())-1;

		p1=p2+1;
		p2 = sSQLDate.find(' ',p1+1);
		if( p2 )
			t.tm_mday = atoi(sSQLDate.substr(p1,p2-p1).c_str());

		p1=p2+1;
		p2 = sSQLDate.find(':',p1+1);
		if( p2 )
			t.tm_hour = atoi(sSQLDate.substr(p1,p2-p1).c_str());

		p1=p2+1;
		p2 = sSQLDate.find(':',p1+1);
		if( p2 )
			t.tm_min = atoi(sSQLDate.substr(p1,p2-p1).c_str());

		t.tm_sec = atoi(sSQLDate.substr(p2+1).c_str());
	}
	else 
    {
        const char *pcDate = sSQLDate.c_str();
        t.tm_year = atoi( &pcDate[0] )-1900;
        t.tm_mon = atoi( &pcDate[5] )-1;
        t.tm_mday = atoi( &pcDate[8] );
        t.tm_hour = atoi( &pcDate[11] );
        t.tm_min = atoi( &pcDate[14] );
        t.tm_sec = atoi( &pcDate[17] );
    }
    return mktime( &t );
}

string StringUtils::SQLDateTime(time_t t)
{
    char acDateTime[100];

    if( t == 0 )
        t = time(NULL);
    struct tm *tm = localtime(&t);

    sprintf( acDateTime, "%d/%d/%d %d:%d:%d",
            tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec );

    return acDateTime;
}

string StringUtils::SQLEscape( string sInput )
{
    string sOutput;
    char* pcPtr = (char*)sInput.c_str();
    while( *pcPtr )
    {
        switch( *pcPtr )
        {
            case '\'':  sOutput += "\\\'";  break;
            case '\"':  sOutput += "\\\"";  break;
//            case '%':   sOutput += "\\%";   break;  11/19/2004 Aaron -- This is breaking MySQL which doesn't seem to like escaped %'s in the select/update strings
            case '\\':  sOutput += "\\\\";  break;
            default:    sOutput += *pcPtr;
        }
        pcPtr++;
    }
    return sOutput;
}

bool StringUtils::StartsWith( string sFirst, string sSecond, bool bIgnoreCase )
{
    if( bIgnoreCase )
        return sFirst.length()>=sSecond.length() && StringUtils::ToLower(sFirst.substr(0,sSecond.length()))==StringUtils::ToLower(sSecond);
    else
        return sFirst.length()>=sSecond.length() && sFirst.substr(0,sSecond.length())==sSecond;
}

bool StringUtils::EndsWith( string sFirst, string sSecond, bool bIgnoreCase )
{
    if( bIgnoreCase )
        return sFirst.length()>=sSecond.length() && StringUtils::ToLower(sFirst.substr(sFirst.length()-sSecond.length()))==StringUtils::ToLower(sSecond);
    else
        return sFirst.length()>=sSecond.length() && sFirst.substr(sFirst.length()-sSecond.length())==sSecond;
}

time_t StringUtils::StringToDate(string Value)
{
	return 0;
}

string StringUtils::GetStringFromConsole()
{
    string sOutput;
    while(true)
    {
#ifndef WINCE
        char c = getch();
#else
        char c = getchar();
#endif

        // 8 and 127 are the two (known to me) backspace characters a terminal can send
        if ((c == 8 || c == 127))
        {
            if (sOutput.length() > 0)
            {
                sOutput = sOutput.substr(0, sOutput.length() - 1);
#ifndef WINCE
                cout << c << " " << c;
#else
                printf("%c %c", c, c);
#endif
            }
            continue;
        }
        else if( c==3 ) // CTRL+C
        {
#ifdef WIN32
            exit(1); // I don't know of another way
#else
            kill(getpid(), SIGINT);
#endif
        }
#ifndef WINCE
                cout << c;
#else
                printf("%c", c);
#endif
        if( c=='\n' || c=='\r' )
            return sOutput;

        sOutput+=c;
    }
}

string StringUtils::GetDow( int iDow, bool bFull )
{
	if( !bFull )
	{
		switch(iDow)
		{
		case 0:
			return "Sun";
		case 1:
			return "Mon";
		case 2:
			return "Tue";
		case 3:
			return "Wed";
		case 4:
			return "Thu";
		case 5:
			return "Fri";
		case 6:
			return "Sat";
		}
	}
	return "";
}

char **StringUtils::ConvertStringToArgs(string sInput,int &iNumArgs,int *p_iPosNext)
{
	if(p_iPosNext)
		(*p_iPosNext)=0;
	iNumArgs=0;
	char **pArgs = new char*[500];
	const char *pc_str = sInput.c_str();

	string::size_type pos_start=0,pos_end;

	while(pos_start<sInput.size())
	{
		// trim spaces
		while(sInput[pos_start]==' ' && pos_start<sInput.size())
			pos_start++;

		if( pos_start>=sInput.size() )
			return pArgs;

		if( sInput[pos_start]=='\n' )  // There's another set of args that follows
		{
			if( p_iPosNext )
				*p_iPosNext = pos_start+1;
			return pArgs;
		}

		pos_end=sInput.find(' ',pos_start+1);
		string::size_type pos_nextmessage = sInput.find('\n',pos_start+1);
		if( pos_nextmessage!=string::npos &&
				(pos_nextmessage<pos_end || pos_end==string::npos) )
			pos_end=pos_nextmessage;

		// Starting some new "
		if( sInput[pos_start]=='"' && (pos_end!=string::npos || sInput[sInput.size()-1]=='"') )
		{
			pos_start++;
			pos_end=pos_start;
			while(pos_end<sInput.size() && (sInput[pos_end]!='\"' || sInput[pos_end-1]=='\\') )
				pos_end++;
		}

		if( pos_end==string::npos )
			pos_end = sInput.size();

		char *pString = new char[pos_end-pos_start+1];
		strncpy(pString,&pc_str[pos_start],pos_end-pos_start);
		pString[pos_end-pos_start]=0;
		pArgs[iNumArgs++]=pString;
		pos_start=pos_end + (sInput[pos_end]=='\n' ? 0 : 1);  // This will either be a final " or a space.  We skip leading spaces anyway
	}

	return pArgs;
}

void StringUtils::FreeArgs(char **pArgs,int iNumArgs)
{
	for(int i=0;i<iNumArgs;++i)
		delete[] pArgs[i];

	delete[] pArgs;
}

string StringUtils::makeUpPlayerAddressFromPlayerId(unsigned int playerId)
{
	unsigned char values[4];

	int position = 0;
	values[0] = values[1] = values[2] = values[3] = 0;
	while ( playerId > 0 && position < 4)
	{
		values[position++] = (unsigned char)(playerId % 256);
		playerId /= 256;
	}

	return Format( "00:00:%02x:%02x:%02x:%02x", values[3], values[2], values[1], values[0]);
}

#endif //#ifndef SYMBIAN

string StringUtils::UpperAZ09Only(string sInput)
{
	string sOutput;
	for(int i=0;i<sInput.size();++i)
	{
		if( (sInput[i]>='0' && sInput[i]<='9') || (sInput[i]>='A' && sInput[i]<='Z') )
			sOutput+=sInput[i];
		else if( sInput[i]>='a' && sInput[i]<='z' )
			sOutput+=sInput[i]-32;
	}
	return sOutput;
}

