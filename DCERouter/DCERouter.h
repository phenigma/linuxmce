#ifndef Router_H
#define Router_H

#include "SocketListener.h"
#include "DCE/Logger.h"
#include "AlarmManager.h"
#include "SerializeClass/SerializeClass.h"
#include "DeviceData_Router.h"
#include "PlutoUtils/MySqlHelper.h"

#include <set>
#include <queue>
#include <mysql.h>

#include "Command_Impl.h"

class Database_pluto_main;
class Row_Device;

/*

Classes to hold pointers to the call back functions, and general helper classes

*/

namespace DCE
{
//class ClientSocket;

	// Contains a pointer to the message interceptor
	class MessageInterceptorCallBack
	{
	public:
		class Command_Impl *m_pPlugIn;
		MessageInterceptorFn m_pMessageInterceptorFn;

		MessageInterceptorCallBack(class Command_Impl *pPlugIn,MessageInterceptorFn pMessageInterceptorFn)
		{
			m_pPlugIn=pPlugIn;
			m_pMessageInterceptorFn=pMessageInterceptorFn;
		}

	};


	// Store information about MessageInterceptors

	class MessageFromInterceptor
	{
	public:
		list<class MessageInterceptorCallBack *> m_listMessageInterceptor;
	};

	class MessageToCategoryInterceptor
	{
	public:
		map<int,class MessageFromInterceptor *> m_mapMessageFromInterceptor;
	};

	class MessageToInterceptor
	{
	public:
		map<int,class MessageToCategoryInterceptor *> m_mapMessageToCategoryInterceptor;
	};
	class MessageMDLInterceptor
	{
	public:
		map<int,class MessageToInterceptor *> m_mapMessageToInterceptor;
	};
	class MessageIDInterceptor
	{
	public:
		map<int,class MessageMDLInterceptor *> m_mapMessageMDLInterceptor;
	};
	class MessageTypeInterceptor
	{
	public:
		map<int,class MessageIDInterceptor *> m_mapMessageIDInterceptor;
	};

	/*

	The DCE Router class

	*/

	class Router : public SocketListener, AlarmEvent, public MySqlHelper
	{
	private:
		/*
		Data

		*/

		// There are 3 layers to Device classes: _Base, _Impl and _Router.  Each layer adds
		// more data.  m_DeviceStructure contains the minimal information, _Base, which is sent to
		// all devices when they register
		char *m_pDeviceStructure;
		unsigned long m_dwIDeviceStructure_Size;

		// The Plug-in's and interceptors
		map<int,class Command_Impl *> m_mapPlugIn;
		map<int,ListCommand_Impl *> m_mapPlugIn_DeviceTemplate;

		map<int,class MessageTypeInterceptor *> m_mapMessageTypeInterceptor;
		list<class MessageInterceptorCallBack *> m_listMessageInterceptor_Global;   // Interceptors that want all messages

		// Miscellaneous Data for internal use
		AlarmManager* m_pAlarmManager;
		pluto_pthread_mutex_t m_CoreMutex;
		pthread_mutex_t m_MessageQueueMutex;
		pthread_cond_t m_MessageQueueCond;
		int m_Port;
		string m_sBasePath;
		pthread_t m_pthread_queue_id;
		map<int,string> m_DeviceID_To_CommandLine;
		map<int,int> m_Routing_DeviceToController;
		map<int,int> m_Routing_VideoDeviceToController;
		map<int,string> m_Routing_ControllerToIP;
		bool m_bStartup, m_bIsMaster, m_bReload, m_bQuit, m_bIsLoading;
		int m_iPK_Device,m_iFileVersion,m_iPK_Installation;
		string m_sDBHost,m_sDBUser,m_sDBPassword,m_sDBName;
		int m_dwIDBPort;
		Row_Device *m_pRow_Device_Me;
		set<int> m_RunningDevices;
		list<Message *> m_MessageQueue;
		Database_pluto_main *m_pDatabase_pluto_main;

		// Lots of maps
		map<int,class DCERoom *> m_mapDCERoom;
		map<int,class Command *> m_mapCommand;
		map<int,class CommandGroup *> m_mapCommandGroup;
		map<int,string> m_mapCommandParmNames;
		map<int,string> m_mapEventNames;
		map<int,string> m_mapEventParmNames;
		map<int,class DeviceData_Router *> m_mapDeviceData_Router;
		map<int,ListDeviceData_Router *> m_mapDeviceTemplate;
		map<int,class DeviceGroup *> m_mapDeviceGroup;
		Map_DeviceCategory m_mapDeviceCategory;

	public:
		// Accessors
		Command_Impl *m_mapPlugIn_Find(int PK_Device) { map<int,class Command_Impl *>::iterator it = m_mapPlugIn.find(PK_Device); return it==m_mapPlugIn.end() ? NULL : (*it).second; }
		ListCommand_Impl *m_mapPlugIn_DeviceTemplate_Find(int PK_DeviceTemplate) { map<int,ListCommand_Impl *>::iterator it = m_mapPlugIn_DeviceTemplate.find(PK_DeviceTemplate); return it==m_mapPlugIn_DeviceTemplate.end() ? NULL : (*it).second; }
		DeviceCategory *m_mapDeviceCategory_Find(int PK_DeviceCategory) { Map_DeviceCategory::iterator it = m_mapDeviceCategory.find(PK_DeviceCategory); return it==m_mapDeviceCategory.end() ? NULL : (*it).second; }
		DeviceGroup *m_mapDeviceGroup_Find(int PK_Device) {map<int,class DeviceGroup *>::iterator it = m_mapDeviceGroup.find(PK_Device); return it==m_mapDeviceGroup.end() ? NULL : (*it).second; }
		ListDeviceData_Router *m_mapDeviceTemplate_Find(int PK_DeviceTemplate) { map<int,ListDeviceData_Router *>::iterator it = m_mapDeviceTemplate.find(PK_DeviceTemplate); return it==m_mapDeviceTemplate.end() ? NULL : (*it).second; }
		DeviceData_Router *m_mapDeviceData_Router_Find(int PK_Device) { map<int,class DeviceData_Router *>::iterator it = m_mapDeviceData_Router.find(PK_Device); return it==m_mapDeviceData_Router.end() ? NULL : (*it).second; }
		CommandGroup *mapCommandGroup_Find(int PK_CommandGroup) { map<int,class CommandGroup *>::iterator it = m_mapCommandGroup.find(PK_CommandGroup); return it==m_mapCommandGroup.end() ? NULL : (*it).second;}
		Command *mapCommand_Find(int PK_Command) { map<int,class Command *>::iterator it = m_mapCommand.find(PK_Command); return it==m_mapCommand.end() ? NULL : (*it).second; }
		DCERoom *m_mapDCERoom_Find(int PK_Room) { map<int,class DCERoom *>::iterator it = m_mapDCERoom.find(PK_Room); return it==m_mapDCERoom.end() ? NULL : (*it).second; }
		const map<int,class DeviceData_Router *> *m_mapDeviceData_Router_get() { return &m_mapDeviceData_Router; };

		int iPK_Installation_get() { return m_iPK_Installation; }
		string sBasePath_get() { return m_sBasePath; }

		string sDBHost_get() { return m_sDBHost; }
		string sDBUser_get() { return m_sDBUser; }
		string sDBPassword_get() { return m_sDBPassword; }
		string sDBName_get() { return m_sDBName; }
		int iDBPort_get() { return m_dwIDBPort; }

		/*
		METHODS
		*/

		Router(int PK_Device,int PK_Installation,string BasePath,string DBHost,string DBUser,string DBPassword,string DBName,int DBPort,int ListenPort);
		virtual ~Router();

		// Plug-In's
		void RegisterAllPlugins(); // Iterates through all plug-ins, calling the register method

		// HACK -- We need to do this inline since a lot of plug-ins will be calling this method and we don't have a way for dcerouter to export it's classes and we don't want to add dcerouter.cpp to the plug-ins
		void RegisterInterceptor(class MessageInterceptorCallBack *pCallBack,int PK_Device_From,int PK_Device_To,int PK_DeviceTemplate,int PK_DeviceCategory,int MessageType,int MessageID)
		{
			if( PK_Device_From==0 && PK_Device_To==0 && PK_DeviceTemplate==0 && PK_DeviceCategory==0 && MessageType==0 && MessageID==0 )
			{
				m_listMessageInterceptor_Global.push_back(pCallBack);
				return;
			}
			MessageTypeInterceptor *pMessageTypeInterceptor = m_mapMessageTypeInterceptor[MessageType];
			if( !pMessageTypeInterceptor )
			{
				pMessageTypeInterceptor = new MessageTypeInterceptor();
				m_mapMessageTypeInterceptor[MessageType] = pMessageTypeInterceptor;
			}

			MessageIDInterceptor *pMessageIDInterceptor = pMessageTypeInterceptor->m_mapMessageIDInterceptor[MessageID];
			if( !pMessageIDInterceptor )
			{
				pMessageIDInterceptor = new MessageIDInterceptor();
				pMessageTypeInterceptor->m_mapMessageIDInterceptor[MessageID] = pMessageIDInterceptor;
			}

			MessageMDLInterceptor *pMessageMDLInterceptor = pMessageIDInterceptor->m_mapMessageMDLInterceptor[PK_DeviceTemplate];
			if( !pMessageMDLInterceptor )
			{
				pMessageMDLInterceptor = new MessageMDLInterceptor();
				pMessageIDInterceptor->m_mapMessageMDLInterceptor[PK_DeviceTemplate] = pMessageMDLInterceptor;
			}

			MessageToInterceptor *pMessageToInterceptor = pMessageMDLInterceptor->m_mapMessageToInterceptor[PK_Device_To];
			if( !pMessageToInterceptor )
			{
				pMessageToInterceptor = new MessageToInterceptor();
				pMessageMDLInterceptor->m_mapMessageToInterceptor[PK_Device_To] = pMessageToInterceptor;
			}

			MessageToCategoryInterceptor *pMessageToCategoryInterceptor = pMessageToInterceptor->m_mapMessageToCategoryInterceptor[PK_DeviceCategory];
			if( !pMessageToCategoryInterceptor )
			{
				pMessageToCategoryInterceptor = new MessageToCategoryInterceptor();
				pMessageToInterceptor->m_mapMessageToCategoryInterceptor[PK_DeviceCategory] = pMessageToCategoryInterceptor;
			}

			MessageFromInterceptor *pMessageFromInterceptor = pMessageToCategoryInterceptor->m_mapMessageFromInterceptor[PK_Device_From];
			if( !pMessageFromInterceptor )
			{
				pMessageFromInterceptor = new MessageFromInterceptor();
				pMessageToCategoryInterceptor->m_mapMessageFromInterceptor[PK_Device_From] = pMessageFromInterceptor;
			}

			pMessageFromInterceptor->m_listMessageInterceptor.push_back(pCallBack);
		}

		// Internal use
//		void AddRoutingForDevice(OCDeviceData *pParentDevice, OCDeviceData *pDevice);
		bool Run();
		virtual void RegisteredCommandHandler(int DeviceID);
		virtual bool ReceivedString(Socket *Socket, string Line);
		virtual void ReceivedMessage(Socket *pSocket, Message *pMessage);
		virtual void OnDisconnected(int DeviceID);
		void AddMessageToQueue(Message *pMessage);
		void ProcessQueue();
		void RealSendMessage(Socket *pSocket,SafetyMessage *pSafetyMessage);
		void CreatedDevice(DeviceData_Router *pDevice);
		void ParseDevice(int MasterDeviceID, int ParentDeviceID, class DeviceData_Impl *pDevice);
		void DoReload();
		void OutputChildren(class DeviceData_Impl *pDevice,string &Response);
		void AlarmCallback(int id, void* param) {};
		void Configure(); // Build the config information from the database

#ifdef AUDIDEMO
		class Command_Impl *CreatePlugInHardCoded(int PK_Device, int PK_DeviceTemplate, string sCommandLine);  // Load the plug-in on the command line
#endif

		class Command_Impl *CreatePlugIn(int PK_Device, int PK_DeviceTemplate, string sCommandLine);  // Load the plug-in on the command line
		string GetDevicesByDeviceTemplate(int PK_DeviceTemplate,eBroadcastLevel BroadcastLevel);
		string GetDevicesByCategory(int PK_DeviceCategory,eBroadcastLevel BroadcastLevel);
		void ErrorResponse(Socket *pSocket,Message *pMessage); // Respond with an error condition if the sender is waiting for a response

		// Public
		void Die(const char *Why);
		virtual void DispatchMessage(Message *pMessage) { ReceivedMessage(NULL,pMessage); };
		bool GetParameterWithDefinedMessage(Message *sendMessage, string &sResult);
		bool GetParameter(int ToDevice,int ParmType, string &sResult);
		bool GetVideoFrame(int CameraDevice, void* &ImageData, int &ImageLength);
		string GetPublicIP();
		void CleanFileName(string &FileName);
		Message *GetActionForInput(int PK_Device,int PK_Input);
		bool DeviceIsRegistered(int PK_Device);
		int FindClosestRelative(int MasterDeviceType, int CurrentDevice);
		Database_pluto_main *GetDatabase() { return m_pDatabase_pluto_main; }
		void StartListening() { SocketListener::StartListening(m_Port); }
		void Quit() { m_bQuit=true; }

		// Helper functions for message interceptors, declared inline for performance
		void CheckInterceptor(class MessageTypeInterceptor *pMessageTypeInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
		{
			map<int,class MessageIDInterceptor *>::iterator itMessageID = pMessageTypeInterceptor->m_mapMessageIDInterceptor.find(pMessage->m_dwID);
			if( itMessageID != pMessageTypeInterceptor->m_mapMessageIDInterceptor.end() )
			{
				CheckInterceptor( (*itMessageID).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
			}
			if( pMessage->m_dwID )
			{
				itMessageID = pMessageTypeInterceptor->m_mapMessageIDInterceptor.find(0);
				if( itMessageID != pMessageTypeInterceptor->m_mapMessageIDInterceptor.end() )
				{
					CheckInterceptor( (*itMessageID).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
				}
			}
		}

		void CheckInterceptor(class MessageIDInterceptor *pMessageIDInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
		{
			map<int,class MessageMDLInterceptor *>::iterator itMessageMDL;
			if( pDeviceFrom )
			{
				itMessageMDL = pMessageIDInterceptor->m_mapMessageMDLInterceptor.find(pDeviceFrom->m_iPK_DeviceTemplate);
				if( itMessageMDL != pMessageIDInterceptor->m_mapMessageMDLInterceptor.end() )
				{
					CheckInterceptor( (*itMessageMDL).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
				}
			}
			if( !pDeviceFrom || pDeviceFrom->m_iPK_DeviceTemplate!=0 )
			{
				itMessageMDL = pMessageIDInterceptor->m_mapMessageMDLInterceptor.find(0);
				if( itMessageMDL != pMessageIDInterceptor->m_mapMessageMDLInterceptor.end() )
				{
					CheckInterceptor( (*itMessageMDL).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
				}
			}
		}

		void CheckInterceptor(class MessageMDLInterceptor *pMessageMDLInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
		{
			map<int,class MessageToInterceptor *>::iterator itMessageTo = pMessageMDLInterceptor->m_mapMessageToInterceptor.find(pMessage->m_dwPK_Device_To);
			if( itMessageTo != pMessageMDLInterceptor->m_mapMessageToInterceptor.end() )
			{
				CheckInterceptor( (*itMessageTo).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
			}
			// If we didn't already check for 0, check now
			if( pMessage->m_dwPK_Device_To )
			{
				itMessageTo = pMessageMDLInterceptor->m_mapMessageToInterceptor.find(0);
				if( itMessageTo != pMessageMDLInterceptor->m_mapMessageToInterceptor.end() )
				{
					CheckInterceptor( (*itMessageTo).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
				}
			}
		}

		void CheckInterceptor(class MessageToInterceptor *pMessageToInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
		{
			map<int,class MessageToCategoryInterceptor *>::iterator itMessageToCat;
			if( pDeviceTo )
			{
				itMessageToCat = pMessageToInterceptor->m_mapMessageToCategoryInterceptor.find(pDeviceTo->m_iPK_DeviceCategory);
				if( itMessageToCat != pMessageToInterceptor->m_mapMessageToCategoryInterceptor.end() )
				{
					CheckInterceptor( (*itMessageToCat).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
				}
			}
			if( !pDeviceTo || pDeviceTo->m_iPK_DeviceCategory!=0 )
			{
				itMessageToCat = pMessageToInterceptor->m_mapMessageToCategoryInterceptor.find(0);
				if( itMessageToCat != pMessageToInterceptor->m_mapMessageToCategoryInterceptor.end() )
				{
					CheckInterceptor( (*itMessageToCat).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
				}
			}
		}

		void CheckInterceptor(class MessageToCategoryInterceptor *pMessageToCategoryInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
		{
			map<int,class MessageFromInterceptor *>::iterator itMessageFrom = pMessageToCategoryInterceptor->m_mapMessageFromInterceptor.find(pMessage->m_dwPK_Device_From);
			if( itMessageFrom != pMessageToCategoryInterceptor->m_mapMessageFromInterceptor.end() )
			{
				CheckInterceptor( (*itMessageFrom).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
			}
			if( pMessage->m_dwPK_Device_From )
			{
				itMessageFrom = pMessageToCategoryInterceptor->m_mapMessageFromInterceptor.find(0);
				if( itMessageFrom != pMessageToCategoryInterceptor->m_mapMessageFromInterceptor.end() )
				{
					CheckInterceptor( (*itMessageFrom).second, pSocket, pMessage, pDeviceFrom, pDeviceTo );
				}
			}
		}

		void CheckInterceptor(class MessageFromInterceptor *pMessageFromInterceptor,class Socket *pSocket,class Message *pMessage,class DeviceData_Router *pDeviceFrom,class DeviceData_Router *pDeviceTo)
		{
			list<class MessageInterceptorCallBack *>::iterator itMessageInterceptor;
			for(itMessageInterceptor=pMessageFromInterceptor->m_listMessageInterceptor.begin();
				itMessageInterceptor!=pMessageFromInterceptor->m_listMessageInterceptor.end();
				++itMessageInterceptor)
			{
				class MessageInterceptorCallBack *pMessageInterceptorCallBack = (*itMessageInterceptor);

				class Command_Impl *pPlugIn = pMessageInterceptorCallBack->m_pPlugIn;
				MessageInterceptorFn pMessageInterceptorFn = pMessageInterceptorCallBack->m_pMessageInterceptorFn;

				CALL_MEMBER_FN(*pPlugIn,pMessageInterceptorFn) (pSocket, pMessage, pDeviceFrom, pDeviceTo);

			}
		}
	};
}

#endif
