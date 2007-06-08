//
// C++ Interface: UpdatesManager
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef UPDATESMANAGER_H
#define UPDATESMANAGER_H

#include "UpdatesXML.h"

#include "DCE/DCEConfig.h"
#include "PlutoUtils/MySQLHelper.h"

using namespace std;
using namespace DCE;


/**
It will analize what updates has to be downloaded and where.

	@author Eugen Constantinescu <eugen.c@plutohome.com>
*/
class UpdatesManager
{
	public:
		
		UpdatesManager(const char * xmlPath, const char * updatesPath, int iInput, int iOutput);

		~UpdatesManager();
		
		bool Init(bool bDownload);
		
		bool Run();
		
		bool ExistingUpdates(vector<unsigned> & updList);
		
		bool Debug(const char *);
		
		bool isIOError() const;
		
		// system calls
		
		bool CheckUpdate(unsigned uId);
		
		bool DownloadUpdate(unsigned uId);
		
		bool ValidateUpdate(unsigned uId);
		
		bool RemoveUpdate(unsigned uId);
		
		bool ProcessUpdate(unsigned uId);
		
		bool ProcessOptionUpdate(unsigned uId, const char * type);
		
	private:
		
		bool SetLastUpdate(unsigned uLastUpdate);
		
		string updatesPath;
		
		string xmlUpdatesFile;
		
		int inputFd;
		
		int outputFd;
		
		bool download;
		
		bool ioError;
		
		UpdatesXML xml;
		
		DCEConfig dceconf;
		
		MySqlHelper mySqlHelper;
		
		map<long, string> id2model;
		
		map<long, unsigned> id2update;
		
		map<string, unsigned> model2update;
		
		vector<unsigned> downloadUpdates;

};

#endif
