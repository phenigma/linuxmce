/**
 *
 * @file FileUtils.cpp
 * @brief source file for the file utils namespace
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
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#ifndef WIN32
#include "dirent.h"
#endif

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
#include <io.h>
#else
#include <dirent.h>
#endif



#endif //#ifndef SYMBIAN

char *FileUtils::ReadFileIntoBuffer( string sFileName, size_t &Size )
{
    if( sFileName.length()==0 )
        return NULL;

    FILE *pFile = fopen( sFileName.c_str(), "rb" );
    if ( pFile == NULL )
        return NULL;

    fseek( pFile, 0L, SEEK_END );
    Size = ftell( pFile );
    fseek( pFile, 0L, SEEK_SET );

    char *pChr = new char[Size+1];
    fread( pChr, Size, 1, pFile );
    pChr[Size] = 0;
    fclose( pFile );
    return pChr;
}

string FileUtils::FindExtension( string sFileName )
{
    // This function is a bit primitive because
    // Linux doesn't have a reverse_iterator for
    // the string class, and CE doesn't have a strlwr
    // fucntion.

    string sResult = "";
    const char *pcBeginning = sFileName.c_str();
    const char *pcPtr = pcBeginning + sFileName.length();

    while( pcPtr-- > pcBeginning )
    {
        if ( *pcPtr == '.' )
        {
            while( *(++pcPtr) )
            {
                sResult += ( *pcPtr >= 'A' && *pcPtr <= 'Z' ) ? (*pcPtr) + 32 : *pcPtr;
            }
            break;
        }
    }
    return sResult;
}

string FileUtils::BasePath( string sInput )
{
    while ( sInput.length() > 0 && sInput[sInput.length() - 1] == '/')
        sInput = sInput.substr( 0, sInput.length() - 1 );

    string::size_type pos = sInput.rfind("/");
    if (pos == string::npos)
        sInput = "";
    else
        sInput = sInput.substr(0, pos);

    return sInput;
}

string FileUtils::FilenameWithoutPath( string sFullPath, bool bIncludeExtension )
{
    if( sFullPath.length()<3 ) // cannot be anything but the file name
        return sFullPath;

    string::size_type iPos=0, iLastSlash=-1; // this way +1 will be 0.  Last slash will be -1, so if there is none, all the +1's below will start with the first character

#ifdef WIN32
    string::size_type s;
    while( ( s = sFullPath.find('\\') ) != string::npos ) // replacing all the \ in a windows path with /
        sFullPath.replace( s, 1, "/" );
#endif

    iPos--; // We start with a +1 below
    while( ( iPos = sFullPath.find( "/", iPos+1 ) ) != string::npos && iPos < sFullPath.length()-1 ) // finding the last slash
        iLastSlash = iPos;

    if( bIncludeExtension ) // @ask
    {
        if( sFullPath[sFullPath.length()-1]=='/' )
            return sFullPath.substr( iLastSlash+1, sFullPath.length()- iLastSlash-2 );
        else
            return sFullPath.substr( iLastSlash+1 );
    }

    if( sFullPath[sFullPath.length()-1]=='/' )
        sFullPath = sFullPath.substr( iLastSlash+1, sFullPath.length() - iLastSlash - 2 );
    else
        sFullPath = sFullPath.substr( iLastSlash+1 );

    iPos=0; --iPos;// We start with a +1 below
    while( ( iPos = sFullPath.find( ".", iPos+1 )) != string::npos && iPos < sFullPath.length()-1 )
        iLastSlash = iPos;

    if( iLastSlash != string::npos && iLastSlash>0 ) // 10/30/04 aaron -- what was this for??? ->  it means a long extension and short filename don't work && iLastSlash >= sFullPath.length()-5 )
        return sFullPath.substr( 0, iLastSlash );
    else
        return sFullPath;
}

string FileUtils::ValidCPPName(string sInput)
{
    StringUtils::TrimSpaces(sInput); // elliminting the spaces

    // replacing all the invalid characters
    StringUtils::Replace(sInput," ","_");
    StringUtils::Replace(sInput,":","_");
    StringUtils::Replace(sInput,"=","_");
    StringUtils::Replace(sInput,",","_");
    StringUtils::Replace(sInput,"-","");
    StringUtils::Replace(sInput,"/","");
    StringUtils::Replace(sInput,"#","Num");
    StringUtils::Replace(sInput,"*","");
    StringUtils::Replace(sInput,"$","");
    StringUtils::Replace(sInput,".","");
    StringUtils::Replace(sInput,"\"","");
    StringUtils::Replace(sInput,"(","_");
    StringUtils::Replace(sInput,")","_");
    StringUtils::Replace(sInput,"?","_");
    StringUtils::Replace(sInput,"<","_");
    StringUtils::Replace(sInput,">","");
    StringUtils::Replace(sInput,"__","_");
    StringUtils::Replace(sInput,"__","_");
    StringUtils::Replace(sInput,"%","");

    // taking out the underscores from the begining of the file name
    while(sInput.length() && sInput[0]=='_')
        sInput=sInput.substr(1);

    // taking out the underscores from the end of the file name
    while(sInput.length() && sInput[ sInput.length()-1 ]=='_')
        sInput=sInput.substr(0,sInput.length()-1);

    return sInput;
}

bool FileUtils::FileExists( string sFile )
{
    struct stat buf;
    int iResult;

    if( sFile.length() && sFile[sFile.length()-1]=='/' )
        iResult = stat( sFile.substr(0, sFile.length()-1).c_str(), &buf );
    else
        iResult = stat( sFile.c_str(), &buf );

    return iResult == 0;
}

bool FileUtils::DirExists( string sFile )
{
    struct stat buf;
    int iResult;

    if( sFile.length() && sFile[sFile.length()-1]=='/' )
        iResult = stat( sFile.substr(0, sFile.length()-1).c_str(), &buf );
    else
        iResult = stat( sFile.c_str(), &buf );

    return iResult == 0;
}

time_t FileUtils::FileDate(string sFile)
{
    struct stat buf;
    int iResult;

    iResult = stat( sFile.c_str(), &buf );

    if( iResult != 0 ) // checks if statistics are valid
        return 0;
    else
        return buf.st_mtime;

}

long FileUtils::FileSize(string sFile)
{
    struct stat buf;
    int iResult;

    iResult = stat( sFile.c_str(), &buf );

    if( iResult != 0 ) // checks if statistics are valid
        return 0;
    else
        return buf.st_size;
}

void FileUtils::MakeDir(string sDirectory)
{
    // Windows understands both
    sDirectory = StringUtils::Replace( sDirectory, "\\", "/" );

    // Run it on each Directory in the path just to be sure
    string::size_type pos = -1;
    while( (pos = sDirectory.find( '/', pos+1 )) != string::npos )
#ifdef WIN32
    mkdir( sDirectory.substr(0,pos).c_str() );
    mkdir( sDirectory.c_str() );
#else
    mkdir( sDirectory.substr(0,pos).c_str(),0777 );
    mkdir( sDirectory.c_str(),0777 );
#endif
}


#include <iostream>
using namespace std;

void FileUtils::FindFiles(list<string> &listFiles,string sDirectory,string sFileSpec_CSV,bool bRecurse,string PrependedPath)
{
    if( !StringUtils::EndsWith(sDirectory,"/") )
        sDirectory += "/";

#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;

    ptrFileList = _findfirst((sDirectory + "*.*").c_str(), & finddata);
    while (ptrFileList != -1)
    {
        if (finddata.attrib != _A_SUBDIR && finddata.name[0] != '.')
        {
            string::size_type pos = 0;
            for (;;)
            {
                string s = StringUtils::Tokenize(sFileSpec_CSV, ",", pos);
                if (s.substr(0,1) == "*")
                    s = s.substr(1);  // If it's a *.ext drop the *
                if (s == "")
                    break;
                if (s == ".*" || StringUtils::EndsWith(finddata.name, s.c_str(),true) )
                {
                    listFiles.push_back(PrependedPath + finddata.name);
                    break;
                }
            }
        }
        else if (bRecurse && finddata.attrib == _A_SUBDIR && finddata.name[0] != '.')
		{
			FindFiles(listFiles,sDirectory + finddata.name,sFileSpec_CSV,true,PrependedPath + finddata.name + "/");
		}
        if (_findnext(ptrFileList, & finddata) < 0)
            break;
    }

    _findclose(ptrFileList);
#else // Linux
    DIR * dirp = opendir(sDirectory.c_str());
    struct dirent entry;
    struct dirent * direntp = & entry;
// g_pPlutoLogger->Write(LV_STATUS, "opened dir %s", BasePath.c_str());

    if (dirp == NULL)
    {
//      g_pPlutoLogger->Write(LV_CRITICAL, "opendir1 %s failed: %s", BasePath.c_str(), strerror(errno));
        return;
    }

    int x;
    while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
    {
		struct stat s;
		stat((sDirectory + entry.d_name).c_str(), &s);
        if (!S_ISDIR(s.st_mode) && entry.d_name[0] != '.' )
        {
// g_pPlutoLogger->Write(LV_STATUS, "found file entry %s", entry.d_name);
            size_t pos = 0;
            for (;;)
            {
                string s = StringUtils::Tokenize(sFileSpec_CSV, string(","), pos);
// g_pPlutoLogger->Write(LV_STATUS, "comparing extension %s", s.c_str());
                if (s.substr(0,1) == "*")  // If it's a *.ext drop the *
                    s = s.substr(1);
                if (s == "")
                    break;
                if (s == ".*" || StringUtils::EndsWith(entry.d_name, s.c_str(),true) )
                {
// g_pPlutoLogger->Write(LV_STATUS, "added file %s", entry.d_name);
                    listFiles.push_back(PrependedPath + entry.d_name);
                    break;
                }
            }
        }
        else if (bRecurse && S_ISDIR(s.st_mode) && entry.d_name[0] != '.')
		{
			FindFiles(listFiles,sDirectory + entry.d_name,sFileSpec_CSV,true,PrependedPath + entry.d_name + "/");
		}
    }
    closedir (dirp);
#endif
}

// Use a 1 meg buffer
#define BUFFER_SIZE		1000000
#include <iostream>
bool FileUtils::PUCopyFile(string sSource,string sDestination)
{
#ifndef WIN32
	system(("mkdir -p \"" + FileUtils::BasePath(sDestination) + "\"").c_str());
#endif

	FILE *fileSource = fopen(sSource.c_str(),"rb");
	if( !fileSource )
		return false;

	FILE *fileDest = fopen(sDestination.c_str(),"wb");
	if( !fileDest )
	{
		fclose(fileSource);
		return false;
	}
	
#ifndef WIN32
	{
		struct stat srcStat;
		fstat(fileno(fileSource), &srcStat);
		fchmod(fileno(fileDest), srcStat.st_mode);
	}
#endif
	
	void *Buffer = malloc(BUFFER_SIZE);
	size_t BytesRead;
	while((BytesRead = fread(Buffer,1,BUFFER_SIZE,fileSource)) > 0)
	{
		size_t BytesWritten = fwrite(Buffer,1,BytesRead,fileDest);
		if( BytesWritten!=BytesRead )
		{
			fclose(fileSource);
			fclose(fileDest);
			return false;
		}
	}
	fclose(fileSource);
	fclose(fileDest);
	return true;
}

