#ifndef RippingJob_h
#define RippingJob_h

namespace DCE
{
	class RippingJob
	{
	public:

		/** @brief constructor*/

		MediaDevice *m_pMediaDevice_Disk_Drive;
		int m_iPK_Orbiter,m_iPK_User,m_iPK_Disc,m_iPK_MediaType;
		string m_sName,m_sTracks;

		RippingJob(MediaDevice *pMediaDevice_Disk_Drive,int iPK_Orbiter,int iPK_Disc,int iPK_MediaType,int iPK_User,string sName,string sTracks)
		{ m_pMediaDevice_Disk_Drive=pMediaDevice_Disk_Drive; m_iPK_Orbiter=iPK_Orbiter; m_iPK_Disc=iPK_Disc; m_iPK_MediaType=iPK_MediaType; m_iPK_User=iPK_User; m_sName=sName; m_sTracks=sTracks;}
	};
}

#endif
