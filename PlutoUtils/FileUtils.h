/**
 *
 * @file FileUtils.h
 * @brief header file for the file utils namespace
 * @author
 * @todo notcommented
 *
 */



#ifndef FILEUTILS
#define FILEUTILS

#ifndef SYMBIAN
#include <string>
#include <vector>
#include <list>
using namespace ::std;
#else

#include "MyString.h" //for Symbian compatibility

#define abs(x) Abs(x)
#define atoi(x) SymbianAtoi(x, 0)
#define itos(x) SymbianItoa(x)
#define strchr(x, y) (x)

#endif

namespace FileUtils
{
#ifndef SYMBIAN
    char *ReadFileIntoBuffer( string sFileName, size_t &Size ); /** < reads file into buffer; buffer must be freed by caller */
	bool WriteBufferIntoFile( string sFileName, char *pBuffer, size_t Size ); /** < writes a buffer into a file */
    void ReadFileIntoVector( string sFileName, vector<string> &vectString ); /** < reads file into a vector of strings for each line */
    bool WriteVectorToFile( string sFileName, vector<string> &vectString ); /** < writes a file where the vector of strings are the lines */
    bool FileExists( string sFileName ); /** < checks if the specified file exists */
    long FileSize(string sFileName); /** < returns the file size or zero if error */
    string FindExtension ( string sFileName ); /** < returns the extension of the file name specified */

#ifndef WINCE
    string FileWithoutExtension ( string sFileName ); /** < returns the filename without the extension */
    string BasePath( string sFileName ); /** < returns the path one dir up @todo ask */
    string FilenameWithoutPath( string sFullPath, bool bIncludeExtension=true ); /** < returnes only the file name from a path/filename string */
    string ValidCPPName( string sInput ); /** < converts the input file name to a valid CPP file name */
    time_t FileDate(string sFileName); /** < returns the date of the last file modification or 0 if error */
    void MakeDir(string sDirectory); /** < makes a directory, including all necessary parent directories, like mkdir -p in Linux */
    bool DirExists(string sDirectory); /** < Returns true if the directory exists */
	void FindFiles(list<string> &listFiles,string sDirectory,string sFileSpec_CSV,bool bRecurse=false,bool bFullyQualifiedPath=false, string PrependedPath=""); /** < scan sDirectory, for files matching any of the sFilespec_CVS (ie *.jpg,a*,bcd*.mpg), and store the names in listFiles.  Optionally recurse into sub-directories.  The prepended path will be prepended to any files that are found.  Normally this is for internal use only while recursing. */
	bool PUCopyFile(string sSource,string sDestination); /** < Copies a file from sSource to sDestination.  returns false if it fails, true otherwise */
#endif

#endif //#ifndef SYMBIAN
}

#endif

