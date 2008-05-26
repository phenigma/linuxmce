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
//<-dceag-d-b->
#ifndef Powerfile_C200_h
#define Powerfile_C200_h

//	DCE Implemenation for #1889 VAIO DVD Changer

#include "Gen_Devices/Powerfile_C200Base.h"
//<-dceag-d-e->
#ifdef NOTDEF
#include "Disk_Drive_Functions/Disk_Drive_Functions.h"
#include <JobHandler/Job.h>
#include <JobHandler/Task.h>
#include "Media_Plugin/MediaAttributes_LowLevel.h"
using namespace nsJobHandler;
#endif
#include <vector>

class Row_Disc;

namespace nsJukeBox
{
	class PowerfileJukebox;
}

//<-dceag-decl-b->
namespace DCE
{
	class Powerfile_C200 : public Powerfile_C200_Command
	{
//<-dceag-decl-e->
		// Private member variables
		friend class PowerfileRip_Task;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		Powerfile_C200(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~Powerfile_C200();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Impl *pDeviceData_Impl,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		bool SafeToReload(string &sReason);
		bool ReportPendingTasks(PendingTaskList *pPendingTaskList);
		void PostConnect();

//<-dceag-const2-b->!
		// The following constructor is only used if this a class instance embedded within a DCE Device.  In that case, it won't create it's own connection to the router

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	string DATA_Get_Defective_Units();
	string DATA_Get_Hacks();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
		private:
			nsJukeBox::PowerfileJukebox * m_pPowerfileJukebox;

		public:
			bool RippingProgress(class Socket *pSocket, class Message *pMessage, class DeviceData_Base *pDeviceFrom, class DeviceData_Base *pDeviceTo);

			int GetFreeDrive(int iSlot);
#ifdef NOTDEF
			Disk_Drive_Functions * GetDDF(int iDrive_Number);
#endif

		private:
			void ReleaseDrive(int iDrive_Number, int iSlot);
			void ReleaseDrive_NoMutex(int iDrive_Number, int iSlot);
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
/*
namespace DCE
{
	class Powerfile_Job : public Job
	{
		public:
			Powerfile_Job(string sName, Powerfile_C200 * pPowerfile_C200) : Job(sName), m_pPowerfile_C200(pPowerfile_C200), m_JobMutex("Powerfile Job Mutex", true) { }
			int PercentComplete() { return 0; }
			int SecondsRemaining() { return 0; }
			string ToString();

			void Remove_PowerfileTask_Slot(int iSlot);
			
			int MediaIdentified(int iSlot);
			void RippingProgress_End(int iDrive_Number, int iResult);
			void RippingProgress_Going(int iDrive_Number, int iPercent, string sName);

			Powerfile_C200 * m_pPowerfile_C200;
			pluto_pthread_mutex_t m_JobMutex;
	};

	class Powerfile_Task : public Task
	{
		public:
			Powerfile_Task(string sName, int iPriority, Job * pJob) : Task(sName, iPriority, pJob),
				m_bStop(false), m_pDDF(NULL), m_ePreTaskStatus(TASK_NOT_STARTED) {}
			
			bool m_bStop;
			int m_iDrive_Number;
			int m_iSlot;
			TaskStatus m_ePreTaskStatus;
			Disk_Drive_Functions * m_pDDF;

		protected:
			void ThreadStarted();
			void ThreadEnded();
	};
	
	class PowerfileRip_Task : public Powerfile_Task
	{
		public:
			int m_iPK_Users;
			string m_sFormat,m_sPath;
			map<int,string> m_mapTracks;

			PowerfileRip_Task(int PK_Users, string sFormat, string sPath, string sName, int iPriority, Job * pJob) : Powerfile_Task(sName, iPriority, pJob) { m_iPK_Users=PK_Users; m_sFormat=sFormat; m_sPath=sPath; }
			string Type() { return "Rip"; }
			string ToString();
			void Run();

			void RipCD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_);
			void RipDVD(Row_Disc *pRow_Disc,listMediaAttribute &listMediaAttribute_);
	};

	class PowerfileIdentify_Task : public Powerfile_Task
	{
		public:
			PowerfileIdentify_Task(string sName, int iPriority, Job * pJob) : Powerfile_Task(sName, iPriority, pJob) {}
			string Type() { return "Identify"; }
			string ToString();
			void Run();
	};
}
	*/
