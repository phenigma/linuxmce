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

#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#ifndef SYMBIAN
#include <stdio.h>
#include <time.h>
#include <cctype>
#include <algorithm>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <direct.h>
#else
#include <dirent.h>
#define stricmp(x, y) strcasecmp(x, y)
#endif

#endif //#ifndef SYMBIAN

string StringUtils::Tokenize(string &Input, string Token, string::size_type &CurPos)
{
	if (CurPos >= Input.length())
		return "";

	string::size_type TokenPos;

	if (Token.length() > 1)
	{
		TokenPos = Input.length()+1;

		for(string::size_type i=0;i<Token.length();i++)
		{
			string::size_type NewPos = CurPos;
			Tokenize(Input, Token.substr(i, 1), NewPos);
			if (NewPos < TokenPos)
				TokenPos = NewPos;
		}
		TokenPos--;
	}			
	else
	{
		TokenPos = Input.find(Token, CurPos);
		if (TokenPos==string::npos)
		{
			TokenPos = Input.length();
		}
	}
	string ReturnValue = Input.substr(CurPos, (TokenPos - CurPos));
	CurPos = TokenPos+1;
    return ReturnValue;
} 

string StringUtils::ToUpper(string Input)
{
#ifndef SYMBIAN
	transform(Input.begin(),Input.end(),Input.begin(),toupper);

	return Input;
#else
	string Output(Input);
	Output.toupper();

	return Output;
#endif
}

string StringUtils::ToLower(string Input)
{
#ifndef SYMBIAN
	transform(Input.begin(),Input.end(),Input.begin(),tolower);
	return Input;
#else
	string Output(Input);
	Output.tolower();

	return Output;
#endif
}

string StringUtils::itos(int Num)
{
#ifndef SYMBIAN
	char s[81];
	sprintf(s, "%d", Num);
	return string(s);
#else
	return SymbianItoa(Num);
#endif
}

#ifndef SYMBIAN

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


#define PLUTO_CONFIG_FILE "../etc/pluto.conf"

bool FileUtils::FileExists(string sFile)
{
	struct stat buf;
	int result;

	if( sFile.length() && sFile[sFile.length()-1]=='/' )
		result = stat( sFile.substr(0, sFile.length()-1).c_str(), &buf );
	else
		result = stat( sFile.c_str(), &buf );

	return result == 0;
}

time_t StringUtils::SQLDateTime(string SQLDate)
{
	tm t;
	if( SQLDate.length()==14 && SQLDate.find('-')==string::npos )
	{
		// The YYYYMMDDHHMMSS format of a timestamp
		t.tm_year = atoi(SQLDate.substr(0,4).c_str())-1900;
		t.tm_mon = atoi(SQLDate.substr(4,2).c_str())-1;
		t.tm_mday = atoi(SQLDate.substr(6,2).c_str());
		t.tm_hour = atoi(SQLDate.substr(8,2).c_str());
		t.tm_min = atoi(SQLDate.substr(10,2).c_str());
		t.tm_sec = atoi(SQLDate.substr(12,2).c_str());
	}
	else
	{
		const char *Date = SQLDate.c_str();
		t.tm_year = atoi(&Date[0])-1900;
		t.tm_mon = atoi(&Date[5])-1;
		t.tm_mday = atoi(&Date[8]);
		t.tm_hour = atoi(&Date[11]);
		t.tm_min = atoi(&Date[14]);
		t.tm_sec = atoi(&Date[17]);
	}
	return mktime(&t);
}

string StringUtils::TrimSpaces(string &Input)
{
	while(Input.length() && Input[0]==' ')
		Input=Input.substr(1);

	while(Input.length() && Input[ Input.length()-1 ]==' ')
		Input=Input.substr(0,Input.length()-1);

	return Input;
}

string FileUtils::ValidCPPName(string Input)
{
	TrimSpaces(Input);
	Replace(Input," ","_");
	Replace(Input,":","_");
	Replace(Input,"=","_");
	Replace(Input,"-","");
	Replace(Input,"/","");
	Replace(Input,"#","Num");
	Replace(Input,"*","");
	Replace(Input,"$","");
	Replace(Input,"%","");
	Replace(Input,".","");
	Replace(Input,"\"","");
	Replace(Input,"(","_");
	Replace(Input,")","_");
	Replace(Input,"?","_");
	Replace(Input,"<","_");
	Replace(Input,">","");
	Replace(Input,"__","_");
	Replace(Input,"__","_");

	while(Input.length() && Input[0]=='_')
		Input=Input.substr(1);

	while(Input.length() && Input[ Input.length()-1 ]=='_')
		Input=Input.substr(0,Input.length()-1);

	return Input;
}


string StringUtils::SQLDateTime(time_t t)
{
	char DateTime[100];

	if( t==0 )
		t = time(NULL);
	struct tm *tm = localtime(&t);

	sprintf(DateTime,"%d/%d/%d %d:%d:%d",
		tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

	return DateTime;
}

unsigned int StringUtils::CalcChecksum(unsigned int size,char *data)
{
	unsigned int iCS=0;
	for(unsigned int i=0;i<size;++i)
		iCS += data[i];

	return iCS;
}

::std::string StringUtils::get_pluto_parameter(std::string param) {
  FILE *pfile;
  char line[512];
  char char_result[512];
  ::std::string result;
  char *pos;

  result="";
  pfile=fopen(PLUTO_CONFIG_FILE,"r");

  if( !pfile )
	  return "";

  while (fgets(line,511,pfile)!=NULL) {
      pos=strstr(line,param.c_str());
      if (pos!=NULL) {
          sscanf(line,"%*s = %s",&char_result[0]);
		  result=::std::string(char_result);
      }
  }

  if (pfile!=NULL) {
      fclose(pfile);
  }

  return result;
}

// This will put tabs in every x characters.  output can be null, meaning put it in the input again
void StringUtils::BreakIntoLines(string Input,vector<string> *vectStrings,int NumChars)
{
	int NumCharsSoFar=0;
	string::size_type lastSpace = string::npos;
	for(int i=0;i<(int) Input.length()+1;++i)
	{
		if( NumCharsSoFar++>=NumChars && ( ((int) Input.length())==i || Input[i]==' ') )
		{
			if( lastSpace!=string::npos )
			{
				vectStrings->push_back(Input.substr(i+1-NumCharsSoFar,lastSpace-(i+1-NumCharsSoFar)));
				NumCharsSoFar=(int) (i-lastSpace);
				while( NumCharsSoFar>0 && Input[i+1-NumCharsSoFar]==' ' )
					--NumCharsSoFar;
			}
			lastSpace=i;
		}
		else if( (int) Input.length()>i && Input[i]==' ' )
			lastSpace=i;
	}
	--NumCharsSoFar; // We'll be 1 too many since we looped for the final position too
	if( NumCharsSoFar>0 )
		vectStrings->push_back(Input.substr(Input.length()-NumCharsSoFar));
}

time_t FileUtils::FileDate(string sFile)
{
	struct stat buf;
	int result;

	result = stat( sFile.c_str(), &buf );

   /* Check if statistics are valid: */
	if( result != 0 )
		return 0;
	else
		return buf.st_mtime;

		/* 
      printf( "File size     : %ld\n", buf.st_size );
      printf( "Drive         : %c:\n", buf.st_dev + 'A' );
      printf( "Time modified : %s", _ctime64( &buf.st_mtime )
	  */
}

long FileUtils::FileSize(string sFile)
{
	struct stat buf;
	int result;

	result = stat( sFile.c_str(), &buf );

   /* Check if statistics are valid: */
	if( result != 0 )
		return 0;
	else
		return buf.st_size;

		/* 
      printf( "File size     : %ld\n", buf.st_size );
      printf( "Drive         : %c:\n", buf.st_dev + 'A' );
      printf( "Time modified : %s", _ctime64( &buf.st_mtime )
	  */
}


string FileUtils::FilenameWithoutPath(string FullPath,bool bIncludeExtension)
{
	if( FullPath.length()<3 )
		return FullPath;

	string::size_type pos=0,LastSlash=0; // this way +1 will be 0
#ifdef WIN32
	string::size_type s;
	while( (s=FullPath.find('\\'))!=string::npos )
		FullPath.replace(s,1,"/");
#endif
	pos--; // We start with a +1 below
	while( (pos=FullPath.find("/",pos+1))!=string::npos && pos<FullPath.length()-1 )
		LastSlash=pos;

	if( bIncludeExtension )
	{
		if( FullPath[FullPath.length()-1]=='/' ) 
			return FullPath.substr(LastSlash+1,FullPath.length()-LastSlash-2);
		else
			return FullPath.substr(LastSlash+1);
	}

	if( FullPath[FullPath.length()-1]=='/' ) 
		FullPath = FullPath.substr(LastSlash+1,FullPath.length()-LastSlash-2);
	else
		FullPath = FullPath.substr(LastSlash+1);

	pos=0; --pos;// We start with a +1 below
	while( (pos=FullPath.find(".",pos+1))!=string::npos && pos<FullPath.length()-1 )
		LastSlash=pos;

	if( LastSlash!=string::npos && LastSlash>0 && LastSlash>=FullPath.length()-5 )
		return FullPath.substr(0,LastSlash);
	else
		return FullPath;
}


string StringUtils::Format(const char *Format, ...)
{
	char s[256];

	va_list argList;
	va_start(argList, Format);
#ifdef WIN32
	_vsnprintf(s, sizeof(s), Format, argList);
#else
	vsnprintf(s,sizeof(s), Format, argList);
#endif
	va_end(argList);
    return string(s);   
}

string StringUtils::ltos(long Num)
{
	char s[81];

	sprintf(s, "%ld", Num);
	return string(s);
}

int StringUtils::CompareNoCase(string A, string B)
{
#ifdef UNDER_CE
// TODO: CE doesn't have a stricmp function
	if (A>B)
		return 1;
	if (A<B)
		return -1;
	return 0;
#else
	char a[256], b[256];

	strncpy(a, A.c_str(), sizeof(a));
	strncpy(b, B.c_str(), sizeof(b));

	return stricmp(a,b);
#endif
}

string StringUtils::ParseValueFromString(string ParameterList, string ParameterName, string DefaultValue, const char *Delimiters)
{
	string::size_type CurPos = 0;
	string Token;
	if (Delimiters == NULL)
		Delimiters = ";= ";

	while((Token = StringUtils::Tokenize(ParameterList, Delimiters, CurPos))!="")
	{
		if (StringUtils::CompareNoCase(Token, ParameterName)==0)
			return StringUtils::Tokenize(ParameterList, Delimiters, CurPos);
	}
	return DefaultValue;
}

int StringUtils::ParseIntFromString(string ParameterList, string ParameterName, int DefaultValue)
{
	string::size_type CurPos = 0;
	string Token;

	while((Token = StringUtils::Tokenize(ParameterList, ";= ", CurPos))!="")
	{
		if (StringUtils::CompareNoCase(Token, ParameterName)==0)
			return atoi(StringUtils::Tokenize(ParameterList, ";= ", CurPos).c_str());
	}
	return DefaultValue;
}

void StringUtils::AddressAndPortFromString(string Input, int DefaultPort, string &Address, int &Port)
{
	string::size_type CurPos = 0;

	Address = Tokenize(Input, ":", CurPos);
	if (CurPos >= Input.length())
	{
		Port=DefaultPort;
	}
	else
	{
		Port = atoi(Input.substr(CurPos).c_str());
	}
}

char *FileUtils::ReadFileIntoBuffer(string Filename, size_t &Size)
{
	Size=0;
	if( Filename.length()==0 )
		return NULL;

	FILE *f = fopen(Filename.c_str(),"rb");
	if (f==NULL)
		return NULL;

	fseek(f, 0L, SEEK_END);
	Size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char *c = new char[Size+1];
	fread(c,Size,1,f);
	c[Size] = 0;
	fclose(f);
	return c;
}

string StringUtils::SQLEscape(string in)
{
	string out;
	char* ptr = (char*)in.c_str();
	while(*ptr)
	{
		switch(*ptr)
		{
			case '\'':	out+="\\\'";	break;
			case '\"':	out+="\\\"";	break;
			case '%':	out+="\\%";	break;
			case '\\':	out+="\\\\";	break;
			default:	out+=*ptr;
		}
		ptr++;
	}
	return out;
}

string FileUtils::FindExtension(string name)
{
// This function is a bit primitive because
// Linux doesn't have a reverse_iterator for 
// the string class, and CE doesn't have a strlwr
// fucntion.  

	string result = "";
	const char *beginning = name.c_str(); 
	const char *p = beginning + name.length();

	while(p-- > beginning)
	{
		if (*p=='.')
		{
			while(*(++p))
			{
				result+=(*p>='A' && *p<='Z') ? (*p)+32 : *p;
			}
			break;

		}
	}
	return result;
}

string FileUtils::BasePath(string Input)
{
	while (Input.length() > 0 && Input[Input.length() - 1] == '/')
		Input = Input.substr(0, Input.length() - 1);

	string::size_type pos = Input.rfind("/");
	if (pos == string::npos)
		Input = "";
	else
		Input = Input.substr(0, pos);

	return Input;
}

int FastParser::GetNextNumParm()
{
	if (!(*m_Data) || IsAtEnd())
		return 0;

    register int c;
    int total=0;         
    int sign;          

    c = (int)*m_Data++;
    sign = c;           
    if (c == '-' || c == '+')
		c = (int)*m_Data++;    

	do
	{
		if (c==m_Delimiter)
			break;
		if (c>='0' && c<='9') 
	        total = 10 * total + (c-'0');     
        c = *m_Data++;    
    }  while(c); //(*m_Data);

    if (sign == '-')
        return -total;
    else
        return total;  
}


const char *FastParser::GetNextParm()
{
	if (IsAtEnd())
		return NULL;

	char *ReturnValue = m_Data;

	for ( ; *m_Data ; m_Data++ )
		if ( *m_Data == m_Delimiter) 
		{
			*m_Data = '\0';
            break;
        }
	m_Data++;
	return (const char *)ReturnValue;
}


string StringUtils::URLEncode(string in)
{
	string::size_type s;
	while( (s=in.find(' '))!=string::npos )
		in.replace(s,1,"%20");

	while( (s=in.find('#'))!=string::npos || (s=in.find(':'))!=string::npos || (s=in.find('&'))!=string::npos || (s=in.find('('))!=string::npos || (s=in.find(')'))!=string::npos)
		in.replace(s,1,"");
	return in;
}

string StringUtils::URLDecode(string in)
{
	string::size_type s;
	while( (s=in.find("%20"))!=string::npos )
		in.replace(s,3," ");
	return in;
}

string StringUtils::Replace(string &Input,string Search,string Replace)
{
	string::size_type s=0;

	while( (s=Input.find(Search,s))!=string::npos )
	{
		Input.replace(s,Search.length(),Replace);
		s+=Replace.length();
	}

	return Input;
}

bool StringUtils::Replace(string InputFile,string OutputFile,string Search,string Replace)
{
	size_t size;
	char *buffer = ReadFileIntoBuffer(InputFile,size);
	if( !buffer )
		return false;

	string Input(buffer);
	delete[] buffer;

	StringUtils::Replace(Input,Search,Replace);

	FILE *file = fopen(OutputFile.c_str(),"wb");
	if( !file )
		return false;

	if( fwrite(Input.c_str(),1,Input.size(),file)!=Input.size() )
	{
		fclose(file);
		return false;
	}

	fclose(file);
	return true;
}

string StringUtils::EscapeChars(string Input)
{
	string ReturnValue;

	string::size_type s;
	// Replace the \ with a special sequence so that \\\\to do doesn't get escaped as a tab
	while( (s=Input.find("\\\\"))!=string::npos )
		Input.replace(s,2,"~\t`");
	while( (s=Input.find("\\t"))!=string::npos )
		Input.replace(s,2,"\t");
	while( (s=Input.find("\\n"))!=string::npos )
		Input.replace(s,2,"\n");
	while( (s=Input.find("~\t`"))!=string::npos )
		Input.replace(s,3,"\\");

	string::size_type pos=0, last=0;
	while ((pos = Input.find("\\x", last))!=string::npos)
	{
		ReturnValue+=Input.substr(last, pos-last);
		last=pos+2;
		string Hex;
		char ch;
		int i;
		while(last < Input.length() && Hex.length() < 2)
		{
			char ch = Input[last];
			if ((ch>='0' && ch<='9') || (ch>='a' && ch<='f'))
			{
				Hex+=ch;
				last++;
			}
			else
				break;
		} 	
		sscanf(Hex.c_str(), "%x", &i);
		ch = (char)i;
		ReturnValue+=ch;
	}
	ReturnValue+=Input.substr(last);
	return ReturnValue;
}

char StringUtils::HexByte2Num(char* ptr)
{
	char buf[3];
	buf[0] = ptr[0];
	buf[1] = ptr[1];
	buf[2] = 0;
	
	return (unsigned char) strtol(buf, NULL, 16);
}

void FileUtils::MakeDir(string Directory)
{
	// Windows understands both
	Directory = StringUtils::Replace(Directory,"\\","/");

	// Run it on each Directoryectory in the path just to be sure
	string::size_type pos=-1;
	while( (pos=Directory.find('/',pos+1))!=string::npos )
#ifdef WIN32
		mkdir( Directory.substr(0,pos).c_str() );
	mkdir( Directory.c_str() );
#else
		mkdir( Directory.substr(0,pos).c_str(),0777 );
	mkdir( Directory.c_str(),0777 );
#endif
}

string StringUtils::RepeatChar(char c,int count)
{
	char *cBuffer = new char[count+1];
	for(int i=0;i<count;++i)
		cBuffer[i]=c;
	cBuffer[count]=0;
	return cBuffer;
}

#endif //#ifndef SYMBIAN

