/*
	FileUtils

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
 * @file FileUtils.cpp
 * @brief source file for the file utils namespace
 * @author
 * @todo notcommented
 *
 */


#include "FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#ifndef WIN32
	#include <dirent.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <fnmatch.h>
#else
//	#include <shellapi.h>
#endif

#ifndef SYMBIAN
	#include <stdio.h>
	#include <time.h>
	#include <cctype>
	#include <algorithm>
	#include <stdarg.h>

	#ifdef WIN32
		#include <windows.h>
		#ifndef WINCE
			#include <direct.h>
			#include <io.h>
		#endif
	#else
		#include <dirent.h>
		#include <unistd.h>
		#include <fcntl.h>
	#endif

	#ifndef WINCE
        #include "md5.h"
        #include <sys/types.h>
		#include <sys/stat.h>
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

bool FileUtils::WriteBufferIntoFile( string sFileName, char *pBuffer, size_t Size )
{
	if( sFileName.length()==0 )
		return false;

	FILE *pFile = fopen( sFileName.c_str(), "wb" );
	if ( pFile == NULL )
		return false;

	fwrite( pBuffer, Size, 1, pFile );
	fclose( pFile );

	return true;
}

void FileUtils::ReadFileIntoVector( string sFileName, vector<string> &vectString )
{
	size_t s;
	char *Buffer = ReadFileIntoBuffer( sFileName, s );
	if( Buffer )
	{
		string sBuffer(Buffer);
		StringUtils::Tokenize(sBuffer,"\n",vectString);
	}
	// Strip any \r that will be in a Windows file
	for(s=0;s<vectString.size();++s)
		vectString[s] = StringUtils::Replace(vectString[s],"\r","");

	delete [] Buffer;
}

bool FileUtils::WriteVectorToFile( string sFileName, vector<string> &vectString )
{
	FILE *file = fopen(sFileName.c_str(),"wb");
	if( !file )
	{
		MakeDir( FileUtils::BasePath(sFileName) );
		file = fopen(sFileName.c_str(),"wb");
		if( !file )
			return false;
	}

	for(size_t s=0;s<vectString.size();++s)
	{
		fputs(vectString[s].c_str(),file);
#ifdef WIN32
		fputc('\r',file);
#endif
		fputc('\n',file);
	}
	fclose(file);
	return true;
}

bool FileUtils::FileExists( string sFile )
{
#ifndef WINCE
    struct stat buf;
    int iResult;

    if( sFile.length() && sFile[sFile.length()-1]=='/' )
        iResult = stat( sFile.substr(0, sFile.length()-1).c_str(), &buf );
    else
        iResult = stat( sFile.c_str(), &buf );

    return iResult == 0;
#else
	FILE *file = fopen(sFile.c_str(), "r");

	if(!file)
		return false;

	fclose(file);

	return true;
#endif
}

long FileUtils::FileSize(string sFile)
{
#ifndef WINCE
    struct stat buf;
    int iResult;

    iResult = stat( sFile.c_str(), &buf );

    if( iResult != 0 ) // checks if statistics are valid
        return 0;
    else
        return buf.st_size;
#else

	FILE *file = fopen(sFile.c_str(), "r");

	if(!file)
		return 0;

	long dwSize;

	fseek(file, 0, SEEK_END);
	dwSize = ftell(file);
	fclose(file);

	return 0;

#endif
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

void FileUtils::MakeDir(string sDirectory)
{
    // Windows understands both
    sDirectory = StringUtils::Replace( sDirectory, "\\", "/" );

    if( !StringUtils::EndsWith(sDirectory,"/") )
        sDirectory += "/";

    // Run it on each Directory in the path just to be sure
	string::size_type pos = 0; // string::size_type is UNSIGNED

    while( (pos = sDirectory.find( '/', pos+1 )) != string::npos )
	{
#ifdef WIN32
	#ifdef WINCE

		wchar_t pDirectoryW[256];
		mbstowcs(pDirectoryW, sDirectory.c_str(), 256);
		::CreateDirectory(pDirectoryW, NULL);

	#else
		mkdir( sDirectory.substr(0,pos).c_str() );
		mkdir( sDirectory.c_str() );
	#endif
#else
    mkdir( sDirectory.substr(0,pos).c_str(),0777 );
    mkdir( sDirectory.c_str(),0777 );
#endif
	}
}

bool FileUtils::DirExists( string sFile )
{
    int iResult = 0;

#ifndef WINCE
	struct stat buf;

    if( sFile.length() && sFile[sFile.length()-1]=='/' )
        iResult = stat( sFile.substr(0, sFile.length()-1).c_str(), &buf );
    else
        iResult = stat( sFile.c_str(), &buf );
#else

	wchar_t pDirectoryW[256];
	mbstowcs(pDirectoryW, sFile.c_str(), 256);

	WIN32_FIND_DATA find_data;
	HANDLE hFindFile = FindFirstFile(pDirectoryW, &find_data);

	if(hFindFile == INVALID_HANDLE_VALUE)
		return false;

	FindClose(hFindFile);
	return true;
#endif

    return iResult == 0;
}

void FileUtils::DelFile(string sFileName)
{
#ifdef WIN32
	#ifdef WINCE
		wchar_t pFileNameW[256];
		mbstowcs(pFileNameW, sFileName.c_str(), 256);
		::DeleteFile(pFileNameW);
	#else
		::DeleteFile(sFileName.c_str());
	#endif
#else
	system(("rm " + sFileName).c_str());
#endif
}

void FileUtils::DelDir(string sDirectory)
{
#ifdef WIN32

		string sDir = sDirectory + "/*";

		WIN32_FIND_DATA findData;
		::ZeroMemory(&findData, sizeof(findData));


		#ifdef WINCE
			wchar_t pDirectoryW[256];
			mbstowcs(pDirectoryW, sDir.c_str(), 256);
			HANDLE findFileHandle = FindFirstFile(pDirectoryW, &findData);
		#else
			HANDLE findFileHandle = FindFirstFile(sDir.c_str(), &findData);
		#endif

		do
		{
			string sPath = sDirectory + "/";
			string sCurrentItem;
		#ifdef WINCE
			char pPath[256];
			wcstombs(pPath, findData.cFileName, 256);
			sPath += pPath;

			sCurrentItem = string(pPath);
		#else
			sPath += findData.cFileName;
			sCurrentItem = findData.cFileName;			
		#endif

			if(sCurrentItem == "." || sCurrentItem == "..")
				continue;

			if(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				FileUtils::DelDir(sPath);
			else
				FileUtils::DelFile(sPath);
		}
		while(FindNextFile(findFileHandle, &findData));

		FindClose(findFileHandle);

		#ifdef WINCE
			mbstowcs(pDirectoryW, sDirectory.c_str(), 256);
			::RemoveDirectory(pDirectoryW);
		#else
			::RemoveDirectory(sDirectory.c_str());
		#endif
#else
	system(("rm -rf " + sDirectory).c_str());
#endif
}

string FileUtils::BasePath( string sInput )
{
#ifdef WIN32
    string::size_type s;
    while( ( s = sInput.find('\\') ) != string::npos ) // replacing all the \ in a windows path with /
        sInput.replace( s, 1, "/" );
#endif

	while ( sInput.length() > 0 && sInput[sInput.length() - 1] == '/')
        sInput = sInput.substr( 0, sInput.length() - 1 );

    string::size_type pos = sInput.rfind("/");
    if (pos == string::npos)
        sInput = "";
    else
        sInput = sInput.substr(0, pos);

    return sInput;
}

#ifndef WINCE

string FileUtils::FileWithoutExtension ( string sFileName )
{
	string sExtension = FindExtension(sFileName);
	if( sExtension.length() )
		return sFileName.substr(0, sFileName.length()-sExtension.length()-1);
	return sFileName;
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
    StringUtils::Replace(sInput,"@","_");
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
    StringUtils::Replace(sInput,"&","");

    // taking out the underscores from the begining of the file name
    while(sInput.length() && sInput[0]=='_')
        sInput=sInput.substr(1);

    // taking out the underscores from the end of the file name
    while(sInput.length() && sInput[ sInput.length()-1 ]=='_')
        sInput=sInput.substr(0,sInput.length()-1);

    return sInput;
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

#include <iostream>
using namespace std;

bool FileUtils::FindFiles(list<string> &listFiles, string sDirectory, string sFileSpec_CSV, bool bRecurse, bool bFullyQualifiedPath, int iMaxFileCount, string PrependedPath)
{
    if( !StringUtils::EndsWith(sDirectory,"/") )
        sDirectory += "/";

#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;

    ptrFileList = _findfirst((sDirectory + "*.*").c_str(), & finddata);
    while (ptrFileList != -1)
    {
        if (!(finddata.attrib & _A_SUBDIR) && finddata.name[0] != '.')
        {
            string::size_type pos = 0;
            while(true)
            {
                string s = StringUtils::Tokenize(sFileSpec_CSV, ",", pos);
                if (s.size() && s.substr(0,1) == "*")
                    s = s.substr(1);  // If it's a *.ext drop the *
                if (sFileSpec_CSV.size()==0 || (s.size() && (s == ".*" || s == "*" || StringUtils::EndsWith(finddata.name, s.c_str(),true) )) )
                {
					if( bFullyQualifiedPath )
	                    listFiles.push_back(sDirectory + finddata.name);
					else
	                    listFiles.push_back(PrependedPath + finddata.name);
                    break;
                }
                if (pos>=sFileSpec_CSV.size())
                    break;
            }

			if ( iMaxFileCount && iMaxFileCount < listFiles.size() )
				return true; // max depth hit
        }
        else if (bRecurse && (finddata.attrib & _A_SUBDIR) && finddata.name[0] != '.')
		{
			if ( FindFiles(listFiles, sDirectory + finddata.name, sFileSpec_CSV, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + finddata.name + "/") )
				return true; // if one recursive call hit the maximum depth then return now.
		}
        if (_findnext(ptrFileList, & finddata) < 0)
            break;
    }

    _findclose(ptrFileList);
#else // Linux
    DIR * dirp = opendir(sDirectory.c_str());
    struct dirent entry;
    struct dirent * direntp = & entry;

    if (dirp == NULL)
    {
        return false;
    }

    int x;
    while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
    {
		struct stat s;
		lstat((sDirectory + entry.d_name).c_str(), &s);

		if ( S_ISLNK(s.st_mode) )
		{
			char *pLinkContents = new char[s.st_size + 1];
			if ( -1 == readlink((sDirectory + entry.d_name).c_str(),  pLinkContents, s.st_size) )
			{
				printf("Error reading link contents for symlink %s (%s).\n", (sDirectory + entry.d_name).c_str(), strerror(errno));
				delete[] pLinkContents;
				continue;
			}
			if ( pLinkContents[0] == '.' && s.st_size == 1 )
			{
				delete[] pLinkContents;
				continue;
			}

			delete[] pLinkContents;
			stat((sDirectory + entry.d_name).c_str(), &s);
		}

        if (!S_ISDIR(s.st_mode) && entry.d_name[0] != '.' )
        {
            size_t pos = 0;
            while(true)
            {
                string s = StringUtils::Tokenize(sFileSpec_CSV, string(","), pos);
//				if (s.substr(0,1) == "*")  // If it's a *.ext drop the *
//					s = s.substr(1);
//				if (s == ".*" || s == "*" || StringUtils::EndsWith(entry.d_name, s.c_str(),true) )
				if (sFileSpec_CSV.size()==0 || fnmatch(s.c_str(), entry.d_name, 0) == 0 || StringUtils::EndsWith(entry.d_name, s.c_str(),true))
				{
					if( bFullyQualifiedPath )
						listFiles.push_back(sDirectory + entry.d_name);
					else
						listFiles.push_back(PrependedPath + entry.d_name);
					break;
				}
                if (pos>=sFileSpec_CSV.size())
                    break;
            }

			if ( iMaxFileCount && iMaxFileCount < listFiles.size() )
				return true;
        }
        else if (bRecurse && S_ISDIR(s.st_mode) && entry.d_name[0] != '.')
		{
			if ( FindFiles( listFiles, sDirectory + entry.d_name, sFileSpec_CSV, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + entry.d_name + "/" ) )
				return true;
		}
    }
    closedir (dirp);
#endif

	return false; // if we are here it means we didn't hit the bottom return false.
}

bool FileUtils::FindDirectories(list<string> &listDirectories, string sDirectory, bool bRecurse, bool bFullyQualifiedPath, int iMaxFileCount, string PrependedPath)
{
    if( !StringUtils::EndsWith(sDirectory,"/") )
        sDirectory += "/";

#ifdef WIN32
    intptr_t ptrFileList;
    _finddata_t finddata;

    ptrFileList = _findfirst((sDirectory + "*.*").c_str(), & finddata);
    while (ptrFileList != -1)
    {
        if ( finddata.attrib & _A_SUBDIR && finddata.name[0] != '.')
        {
			if( bFullyQualifiedPath )
	            listDirectories.push_back(sDirectory + finddata.name);
			else
	            listDirectories.push_back(PrependedPath + finddata.name);

			if ( iMaxFileCount && iMaxFileCount < listDirectories.size() )
				return true; // max depth hit
			if (bRecurse && FindDirectories(listDirectories, sDirectory + finddata.name, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + finddata.name + "/") )
					return true; // if one recursive call hit the maximum depth then return now.
		}
        if (_findnext(ptrFileList, & finddata) < 0)
            break;
    }

    _findclose(ptrFileList);
#else // Linux
    DIR * dirp = opendir(sDirectory.c_str());
    struct dirent entry;
    struct dirent * direntp = & entry;

    if (dirp == NULL)
    {
        return false;
    }

    int x;
    while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
    {
		struct stat s;
		lstat((sDirectory + entry.d_name).c_str(), &s);

		if ( S_ISLNK(s.st_mode) )
		{
			char *pLinkContents = new char[s.st_size + 1];
			if ( -1 == readlink((sDirectory + entry.d_name).c_str(),  pLinkContents, s.st_size) )
			{
				printf("Error reading link contents for symlink %s (%s).\n", (sDirectory + entry.d_name).c_str(), strerror(errno));
				delete[] pLinkContents;
				continue;
			}
			if ( pLinkContents[0] == '.' && s.st_size == 1 )
			{
				delete[] pLinkContents;
				continue;
			}

			delete[] pLinkContents;
			stat((sDirectory + entry.d_name).c_str(), &s);
		}

        if (S_ISDIR(s.st_mode) && entry.d_name[0] != '.' )
        {
			if( bFullyQualifiedPath )
				listDirectories.push_back(sDirectory + entry.d_name);
			else
				listDirectories.push_back(PrependedPath + entry.d_name);

			if ( iMaxFileCount && iMaxFileCount < listDirectories.size() )
				return true;
        
			if (bRecurse && FindDirectories( listDirectories, sDirectory + entry.d_name, true, bFullyQualifiedPath, iMaxFileCount, PrependedPath + entry.d_name + "/" ) )
				return true;
		}
    }
    closedir (dirp);
#endif

	return false; // if we are here it means we didn't hit the bottom return false.
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
			free(Buffer);
			return false;
		}
	}
	fclose(fileSource);
	fclose(fileDest);
	free(Buffer);
	return true;
}

bool FileUtils::LaunchProcessInBackground(string sCommandLine)
{
    if (sCommandLine == "")
    {
        return false;
    }

#ifndef WIN32
    //parse the args
    const int MaxArgs = 32;

    char * ptr;

	ptr = (char *) sCommandLine.c_str();
    char * args[MaxArgs];
    int i = 0;

	// this looks too complicated but i don't have time to make it cleaner :-( mtoader@gmail.com)
    args[0] = (char *) sCommandLine.c_str();
    while ( *ptr && i < MaxArgs - 1)
    {
		if ( *ptr == ' ' || *ptr == '\t' )
			*ptr++ = 0;
		while ( *ptr && (*ptr == ' ' || *ptr == '\t') ) ptr++;  // skip the white spaces.
		if ( *ptr )
		{
			args[++i] = ptr; 			// put the next thing as a parameter
			while ( *ptr && *ptr != ' ' && *ptr != '\t' ) ptr++;  // skip to the next white space. (this doesn't take into account quoted parameters) )
		}
    }

	args[++i] = 0;
    fprintf(stderr, "Found %d arguments\n", i);

    for (int x = 1 ; x < i; x++)
		fprintf(stderr, "Argument %d: %s\n", x, args[x]);

    pid_t pid = fork();
    switch (pid)
    {
        case 0: //child
        {
            fprintf(stderr, "FileUtils::LaunchProcessInBackground(), Fork was succesfull.\n");

			// set the Close on Exec flag on all the descriptors bigger than 3.
			for ( int i = 3; i < 255; i++ )
				close(i);

			fprintf(stderr, "File descriptors closed.\n");

            if ( execvp(args[1], args + 1) == -1)
			{
                fprintf(stderr, "Exec failed %s\n", strerror(errno));
				exit(99);
			}
        }

        case -1:
            fprintf(stderr, "Error starting %s, err: %s\n", sCommandLine.c_str(), strerror(errno));
            return false;

        default:
			return true;
    }
#else
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory (&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory (&pi, sizeof(pi));
    CreateProcess("C:\\WINDOWS\\system32\\cmd.exe", "/c bogus.bat", NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	return true;
#endif
}

string FileUtils::FileChecksum( string sFileName)
{
    size_t iSize = 0;
    char *pData = ReadFileIntoBuffer(sFileName, iSize);
    if( pData == NULL )
        return "";

    string sChecksum = FileChecksum(pData, iSize);
    delete pData;

    return sChecksum;
}

string FileUtils::FileChecksum( char *pData, size_t iSize)
{
    MD5_CTX ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char *)pData, (unsigned int)iSize);
    unsigned char digest[16];
    MD5Final(digest, &ctx);

    char tmp[3];
    string md5;
    for (int i = 0; i < 16; i++)
    {
        sprintf(tmp, "%02x", digest[i]);
        md5 += tmp;
    }

    return md5;
}


#endif //not WINCE
