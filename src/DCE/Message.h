/**
 *
 * @file Message.h
 * @brief header file for the Message and SafetyMessage classes; it also includes definition for enums
 * @author
 * @todo notcommented
 *
 */


#ifndef MESSAGEH
#define MESSAGEH

#include "SerializeClass/SerializeClass.h"

#define DEVICEID_NULL           -1  // Invalid device

// Some system devices
#define DEVICEID_LOGGER         -1000
#define DEVICEID_EVENTMANAGER   -1001   // Not a real device, just the destination of events
#define DEVICEID_DCEROUTER      -1002   // System commands for the router itself
#define DEVICEID_MESSAGESEND	-1003	// The message send utility

// Virtual devices
#define DEVICEID_MASTERDEVICE   -2000
#define DEVICEID_CATEGORY       -2001
#define DEVICEID_LIST           -2002
#define DEVICEID_GROUP          -2003

// Other
#define DEVICEID_HANDLED_INTERNALLY -300

#if (defined(LL_DEBUG) || defined(LL_DEBUG_FILE))
	// A counter used to mark messages so they can be traced in the low level logs
	extern int g_MessageID;
#endif

namespace DCE
{

    /**
     * @brief enum containing values associated with message types
     */
    enum { MESSAGETYPE_COMMAND=1, MESSAGETYPE_EVENT=2, MESSAGETYPE_DATAPARM_CHANGE=3, MESSAGETYPE_REPLY=4, 
		MESSAGETYPE_DATAPARM_REQUEST=6, MESSAGETYPE_LOG=6, MESSAGETYPE_SYSCOMMAND=7, 
		MESSAGETYPE_REGISTER_INTERCEPTOR=8, MESSAGETYPE_MESSAGE_INTERCEPTED=9, MESSAGETYPE_EXEC_COMMAND_GROUP=10 };

    /**
     * @brief enum containing values for the criteria used in a message interceptor
     */
	enum { PARM_FROM=1, PARM_TO, PARM_TEMPLATE, PARM_CATEGORY, PARM_MESSAGE_TYPE, PARM_MESSAGE_ID };


    /**
     * @brief enum containing values associated with log entry params
     */
    enum { LOG_PARAM_TIMESTAMP=0, LOG_PARAM_NAME=1, LOG_PARAM_DEVICE=2,LOG_PARAM_DATA=3, LOG_PARAM_LEVEL=4 };

    /**
     * @brief enum containing values associeted with syscommands
		RELOAD_FORCED means do the reload no matter what, ignoring the plugins SafeToReload
     */
    enum { SYSCOMMAND_QUIT=0, SYSCOMMAND_RELOAD=1, SYSCOMMAND_START_MODULE=2, SYSCOMMAND_STOP_MODULE=3, SYSCOMMAND_SEGFAULT=4, SYSCOMMAND_DEADLOCK=5, SYSCOMMAND_RELOAD_FORCED=6 };

    /**
     * @brief enum containing values associted with priorities
     */
    enum { PRIORITY_LOW=0, PRIORITY_NORMAL=1, PRIORITY_HIGH=2, PRIORITY_URGENT=3 };

    /**
     * @brief enum containing values associated with broadcast levels
     */
    enum eBroadcastLevel { BL_None=0, BL_DirectSiblings=1, BL_SameComputer=2, BL_SameRoom=3, BL_SameHouse=4, BL_AllHouses=5 };

    /**
     * @brief enum containing vlues associated with expected responses
     */
    enum eExpectedResponse { ER_None=0, ER_ReplyMessage, ER_ReplyString, ER_DeliveryConfirmation };

    /**
     * @brief Data command and events message (sent between various devices)
     */
    class Message : public SerializeClass
    {

    public:

		int m_MessageID;  // For low-level debugging only

        /** fields that corespond to primary keys */

        long m_dwPK_Device_From; /** < the id of the device from witch the message is comming */
        long m_dwPK_Device_To; /** < the id of the device to witch the messge is going */
        long m_dwPK_Device_Group_ID_To; /** < the id of the group of devices to witch the message is going */
        long m_dwPK_Device_Category_To; /** < the device category to send the message to (all devices in that category will receive it) */
        long m_dwPK_Device_Template; /** < the master device for the current message */
        string m_sPK_Device_List_To; /** < the message will be sent to all the devices build using this template */

        /** fields describing the message */

        long m_dwID; /** < the message ID */
        long m_dwPriority; /** < the message priority */
        long m_dwMessage_Type; /** < the message type */

        /** flags for setting the way the framework will treat the message */

        bool m_bIncludeChildren;

        /**
         * @brief specifies if the framework will allways sen a response
         * If m_eExpectedResponse is set, then the sending device will be expecting a response.  If any message
         * interceptor takes care of this and sets this flag to true, then the framework knows it does not
         * need to send a response.  If this flag is false, the framework will always send a response.
         */
        bool m_bRespondedToMessage;

        /**
         * @brief specifies to who the message will be sent; see the full comment
         * If broadcast level is not none, then rather than sending the message to the destination,
         * the router will send it to the corresponding masterdevice/devicecategory.  Normally,
         * the "to" device is what is used to determine the broadcast devices (ie devices in the same
         * room as the "to" device).  But if this is set to true, the sending device is used to determine
         * the target devices.  Example: A controller is marked as being in the bedroom, but the user is
         * controlling a DVD player in the living room.  The DVD player is the "to" device used on the remote.
         * If a message is sent to all device category "lights" with broadcast level set to room, then all lights
         * in the living room will get the message, unless relative to sender is 'true', which means all lights
         * in the bedroom
         */
        bool m_bRelativeToSender;

        /**
         * @brief the level at witch the message is broadcasted
         */
        eBroadcastLevel m_eBroadcastLevel;

        /**
         * @brief specifies the expected response, so that the socket filter stops all other responses
         */
        eExpectedResponse m_eExpectedResponse;

        map<long, string> m_mapParameters; /** < the message parameters */
        map<long, char *> m_mapData_Parameters; /** < the data parameters (pointers to the actual data) */
        map<long, unsigned long>  m_mapData_Lengths; /** < the length of the data from data parameters */
        vector<class Message *> m_vectExtraMessages; /** < vector of messages */

        string sOutputParametersPath; /** < where to store output parameters files */

        /**
         * @brief just calls the Clear method
         * @see the Message::Clear() method
         */
        Message();

        /**
         * @brief calls the FromData method to deserialize the data from pcData and after finishin assignes null to SerializeClass::m_pcDataBlock
         * @see Message::FromData( ... )
         * @warning This replaces the delimiters in char *pcData with NULLs.
         */
        Message( unsigned long dwSize, char *pcData );

        /**
         * @brief copies all the member data from the Message pointed by the pMessage_Clone parameter
         */
        Message( Message *pMessage_Clone );

        /**
         * @brief this function takes a variable list of arguments and returns a string that can be sent through a TCP socket
         */
        Message( long dwDeviceIDFrom, long dwDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... );

        /**
         * @brief this allows for a comma-delimited list of devices to send the command to
         */
        Message( long dwDeviceIDFrom, string sDeviceIDTo, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ...);

        /**
         * @brief this allows for a message to be sent to a category of device
         * if bIncludeChildren is true, then if a device belongs to a child category, it will still apply.  If the message is sent to devices with
         * the category: AVEquipment, it will still go to a device with the category DVDPlayers, because that category is a child of AVEquipment
         */
        Message( long dwDeviceIDFrom, long dwDeviceCategory, bool bIncludeChildren, eBroadcastLevel eB, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... );

        /**
         * @brief this allows for a message to be sent to a particular type of device specified by dwMasterDevice
         */
        Message( long dwDeviceIDFrom, long dwMasterDevice, eBroadcastLevel eB, long dwPriority, long dwMessageType, long dwID, unsigned long dwParameterCount, ... );

        /**
         * @brief this allows for a string formatted message to be sent to a particular device.
         */
        Message( string sMessageInStringFormat );

        /**
         * @brief this allows for a message coming in from command line arguments
         */
		Message( int iNumArgs, char *cArguments[], int dwPK_DeviceFrom=0 );

        /**
         * @brief just calls the Clear method
         * @see the Message::Clear() method
         */
        virtual ~Message();

        /**
         * @brief assignes default values to the member data and clears the map members data
         */
        void Clear();


        virtual void ToData( unsigned long &dwSize, char* &pcData, bool bWithHeader=false );

        /**
         * @brief start deserializing the pcData to build the instance of the Message
         */
        void FromData( unsigned long dwSize, char *pcData );

        /**
         * @brief Build from command line arguments
         */
		void BuildFromArgs( int iNumArgs, char *cArguments[], int dwPK_DeviceFrom=0 );
	};

    /**
     * @brief ...
     * @todo ask
     */
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

        SafetyMessage( Message *pMessage ) { m_pMessage = pMessage; };

        ~SafetyMessage() { delete m_pMessage; };
    };
}

#endif
