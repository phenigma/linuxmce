#ifndef _STRINGUTILS
#define _STRINGUTILS

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

class StringUtils
{
public:
	// Call repeatedly to break a delimited string into tokens
	static string Tokenize(string &Input, string Tokens, string::size_type &CurPos);
	static string ToLower(string Input);
	static string ToUpper(string Input);
	static string itos(int Num);

#ifndef SYMBIAN
	// Break a delimited string into a list of the tokens
	static void Tokenize(string &Input, string Tokens, vector<string> &vect_strings);
	static void Tokenize(string &Input, string Tokens, deque<string> &deque_strings, bool bPushToFront);

	static string RepeatChar(char c,int count);
	static int CompareNoCase(string A, string B);
	static string ParseValueFromString(string ParameterList, string ParameterName, string DefaultValue, const char *Delimiters = NULL);
	static int ParseIntFromString(string ParameterList, string ParameterName, int DefaultValue);
	static void AddressAndPortFromString(string Input, int DefaultPort, string &Address, int &Port); 
	static char *ReadFileIntoBuffer(string Filename, size_t &Size);
	static string FindExtension(string name);
	static string SQLEscape(string in);
	static string URLEncode(string in);
	static string URLDecode(string in);
	static string BasePath(string Filename);
	static string Format(const char *Format, ...);
	static string EscapeChars(string Input);
	static char HexByte2Num(char* ptr);
	static string FilenameWithoutPath(string FullPath,bool bIncludeExtension=true);
	static string ValidCPPName(string Input);
	static string TrimSpaces(string &Input);
	static void BreakIntoLines(string Input,vector<string> *vectStrings,int NumChars);
	static string ltos(long Num);
	static string get_pluto_parameter(::std::string param);
	static unsigned int CalcChecksum(unsigned int size,char *data);
	static time_t SQLDateTime(string SQLDate);
	static string SQLDateTime(time_t t=0);
	static string Replace(string &Input,string Search,string Replace);
	static bool Replace(string InputFile,string OutputFile,string Search,string Replace);
	static bool FileExists(string File);
	static time_t FileDate(string sFile);
	static long FileSize(string sFile);
	// Makes a directory, including all necessary parent directories, like mkdir -p in Linux
	static void MakeDir(string Directory);
#endif //#ifndef SYMBIAN
};


#ifndef SYMBIAN

class FastParser
{
	char m_Delimiter;
	char *m_Data, *m_End;
public:
	inline bool IsAtEnd() { return (m_Data > m_End); };
	inline const char *GetCurrentPtr() { return m_Data; };
	void Setup(char *Data, const char Delimiter) { m_End = Data+strlen(Data); m_Data=Data; m_Delimiter=Delimiter; };
	void Setup(char *Data, const char Delimiter, int Length) { m_End = Data+Length-1; m_Data=Data; m_Delimiter=Delimiter; };

	int FastParser::GetNextNumParm();
	const char *FastParser::GetNextParm();
};

#ifndef WIN32

// Not really a string util.. but putting it here for a lack of a better place.

extern long GetTickCount();

#endif //#ifndef WIN32

#endif //#ifndef SYMBIAN

#endif //#ifndef _STRINGUTILS

