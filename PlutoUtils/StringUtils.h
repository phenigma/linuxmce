/**
 *
 * @file StringUtils.h
 * @brief header file for the StringUtils namespace
 * @author
 * @todo notcommented
 *
 */



#ifndef STRINGUTILS
#define STRINGUTILS

#ifndef SYMBIAN

#include <string>
#include <vector>
#include <deque>
using namespace ::std;

#else

#include "../VIPShared/MyString.h"

#define abs(x) Abs(x)
#define atoi(x) SymbianAtoi(x, 0)
#define itos(x) SymbianItoa(x)
#define strchr(x, y) (x)

#endif

namespace StringUtils
{
    /**
     * @brief breakes the input string into pieces sparated by sTokens; call it repeatedly to get all the pieces
     */
    string Tokenize( string &sInput, string sTokens, string::size_type &CurPos );

    /**
     * @brief converts input string to lowercase
     */
    string ToLower( string sInput );

    /**
     * @brief converts input string to uppercase
     */
    string ToUpper( string sInput );

    /**
     * @brief returns the string representing the integer
     */
    string itos( int iNum );

#ifndef SYMBIAN
  
    /**
     * @brief breaks the Input string in tokens and putes them in the deque_strings parameter, or a vector
     */
    void Tokenize(string &Input, string Tokens, deque<string> &deque_strings, bool bPushToFront);
	void Tokenize(string &Input, string Tokens, vector<string> &vect_strings);
    
    /**
     * @brief retrns a sring containing the specified char repeated count times
     */
    string RepeatChar(char c,int count);

    /**
     * @brief returns the string representing the long
     */
    string ltos( long lNum );

    /**
     * @brief eliminates spaces from the begining and the ending of the input string
     */
    string TrimSpaces( string &sInput );

    /**
     * @brief compares strings ignoring the case
     */
    int CompareNoCase( string sFirst, string sSecond );

	/**
     * @brief returns true if sFirst starts with sSecond
     */
    bool StartsWith( string sFirst, string sSecond );

	/**
     * @brief returns true if sFirst ends with sSecond
     */
    bool EndsWith( string sFirst, string sSecond );

	/**
     * @brief replaces the sSearch string with the sReplace string in the sInput string
     */
    string Replace( string &sInput, string sSearch, string sReplace );

    /**
     * @brief replaces the sSearch string with the sReplace string in the specified file and returns false if errors
     */
    bool Replace( string sInputFile, string sOutputFile, string sSearch, string sReplace);

    /**
     * @brief returns the value folowing the parameter name in the parameter list tokenized using the delimiters
     */
    string ParseValueFromString( string sParameterList, string sParameterName, string sDefaultValue, const char *pcDelimiters = NULL );

    /**
     * @brief same, but it returns an integer (the parameter is assumed to be a number)
     */
    int ParseIntFromString( string sParameterList, string sParameterName, int iDefaultValue );


    /**
     * @brief converts a printf format string with the parameter list into a string, like this: "%d bla", 12 becomes "12 bla"
     */
    string Format( const char *pcFormat, ... );

    /**
     * @brief @todo ask
     */
    string EscapeChars( string sInput );


    /**
     * @brief @todo ask
     */
    void BreakIntoLines( string sInput, vector<string> *vectStrings, int iNumChars );

    /**
     * @brief converts hex number stored in the first two chars of the parameter to a long and returns it as a char
     */
    char HexByte2Num( char* pcPtr );


    /**
     * @brief assumes the input to have the form: ipaddress:port and does the parsing
     */
    void AddressAndPortFromString( string sInput, int iDefaultPort, string &sAddress, int &iPort );


    /**
     * @brief adds some URL compliancy
     * @todo extend
     */
    string URLEncode( string in );

    /**
     * @brief removes some of the URL special (like %20)
     * @todo extend
     */
    string URLDecode(string in);


    /**
     * @brief the checksum is calulated by adding the values in the char representation of the data
     */
    unsigned int CalcChecksum( unsigned int iSize, char *pData );

    time_t SQLDateTime( string sSQLDate ); /** < converts the sql formatted date time into a time_t structure */
    string SQLDateTime( time_t t=0 ); /** < converts a time_t structure into a sql formatted date time */
    string SQLEscape( string sInput ); /** < convert a escape sequence so that it can be understood by sql */

#endif //#ifndef SYMBIAN
};

#endif //#ifndef _STRINGUTILS
