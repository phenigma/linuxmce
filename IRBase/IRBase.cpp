#include "IRBase.h"

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceTemplate.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "Infrared_Plugin/Infrared_Plugin.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Logger.h"
#include "Serial/SerialPort.h"

#include <math.h>
#include <time.h>

void IRBase::Init(Command_Impl * pCommand_Impl)
{
	m_CurrentChannelSequenceNumber = 1;
	m_pCommand_Impl = pCommand_Impl;
	pthread_mutex_init(&m_IRMutex.mutex, 0);
	ParseDevices();
}

IRBase::~IRBase()
{
	pthread_mutex_destroy(&m_IRMutex.mutex);
}

// get details (IR sequences, Delays, etc.) of all child devices (it seems)
void IRBase::ParseDevices()
{
	MapCommand_Impl::iterator iChild;
	// pair: {long (DeviceID), Command_Impl *}
	for (iChild = m_pCommand_Impl->m_mapCommandImpl_Children.begin(); iChild != m_pCommand_Impl->m_mapCommandImpl_Children.end(); ++iChild)
	{
		long DeviceID = (*iChild).first;
		
		string codes;
		m_mapsDevicePort[DeviceID] = (*iChild).second->m_pData->m_mapParameters[DEVICEDATA_Channel_CONST];
		DCE::CMD_Get_Infrared_Codes_Cat vCMD_Get_Infrared_Codes_Cat(m_pCommand_Impl->m_dwPK_Device, DEVICETEMPLATE_Infrared_Plugin_CONST,
			false, BL_SameHouse, DeviceID, &codes);
		m_pCommand_Impl->SendCommand(vCMD_Get_Infrared_Codes_Cat);
	
		long iPK_Command, count;
		string code;
		size_t pos = 0;

		count = atoi(StringUtils::Tokenize(codes, "\t", pos).c_str());
		for(long i=0;i<count;i++)
		{
			iPK_Command = atoi(StringUtils::Tokenize(codes, "\t", pos).c_str());
			code = StringUtils::Tokenize(codes, "\t", pos);
			m_CodeMap[longPair(DeviceID, iPK_Command)] = code;	
			g_pPlutoLogger->Write(LV_STATUS, "Loaded IR code for Device %d, Action %d", DeviceID, iPK_Command);
		}
		m_DevicePreDelays[DeviceID] = DEFAULT_PREDELAY;
		m_DeviceDefaultDelays[DeviceID] = DEFAULT_POSTDELAY;
		
		map<int, string>::iterator param;
       	param = (*iChild).second->m_pData->m_mapParameters.find(DEVICEDATA_IR_Sleep_BeforeAfter_CONST);
		// IR_Sleep_BeforeAfter syntax: PreDelay[,DefaultDelay]
		if (param != m_pCommand_Impl->m_mapCommandImpl_Children[DeviceID]->m_pData->m_mapParameters.end())
		{
			string::size_type pos = 0;
			string tok = StringUtils::Tokenize((*param).second, ",", pos);
			if (tok.length())
			{
				m_DevicePreDelays[DeviceID] = atoi(tok.c_str());
				tok = StringUtils::Tokenize((*param).second, ",", pos);
				if (tok.length())
				{
					m_DeviceDefaultDelays[DeviceID] = atoi(tok.c_str());
				}
			}
		}
	}
}

void IRBase::AddIRToQueue(string Port, string IRCode, long Delay, long DeviceNum, long CommandNum, long Count, long ChannelSequenceNumber)
{
	
	PLUTO_SAFETY_LOCK(sl, m_IRMutex);
	
	// If this is a volume command, check the outgoing queue and erase other volume requests. 
	if (CommandNum == COMMAND_Vol_Up_CONST || CommandNum == COMMAND_Vol_Down_CONST)
	{
		list<IRQueue *>::iterator ipQueue, ipNextQueue;
		for(ipNextQueue = m_IRQueue.begin(); ipNextQueue != m_IRQueue.end(); )
		{
			ipQueue = ipNextQueue++;
			
			if (((*ipQueue)->m_CommandNum == COMMAND_Vol_Up_CONST || (*ipQueue)->m_CommandNum == COMMAND_Vol_Down_CONST) && 
				(*ipQueue)->m_DeviceNum == DeviceNum)
			{
				m_IRQueue.erase(ipQueue);
			}
		}
	}
	else if (ChannelSequenceNumber)
	{
		list<IRQueue *>::iterator ipQueue, ipNextQueue;
		for(ipNextQueue = m_IRQueue.begin(); ipNextQueue != m_IRQueue.end(); )
		{
			ipQueue = ipNextQueue++;
			
			if (((*ipQueue)->m_ChannelSequenceNumber && DeviceNum > -1 &&
				(*ipQueue)->m_ChannelSequenceNumber != ChannelSequenceNumber &&
				(*ipQueue)->m_DeviceNum == DeviceNum) && 
				(*ipQueue)->m_ChannelSequenceNumber != m_ExecutingChannelSequenceNumber[DeviceNum])
			{
				printf("Removing redundant channel change digit (action %d, seq %d).\n", (*ipQueue)->m_CommandNum, (*ipQueue)->m_ChannelSequenceNumber);
				m_IRQueue.erase(ipQueue);
			}
		}
	}
	
	longPair IRCodePair(DeviceNum, CommandNum);
	if (IRCode.length() == 0)
	{
		if (CommandNum == -1 || DeviceNum == -1)
		{
			g_pPlutoLogger->Write(LV_CRITICAL, "Tried to AddIRToQueue without either an IR code or a device/action pair.");
		}
		else
		{
			// No IR code.  We need to look it up
			map <longPair, string>::iterator iCode = m_CodeMap.find(IRCodePair);
			if (iCode == m_CodeMap.end())
			{
				g_pPlutoLogger->Write(LV_WARNING, "Could not find a code for Device %d, Action %d", DeviceNum, CommandNum);
			}
			else
			{
				IRCode = (*iCode).second;
			}
		}
	}
	else
	{
		if (CommandNum != -1 && DeviceNum != -1)
		{
			// We have an IRCode and an action pair, this is usually the result of being sent a new code from 
			// the server, so, update our map.
			m_CodeMap[IRCodePair] = IRCode;
		}
	}
	if (Port.length() == 0)
	{
		map <long, string>::iterator iPort = m_mapsDevicePort.find(DeviceNum);
		if (iPort != m_mapsDevicePort.end())
		{
			Port = (*iPort).second;

		}
	}
	else
	{
		if (DeviceNum != -1)
		{
			m_mapsDevicePort[DeviceNum] = Port;
		}
	}
	long Step = 1;
	IRQueue *pLastQueued=NULL;
	long i;
	for(i=0;i<Count;++i)
	{
		string::size_type pos=0;
		string tok;
		while((tok = StringUtils::Tokenize(IRCode, "&", pos))!="")
		{
			string::size_type pos2=0;
			string Code = StringUtils::Tokenize(tok, "@", pos2);
			long Delay = atoi(StringUtils::Tokenize(tok, "@", pos2).c_str());
			m_IRQueue.push_back((pLastQueued = new IRQueue(Port, Code, Delay, DeviceNum, CommandNum, Step++, m_DevicePreDelays[DeviceNum], ChannelSequenceNumber)));
		}
		if (pLastQueued)
		{
			pLastQueued->m_Delay+=Delay;
		}
	}
}

bool IRBase::AddChannelChangeToQueue(long ChannelNumber, long Device)
{
	if (m_pCommand_Impl->m_mapCommandImpl_Children.count(Device) == 0)
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Cannot send channel change to non-existent device %d!", Device);
		return false;
	}
	long NumDigits = 3;
	bool bSendEnter = false;
	map<int,string>::iterator param;
	param = m_pCommand_Impl->m_mapCommandImpl_Children[Device]->m_pData->m_mapParameters.find(DEVICEDATA_Number_of_Digits_CONST);
	if (param != m_pCommand_Impl->m_mapCommandImpl_Children[Device]->m_pData->m_mapParameters.end())
	{
		string::size_type pos = 0;

		NumDigits = atoi(StringUtils::Tokenize((*param).second, ",", pos).c_str());
		string tok = StringUtils::ToUpper(StringUtils::Tokenize((*param).second, ",", pos));

		bSendEnter = (tok=="E" || tok=="e");
	}
	else
	{
		printf("Device id %d has no number digits parameter.  Assuming 3 and no enter.\n", Device);
	}
	long DigitDelay = 0;
	param = m_pCommand_Impl->m_mapCommandImpl_Children[Device]->m_pData->m_mapParameters.find(DEVICEDATA_Digit_Delay_CONST);
	if (param != m_pCommand_Impl->m_mapCommandImpl_Children[Device]->m_pData->m_mapParameters.end())
	{
		DigitDelay = atoi((*param).second.c_str());
	}
	long TotalDigits = StringUtils::ltos(ChannelNumber).length();
	if (NumDigits < TotalDigits)
	{
		if (NumDigits > 0)
		{
			g_pPlutoLogger->Write(LV_WARNING, "Warning, number of digits specified as %d but channel is %d!", NumDigits, ChannelNumber);
		}
		NumDigits = TotalDigits;
	}
	for(long i=NumDigits;i>0;i--)
	{
		unsigned char digit = (ChannelNumber % (long) pow((double) 10, (double) i)) / (long) pow((double) 10, (double) (i-1));
		g_pPlutoLogger->Write(LV_STATUS, "Sending digit %d...",digit);	
		AddIRToQueue("", "", DigitDelay, Device, digit + 80, 1, m_CurrentChannelSequenceNumber);
	}
	if (bSendEnter)
	{
		g_pPlutoLogger->Write(LV_STATUS, "Sending <enter>...");			
		AddIRToQueue("", "", DigitDelay, Device, COMMAND_Send_Generic_EnterGo_CONST, 1, m_CurrentChannelSequenceNumber);
	}
	m_CurrentChannelSequenceNumber++;
	return true;
}

bool IRBase::ProcessMessage(Message *pMessage)
{
	long TargetDevice = pMessage->m_dwPK_Device_To;

	// Is this a slave of another device?  If so, change the target

	MapCommand_Impl::iterator iTD = m_pCommand_Impl->m_mapCommandImpl_Children.find(TargetDevice);
	if (iTD != m_pCommand_Impl->m_mapCommandImpl_Children.end())
	{
// TODO: Aaron says something about "Autoroute children" and "changing the framework a little bit"..
//		map<int,string>::iterator param = (*iTD).second->m_pData->m_mapParameters.find(C_DEVICEPARAMETER_DEVICE_SLAVE_TO_CONST);
#define MSG "TODO: The following parameter is uninitialized!!! If it crashes, search here"
#ifdef WIN32
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOCWRN__ __FILE__ "("__STR1__(__LINE__)")"
#pragma message(__LOCWRN__ " : warning X1000: " MSG)
#else
#warning MSG
#endif
		map<int,string>::iterator param;
		if (param != m_pCommand_Impl->m_mapCommandImpl_Children[TargetDevice]->m_pData->m_mapParameters.end())
		{
			TargetDevice = atoi((*param).second.c_str());
			g_pPlutoLogger->Write(LV_STATUS, "Destination is slave device, changing target ID from %d to %d.", pMessage->m_dwPK_Device_To, TargetDevice);
		}
	}

	if (pMessage->m_dwID == COMMAND_Send_Code_CONST)
	{
		long CommandNum = -1, DeviceNum = -1;
		m_pCommand_Impl->SendString("OK");

		// Is an action ID specified? 
		if (pMessage->m_mapParameters.count(COMMANDPARAMETER_ID_CONST)>0) 
		{
			CommandNum=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST].c_str());
			DeviceNum=TargetDevice;
		}
		AddIRToQueue(pMessage->m_mapParameters[COMMANDPARAMETER_Absolute_Channel_CONST],pMessage->m_mapParameters[COMMANDPARAMETER_Tokens_CONST], 0, DeviceNum, CommandNum);
		return true;
	}
	if( pMessage->m_dwID == COMMAND_Tune_to_channel_CONST)
	{
		m_pCommand_Impl->SendString("OK");
		AddChannelChangeToQueue(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Absolute_Channel_CONST].c_str()), TargetDevice);
		return true;
	}

	// Ok, not a system-handled action.  

	//Check the device/code map to see if we have an IR code for it.
	if (m_CodeMap.count(longPair(TargetDevice, pMessage->m_dwID))>0)
	{
		m_pCommand_Impl->SendString("OK");
		long Delay=0, Count=1;
		map<int,string>::iterator param;

		if (pMessage->m_dwID == COMMAND_Generic_On_CONST || pMessage->m_dwID == COMMAND_Generic_Off_CONST || pMessage->m_dwID == COMMAND_Power_CONST)
		{
			printf("%d: Got power command: ", TargetDevice);
			param = m_pCommand_Impl->m_mapCommandImpl_Children[TargetDevice]->m_pData->m_mapParameters.find(DEVICEDATA_IR_Power_Delay_CONST);
            if (param != m_pCommand_Impl->m_mapCommandImpl_Children[TargetDevice]->m_pData->m_mapParameters.end())
		    {
				Delay = atoi((*param).second.c_str());
				printf("using delay of %d.\n", Delay);
			}
			else
			{
				printf("no delay found.\n");
			}
		}
		else if (pMessage->m_mapParameters.count(COMMANDPARAMETER_PK_Input_CONST)>0)
		{
			param = m_pCommand_Impl->m_mapCommandImpl_Children[TargetDevice]->m_pData->m_mapParameters.find(DEVICEDATA_IR_Mode_Delay_CONST);
            if (param != m_pCommand_Impl->m_mapCommandImpl_Children[TargetDevice]->m_pData->m_mapParameters.end())
			{
				Delay = atoi((*param).second.c_str());
				g_pPlutoLogger->Write(LV_STATUS, "Found mode delay of %d", Delay);
			}
		}
        if(pMessage->m_mapParameters.count(COMMANDPARAMETER_Repeat_CONST) > 0 && pMessage->m_mapParameters[COMMANDPARAMETER_Repeat_CONST].length() > 0)
		{
			Count = atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Repeat_CONST].c_str());
			if( Count==0 )
				Count=1;
		}
		AddIRToQueue("", "", Delay, TargetDevice, pMessage->m_dwID, Count);
		return true;
	}
	return false;
}

long MS_TO_CLK(long miliseconds)
{
	return miliseconds * CLOCKS_PER_SEC / 1000;
}

bool IRBase::ProcessQueue()
{
	PLUTO_SAFETY_LOCK(sl, m_IRMutex);
	list<IRQueue *>::iterator ipQueue;
	clock_t CurTime = clock();

	// printf("Clk = %d, CLK_TCK = %d\n", clock(), CLK_TCK);

	for(ipQueue = m_IRQueue.begin(); ipQueue!= m_IRQueue.end(); ++ipQueue)
	{
		// Is the last IR on this port + this pre delay still in the future?  If so continue on to the next queued item
		
		if ((*ipQueue)->m_PreDelay && MS_TO_CLK((*ipQueue)->m_PreDelay) + m_PortLastIRSent[(*ipQueue)->m_Port] > CurTime)
			continue;

		// Can this device accept IR yet?
		if ((*ipQueue)->m_DeviceNum > -1 && m_DeviceNextAvailableTime[(*ipQueue)->m_DeviceNum] > CurTime)
			continue;

		if ((*ipQueue)->m_DeviceNum > -1 && (*ipQueue)->m_ChannelSequenceNumber)
			m_ExecutingChannelSequenceNumber[(*ipQueue)->m_DeviceNum] = (*ipQueue)->m_ChannelSequenceNumber;

		// All above checks passed for this entry.  Fire it off, update the scheduler, remove, and return.

		if ((*ipQueue)->m_Port.substr(0,4)!="ttyS")
		{ 
			g_pPlutoLogger->Write(LV_STATUS, "IRQueue: Device %d, Action %d, Step %d", (*ipQueue)->m_DeviceNum, (*ipQueue)->m_CommandNum, (*ipQueue)->m_StepNumber);
			SendIR((*ipQueue)->m_Port, (*ipQueue)->m_IRCode);
			CurTime = clock();
		}
		else 
		{
			// If the target is a serial port, send it directly to the port

			if ((*ipQueue)->m_IRCode.substr(0,4)=="SER,")
			{
                string::size_type spos=0;
                string Port = StringUtils::Tokenize((*ipQueue)->m_Port, ",", spos);
                int Speed = atoi(StringUtils::Tokenize((*ipQueue)->m_Port, ",", spos).c_str());
                // todo string ParityBitStop = StringUtils::Tokenize(sPort, ",", spos);

                CSerialPort sp(Port, Speed, epbsN81);
				string OutStr = StringUtils::EscapeChars((*ipQueue)->m_IRCode.substr(4)); // will handle hex conversions
                sp.Write((char *)OutStr.c_str(), OutStr.length());
                printf("Writing %s to port %s speed %d\n",OutStr.c_str(), Port.c_str(), Speed);
			}
			else
			{
				g_pPlutoLogger->Write(LV_WARNING, "Trying to send code to a serial port that's not serial data (=%s)", (*ipQueue)->m_IRCode.c_str());
			}
		}
		// Now, update the port's last tick count and set the device's next readiness time
		if ((*ipQueue)->m_DeviceNum > -1)
		{
			(*ipQueue)->m_Delay = max((*ipQueue)->m_Delay, m_DeviceDefaultDelays[(*ipQueue)->m_DeviceNum]);
			m_DeviceNextAvailableTime[(*ipQueue)->m_DeviceNum] = CurTime + MS_TO_CLK((*ipQueue)->m_Delay);
		}
		m_PortLastIRSent[(*ipQueue)->m_Port] = CurTime;
		m_IRQueue.erase(ipQueue);
		return true;
	}
	return false;
}
