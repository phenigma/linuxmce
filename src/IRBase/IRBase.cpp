/*
	IRBase

	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	www.plutohome.com

 	Phone: +1 (877) 758-8648

	This program is distributed according to the terms of the Pluto Public License, available at:
	http://plutohome.com/index.php?section=public_license

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */

#include "IRBase.h"

#include "pluto_main/Define_DeviceData.h"
#include "pluto_main/Define_DeviceCategory.h"
#include "pluto_main/Define_Command.h"
#include "pluto_main/Define_CommandParameter.h"
#include "Infrared_Plugin/Infrared_Plugin.h"
#include "Gen_Devices/AllCommandsRequests.h"
#include "DCE/Logger.h"
#include "Serial/SerialPort.h"

#include <math.h>
#include <time.h>

// MessageQueue processor (the one that does the actual IR sending and relay setting)
void * StartMessageQueueThread(void * Arg)
{
	IRBase * pIRBase = (IRBase *) Arg;
	pIRBase->MessageQueueThread();
	return NULL;
}

void IRBase::Init(Command_Impl * pCommand_Impl)
{
	m_CurrentChannelSequenceNumber = 1;
	m_pCommand_Impl = pCommand_Impl;
	pthread_mutex_init(&m_IRMutex.mutex, 0);
	pthread_cond_init(&m_IRCond, NULL);
	ParseDevices();

	if (pthread_create(&m_MessageQueueThread, NULL, StartMessageQueueThread, (void *) this))
	{
		g_pPlutoLogger->Write(LV_CRITICAL, "Failed to create Message Queue Thread");
		SetQuitFlag();
		exit(1);
	}
}

IRBase::~IRBase()
{
	pthread_mutex_destroy(&m_IRMutex.mutex);
}

// get details (IR sequences, Delays, etc.) of all child devices
void IRBase::ParseDevices()
{
	g_pPlutoLogger->Write(LV_STATUS, "In IRBase::ParseDevices");
	MapCommand_Impl::iterator iChild;
	// pair: {long (DeviceID), Command_Impl *}
	for (iChild = m_pCommand_Impl->m_mapCommandImpl_Children.begin(); iChild != m_pCommand_Impl->m_mapCommandImpl_Children.end(); ++iChild)
	{
		long DeviceID = (*iChild).first;

		string codes;
		m_mapsDevicePort[DeviceID] = (*iChild).second->m_pData->m_mapParameters[DEVICEDATA_Port_CONST];
		DCE::CMD_Get_Infrared_Codes_Cat CMD_Get_Infrared_Codes_Cat(m_pCommand_Impl->m_dwPK_Device, DEVICECATEGORY_Infrared_Plugins_CONST,
			false, BL_SameHouse, DeviceID, &codes);
		m_pCommand_Impl->SendCommand(CMD_Get_Infrared_Codes_Cat);

		long iPK_Command, count;
		string code;
		size_t pos = 0;

		count = atoi(StringUtils::Tokenize(codes, "\t", pos).c_str());
		g_pPlutoLogger->Write(LV_STATUS, "Requested IR cored for device: %lu", DeviceID);
		g_pPlutoLogger->Write(LV_STATUS, "IR Code request got this reply: %s", codes.c_str());
		g_pPlutoLogger->Write(LV_STATUS, "IR Code count: %d", count);
		for(long i = 0; i < count; i++)
		{
			iPK_Command = atoi(StringUtils::Tokenize(codes, "\t", pos).c_str());
			code = StringUtils::Tokenize(codes, "\t", pos);
			m_CodeMap[longPair(DeviceID, iPK_Command)] = code;
			g_pPlutoLogger->Write(LV_STATUS, "Loaded IR code for Device %ld, Action %ld", DeviceID, iPK_Command);
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
				m_DevicePreDelays[DeviceID] = ms_to_timespec(atoi(tok.c_str()));
				tok = StringUtils::Tokenize((*param).second, ",", pos);
				if (tok.length())
				{
					m_DeviceDefaultDelays[DeviceID] = ms_to_timespec(atoi(tok.c_str()));
				}
			}
		}
	}
}


void IRBase::AddIRToQueue(string Port, string IRCode, unsigned long Delay, long DeviceNum, long CommandNum, long Count, long ChannelSequenceNumber)
{
	AddIRToQueue(Port, IRCode, ms_to_timespec(Delay), DeviceNum, CommandNum, Count, ChannelSequenceNumber);
}

void IRBase::AddIRToQueue(string Port, string IRCode, timespec Delay, long DeviceNum, long CommandNum, long Count, long ChannelSequenceNumber)
{
	pthread_mutex_lock(&m_IRMutex.mutex);

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
				printf("Removing redundant channel change digit (action %ld, seq %ld).\n", (*ipQueue)->m_CommandNum, (*ipQueue)->m_ChannelSequenceNumber);
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
			timespec tsDelay = ms_to_timespec(atoi(StringUtils::Tokenize(tok, "@", pos2).c_str()));
			m_IRQueue.push_back((pLastQueued = new IRQueue(Port, Code, tsDelay, m_DevicePreDelays[DeviceNum], DeviceNum, CommandNum, Step++, ChannelSequenceNumber)));
		}
		if (pLastQueued)
		{
			pLastQueued->m_Delay += Delay;
		}
	}

	pthread_cond_broadcast(&m_IRCond);
	pthread_mutex_unlock(&m_IRMutex.mutex);
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
		printf("Device id %ld has no number digits parameter.  Assuming 3 and no enter.\n", Device);
	}
	timespec DigitDelay = {0, 0};
	param = m_pCommand_Impl->m_mapCommandImpl_Children[Device]->m_pData->m_mapParameters.find(DEVICEDATA_Digit_Delay_CONST);
	if (param != m_pCommand_Impl->m_mapCommandImpl_Children[Device]->m_pData->m_mapParameters.end())
	{
		DigitDelay = ms_to_timespec(atoi((*param).second.c_str()));
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

	if (pMessage->m_dwID == COMMAND_Send_Code_CONST)
	{
		long CommandNum = -1, DeviceNum = -1;

		// Is an action ID specified?
		if (pMessage->m_mapParameters.count(COMMANDPARAMETER_ID_CONST)>0)
		{
			CommandNum=atoi(pMessage->m_mapParameters[COMMANDPARAMETER_ID_CONST].c_str());
			DeviceNum=TargetDevice;
		}
		AddIRToQueue(pMessage->m_mapParameters[COMMANDPARAMETER_Absolute_Channel_CONST],
				pMessage->m_mapParameters[COMMANDPARAMETER_Tokens_CONST], 0, DeviceNum, CommandNum);
		return true;
	}
	if( pMessage->m_dwID == COMMAND_Tune_to_channel_CONST)
	{
		//AddChannelChangeToQueue(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_Absolute_Channel_CONST].c_str()), TargetDevice);
		AddChannelChangeToQueue(atoi(pMessage->m_mapParameters[COMMANDPARAMETER_ProgramID_CONST].c_str()), TargetDevice);
		return true;
	}

	// Ok, not a system-handled action.

	//Check the device/code map to see if we have an IR code for it.
	if (m_CodeMap.count(longPair(TargetDevice, pMessage->m_dwID))>0)
	{
		long Delay=0, Count=1;
		map<int,string>::iterator param;

		if (pMessage->m_dwID == COMMAND_Generic_On_CONST || pMessage->m_dwID == COMMAND_Generic_Off_CONST || pMessage->m_dwID == COMMAND_Toggle_Power_CONST)
		{
			printf("%ld: Got power command: ", TargetDevice);
			param = m_pCommand_Impl->m_mapCommandImpl_Children[TargetDevice]->m_pData->m_mapParameters.find(DEVICEDATA_IR_Power_Delay_CONST);
            if (param != m_pCommand_Impl->m_mapCommandImpl_Children[TargetDevice]->m_pData->m_mapParameters.end())
		    {
				Delay = atoi((*param).second.c_str());
				printf("using delay of %ld.\n", Delay);
			}
			else
			{
				printf("no delay found.\n");
			}
		}
		else if (pMessage->m_mapParameters.count(COMMANDPARAMETER_PK_Command_Input_CONST)>0)
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

bool IRBase::ProcessQueue()
{
	bool retval = false;

	pthread_mutex_lock(&m_IRMutex.mutex);

	while (m_IRQueue.size() == 0)
	{
		pthread_cond_wait(&m_IRCond, &m_IRMutex.mutex);
	}

	// We are holding the mutex

	list<IRQueue *>::iterator ipQueue;
	list<IRQueue *> NewIRQueue;
	timeval tvCurTime;
	timespec tsCurTime;
	gettimeofday(&tvCurTime, NULL);
	timeval_to_timespec(&tvCurTime, &tsCurTime);

	// printf("Clk = %d, CLK_TCK = %d\n", xClock(), CLK_TCK);

//	printf("Queue length: %d\n", m_IRQueue.size());
	for (ipQueue = m_IRQueue.begin(); ipQueue!= m_IRQueue.end(); ipQueue++)
	{
		// Is the last IR on this port + this pre delay still in the future?  If so continue on to the next queued item

		if ((*ipQueue)->m_PreDelay > ms_to_timespec(0) && tsCurTime < (*ipQueue)->m_PreDelay + m_PortLastIRSent[(*ipQueue)->m_Port])
		{
//			printf("Not ready yet: %d %d %d %d (1)\n", (*ipQueue)->m_PreDelay, MS_TO_CLK((*ipQueue)->m_PreDelay), m_PortLastIRSent[(*ipQueue)->m_Port], CurTime);
			NewIRQueue.push_back(* ipQueue);
			continue;
		}

		// Can this device accept IR yet?
		if ((*ipQueue)->m_DeviceNum > -1 && tsCurTime < m_DeviceNextAvailableTime[(*ipQueue)->m_DeviceNum])
		{
//			printf("Not ready yet (2)\n");
			NewIRQueue.push_back(* ipQueue);
			continue;
		}

		if ((*ipQueue)->m_DeviceNum > -1 && (*ipQueue)->m_ChannelSequenceNumber)
			m_ExecutingChannelSequenceNumber[(*ipQueue)->m_DeviceNum] = (*ipQueue)->m_ChannelSequenceNumber;

		// All above checks passed for this entry.  Fire it off, update the scheduler, remove, and return.

		if ((*ipQueue)->m_Port.substr(0,4)!="ttyS")
		{
			g_pPlutoLogger->Write(LV_STATUS, "IRQueue: Device %d, Action %d, Step %d", (*ipQueue)->m_DeviceNum, (*ipQueue)->m_CommandNum, (*ipQueue)->m_StepNumber);
			SendIR((*ipQueue)->m_Port, (*ipQueue)->m_IRCode);
			gettimeofday(&tvCurTime, NULL);
			timeval_to_timespec(&tvCurTime, &tsCurTime);
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
			m_DeviceNextAvailableTime[(*ipQueue)->m_DeviceNum] = tsCurTime + (*ipQueue)->m_Delay;
		}
		m_PortLastIRSent[(*ipQueue)->m_Port] = tsCurTime;
//		m_IRQueue.erase(ipQueue);
		retval = true;
	}

	m_IRQueue = NewIRQueue;

	pthread_mutex_unlock(&m_IRMutex.mutex);
	return retval;
}

void IRBase::MessageQueueThread()
{
	while (! m_pCommand_Impl->m_bQuit)
	{
		ProcessQueue();
	}
}
