//<-dceag-d-b->
#ifndef gc100_h
#define gc100_h

//	DCE Implemenation for #40 gc100

#include "Gen_Devices/gc100Base.h"
//<-dceag-d-e->

#include "IRBase/IRBase.h"
#include <map>
using namespace std;

typedef pair<int, int> IntPair;

//<-dceag-decl-b->!
namespace DCE
{
	class gc100 : public gc100_Command, public IRBase
	{
//<-dceag-decl-e->
		// Private member variables

		// Private methods
protected:
	virtual void SendIR(string Port, string IRCode);
	class Message *m_pThisMessage;

private:
	void parse_gc100_reply(std::string message);
	void parse_message_device(std::string message);
	void parse_message_completeir(std::string message);
	void parse_message_statechange(std::string message, bool change);

		// Public member variables

	// Begin shared section
	// These items may be accessed by multiple threads therefore must be locked
	
	pluto_pthread_mutex_t shared_lock;
	
	std::map<std::string, class module_info> module_map; // Keep track of the GC100's configuration // OK
	std::map<std::string, int> slot_map;		// OK
	int gc100_socket;				// OK
	std::string ip_addr;				// OK
	bool receiving_data;				// OK
	bool is_open_for_learning;			// OK
	int learn_fd;					// OK
	std::list<std::string> command_list;		// OK
	bool ready;					// OK
	int pending_ir_cmd;				// OK
	char recv_buffer[4096]; // Replies from the GC100 go here // OK
	int recv_pos;					// OK
	bool m_bLearning; 				// OK
	std::string learn_input_string; 		// OK
	int m_IRDeviceID, m_IRActionID, m_ControllerID; // OK
	int timeout_count;				// OK

	// End Shared section
	
	int ir_cmd_id;
	
	// Learning stuff
	std::string learn_device;
	bool open_for_learning();
	bool learning_timeout;
	int learning_timeout_count;

	map<IntPair, string> m_CodeMap;

//<-dceag-const-b->
public:
		// Constructors/Destructor
		gc100(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~gc100();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


//<-dceag-h-e->
	virtual Command_Impl *CreateCommand(int iPK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent);
	
	virtual bool ReceivedMessage(class Message *pMessage);
	virtual void LEARN_IR(string PKID_Device,string ABSOLUTE_CHANNEL,string ID,string IR_FREQUENCY);
	virtual void SEND_CODE(string ABSOLUTE_CHANNEL,string TOKENS,string TEXT) {};
	virtual void LEARN_IR_CANCEL(string ABSOLUTE_CHANNEL) { m_bLearning = false; };
	virtual void SET_LEARN_FREQUENCY(string IR_FREQUENCY); 
	virtual void recv_data(string CHANNEL_NAME);
	virtual void send_data(string TEXT,string CHANNEL_NAME);

	bool Open_gc100_Socket();
	void MonitorIR();
	bool send_to_gc100();
	std::string read_from_gc100();
	bool ConvertPronto(string ProntoCode, string &gc_code);
	std::string IRL_to_pronto(string learned_string);
	
	void relay_power(class Message *pMessage, bool power_on);

	void Start_seriald();
	virtual void CreateChildren();
	void MainLoop();

	};

	class module_info {
	public:
		int module; 	// GC-100 module number
		int slot; 	// within the module, 1-3
		std::string type; // SERIAL, REALY, IR, or IRL
		std::string key; // e.g. 4:3 - might make a good primary key
		int global_slot; // i.e. 1-9 if there are 3 IR emitter blocks. e.g. the "1-6" IR labels on the back
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
