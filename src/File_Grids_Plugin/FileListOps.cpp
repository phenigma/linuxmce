/*
 FileListOps

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com

 Phone: +1 (877) 758-8648

 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "FileListOps.h"

#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"
#include "PlutoUtils/Other.h"

#ifndef WIN32
#include "dirent.h"
#endif

using namespace DCE;

bool PathEq(string Path1, string Path2)
{
    Path1 += "/";
    Path2 += "/";

    string::size_type pos;

    while ((pos = Path1.find("//")) != string::npos)
        Path1.replace(pos, 2, "/");
    while ((pos = Path2.find("//")) != string::npos)
        Path2.replace(pos, 2, "/");

    return Path1 == Path2;
}

void GetDirContents(list<FileDetails *> &listFileNames,string Path, string sValidExtensions_CSV)
{
    string BasePath;

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "get dir contents Base Path: %s",Path.c_str());
#endif
    string::size_type pos=0;
	int iDirNumber=0;
    while( (BasePath=StringUtils::Tokenize(Path,"\t",pos)).length() )
    {
        if (BasePath[BasePath.length()-1] != '/')
            BasePath += '/';

#ifdef WIN32
        intptr_t ptrFileList;
        _finddata_t finddata;

        ptrFileList = _findfirst((BasePath + "*.*").c_str(), & finddata);
        while (ptrFileList != -1)
        {
            if (finddata.attrib & _A_SUBDIR) //if (finddata.attrib == _A_SUBDIR)
            {
                if (finddata.name[0] != '.')
                {
                    FileDetails *fi = new FileDetails(BasePath, finddata.name, true, iDirNumber, time(NULL));
                    listFileNames.push_back(fi);
                }
            }
            else if (finddata.name[0] != '.')
            {
                string::size_type pos = 0;
                while(pos<sValidExtensions_CSV.length() || sValidExtensions_CSV.length()==0 )
                {
                    string s = StringUtils::Tokenize(sValidExtensions_CSV, ",", pos);
                    if (s.length()>0 && s.substr(0,1) == "*")
                        s = s.substr(1);
                    if ( s.length()==0 || s == ".*" || strstr(finddata.name, s.c_str()) != NULL)
                    {
                        FileDetails *fi = new FileDetails(BasePath, finddata.name, false, iDirNumber, time(NULL));
                        listFileNames.push_back(fi);
                        break;
                    }
                }
            }
            if (_findnext(ptrFileList, & finddata) < 0)
                break;
        }

        _findclose(ptrFileList);
#else // Linux
        DIR * dirp = opendir(BasePath.c_str());
        struct dirent entry;
        struct dirent * direntp = & entry;
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "opened dir %s", BasePath.c_str());
#endif
        if (dirp == NULL)
        {
            g_pPlutoLogger->Write(LV_CRITICAL, "opendir1 %s failed: %s", BasePath.c_str(), strerror(errno));
            continue;
        }

        // int x;
		// LSB-generic Large file support standard for accessing large files
        struct stat64 dirEntryStat;
        while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
        {
			// LSB-generic Large file support standard for accessing large files
            if ( stat64((BasePath + "/" + entry.d_name).c_str(), &dirEntryStat) != 0 )
            {
                g_pPlutoLogger->Write(LV_STATUS, "Could not stat directory entry: %s (%s)", entry.d_name, strerror(errno));
                continue;
            }

            // if (entry.d_type == DT_DIR )
            if ( S_ISDIR(dirEntryStat.st_mode) )
            {
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "found dir entry %s", entry.d_name);
#endif
                if (entry.d_name[0] != '.') // ignore folders starting with . {hidden and the . .. ones}
                {
                    FileDetails *fi = new FileDetails(BasePath, entry.d_name, true, iDirNumber, time(NULL));
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "adding dir");
#endif
                    listFileNames.push_back(fi);
                }
            }
            else if ( S_ISREG(dirEntryStat.st_mode) && entry.d_name[0] != '.') // ignore hidden files
            {
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "found file entry %s", entry.d_name);
#endif
                size_t pos = 0;
                while(pos<sValidExtensions_CSV.length() || sValidExtensions_CSV.length()==0 )
                {
                    string s = StringUtils::Tokenize(sValidExtensions_CSV, ",", pos);
                    if (s.length()>0 && s.substr(0,1) == "*")
                        s = s.substr(1);
                    if ( s.length()==0 || s == ".*" || strstr(entry.d_name, s.c_str()) != NULL)
                    {
                        FileDetails *fi = new FileDetails(BasePath, entry.d_name, false, iDirNumber, time(NULL));
#ifdef DEBUG
g_pPlutoLogger->Write(LV_STATUS, "added file %s", entry.d_name);
#endif
                        listFileNames.push_back(fi);
                        break;
                    }
                }
            }
        }
        closedir (dirp);
#endif
		iDirNumber++;
    }

#ifdef DEBUG
    g_pPlutoLogger->Write(LV_STATUS, "get dir contents returning: %d records",(int) listFileNames.size());
#endif
}
