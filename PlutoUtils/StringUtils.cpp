/**
 *
 * @file StringUtils.cpp
 * @brief source file for the StringUtils
 * @author
 * @todo notcommented
 *
 */

 /**
  *
  * Copyright Notice goes here
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
    #include <sys/types.h>
    #include <sys/stat.h>
	#include <iostream>
    #ifdef WIN32
        #include <direct.h>
		#include <conio.h>
    #else
        #include <dirent.h>
        #define stricmp(x, y) strcasecmp(x, y)
    #endif
#endif //#ifndef SYMBIAN

string StringUtils::Tokenize( string &sInput, string sToken, string::size_type &CurPos )
{
    if ( CurPos >= sInput.length() ) // allready extracted all the pices
        return "";

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
//	transform( sInput.begin(), sInput.end(), sInput.begin(), toupper );
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
//	transform( sInput.begin(), sInput.end(), sInput.begin(), tolower );
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

#ifndef SYMBIAN

string StringUtils::ltos( long lNum )
{
    char acS[81];
    sprintf( acS, "%ld", lNum );
    return string( acS );
}

void StringUtils::Tokenize(string &Input, string Tokens, vector<string> &vect_strings)
{
    string::size_type pos=0;
    string sToken;
    while( (sToken=StringUtils::Tokenize(Input,Tokens,pos)).length() )
        vect_strings.push_back(sToken);
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

string StringUtils::Replace( string &sInput, string sSearch, string sReplace )
{
    string::size_type s=0;

    while( ( s=sInput.find( sSearch, s ) ) != string::npos ) // search for the first apperence of the sSearch string
    {
        sInput.replace( s, sSearch.length(), sReplace );
        s += sReplace.length();
    }

    return sInput;
}

bool StringUtils::Replace( string sInputFile, string sOutputFile, string sSearch, string sReplace )
{
    size_t size;
    char *buffer = FileUtils::ReadFileIntoBuffer( sInputFile, size );

    if( !buffer ) // file is empty
        return false;

    string sInput( buffer );
    delete[] buffer; // freeing the buffer allocated by ReadFileIntoBuffer

    StringUtils::Replace( sInput, sSearch, sReplace );

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

    for( int i=0; i< (int)sInput.length() + 1; ++i )
    {
        if( iNumCharsSoFar++ >= iNumChars && ( ( (int)sInput.length() ) == i || sInput[i]==' ' ) )
        {
            if( lastSpace != string::npos )
            {
                vectStrings->push_back( sInput.substr( i + 1 - iNumCharsSoFar, lastSpace - ( i + 1 - iNumCharsSoFar ) ) );
                iNumCharsSoFar = (int)( i - lastSpace );
                while( iNumCharsSoFar > 0 && sInput[ i + 1 - iNumCharsSoFar ] == ' ' )
                    --iNumCharsSoFar;
            }
            lastSpace=i;
        }
        else if ( (int)sInput.length() > i && sInput[i] == ' ' )
            lastSpace = i;
    }

    --iNumCharsSoFar; // we'll be 1 too many since we looped for the final position too
    if( iNumCharsSoFar > 0 )
        vectStrings->push_back( sInput.substr( sInput.length() - iNumCharsSoFar));
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
    string::size_type s;
    while( ( s = sInput.find( ' ' ) ) != string::npos ) // replace spaces with '%20'
        sInput.replace( s, 1, "%20" );

    while( ( s = sInput.find( '#' ) ) != string::npos || ( s = sInput.find( ':' ) ) != string::npos
            || ( s = sInput.find( '&' ) ) != string::npos || ( s = sInput.find( '(' ) ) != string::npos
            || ( s = sInput.find( ')' ) ) != string::npos ) // eliminate other URL non-compliant characters
        sInput.replace( s, 1, "" );
    return sInput;
}

string StringUtils::URLDecode( string sInput )
{
    string::size_type s;
    while( ( s = sInput.find( "%20" ) ) != string::npos ) // replace the '%20' with ' '
        sInput.replace( s, 3, " " );
    return sInput;
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
            case '%':   sOutput += "\\%";   break;
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

string StringUtils::GetStringFromConsole()
{
	string sOutput;
	while(true)
	{
		char c = getch();
		// 8 and 127 are the two (known to me) backspace characters a terminal can send
		if ((c == 8 || c == 127) && sOutput.length() > 0)
		{
			sOutput = sOutput.substr(0, sOutput.length() - 1);
			cout << c << " " << c;
			continue;
		}
		else if( c==3 )
			exit(1); // Ctrl+c
		cout << c;
		if( c=='\n' || c=='\r' )
			return sOutput;

		sOutput+=c;
	}
}

#endif //#ifndef SYMBIAN
