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
