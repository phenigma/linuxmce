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

void GetDirContents(list<FileDetails *> &listFileNames,string Path, bool bSortByDate, string sValidExtensions_CSV)
{
	string BasePath;

	g_pPlutoLogger->Write(LV_STATUS, "get dir contents Base Path: %s",Path.c_str());

	string::size_type pos=0;
	while( (BasePath=StringUtils::Tokenize(Path,",",pos)).length() )
	{
		if (BasePath[BasePath.length()] != '/')
			BasePath += '/';

		g_pPlutoLogger->Write(LV_STATUS,"Scanning dir: %s",BasePath.c_str());
#ifdef WIN32
		intptr_t ptrFileList;
		_finddata_t finddata;

		ptrFileList = _findfirst((BasePath + "*.*").c_str(), & finddata);
		while (ptrFileList != -1)
		{
			if (finddata.attrib == _A_SUBDIR)
			{
				if (finddata.name[0] != '.')
				{
					FileDetails *fi = new FileDetails(BasePath, finddata.name, true, time(NULL));
					listFileNames.push_back(fi);
				}
			}
			else if (finddata.name[0] != '.')
			{
				string::size_type pos = 0;
				for (;;)
				{
					string s = StringUtils::Tokenize(sValidExtensions_CSV, ",", pos);
					if( s.length()==0 )
						break;
					if (s.substr(0,1) == "*")
						s = s.substr(1);
					if (s == ".*" || s.length()==0 || strstr(finddata.name, s.c_str()) != NULL)
					{
g_pPlutoLogger->Write(LV_WARNING, "Adding '%s' to datagrid", finddata.name);
						FileDetails *fi = new FileDetails(BasePath, finddata.name, false, time(NULL));
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
g_pPlutoLogger->Write(LV_STATUS, "opened dir %s", BasePath.c_str());

		if (dirp == NULL)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "opendir1 %s failed: %s", BasePath.c_str(), strerror(errno));
			listFileNames.clear();
			continue;
		}

		int x;
		while (dirp != NULL && (readdir_r(dirp, direntp, & direntp) == 0) && direntp)
		{
			if (entry.d_type == DT_DIR)
			{
g_pPlutoLogger->Write(LV_STATUS, "found dir entry %s", entry.d_name);
				if (entry.d_name[0] != '.')
				{
					FileDetails *fi = new FileDetails(BasePath, entry.d_name, true, time(NULL));
g_pPlutoLogger->Write(LV_STATUS, "adding dir");
					listFileNames.push_back(fi);
				}
			}
			else if (entry.d_name[0] != '.')
			{
g_pPlutoLogger->Write(LV_STATUS, "found file entry %s", entry.d_name);
				size_t pos = 0;
				for (;;)
				{
					string s = StringUtils::Tokenize(sValidExtensions_CSV, string(","), pos);
g_pPlutoLogger->Write(LV_STATUS, "comparing extension %s", s.c_str());
					if (s.substr(0,1) == "*")
						s = s.substr(1);
					if (s == ".*" || s.length()==0 || strstr(entry.d_name, s.c_str()) != NULL)
					{
						FileDetails *fi = new FileDetails(BasePath, entry.d_name, false, time(NULL));
g_pPlutoLogger->Write(LV_STATUS, "added file %s", entry.d_name);
						listFileNames.push_back(fi);
						break;
					}
				}
			}
		}
		closedir (dirp);
#endif
	}

	g_pPlutoLogger->Write(LV_STATUS, "get dir contents returning: %d records",(int) listFileNames.size());
}
