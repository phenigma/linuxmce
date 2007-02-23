/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
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
		string m_sName,m_sTracks,m_sStatus,m_sPercentage;
		bool m_bAborted;

		RippingJob(MediaDevice *pMediaDevice_Disk_Drive,int iPK_Orbiter,int iPK_Disc,int iPK_MediaType,int iPK_User,string sName,string sTracks)
		{ m_pMediaDevice_Disk_Drive=pMediaDevice_Disk_Drive; m_iPK_Orbiter=iPK_Orbiter; m_iPK_Disc=iPK_Disc; m_iPK_MediaType=iPK_MediaType; m_iPK_User=iPK_User; m_sName=sName; m_sTracks=sTracks; m_bAborted=false;}
	};
}

#endif
