//
// C++ Implementation: UpdatesManager
//
// Description:
//
//
// Author: Eugen Constantinescu <eugen.c@plutohome.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "UpdatesManager.h"

#include "../pluto_main/Define_DeviceData.h"

#include <sys/types.h>
#include <dirent.h>


UpdatesManager::UpdatesManager(const char * xmlPath, const char * updPath, int iInput, int iOutput)
	: updatesPath(updPath),
	  xmlUpdatesFile(xmlPath),
	  inputFd(iInput),
	  outputFd(iOutput),
	  download(true),
	  ioError(false),
	  mySqlHelper(dceconf.m_sDBHost, dceconf.m_sDBUser, dceconf.m_sDBPassword, dceconf.m_sDBName,dceconf.m_iDBPort)
{
}

UpdatesManager::~UpdatesManager()
{
}

bool UpdatesManager::Init(bool bDownload)
{
	download = bDownload;
	
	// check the current Linux MCE systems configuration
	PlutoSqlResult result_set;
	MYSQL_ROW row = NULL;
	string sql_buff = "select FK_Device,IK_DeviceData FROM Device_DeviceData where FK_DeviceData = ";
	sql_buff += StringUtils::itos( DEVICEDATA_Model_CONST );
	
	// select the models from the system
	if( (result_set.r=mySqlHelper.mysql_query_result(sql_buff.c_str())) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "UpdatesManager::Init : SQL FAILED : %s",sql_buff.c_str());
		return false;
	}
	while((row = mysql_fetch_row(result_set.r)))
	{
		if( NULL != row[0] && NULL != row[1] )
		{
			// fill id2model
			int iDevice = atoi(row[0]);
			if( !iDevice )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "UpdatesManager::Init : bad device id : %s", row[0]);
			}
			else
			{
				id2model[iDevice] = row[1];
			}
		}
	}
	
	// select the last updates from the system
	sql_buff = "select FK_Device,IK_DeviceData FROM Device_DeviceData where FK_DeviceData = ";
	sql_buff += StringUtils::itos( DEVICEDATA_LastUpdate_CONST );
	if( (result_set.r=mySqlHelper.mysql_query_result(sql_buff.c_str())) == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "UpdatesManager::Init : SQL FAILED : %s",sql_buff.c_str());
		return false;
	}
	while((row = mysql_fetch_row(result_set.r)))
	{
		if( NULL != row[0] && NULL != row[1] )
		{
			//fill id2update
			int iDevice = atoi(row[0]);
			if( !iDevice )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "UpdatesManager::Init: bad device id: %s", row[0]);
			}
			else
			{
				id2update[iDevice] = atoi(row[1]);
			}
		}
	}
	
	if( download )
	{
		// fill model2update
		for(map<long, string>::const_iterator it=id2model.begin(); it!=id2model.end(); ++it)
		{
			map<long, unsigned>::iterator itUpdate = id2update.find( (*it).first );
			if( itUpdate == id2update.end() )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "Device id %d has model %s but no update info",
					(*it).first, (*it).second.c_str());
				continue;
			}
		
			map<string, unsigned>::iterator itFind = model2update.find( (*it).second );
			if( itFind != model2update.end() )
			{
				if( (*itUpdate).second < (*itFind).second )
					(*itFind).second = (*itUpdate).second;
			}
			else
			{
				model2update[(*it).second] = (*itUpdate).second;
			}
		}
	
		// load the xml data
		xml.Clean();
		xml.SetXML(updatesPath + "/" + xmlUpdatesFile);
		xml.ParseXML();
		if( xml.Failed() )
		{
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "UpdatesManager::init : XML parser error");
			return false;
		}
	
		// check what updates has to be downloaded
		// check each model
		for(map<string, unsigned>::iterator itModel=model2update.begin();
				  itModel!=model2update.end(); ++itModel)
		{
			// check each update
			vector<UpdateNode*> &updates = xml.Updates();
			for(vector<UpdateNode*>::iterator it=updates.begin(); it!=updates.end(); ++it)
			{
				UpdateNode* pUpdate = (*it);
				// check if the update is available for current model
				if( pUpdate->UpdateId() > (*itModel).second &&
								pUpdate->IsModel((*itModel).first) )
				{
					// insert the update into the list, if it's not already there
					vector<unsigned>::iterator itD=downloadUpdates.begin();
					for(; itD!=downloadUpdates.end(); ++itD)
					{
						if( (*itD) == pUpdate->UpdateId() )
							break;
					}
					if( itD == downloadUpdates.end() )
					{
						downloadUpdates.push_back( pUpdate->UpdateId() );
					}
				}
			}
		}
	}
	
	return true;
}

bool UpdatesManager::Run()
{
	if( download )
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Updates download - start");
		// download updates
		for(vector<unsigned>::iterator it=downloadUpdates.begin(); it!=downloadUpdates.end(); ++it)
		{
			// check if it's available
			if( !CheckUpdate(*it) )
			{
				if( isIOError() )
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "IOError: check updates");
					return false;
				}
				
				LoggerWrapper::GetInstance()->Write(LV_DEBUG, "eug: updates not available");
				// try to download it
				if( DownloadUpdate(*it) )
				{
					if( isIOError() )
					{
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "IOError: download updates");
						return false;
					}
				
					LoggerWrapper::GetInstance()->Write(LV_DEBUG, "eug: updates downloaded");
					// try to validate it
					if( !ValidateUpdate(*it) )
					{
						LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update %u is invalid.", (*it));
						return false;
					}
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update %u checksum is invalid.", (*it));
					return false;
				}
			}
			
			if( isIOError() )
			{
				LoggerWrapper::GetInstance()->Write(LV_WARNING, "IOError: other ");
				return false;
			}
		}
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Updates download - end");
	}
	else
	{
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Updates processing - start");
		
		// for all updates from /home/updates, check the update id
		string model = id2model[dceconf.m_iPK_Device_Computer];
		unsigned lastUpdate = id2update[dceconf.m_iPK_Device_Computer];
		
		vector<unsigned> existingUpdates;
		if( ExistingUpdates(existingUpdates) )
		{
			for(vector<unsigned>::const_iterator it=existingUpdates.begin(); it!=existingUpdates.end(); ++it)
			{
				char temp[256];
				snprintf(temp, sizeof(temp), "%d", (*it));
				string xmlPath = updatesPath + "/" + temp + "/update.xml";
				
				xml.Clean();
				xml.SetXML(xmlPath);
				xml.ParseXML();
				
				if( !xml.Failed() )
				{
					UpdateNode * pUpdate = xml.Updates().front();
					if( lastUpdate < pUpdate->UpdateId() && 
									   pUpdate->IsModel(model) )
					{
						if( !ProcessOptionUpdate( pUpdate->UpdateId(), UpdatesXML::attrOptionsPre ) )
						{
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "ProcessOptionUpdate error: %u | %s",
								pUpdate->UpdateId(), UpdatesXML::attrOptionsPre);
							return false;
						}
						
						// process the update
						if( !ProcessUpdate( pUpdate->UpdateId() ) )
						{
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "ProcessUpdate error: %u",
								pUpdate->UpdateId());
							return false;
						}
						
						if( !SetLastUpdate( pUpdate->UpdateId() ) )
						{
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "SetLastUpdate error: %u",
								pUpdate->UpdateId());
							return false;
						}
						
						if( !ProcessOptionUpdate( pUpdate->UpdateId(), UpdatesXML::attrOptionsPost ) )
						{
							LoggerWrapper::GetInstance()->Write(LV_WARNING, "ProcessOptionUpdate error: %d | %s",
								pUpdate->UpdateId(), UpdatesXML::attrOptionsPost);
							return false;
						}
					}
				}
				else
				{
					LoggerWrapper::GetInstance()->Write(LV_CRITICAL, "XML error for update %u", (*it));
				}
			}
		}
		LoggerWrapper::GetInstance()->Write(LV_DEBUG, "Updates processing - end");
	}
	
	return true;
}

bool UpdatesManager::isIOError() const
{
	return ioError;
}

bool UpdatesManager::SetLastUpdate(unsigned uLastUpdate)
{
	string sql_buff = "update Device_DeviceData set IK_DeviceData = '";
	sql_buff += StringUtils::itos( uLastUpdate );
	sql_buff += "' where FK_DeviceData = '";
	sql_buff += StringUtils::itos( DEVICEDATA_LastUpdate_CONST );
	sql_buff += "' AND FK_Device = '";
	sql_buff += StringUtils::itos( dceconf.m_iPK_Device_Computer );
	sql_buff += "'";
	
	mySqlHelper.threaded_mysql_query(sql_buff);
	
	return true;
}

bool UpdatesManager::CheckUpdate(unsigned uId)
{
	// find the update
	UpdateNode * pUpdate = NULL;
	for(vector<UpdateNode*>::const_iterator it=xml.Updates().begin(); it!=xml.Updates().end(); ++it)
	{
		if( uId == (*it)->UpdateId() )
		{
			pUpdate = (*it);
		}
	}
	if( pUpdate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update object is invalid: %u", uId);
		return false;
	}
	
	// check if the update is available
	char cmd[256];
	snprintf(cmd, sizeof(cmd), "CHECK_UPDATE %u\n", uId);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update check: %s", cmd);
	
	int iRet = 0;
	char message[256] = "\0";
	iRet = write(outputFd, cmd, strlen(cmd));
	if(iRet == -1 || iRet == 0)
	{
		ioError = true;
		return false;
	}
	iRet = read(inputFd, &message, 255);
	if(iRet == -1 || iRet == 0)
	{
		ioError = true;
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "#Update check: %s", message);
	
	if( !strncmp(message, "OK", 2) )
		return true;
	return false;
}

bool UpdatesManager::DownloadUpdate(unsigned uId)
{
	// find the update
	UpdateNode * pUpdate = NULL;
	for(vector<UpdateNode*>::const_iterator it=xml.Updates().begin(); it!=xml.Updates().end(); ++it)
	{
		if( uId == (*it)->UpdateId() )
		{
			pUpdate = (*it);
		}
	}
	if( pUpdate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update object is invalid: %u", uId);
		return false;
	}
	
	// download url files from update
	char cmd[1024] = "\0";
	
	for(vector<UpdateProperty*>::const_iterator it=pUpdate->Files().begin(); it!=pUpdate->Files().end(); ++it)
	{
		snprintf(cmd, sizeof(cmd), "DOWNLOAD %u %s %s\n",
				 uId, (*it)->attributesMap["URL"].c_str(), (*it)->attributesMap["md5"].c_str());
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update download: %s", cmd);
	
		int iRet = 0;
		char message[256] = "\0";
		iRet = write(outputFd, cmd, strlen(cmd));
		if(iRet == -1 || iRet == 0)
		{
			ioError = true;
			return false;
		}
		iRet = read(inputFd, &message, 255);
		if(iRet == -1 || iRet == 0)
		{
			ioError = true;
			return false;
		}
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "#Update download: %s", message);
	
		if( strncmp(message, "OK", 2) )
		{
			return false;
		}
	}
	
	return true;
}

bool UpdatesManager::ValidateUpdate(unsigned uId)
{
	// find the update
	UpdateNode * pUpdate = NULL;
	for(vector<UpdateNode*>::const_iterator it=xml.Updates().begin(); it!=xml.Updates().end(); ++it)
	{
		if( uId == (*it)->UpdateId() )
		{
			pUpdate = (*it);
		}
	}
	if( pUpdate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update object is invalid: %u", uId);
		return false;
	}
	
	// create the xml temp file
	char temp[256];
	snprintf(temp, sizeof(temp), "/tmp/update_%u", pUpdate->UpdateId());
	xml.GenerateUpdateXML(pUpdate, temp);
	
	char cmd[256];
	snprintf(cmd, sizeof(cmd), "VALIDATE_UPDATE %u %s\n", uId, temp);
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update validate: %s", cmd);
	
	int iRet = 0;
	char message[256] = "\0";
	iRet = write(outputFd, cmd, strlen(cmd));
	if(iRet == -1 || iRet == 0)
	{
		ioError = true;
		return false;
	}
	iRet = read(inputFd, &message, 255);
	if(iRet == -1 || iRet == 0)
	{
		ioError = true;
		return false;
	}
	LoggerWrapper::GetInstance()->Write(LV_WARNING, "#Update validate: %s", message);
	
	if( unlink(temp) )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Unlink error: %s", temp);
	}
	
	if( !strncmp(message, "OK", 2) )
		return true;
	return false;
}

bool UpdatesManager::RemoveUpdate(unsigned uId)
{
	// find the update
	UpdateNode * pUpdate = NULL;
	for(vector<UpdateNode*>::const_iterator it=xml.Updates().begin(); it!=xml.Updates().end(); ++it)
	{
		if( uId == (*it)->UpdateId() )
		{
			pUpdate = (*it);
		}
	}
	if( pUpdate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update object is invalid: %u", uId);
		return false;
	}
	
	// remove the update from the updates folder
	int iRet = 0;
	char message[256] = "\0";
	iRet = write(outputFd, "", 255);
	if(iRet == -1 || iRet == 0)
	{
		ioError = true;
		return false;
	}
	iRet = read(inputFd, &message, 255);
	if(iRet == -1 || iRet == 0)
	{
		ioError = true;
		return false;
	}
	
	if( !strncmp(message, "OK", 2) )
		return true;
	return false;
}

bool UpdatesManager::ProcessUpdate(unsigned uId)
{
	// find the update
	UpdateNode * pUpdate = NULL;
	for(vector<UpdateNode*>::const_iterator it=xml.Updates().begin(); it!=xml.Updates().end(); ++it)
	{
		if( uId == (*it)->UpdateId() )
		{
			pUpdate = (*it);
		}
	}
	if( pUpdate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update object is invalid: %u", uId);
		return false;
	}
	
	char cmd[1024] = "\0";
	for(vector<UpdateProperty*>::const_iterator it=pUpdate->Files().begin(); it!=pUpdate->Files().end(); ++it)
	{
		string params;
		for(StringMapConstIt itPar=(*it)->attributesMap.begin(); itPar!=(*it)->attributesMap.end(); ++itPar)
		{
			params += (*itPar).first + "=\"" + (*itPar).second + "\" ";
		}
		snprintf(cmd, sizeof(cmd), "APPLY %u \"%s\" %s\n",
				 uId, (*it)->attributesMap["URL"].c_str(), params.c_str());
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update apply: %s", cmd);
	
		int iRet = 0;
		char message[256] = "\0";
		iRet = write(outputFd, cmd, strlen(cmd));
		if(iRet == -1 || iRet == 0)
		{
			ioError = true;
			return false;
		}
		iRet = read(inputFd, &message, 255);
		if(iRet == -1 || iRet == 0)
		{
			ioError = true;
			return false;
		}
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "#Update apply: %s", message);
	
		if( strncmp(message, "OK", 2) )
		{
			return false;
		}
	}
	
	return true;
}

bool UpdatesManager::ProcessOptionUpdate(unsigned uId, const char * type)
{
	// find the update
	UpdateNode * pUpdate = NULL;
	for(vector<UpdateNode*>::const_iterator it=xml.Updates().begin(); it!=xml.Updates().end(); ++it)
	{
		if( uId == (*it)->UpdateId() )
		{
			pUpdate = (*it);
		}
	}
	if( pUpdate == NULL )
	{
		LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update object is invalid: %u", uId);
		return false;
	}
	
	char cmd[1024] = "\0";
	for(vector<UpdateProperty*>::const_iterator it=pUpdate->Options().begin(); it!=pUpdate->Options().end(); ++it)
	{
		UpdateProperty * prop = (*it);
		if( prop == NULL )
		{
			// TODO
			return false;
		}
		
		StringMapIt itFind = prop->attributesMap.find("type");
		if( itFind != prop->attributesMap.end() && type == (*itFind).second )
		{
			snprintf(cmd, sizeof(cmd), "APPLY_OPTION %u %s\n", uId, prop->value.c_str());
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "Update apply_option: %s", cmd);
	
			int iRet = 0;
			char message[256] = "\0";
			iRet = write(outputFd, cmd, strlen(cmd));
			if(iRet == -1 || iRet == 0)
			{
				ioError = true;
				return false;
			}
			iRet = read(inputFd, &message, 255);
			if(iRet == -1 || iRet == 0)
			{
				ioError = true;
				return false;
			}
			LoggerWrapper::GetInstance()->Write(LV_WARNING, "#Update apply_option: %s", message);
	
			if( strncmp(message, "OK", 2) )
			{
				return false;
			}
		}
	}
	
	return true;
}

bool UpdatesManager::ExistingUpdates(vector<unsigned> & updList)
{
	DIR *dir_pointer;
	struct dirent  *dp;
	struct stat statbuf;
  
/*
	* If can't opendir, user doesn't have permission to read for
	* this dir
     */
	chdir(updatesPath.c_str());
	dir_pointer = opendir(".");
	if (!dir_pointer) {
		printf("Can't open directory %s for reading.\n", ".");
		return false;
	}

	while ((dp = readdir(dir_pointer))) {
		stat(dp->d_name, &statbuf);
        /*
		* check file type from statbuf
		*    Try "man stat" for more information on IS_xxx
		*/
		if (S_ISDIR(statbuf.st_mode)) {
			bool isNumber = true;
			int iLength = strlen(dp->d_name);
			for(int i=0; i<iLength; i++)
			{
				if( !isdigit(dp->d_name[i]) )
				{
					isNumber = false;
					break;
				}
			}
			if( isNumber )
			{
				unsigned updId = atoi(dp->d_name);
				if( updId != 0 && !access( (updatesPath + "/" + dp->d_name + "/update.xml").c_str(), R_OK) )
				{
					updList.push_back(updId);
				}
			}
//			printf("<%s>, updId %u\n", dp->d_name, updId);
		}
	}
	closedir(dir_pointer);
  
	sort(updList.begin(), updList.end());
	
	return true;
}

bool UpdatesManager::Debug(const char * name)
{
	xml.SetXML(xmlUpdatesFile);
	xml.ParseXML();
	xml.GenerateXML(name);
	
	return true;
}
