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

#ifndef RipJob_h
#define RipJob_h

#include "JobHandler/Job.h"
class Disk_Drive_Functions;

namespace DCE
{
	class PendingTaskList;
}
namespace nsJukeBox
{
	class JukeBox;
}

using namespace DCE;
using namespace nsJukeBox;

namespace nsJobHandler
{
	class RipJob : public Job
	{
		friend class RipTask;

	private:
		Disk_Drive_Functions *m_pDisk_Drive_Functions;
		JukeBox *m_pJukeBox;
		int m_iPK_Users, m_iEK_Disc, m_iDrive_Number,m_iPK_Orbiter,m_iPK_MediaType;
		string m_sFormat, m_sFileName, m_sTracks;

	public:
		RipJob(class JobHandler *pJobHandler,
			Disk_Drive_Functions *pDisk_Drive_Functions,
			JukeBox *pJukeBox,
			int iPK_Users, int iEK_Disc, int iDrive_Number, int iPK_MediaType,
			int iPK_Orbiter,
			string sFormat, string sFileName, string sTracks);
		virtual ~RipJob() {};

		virtual bool ReadyToRun();
		void AddRippingTasks();

		virtual bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
		virtual int PercentComplete();
		virtual int SecondsRemaining();
		virtual string ToString();

		virtual string GetType() { return "RipJob"; }
	};
};

#endif
