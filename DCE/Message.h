#ifndef DCEMESSAGE_H
#define DCEMESSAGE_H

#include "SerializeClass/SerializeClass.h"

enum { MESSAGETYPE_COMMAND=1, MESSAGETYPE_EVENT=2, MESSAGETYPE_DATAPARM_CHANGE=3, MESSAGETYPE_REPLY=4, MESSAGETYPE_DATAPARM_REQUEST=6, MESSAGETYPE_LOG=6, MESSAGETYPE_SYSCOMMAND=7 }; 

#define DEVICEID_NULL			-1	  // Invalid device

// Some system devices
#define DEVICEID_LOGGER			-100
#define DEVICEID_EVENTMANAGER	-101  // Not a real device, just the destination of events
#define DEVICEID_DCEROUTER		-102  // System commands for the router itself

// Virtual devices
#define DEVICEID_MASTERDEVICE -200
#define DEVICEID_CATEGORY -201
#define DEVICEID_LIST -202
#define DEVICEID_GROUP -203

// Other
#define DEVICEID_HANDLED_INTERNALLY -300

namespace DCE
{

	// Log entry params
	enum {LOG_PARAM_TIMESTAMP=0, LOG_PARAM_NAME=1, LOG_PARAM_DEVICE=2,LOG_PARAM_DATA=3, LOG_PARAM_LEVEL=4};

	enum { SYSCOMMAND_QUIT=0, SYSCOMMAND_RELOAD=1, SYSCOMMAND_START_MODULE=2, SYSCOMMAND_STOP_MODULE=3, SYSCOMMAND_REACQUIREEPG=4 };

	enum { PRIORITY_LOW=0, PRIORITY_NORMAL=1, PRIORITY_HIGH=2, PRIORITY_URGENT=3};

	enum eBroadcastLevel
	{
		BL_None=0,BL_DirectSiblings=1,BL_SameComputer=2,BL_SameRoom=3,BL_SameHouse=4,BL_AllHouses=5
	};

	enum eExpectedResponse
	{
		ER_None=0, ER_ReplyMessage, ER_ReplyString, ER_DeliveryConfirmation
	};

	class Message : public SerializeClass
	{
	public:
		long m_DeviceIDFrom, m_DeviceIDTo, m_DeviceGroupIDTo, m_ID, m_Priority, m_MessageType;
		long m_DeviceCategoryTo,m_MasterDevice;
		bool m_bIncludeChildren;

		// If m_eExpectedResponse is set, then the sending device will be expecting a response.  If any message 
		// interceptor takes care of this and sets this flag to true, then the framework knows it does not
		// need to send a response.  If this flag is false, the framework will always send a response.
		bool m_bRespondedToMessage;

		// If broadcast level is not none, then rather than sending the message to the destination,
		// the router will send it to the corresponding masterdevice/devicecategory.  Normally, 
		// the "to" device is what is used to determine the broadcast devices (ie devices in the same
		// room as the "to" device).  But if this is set to true, the sending device is used to determine
		// the target devices.  Example: A controller is marked as being in the bedroom, but the user is
		// controlling a DVD player in the living room.  The DVD player is the "to" device used on the remote.
		// If a message is sent to all device category "lights" with broadcast level set to room, then all lights
		// in the living room will get the message, unless relative to sender is 'true', which means all lights
		// in the bedroom
		bool m_bRelativeToSender; 
		eBroadcastLevel m_eBroadcastLevel;
		eExpectedResponse m_eExpectedResponse;

		string m_DeviceListTo;
		map<long, string> m_Parameters;
		map<long, char *> m_DataParameters;
		map<long, unsigned long>  m_DataLengths;
		vector<class Message *> m_vectExtraMessages;

		Message();
		// Warning: This replaces the delimiters in char *Data with NULLs.
		Message(unsigned long Size, char *Data);
		Message(Message *pMessage_Clone);

		// This function takes a variable list of arguments and returns a string that can be sent 
		// through a TCP socket
		Message(long DeviceIDFrom, long DeviceIDTo, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...);

		// This allows for a comma-delimited list of devices to send the command to
		Message(long DeviceIDFrom, string DeviceIDTo, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...);

		// This allows for a message to be sent to a category of device.  If bIncludeChildren is true, then 
		// if a device belongs to a child category, it will still apply.  If the message is sent to devices with
		// the category: AVEquipment, it will still go to a device with the category DVDPlayers, becaues that category
		// is a child of AVEquipment
		Message(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...);

		// This allows for a message to be sent to a particular type of device
		Message(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB, long Priority, long MessageType, long ID, unsigned long ParameterCount, ...);

		virtual ~Message();

		void Clear();


		virtual void ToData(unsigned long &Size, char* &Data, bool bWithHeader=false);
		void FromData(unsigned long Size, char *Data);
	};

	class SafetyMessage
	{
	public:
		Message *m_pMessage;

		Message*& operator*()
		{ 
			return m_pMessage;


		};

		Message *Detach()
		{
			Message *ReturnValue = m_pMessage;
			m_pMessage = NULL;
			return ReturnValue;

		}

		SafetyMessage(Message *pMessage) { m_pMessage = pMessage; };
		~SafetyMessage() { delete m_pMessage; };
	};
}

#endif

