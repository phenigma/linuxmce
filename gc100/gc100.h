//<-dceag-d-b->
#ifndef gc100_h
#define gc100_h

//	DCE Implemenation for #40 gc100

#include "Gen_Devices/gc100Base.h"
//<-dceag-d-e->

#include "IRBase/IRBase.h"
#include "pluto_main/Define_DeviceCategory.h"
#include <map>
using namespace std;

typedef pair<int, int> IntPair;

namespace DCE
{
	class LearningInfo
	{
	public:
		int m_PK_Device, m_PK_Command, m_PK_Device_Orbiter;
		class gc100 *m_pgc100;
		LearningInfo(int PK_Device, int PK_Command, int PK_Device_Orbiter, gc100 *pgc100)
		{
			m_PK_Device = PK_Device;
			m_PK_Command = PK_Command;
			m_PK_Device_Orbiter = PK_Device_Orbiter;
			m_pgc100 = pgc100;
		}
	};

	class module_info
	{
	public:
		int module; 	// GC-100 module number
		int slot; 	// within the module, 1-3
		std::string type; // SERIAL, REALY, IR, or IRL
		std::string key; // e.g. 4:3 - might make a good primary key
		int global_slot; // i.e. 1-9 if there are 3 IR emitter blocks. e.g. the "1-6" IR labels on the back
	};
}

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
	void parse_message_statechange(std::string message, bool change);

	pthread_t m_EventThread, m_LearningThread;

	// Public member variables

	// Begin shared section
	// These items may be accessed by multiple threads therefore must be locked
	
	pluto_pthread_mutex_t gc100_mutex;
	
	std::map<std::string, class module_info> module_map; // Keep track of the GC100's configuration // OK
	std::map<std::string, int> slot_map;		// OK
	int gc100_socket;				// OK
	std::string ip_addr;				// OK
	bool receiving_data;				// OK
	bool is_open_for_learning;			// OK
	int learn_fd;					// OK
	char recv_buffer[4096]; // Replies from the GC100 go here // OK
	int recv_pos;					// OK
	std::string learn_input_string; 		// OK
	int m_IRDeviceID, m_IRCommandID, m_ControllerID; // OK

	// End Shared section
	
	int ir_cmd_id;
	
	// Learning stuff
	std::string learn_device;
	bool open_for_learning();
	bool learning_timeout;
	int learning_timeout_count;

	map<IntPair, string> m_CodeMap;
	bool m_bIRComplete;
	bool m_bLearning;
	bool m_bStopLearning;

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
		virtual void LEARN_IR(string PKID_Device, string CommandID, long OrbiterID);
		virtual void LEARN_IR_CANCEL();

		void LearningThread(LearningInfo * pLearningInfo);
		void EventThread();

		bool Open_gc100_Socket();
		bool send_to_gc100(string Cmd);
		std::string read_from_gc100();
		bool ConvertPronto(string ProntoCode, string &gc_code);
		std::string IRL_to_pronto(string learned_string);
	
		void relay_power(class Message *pMessage, bool power_on);

		void Start_seriald();
		virtual void CreateChildren();
		virtual void SetQuitFlag();
	};
//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
//<-dceag-const2-b->!
