// All the datagrid generators

class DCEFileInfo
{
public:
	DCEFileInfo(const string &sPath,const string &sName,const string &sDisplayName,bool bIsPublic,bool bIsDirectory,const time_t &tCreationDate) :
	  m_sPath(sPath),m_sRealFileName(sName),m_sCapsFileName(StringUtils::ToUpper(sName)),m_bIsPublic(bIsPublic),
	  m_sDisplayName(sDisplayName), m_bIsDirectory(bIsDirectory),m_tCreationDate(tCreationDate) {}

	DCEFileInfo(const string &sPath,const string &sName,bool bIsPublic,bool bIsDirectory,const time_t &tCreationDate) :
	  m_sPath(sPath),m_sRealFileName(sName),m_sCapsFileName(StringUtils::ToUpper(sName)),m_bIsPublic(bIsPublic),
	  m_sDisplayName(m_sRealFileName), m_bIsDirectory(bIsDirectory),m_tCreationDate(tCreationDate) {}

	string m_sPath;
	string m_sRealFileName,m_sCapsFileName,m_sDisplayName;
	bool m_bIsPublic,m_bIsDirectory;
	time_t m_tCreationDate;
};
typedef list<DCEMI_PS_VirtualDevices::DCEFileInfo *> listDCEFileInfo;

class DataGridTable *PopulateWithDevicesCommands(string GridID,string Parms,void *ExtraData=NULL);
class DataGridTable *PopulateWithFileList(string GridID,string Parms,void *ExtraData=NULL);
listDCEFileInfo *CreateDCEFileInfo(int TypeOfRequest,string Path,string sPK_User,bool bSortByDate,string &IconFile);
/*
class DataGridTable *PopulateWithResolutions(string GridID,string Parms);
class DataGridTable *PopulateWithPhoneBookAutoComplete(string GridID,string Parms);
class DataGridTable *PopulateWithCallManager(string GridID,string Parms);
class DataGridTable *PopulateWithVoicemailManager(string GridID,string Parms);
class DataGridTable *PopulateWithCallTypes(string GridID,string Parms);
class DataGridTable *PopulateWithAlarms(string GridID,string Parms);
class DataGridTable *PopulateWithRecentCalls(string GridID,string Parms);
class DataGridTable *PopulateWithSpeedDial(string GridID,string Parms);
class DataGridTable *PopulateWithDiscsInConsoles(string GridID,string Parms);
class DataGridTable *PopulateWithDiscsInJukebox(string GridID,string Parms);
class DataGridTable *PopulateWithConsoles(string GridID,string Parms);
class DataGridTable *PopulateWithFavoriteChannels(string GridID,string Parms);
class DataGridTable *PopulateWithFavoriteShows(string GridID,string Parms);
class DataGridTable *PopulateWithEPG(string GridID,string Parms);
class DataGridTable *PopulateWithEPGGenre(string GridID,string Parms);
class DataGridTable *PopulateWithEPGSearch(string GridID,string Parms);
class DataGridTable *PopulateWithEPGAutoComplete(string GridID,string Parms);
class DataGridTable *PopulateWithTracks(string GridID,string Parms);
class DataGridTable *PopulateWithWebFavorites(string GridID,string Parms);
class DataGridTable *PopulateWithAudioTracks(string GridID,string Parms);
class DataGridTable *PopulateWithSubtitleTracks(string GridID,string Parms);
class DataGridTable *PopulateWithDVDAngles(string GridID,string Parms);
class DataGridTable *PopulateWithMedialibSearch(string GridID, string Parms);
class DataGridTable *PopulateWithMedialibMarkers(string GridID, string Parms);
class DataGridTable *PopulateWithMedialibRecord(string GridID, string Parms);
class DataGridTable *PopulateWithCurrentShows(string GridID,string Parms);
class DataGridTable *PopulateWithUpcomingShows(string GridID,string Parms);
class DataGridTable *PopulateWithMediaActivities(string GridID,string Parms);
class DataGridTable *PopulateWithResetAV(string GridID,string Parms);
class DataGridTable *PopulateWithPlaylist(string GridID,string Parms);
class DataGridTable *PopulateWithScheduledRecording(string GridID,string Parms);
class DataGridTable *PopulateWithRecordingRules(string GridID,string Parms);
class DataGridTable *PopulateWithHAGActivities(string GridID,string Parms);
class DataGridTable *PopulateWithMediaAttrFiles(string GridID,string Parms);
class DataGridTable *PopulateWithMediaAttrCollections(string GridID,string Parms);
class DataGridTable *PopulateWithMediaAttrXref(string GridID,string Parms);
class DataGridTable *PopulateWithMediaAttrSearchAC(string GridID,string Parms);
class DataGridTable *PopulateWithMediaItemAttr(string GridID,string Parms);
class DataGridTable *PopulateWithAvailableTuners(string GridID,string Parms);
class DataGridTable *PopulateWithSpecialCommands(string GridID,string Parms);
*/
