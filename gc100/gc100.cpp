//<-dceag-d-b->
#include "gc100.h"
#include "DCE/Logger.h"
#include "PlutoUtils/FileUtils.h"
#include "PlutoUtils/StringUtils.h"
#include "PlutoUtils/Other.h"

#include <iostream>
using namespace std;
using namespace DCE;

#include "Gen_Devices/AllCommandsRequests.h"
//<-dceag-d-e->

#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Event.h"
#include "pluto_main/Define_EventParameter.h"
#include "Gen_Devices/Generic_Input_OuputBase.h"

#include <fcntl.h>

#ifdef WIN32
#define strcasecmp _stricmp
#endif

// Items in the 'fast' loop
#define MONITOR_DELAY_MS 25 // number of ms to delay each time trough loop - "short" delay
#define LEARNING_TIMEOUT (1000/MONITOR_DELAY_MS) * 1 // 1 sec. at 1ms sleep times in main

// Figure the poll multiplier so that the slow loop happens every 100ms
#define POLL_MULTIPLIER 100/MONITOR_DELAY_MS
#define READY_TIMEOUT (1000/MONITOR_DELAY_MS)/POLL_MULTIPLIER * 0.800 // 800 msec. ready timeout

#define LEARN_PREFIX "GC-IRL"
#define GC100_COMMAND_PORT 4998

// Sleep for the specified no. of ms.
void sleep_ms(int ms)
{
	struct timeval tmo;

	tmo.tv_sec = 0;
	tmo.tv_usec = ms * 1000;

	select(0, NULL, NULL, NULL, &tmo);
} // end sleep_ms

//<-dceag-const-b->
gc100::gc100(int DeviceID, string ServerAddress,bool bConnectEventHandler,bool bLocalMode,class Router *pRouter)
	: gc100_Command(DeviceID, ServerAddress,bConnectEventHandler,bLocalMode,pRouter)
//<-dceag-const-e->
	, shared_lock("gc100")
{
	// TODO: figure this one out: ReplaceParams
	//ReplaceParams(replacement);
	recv_pos=0;

	ready=true;
	ir_cmd_id=1;
	pending_ir_cmd=0;
	timeout_count=0;

	// TODO: learn_device
	//learn_device=GetData()->Get_SERIAL_PORT();
	learn_device="/dev/gcs0"; // DEBUG
}

//<-dceag-dest-b->
gc100::~gc100()
//<-dceag-dest-e->
{
	//delete m_SlinkeBase;
	//delete m_SerialPort;
}

//<-dceag-reg-b->
// This function will only be used if this device is loaded into the DCE Router's memory space as a plug-in.  Otherwise Connect() will be called from the main()
bool gc100::Register()
//<-dceag-reg-e->
{
	return Connect(); 
}

//<-dceag-sample-b->
/*		**** SAMPLE ILLUSTRATING HOW TO USE THE BASE CLASSES ****

void gc100::SomeFunction()
{
	// If this is going to be loaded into the router as a plug-in, you can implement: 	virtual bool Register();
	// to do all your registration, such as creating message interceptors

	// If you use an IDE with auto-complete, after you type DERC:: it should give you a list of all
	// commands and requests, including the parameters.  See "AllCommandsRequests.h"

	// Send Orbiters the "CMD_Simulate_Mouse_Click" command, which takes an X and Y parameter.  We'll use 55,77 for X and Y.

	// Send the message to a specific orbiter, identified by OrbiterID
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77);

	// Send the message to orbiters 32898 and 27283
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,"32898,27283",55,77);

	// Send the message to all orbiters within the house, which is all devices with the category DEVICECATEGORY_Orbiter_CONST (see pluto_main/Define_DeviceCategory.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DEVICECATEGORY_Orbiter_CONST,true,BL_SameHouse,55,77);

	// Send the message to all "DeviceTemplate_Orbiter_CONST" devices within the room (see pluto_main/Define_DeviceTemplate.h)
	QueueMessage(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,DeviceTemplate_Orbiter_CONST,true,BL_SameRoom,55,77);

	// This time we want to wait to be sure the orbiter gets the message, and the thread will block until the orbiter receives the message
	string sResult="";
	bool bResult = SendMessageWithConfirm(new DERC::CMD_Simulate_Mouse_Click(m_dwPK_Device,OrbiterID,55,77),sResult);
	// If bResult is true, the message was received ok.  Otherwise it failed, and sResult contains an explanation of the failure

	// A request is like a command, except that it has both "in" and "out" parameters, and the 
	// thread blocks until the request is completed.  Note that requests are sent on a separate socket, so you can continue to receive commands and send
	// events while a thread is waiting for a request to complete.  However the SendMessageWithConfirm uses the same request socket, and will wait if
	// the socket is busy.  

	// We want to request the contents of a file.  This request takes 1 "in" parameter, the file name, and has 2 "out" parameters: the file's contents, and the date.
	// All out parameters are passed by reference.
	// Note that the "Data" parameter which contains the file is a binary parameter.  Whether in or out, binary parameters always take both a pointer to the object and the size
	// This command will be sent to the "Standard Plug-in" which implements this type of request.  We set the boardcast level to BL_SameHouse.  Note that since this is a
	// request, the router will find the first matching device (ie the first device "Standard Plug-in") and return that device's response.  If this was a command, not a request,
	// and there were multiple devices "Standard Plug-in" they would all get the message.
	PlutoDate plutoDate;
	char *FileContents;
	int FileSize;
	bool bResult = SendRequest(new DERC::REQ_File_Contents(m_dwPK_Device,DeviceTemplate_Standard_Plug_In_CONST,true,BL_SameHouse,"some_file_name",&FileContents,&FileSize,&plutoDate);

	// To access our data and events below, you can type this-> if your IDE supports auto complete to see all the data and events you can access

	// Get our IP address from our data
	string sIP = DATA_Get_IP_Address();

	// Set our data "Filename" to "myfile"
	DATA_Set_Filename("myfile");

	// Fire the "Finished with file" event, which takes no parameters
	EVENT_Finished_with_file();


}
*/
//<-dceag-sample-e->

/*

	COMMANDS TO IMPLEMENT

*/

///*virtual*/ bool gc100::ReceivedMessage(class Message *pMessage)
//{
//	if (gc100_Command::ReceivedMessage(pMessage))
//		return true; // message handled by parent
//	// we handle unknown messages
//	cout << "Message ID: " << pMessage->m_dwID << endl;
//	cout << "From:" << pMessage->m_dwPK_Device_From << endl;
//	cout << "To: " << pMessage->m_dwPK_Device_To << endl;
//	
//	map<long, string>::iterator i;
//	for (i = pMessage->m_mapParameters.begin(); i != pMessage->m_mapParameters.end(); i++)
//	{
//		cout << "Parameter: " << i->first << " Value: " << i->second << endl;
//	}
//	// TODO: not handled until I see what I should be handling :)
//	return false;
//}

// Returns string starting with freq.  The "sendir,<address>,<id>," are prepended elsewhere
bool gc100::ConvertPronto(string ProntoCode, string &gc_code)
{
	std::string token;
	int token_int;
	long int frequency;
	string::size_type pos;
	bool return_value;
	int num_single,num_repeat;
	int expected_length;
	bool error;

	return_value=true;
	error=false;

	//ProntoCode="0000 0048 0000 0018 00c0 00c1 0030 0092 0030 0092 0030 0030 0030 0092 0030 0092 0030 0092 0030 0030 0030 0092 0030 0092 0030 0092 0030 0030 0030 0030 0030 0030 0030 0092 0030 0030 0030 0030 0030 0030 0030 0092 0030 0030 0030 0030 0030 0030 0030 0092 0030 0870"; // DEBUG
	gc_code=""; // Start with this
	pos=0;

	// Get first token
	// Format ID

	if (ProntoCode.length() < 29)
	{
		g_pPlutoLogger->Write(LV_STATUS, "convert_pronto: Total code length is %d, below minimum possible length",ProntoCode.length());
		return_value=false;
	}
	else
	{
		token=StringUtils::Tokenize(ProntoCode," ",pos);
		sscanf(token.c_str(),"%4x",&token_int);

		if (token_int==0)
		{
			// Carrier divider
			token=StringUtils::Tokenize(ProntoCode," ",pos);
			sscanf(token.c_str(),"%4x",&token_int);

			frequency=(((41450/token_int)+5)/10)*1000;
			gc_code=gc_code+StringUtils::itos(frequency)+",1"; // Repeat only once

			// # burst pairs for single sequence
			token=StringUtils::Tokenize(ProntoCode," ",pos);
			sscanf(token.c_str(),"%4x",&token_int);
			num_single=token_int;

			// # burst pairs for repeat sequence
			token=StringUtils::Tokenize(ProntoCode," ",pos);
			sscanf(token.c_str(),"%4x",&token_int);
			num_repeat=token_int;

			// Figure repeat offset
			gc_code+=","+StringUtils::itos(num_single*2+1);

			expected_length=(num_single+num_repeat)*10+19;
			g_pPlutoLogger->Write(LV_STATUS, "convert_pronto: Total code length is %d, expecting %d",ProntoCode.length(),expected_length);

			if (true)
			{ // For right now don't enforce the length check
				for (int i=0; (i< ((num_single+num_repeat)*2)) && !error; i++)
				{
					token=StringUtils::Tokenize(ProntoCode," ",pos);
					if (token=="")
					{
						g_pPlutoLogger->Write(LV_WARNING, "convert_pronto: Token was NULL, probably because code was too short or incorrectly formatted");
						error=true;
					}
					else
					{
						sscanf(token.c_str(),"%4x",&token_int);
						gc_code+=","+StringUtils::itos(token_int);
					}
				}
			}
			else
			{
				g_pPlutoLogger->Write(LV_STATUS, "convert_pronto: Total code length is %d, expecting %d",ProntoCode.length(),expected_length);
				g_pPlutoLogger->Write(LV_STATUS, "convert_pronto: Code was incorrect length");
				return_value=false;
			}
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "convert_pronto: Can't convert code types other than 0000");
			return_value=false;
		}

		// Ending
		//g_pPlutoLogger->Write(LV_STATUS, "convert_pronto: Final conversion: %s",gc_code.c_str());

		//gc_code="58000,1,1,192,193,48,146,48,146,48,48,48,146,48,146,48,146,48,48,48,146,48,146,48,146,48,48,48,48,48,48,48,146,48,48,48,48,48,48,48,146,48,48,48,48,48,48,48,146,48,2160"; // DEBUG
	}
	return return_value;
}

std::string gc100::IRL_to_pronto(string raw_learned_string)
{
	std::string result,token,token2,pronto_pairs;
	string::size_type pos;
	std::string learned_string;
	int freq,freq_token;
	int pair_num;
	int v1,v2;
	bool done;
	char conv_buf[256];
	//char string_search[8192];
	//char *last;
	//char *c_pos;
	int trunc_prefix;

	//snprintf(string_search, sizeof(string_search), "%s", raw_learned_string.c_str());

	//c_pos=NULL;
	//last=&string_search[0];

	//do {
	//	c_pos=strstr(last+1, LEARN_PREFIX);
	//	if (c_pos!=NULL) {
	//		last=c_pos;
	//	}
	//   } while (c_pos!=NULL);

	//  trunc_prefix=(int) &last - (int) &string_search[0];

	trunc_prefix=raw_learned_string.rfind(LEARN_PREFIX);
	if (trunc_prefix <0)
	{
		g_pPlutoLogger->Write(LV_WARNING, "IRL_to_pronto: trunc_prefix was %d",trunc_prefix);
		trunc_prefix=0;
	}

	g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: trunc_prefix is %d",trunc_prefix);

	learned_string=raw_learned_string.substr(trunc_prefix);
	pos=0;

	// Output: 0000 format
	result="P,0000 "; // Pronto raw pwm prefix

	token=StringUtils::Tokenize(learned_string,",",pos); // First token is "GC-IRL" or "receiveir" header
	g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: prefix is %s",token.c_str());

	token=StringUtils::Tokenize(learned_string,",",pos); // Next token is frequency
	sscanf(token.c_str(),"%d",&freq);
	g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: freq is %d",freq);
	freq_token=(int) (41450/((freq/100)-5)) ;
	sprintf(conv_buf,"%04x ",freq_token);
	// Output: frequency word
	result+=std::string(conv_buf);

	/*token=StringUtils::Tokenize(learned_string,",",pos); // Next token is count?
	sscanf(token.c_str(),"%d",&count);
	g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: count is %d",count);

	token=StringUtils::Tokenize(learned_string,",",pos); // Next token is offset?
	sscanf(token.c_str(),"%d",&offset);
	g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: offset is %d",offset);
	*/

	done=false;
	pair_num=1;

	while (!done)
	{
		if ((pos>=learned_string.length())||(pair_num>5000))
		{
			//g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: no more pairs; we're done");
			done=true;
		}
		else
		{
			token=StringUtils::Tokenize(learned_string,",",pos); // ON value
			sscanf(token.c_str(),"%d",&v1);

			token2=StringUtils::Tokenize(learned_string,",",pos); // OFF value
			sscanf(token2.c_str(),"%d",&v2);

			//g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: data pair #%d: %d/%d",pair_num,v1,v2);
			// Do any necessary correcting of values here

			if ((token=="X")||(token2=="X"))
			{
				g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: end-of sequence X character detected");
				done=true;
			}
			else
			{
				if (v2==0)
				{
					v2=v1;
					g_pPlutoLogger->Write(LV_STATUS, "IRL_to_pronto: corrected 0 to %d",v2);
				}

				sprintf(conv_buf," %04x %04x",v1,v2);
				pronto_pairs+=std::string(conv_buf);

				pair_num++;
			}
		}
	}

	pair_num--;
	// Output: # of pairs for single // DEBUG
	result+="0000 ";

	sprintf(conv_buf,"%04x",pair_num); // DEBUG
	//sprintf(conv_buf,"%04x ",pair_num);
	// Output: # of pairs for single burst
	result+=std::string(conv_buf);

	// Output: # of pairs for repeat sequence
	//result+="0000";

	// Output: all of the on/off pairs
	result+=pronto_pairs;

	return result;
}

// Not done
unsigned int GetPortMask(string port)
{
	unsigned int mask = 0xff;

	/*if (port.length() > 0)
	{

	if (port[0]=='a' || port[0]=='A')
	{
	mask = 0x100+atoi(port.substr(1).c_str());
	}
	else
	{
	mask = (unsigned int)pow(2.0,atoi(port.c_str()));
	}
	}*/
	return mask;	
}

Command_Impl *gc100::CreateCommand( int iPK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent )
{
	// TODO: put a pointer to the router instead of NULL
	return new Command_Impl(pPrimaryDeviceCommand, pData, pEvent, NULL);
}

bool gc100::send_to_gc100()
{
	// returns true if it was ready to send, false if blocked because of not ready
	char command[16384];
	std::string next_cmd;
	unsigned int result;
	bool return_value;

	return_value=true;

	PLUTO_SAFETY_LOCK(sl, shared_lock);

	if (command_list.size()>0)
	{
		if (ready)
		{
			next_cmd=command_list.front();
			command_list.pop_front();
			sprintf(command,"%s%c",next_cmd.c_str(),'\x0d');
			g_pPlutoLogger->Write(LV_STATUS, "Sending command %s",command);

			if (strlen(command)>6)
			{
				if (strncmp(command,"sendir",6)==0)
				{
					std::string token;
					std::string::size_type pos;
					int id_int;

					// Extract the ID from the sendir and keep it as the pending IR command
					// s/w will indicate 'not ready' until this ID is seen again with a
					// completeir message.  This ensures that only one IR command is being processed at a time
					pos=0;

					token=StringUtils::Tokenize(next_cmd,",",pos); // sendir
					token=StringUtils::Tokenize(next_cmd,",",pos); // address
					token=StringUtils::Tokenize(next_cmd,",",pos); // ID

					sscanf(token.c_str(),"%d",&id_int);

					if (id_int>0)
					{
						//g_pPlutoLogger->Write(LV_STATUS, "Send ir sets ready to false with pending ID %d",id_int);
						pending_ir_cmd=id_int;
						ready=false;
					}
				}
			}

			result=write(gc100_socket,command,strlen(command));
			if (result<strlen(command))
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "Short write to GC100\n");
			}
		}
		else
		{
			g_pPlutoLogger->Write(LV_STATUS, "Not ready for another command yet, tc=%d",timeout_count);

			timeout_count++;
			if (timeout_count>READY_TIMEOUT)
			{ // 2 sec
				timeout_count=0;
				ready=true;
				g_pPlutoLogger->Write(LV_WARNING, "Timed out waiting for completeir; forcing ready=true");
				// Do not reissue the previous command, because it may be a sendir command to an ir port
				// that is configured as discrete input.  In that situation the command will never work
				// and you'll be forced to retry it forever
			}
			return_value=false;
		}
	}

	return return_value;
}

void gc100::parse_message_device(std::string message)
{
	int module;
	std::string key;
	std::string type;
	int count;
	class module_info scratch;
	string::size_type pos,pos2;
	std::string token,token2;
	int next_slot;
	std::map<std::string,int>::iterator slot_iter;

	pos=0;
	pos2=0;

	// first token is 'device'
	token=StringUtils::Tokenize(message,",",pos);
	if (token != "device")
	{
		g_pPlutoLogger->Write(LV_WARNING,"Sanity check: was expecting 'device' not %s",token.c_str());
	}

	// second token is module number
	token=StringUtils::Tokenize(message,",",pos);
	sscanf(token.c_str(),"%d",&module);
	if (module<1)
	{
		g_pPlutoLogger->Write(LV_WARNING,"Sanity check: module number should be positive not %d",module);
	}

	// third token is number and type of device, e.g. '3 IR'
	token=StringUtils::Tokenize(message,",",pos);

	// Now sub-tokenize the last token to split it into number and type
	token2=StringUtils::Tokenize(token," ",pos2);
	sscanf(token.c_str(),"%d",&count);
	if (count<1)
	{
		g_pPlutoLogger->Write(LV_WARNING,"Sanity check: slot count should be positive not %d",count);
	}

	type=StringUtils::Tokenize(token," ",pos2); // Module type is the only thing left

	// OK we have all the info from the message now.  Create other derived values and add the right number of devices
	// First figure out what the next slot number should be
	slot_iter=slot_map.find(type);
	if (slot_iter==slot_map.end())
	{
		g_pPlutoLogger->Write(LV_STATUS,"Entering default value for type %s",type.c_str());
		slot_map.insert(pair<std::string,int> (type,1));
	}

	slot_iter=slot_map.find(type);
	if (slot_iter==slot_map.end())
	{
		g_pPlutoLogger->Write(LV_WARNING,"Sanity check: slot number not found for type %s",type.c_str());
	}
	else
	{
		next_slot=slot_iter->second;
		g_pPlutoLogger->Write(LV_STATUS,"The next slot for this type should be %d",next_slot);

		for (int i=0; i<count; i++) {
			key=StringUtils::itos(module)+":"+StringUtils::itos(i+1);

			scratch.module=module;
			scratch.slot=i+1;
			scratch.type=type;
			scratch.key=key;
			scratch.global_slot=next_slot;

			module_map.insert(pair<std::string, class module_info> (key,scratch));
			g_pPlutoLogger->Write(LV_STATUS,"Created new module: mod=%d slot=%d type=%s key=%s global=%d",scratch.module,scratch.slot,scratch.type.c_str(),scratch.key.c_str(),scratch.global_slot);

			// An IR module might be used for input, so request the state.  
			// Either it will return the state, or an error if it's really for IR (which we ignore)
			if (scratch.type=="IR")
			{
				command_list.push_back("getstate,"+key);
			}
			next_slot++;

		}
		// Now save next_slot so the next module can number (global number only) starting from where we left off
		slot_map.erase(type);
		slot_map.insert(pair<std::string,int> (type,next_slot));
	}
}

void gc100::Start_seriald()
{
	std::map<std::string, class module_info>::iterator serial_iter;
	int global_slot;
	char command[512];

	sprintf(command,"killall gc_seriald");
	system(command);

	PLUTO_SAFETY_LOCK(sl, shared_lock);
	for (serial_iter=module_map.begin(); serial_iter!=module_map.end(); ++serial_iter) {
		if (serial_iter->second.type == "SERIAL")
		{
			global_slot=serial_iter->second.global_slot;

			sprintf(command,"/usr/pluto/bin/gc_seriald %s %d /dev/gcsd%d &",ip_addr.c_str(), global_slot+GC100_COMMAND_PORT, global_slot-1);
			g_pPlutoLogger->Write(LV_STATUS,"seriald cmd: %s",command);
			system(command);
		}
	}
}

void gc100::parse_message_completeir(std::string message)
{
	int id_int;
	std::string token;
	std::string::size_type pos;

	//g_pPlutoLogger->Write(LV_STATUS, "completeir Reply received from GC100: %s",message.c_str());
	// Extract the ID from the completeir
	pos=0;

	token=StringUtils::Tokenize(message,",",pos); // completeir
	token=StringUtils::Tokenize(message,",",pos); // address
	token=StringUtils::Tokenize(message,",",pos); // ID

	sscanf(token.c_str(),"%d",&id_int);

	if (id_int==pending_ir_cmd)
	{
		//g_pPlutoLogger->Write(LV_STATUS, "completeir: %d is finished, setting READY",id_int);
		timeout_count=0;
		ready=true;

	}
}

void gc100::parse_message_statechange(std::string message, bool change)
{
	std::string token;
	std::string::size_type pos;
	std::string module_address; // probably in 4:3 format
	int input_state;
	MapCommand_Impl::iterator child_iter;
	Generic_Input_Ouput_Command *nomination;
	Generic_Input_Ouput_Command *result;
	int global_pin_target;
	std::map<std::string, class module_info>::iterator map_iter;
	Command_Impl *pChildDeviceCommand;
	std::string target_type;

	pos=0;
	//g_pPlutoLogger->Write(LV_STATUS, "statechange Reply received from GC100: %s",message.c_str());

	token=StringUtils::Tokenize(message,",",pos); // state/statechange
	token=StringUtils::Tokenize(message,",",pos); // address
	module_address=token;

	token=StringUtils::Tokenize(message,",",pos); // input state
	sscanf(token.c_str(),"%d",&input_state);

	g_pPlutoLogger->Write(LV_STATUS, "statechange Reply interpreted as module: %s change to %d",module_address.c_str(),input_state);

	// Now figure out which child this belongs to.
	nomination=NULL;
	result=NULL;
	global_pin_target=0;

	// First look up this module to see what its global pin number is; we might need it later
	map_iter=module_map.find(module_address);

	if (map_iter != module_map.end())
	{
		target_type = map_iter->second.type;

		if ( (target_type == "IR") || (target_type=="RELAY"))
		{
			global_pin_target=map_iter->second.global_slot;
		}
		else
		{
			g_pPlutoLogger->Write(LV_WARNING, "statechange Reply: found module but it's not IR/RELAY; it's %s",target_type.c_str());
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_STATUS, "statechange Reply: did not find module %s in map",module_address.c_str());
	}

	// Now do the search of all the children.  We're going to look at the pin number.
	// What we'd really like to find is something in the 4:3 format.  If we see that,
	// it takes priority.  The next best thing will be a global pin id (ie 5)

	for (child_iter=m_mapCommandImpl_Children.begin(); child_iter!=m_mapCommandImpl_Children.end(); ++child_iter)
	{
		pChildDeviceCommand = (*child_iter).second;

		if (pChildDeviceCommand->m_pData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Input_Ouput_CONST)
		{
			std::string this_pin;
			std::string io_direction;

			Generic_Input_Ouput_Command * child = (Generic_Input_Ouput_Command *) pChildDeviceCommand;

			const char * directions[] = {"IN", "OUT", "BOTH"};
			io_direction = "** UNKNOWN **";
			if (child->DATA_Get_InputOrOutput() >= 1 && child->DATA_Get_InputOrOutput() <= 3)
				io_direction = directions[child->DATA_Get_InputOrOutput()];
			this_pin = child->GetData()->Get_Channel();

			//g_pPlutoLogger->Write(LV_STATUS, "statechange Reply: found a child pin number of %s, direction is %s",this_pin.c_str(),io_direction.c_str());

			if (((target_type == "IR") && (strcasecmp(io_direction.c_str(), "IN") == 0)) ||
				((target_type == "RELAY") && (strcasecmp(io_direction.c_str(), "OUT") == 0)))
			{

				// See if it matches exactly
				if (this_pin == module_address)
				{
					g_pPlutoLogger->Write(LV_STATUS, "statechange Reply: matches exactly in m:s format");
					result = child;
				}

				// See if it matches the global number
				if ( (global_pin_target>0) && (this_pin == StringUtils::itos(global_pin_target)))
				{
					nomination = child;
				}
			}
			else
			{
				//g_pPlutoLogger->Write(LV_STATUS, "statechange Reply: Sorry, not eligible (not the correct type)");
			}
		}
	}

	if (result==NULL)
	{
		g_pPlutoLogger->Write(LV_STATUS, "statechange Reply: no exact match in m:s format, trying global pin number");
		result=nomination;
	}

	if (result!=NULL)
	{
		std::string verify_request;
		result->GetEvents()->Pin_Changed(input_state); 

		if (change)
		{
			// If this was a 'statechange', reissue a state request.  At least once a statechange has been missed.
			verify_request="getstate,"+module_address;
			command_list.push_back(verify_request);
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "statechange Reply: Sorry, after all that searching, I can't determine which child device should sent the pin_changed");
	}
}

void gc100::parse_gc100_reply(std::string message)
{
	// Main parsing routine for all replies received back from GC100.
	// Specific message types are dispatched to a specific handling routine
	// so that this routine stays a manageable size!

	g_pPlutoLogger->Write(LV_STATUS, "Reply received from GC100: %s",message.c_str());

	if (message.length()>6)
	{
		if (message.substr(0,6)=="device")
		{
			parse_message_device(message); // pass it off to the "device message" routine
		}
	}

	if (message.length()>10)
	{
		if (message.substr(0,10)=="completeir")
		{
			parse_message_completeir(message);
		}
	}

	if (message.length()>6)
	{
		if ( (message.substr(0,6)=="state,") )
		{
			parse_message_statechange(message,false);
		}
	}

	if (message.length()>10)
	{
		if ( (message.substr(0,10)=="statechang") )
		{
			parse_message_statechange(message,true);
		}
	}
}

std::string gc100::read_from_gc100()
{
	std::string return_value;

	PLUTO_SAFETY_LOCK(sl, shared_lock);
	return_value="";

	while (read(gc100_socket, (void *) &recv_buffer[recv_pos],1)>0)
	{
		if (recv_buffer[recv_pos]=='\x0d')
		{
			recv_buffer[recv_pos]='\0';
			if (strlen(recv_buffer)>0) {
				parse_gc100_reply(string(recv_buffer));
				return_value=string(recv_buffer);
				recv_pos=0;
			}
		}
		else
		{
			recv_pos++;
		}
	}
	return return_value; // returns the last complete message
}

bool gc100::Open_gc100_Socket()
{
	bool return_value;

	// socket stuff
	struct hostent *hp;
	struct sockaddr_in sin;
	int res;

	PLUTO_SAFETY_LOCK(sl, shared_lock);
	return_value=false;

	ip_addr=GetData()->m_sIPAddress;
	g_pPlutoLogger->Write(LV_STATUS, "Conencting to: %s", ip_addr.c_str());

	// Do the socket connect
	hp = gethostbyname(ip_addr.c_str());

	if (!hp)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Unable to resolve host name.  Could not connect to GC100");
	}
	else
	{
		gc100_socket=socket(PF_INET, SOCK_STREAM, 0);
		if (gc100_socket < 0)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Unable to allocate socket.  Could not connect to GC100");
		}
		else
		{
			memset(&sin,0,sizeof(sin));
			sin.sin_family=AF_INET;
			sin.sin_port=htons(GC100_COMMAND_PORT);

			memcpy(&sin.sin_addr, hp->h_addr, sizeof(sin.sin_addr));
			if (connect(gc100_socket, (sockaddr *) &sin, sizeof(sin)))
			{
				g_pPlutoLogger->Write(LV_CRITICAL, "Unable to connect to GC100 on socket.");
				close(gc100_socket);
			}
			else
			{
#ifdef WIN32
#define fcntl
#define F_GETFL 0
#define F_SETFL 0
#define O_NONBLOCK 0
#endif
				res = fcntl(gc100_socket, F_GETFL);
				if ((res < 0) || (fcntl(gc100_socket, F_SETFL, res | O_NONBLOCK) < 0))
				{
					g_pPlutoLogger->Write(LV_CRITICAL, "Unable to set flags on GC100 socket.");
					close(gc100_socket);
				}
				else
				{
					g_pPlutoLogger->Write(LV_STATUS, "GC100 socket connect OK");
					return_value=true;
				}
			}
		}
	}

	return return_value;
}

bool gc100::ReceivedMessage(class Message *pMessage)
{
	m_pThisMessage = pMessage;

	cout << "Message ID: " << pMessage->m_dwID << endl;
	cout << "From:" << pMessage->m_dwPK_Device_From << endl;
	cout << "To: " << pMessage->m_dwPK_Device_To << endl;
	
	map<long, string>::iterator i;
	for (i = pMessage->m_mapParameters.begin(); i != pMessage->m_mapParameters.end(); i++)
	{
		cout << "Parameter: " << i->first << " Value: " << i->second << endl;
	}

	// Let the IR Base classes try to handle the message
	if (gc100_Command::ReceivedMessage(pMessage))
		return true;

	cout << "Processing..." << endl;
	
	// TODO: use mnemonics instead of numbers
	if (pMessage->m_dwID == 350 /* ACTION_SET_FREQUENCY */)
	{
		SendString("OK");
		string IR_FREQUENCY=pMessage->m_mapParameters[106];
		//SET_LEARN_FREQUENCY(IR_FREQUENCY);
		return true;
	}
	if (pMessage->m_dwID == 348 /*ACTION_LEARN_IR*/)
	{
		SendString("OK");
		string ABSOLUTE_CHANNEL=pMessage->m_mapParameters[35]; // Not used
		string ID=pMessage->m_mapParameters[53]; // Action ID
		string IR_FREQUENCY=pMessage->m_mapParameters[106]; // Not used
		LEARN_IR(StringUtils::itos(pMessage->m_dwPK_Device_To),ABSOLUTE_CHANNEL,ID,IR_FREQUENCY);
		return true;
	}
	if (pMessage->m_dwID == 349 /*ACTION_LEARN_IR_CANCEL*/)
	{
		SendString("OK");
		LEARN_IR_CANCEL("");
		return true;
	}
	if (pMessage->m_dwID == 402 /* send_data */)
	{
		SendString("OK");
		send_data("","");
		return true;
	}
	if (pMessage->m_dwID == 403 /* recv_data */)
	{
		SendString("OK");
		recv_data("");
		return true;
	}
	if (pMessage->m_dwID == 1 /* On */)
	{
		SendString("OK");
		relay_power(pMessage,true);
		return true;
	}
	if (pMessage->m_dwID == 2 /* Off */)
	{
		SendString("OK");
		relay_power(pMessage,false);
		return true;
	}

	g_pPlutoLogger->Write(LV_STATUS, "Mesage Received %d completely unhandled",pMessage->m_dwID);
	//SendString("OK"); // DEBUG
	//return true; // DEBUG

	return false;
}

void gc100::send_data(string TEXT,string CHANNEL_NAME)
{
	//g_pPlutoLogger->Write(LV_STATUS,"Generic send");
	send_to_gc100();
}

void gc100::recv_data(string CHANNEL_NAME)
{
	//g_pPlutoLogger->Write(LV_STATUS,"Generic recv");
	read_from_gc100();
}

// TODO: This resembles a little (or is that a lot?) with (part of) parse_message_statechange (or it looks like it does, anyway) -- Radu
void gc100::relay_power(class Message *pMessage, bool power_on)
{
	MapCommand_Impl::iterator child_iter;
	Command_Impl *pChildDeviceCommand;
	int target_device;

	g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.");
	target_device=pMessage->m_dwPK_Device_To;
	g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: target device is %d",target_device);

	for (child_iter=m_mapCommandImpl_Children.begin(); child_iter!=m_mapCommandImpl_Children.end(); ++child_iter)
	{
		pChildDeviceCommand = (*child_iter).second;

		if (pChildDeviceCommand->m_pData->m_dwPK_DeviceTemplate == DEVICETEMPLATE_Generic_Input_Ouput_CONST)
		{
			std::string this_pin;
			std::string io_direction;
			int this_device_id;

			Generic_Input_Ouput_Command * child = (Generic_Input_Ouput_Command *) pChildDeviceCommand;

			const char * directions[] = {"IN", "OUT", "BOTH"};
			io_direction = "** UNKNOWN **";
			if (child->DATA_Get_InputOrOutput() >= 1 && child->DATA_Get_InputOrOutput() <= 3)
				io_direction = directions[child->DATA_Get_InputOrOutput()];
			this_pin = child->GetData()->Get_Channel();

			if (strcasecmp(io_direction.c_str(),"OUT")==0)
			{
				g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: Pin direction is out, OK");
				this_device_id = child->m_dwPK_Device;

				g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: This device is %d",this_device_id);

				PLUTO_SAFETY_LOCK(sl, shared_lock);
				if (this_device_id == target_device)
				{
					std::string module_id;
					std::map<std::string,class module_info>::iterator module_iter;

					g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: Matched target device");
					module_iter=module_map.find(this_pin);

					if (module_iter==module_map.end())
					{
						std::map<std::string,class module_info>::iterator module_iter2;
						g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: Did not find %s in module map",this_pin.c_str());

						// OK, it must be in the global number format.  Search through the RELAY modules until you find it.
						for (module_iter2=module_map.begin(); module_iter2!=module_map.end(); ++module_iter2++)
						{
							if (module_iter2->second.type == "RELAY") {
								g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: Found a Relay number %d",module_iter2->second.global_slot);

								if (StringUtils::itos(module_iter2->second.global_slot) == this_pin)
								{
									g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: Matched Relay number %d.  ID will be %s",module_iter2->second.global_slot,module_iter2->first.c_str());
									module_id=module_iter2->first;
								}
							}
						}
					}
					else
					{
						g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: Found %s in module map",this_pin.c_str());
						module_id=this_pin;
					}

					// Module ID should now be the key of the desired relay
					g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: Regardless, module ID is %s",module_id.c_str());

					// Sanity check.  Make sure module_id really is there
					module_iter=module_map.find(module_id);

					if (module_iter==module_map.end())
					{
						g_pPlutoLogger->Write(LV_WARNING,"Relay Pwr.: Module_iter %s should be found but isn't",module_id.c_str());
					}
					else
					{
						std::string cmd;

						// Compose the command to the GC100

						cmd="setstate," + module_id +"," ;

						if (power_on)
						{
							cmd = cmd + "1";
						}
						else
						{
							cmd = cmd + "0";
						}

						command_list.push_back(cmd);
					}
				}
			}
			else
			{
				g_pPlutoLogger->Write(LV_STATUS,"Relay Pwr.: This pin is not a relay (direction not OUT)");
			} // End if direction is OUT
		}
	}
}

// Not done
void gc100::SendIR(string Port, string IRCode)
{
	std::string gc_code,send_cmd;
	std::map<std::string,class module_info>::iterator slot_iter;
	//int timeout_count;


	if (IRCode.length()>2)
	{
		if (IRCode.substr(0,2)=="P," || IRCode.substr(0,2)=="p,")
		{
			ConvertPronto(IRCode.substr(2), gc_code);

			//g_pPlutoLogger->Write(LV_STATUS, "SendIR: Result of Pronto conversion was %s",gc_code.c_str());

			// Search through all the IR modules and send the code to any IR modules which either matches
			// the 4:2 style port id or the absolute ('6') port ID or if the port string is blank

			PLUTO_SAFETY_LOCK(sl, shared_lock);
			for (slot_iter=module_map.begin(); slot_iter !=module_map.end(); ++slot_iter)
			{
				//g_pPlutoLogger->Write(LV_STATUS, "SendIR: Examining slot %s",slot_iter->second.key.c_str());

				if ( (slot_iter->second.type=="IR") &&
					( 
					(slot_iter->second.key == Port) || 
					(StringUtils::itos(slot_iter->second.global_slot) == Port) || 
					(Port=="") 
					) 
					)
				{
					//g_pPlutoLogger->Write(LV_STATUS, "SendIR: Yes, this is one I should send to %s",slot_iter->second.key.c_str());

					// Create header for this command: sendir,<address>,<id>,
					send_cmd=std::string("sendir,")+slot_iter->second.key.c_str()+std::string(",")+StringUtils::itos(ir_cmd_id)+","+gc_code;
					command_list.push_back(send_cmd);
					ir_cmd_id++;

				} // end if it's IR
			} // end loop through devices
		}
		else
		{
			g_pPlutoLogger->Write(LV_WARNING, "SendIR: I don't know what to do with non-Pronto code %s.",IRCode.c_str());
		}
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING, "SendIR: IRCode was too short");
	}
}

// Not done
void gc100::LEARN_IR(string PKID_Device,string ABSOLUTE_CHANNEL,string ID,string IR_FREQUENCY)
{
	g_pPlutoLogger->Write(LV_STATUS,"RECEIVED LEARN_IR");

	PLUTO_SAFETY_LOCK(sl, shared_lock);
	m_IRDeviceID = atoi(PKID_Device.c_str()); // Device_To
	m_IRActionID = atoi(ID.c_str());
	m_ControllerID = m_pThisMessage->m_dwPK_Device_From; // Device_From

	learn_input_string=""; // Clear out the IR to learn the next sequence clean

	if (is_open_for_learning)
	{
		m_bLearning = true;
	}
	else
	{
		g_pPlutoLogger->Write(LV_WARNING,"Can't learn because no learning port is open");
	}
}

// Not done

void gc100::SET_LEARN_FREQUENCY(string IR_FREQUENCY)
{
	std::string code;
	ConvertPronto("blah",code);
	g_pPlutoLogger->Write(LV_STATUS,"SET_FREQ: convert returned %s",code.c_str());

	/*if (IR_FREQUENCY.length() > 0)
	{	
	char *dummy;
	double freq = strtod(IR_FREQUENCY.c_str(), &dummy);
	if (freq > 20000.0)
	m_SlinkeBase->SetIRCarrier(freq);
	}	*/
}

bool gc100::open_for_learning()
{
	bool return_value;

	return_value=true;

#ifdef WIN32
#define O_RDONLY _O_RDONLY
#define open _open
#endif

	learn_fd=open(learn_device.c_str(), O_RDONLY | O_NONBLOCK);

	if (learn_fd<1)
	{
		return_value=false;
	}

	return return_value;
}

// Not done
void gc100::MonitorIR()
{
	char learn_buffer[512];
	int retval;
	bool learning_error;

	learning_error=false;

	ProcessMessageQueue();

	// Read any IR data present
	retval=0;

	PLUTO_SAFETY_LOCK(sl, shared_lock);
	if (is_open_for_learning)
	{
		retval=read(learn_fd,learn_buffer,511);
		//		}

		if (retval==0)
		{
			learning_timeout_count++;
		}

		//if (retval>0) {
		while ((retval>0)&&(!learning_error))
		{
			std::string new_string;

			learn_buffer[retval]='\0';

			if (strstr(learn_buffer,"X")!=NULL)
			{ // X from GC-IRL indicates error, probably "code too long"
				learning_error=true;
				g_pPlutoLogger->Write(LV_STATUS, "learning error detected");
			}

			new_string=std::string(learn_buffer);
			g_pPlutoLogger->Write(LV_STATUS, "IR %d Data received: %s",retval,new_string.c_str());
			learn_input_string+=new_string;
			receiving_data=true;
			learning_timeout_count=0;

			retval=read(learn_fd,learn_buffer,511);
		} //else 

		//if ((learning_timeout_count>LEARNING_TIMEOUT)||(retval<0)||((retval>0)&&(strstr(learn_buffer,"X")!=NULL))) {
		if ((learning_timeout_count>LEARNING_TIMEOUT)||(retval<0)|| learning_error)
		{
			if ((m_bLearning)&&(receiving_data))
			{
				std::string pronto_result;


				g_pPlutoLogger->Write(LV_STATUS, "Finished learning, IRDevice=%d, IRAction=%d, controller=%d, size=%d",m_IRDeviceID, m_IRActionID,m_ControllerID,learn_input_string.length());
				g_pPlutoLogger->Write(LV_STATUS, "Terminating on retval of %d err=%d, desc=%s, timeout_count of %d",retval,errno,strerror(errno),learning_timeout_count);
				g_pPlutoLogger->Write(LV_STATUS, "Raw learn string received: %s",learn_input_string.c_str());

				pronto_result=IRL_to_pronto(learn_input_string);
				g_pPlutoLogger->Write(LV_STATUS, "Conversion to Pronto: %s",pronto_result.c_str());

				m_CodeMap[IntPair(m_IRDeviceID, m_IRActionID)] = pronto_result; // Not sure about this

				Message *pMessage = new Message(m_IRDeviceID, 0, 
					PRIORITY_NORMAL, MESSAGETYPE_EVENT, 
					EVENT_Learned_IR_code_CONST, 3, 
					EVENTPARAMETER_PK_Orbiter_CONST, StringUtils::itos(m_ControllerID).c_str(),
					EVENTPARAMETER_Text_CONST, pronto_result.c_str(), 
					EVENTPARAMETER_ID_CONST, StringUtils::itos(m_IRActionID).c_str());

				if (!learning_error)
				{  // Only send this if there was no error detected
					GetEvents()->SendMessage(pMessage);
				}
				else
				{
					g_pPlutoLogger->Write(LV_WARNING, "Learn event not sent because GC-IRL indicated error, possibly code was too long to be learned");
				}
				m_bLearning=false;
			}
			receiving_data=false;
		} // end elseif retval<=0
	} // end if is_open_for_learning

	/*
	m_SlinkeBase->Receive(&code);

	while (code.length() > 0)
	{
		// look for data
		if ((bps = code.find('[')) == string::npos) 
			break;	
		if ((bpe = code.find(']')) == string::npos) 
			break;
		// find device
		dev = code.substr(0,bps);
		// find data
		data = code.substr(bps+1,bpe-bps-1);
		raw = code.substr(0,bpe+1);
		//			raw = StringUtils::TrimLeft(raw);
		// remove from code
		tmp = code.substr(bpe+1);
		code = tmp;
		// identify device
		if ((p=dev.find("ir")) != string::npos) 
		{
			// get ir port number (Slink-e II)
			tmp = dev.substr(p+2);
			devnum = strtol(tmp.c_str(),&nc,10)+4;
		}
		else if ((p=dev.find("slb")) != string::npos) 
		{
			// get bit mode slink port number
			tmp = dev.substr(p+3);
			devnum = strtol(tmp.c_str(),&nc,10)+128;
		}
		else if ((p=dev.find("sl")) != string::npos) 
		{
			// get slink port number
			tmp = dev.substr(p+2);
			devnum = strtol(tmp.c_str(),&nc,10);
		}
		else 
		{
			// send it anyway in case of lowlevel
			devnum = -1;
		}
		*/

		/*	m_IRDeviceID = m_pThisMessage->m_DeviceIDFrom;
		m_IRActionID = atoi(ID.c_str());
		m_bLearning = true; */

		/*
		printf("IRCode: %f, %40.40s...\n", m_SlinkeBase->GetIRCarrier(), data.c_str());
		if (m_bLearning)
		{
			printf("Learning to %d,%d\n", m_IRDeviceID, m_IRActionID);
			string PlutoIR = StringUtils::Format("%f*", m_SlinkeBase->GetIRCarrier())+data;
			m_CodeMap[IntPair(m_IRDeviceID, m_IRActionID)] = PlutoIR;
			m_bLearning = false;
			Message *pMessage = new Message(m_IRDeviceID, 0, PRIORITY_NORMAL, MESSAGETYPE_EVENT, EVENTLIST_LEARNED_CODE_CONST, 3, C_EVENTPARAMETER_PKID_CONTROLLER_CONST, StringUtils::itos(m_ControllerID).c_str(),C_EVENTPARAMETER_TOKEN_CONST, PlutoIR.c_str(), C_EVENTPARAMETER_ID_CONST, StringUtils::itos(m_IRActionID).c_str());
			GetEvents()->SendMessage(pMessage);
		}
	}	*/
	//sleep_ms(25);
}
