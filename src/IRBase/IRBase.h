#ifndef IRBASE
#define IRBASE

#include "DCE/Command_Impl.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/Other.h"

using namespace DCE;


const timespec DEFAULT_PREDELAY = {0, 100000000}; // 100 ms
const timespec DEFAULT_POSTDELAY = {0, 0}; // 0 ms

// IR Queue element
class IRQueue
{
public:
	IRQueue(string Port, string IRCode, timespec Delay, timespec PreDelay, long DeviceNum = -1, long CommandNum = -1, long StepNumber = 0,
		long ChannelSequenceNumber = 0)
	: m_Port(Port), m_IRCode(IRCode), m_Delay(Delay), m_PreDelay(PreDelay),
		m_DeviceNum(DeviceNum), m_CommandNum(CommandNum), m_StepNumber(StepNumber),
		m_ChannelSequenceNumber(ChannelSequenceNumber) {}
	
	IRQueue(string Port, string IRCode, unsigned long Delay = 0, unsigned long PreDelay = 0, long DeviceNum = -1, long CommandNum = -1, long StepNumber = 0,
		long ChannelSequenceNumber = 0)
	: m_Port(Port), m_IRCode(IRCode), m_DeviceNum(DeviceNum), m_CommandNum(CommandNum), m_StepNumber(StepNumber),
		m_ChannelSequenceNumber(ChannelSequenceNumber)
	{
		m_Delay = ms_to_timespec(Delay);
		m_PreDelay = ms_to_timespec(PreDelay);
	}
	
	string m_Port, m_IRCode;
	timespec m_Delay, m_PreDelay;
	long m_DeviceNum, m_CommandNum, m_StepNumber, m_ChannelSequenceNumber;
};

class IRBase
{
protected:
	typedef pair<long, long> longPair;
	map<longPair, string> m_CodeMap;		// maps device,command pair to IR code
	map<long, string> m_mapsDevicePort;		// map deviceIDs to their full port names
	map<long, timespec> m_DeviceNextAvailableTime;
	map<string, timespec> m_PortLastIRSent;
	map<long, timespec> m_DevicePreDelays;
	map<long, timespec> m_DeviceDefaultDelays;
	map<long, long> m_ExecutingChannelSequenceNumber;
	list<IRQueue *> m_IRQueue;
	long m_CurrentChannelSequenceNumber;

	pthread_t m_MessageQueueThread;
	pluto_pthread_mutex_t m_IRMutex;
	pthread_cond_t m_IRCond;

	virtual void SendIR(string Port, string IRCode) = 0;
	
	bool AddChannelChangeToQueue(long ChannelNumber, long Device);
	void AddIRToQueue(string Port, string IRCode, timespec Delay, long DeviceNum = -1, long CommandNum = -1, long Count = 1,
		long ChannelSequenceNumber = 0);
	void AddIRToQueue(string Port, string IRCode, unsigned long Delay = 0, long DeviceNum = -1, long CommandNum = -1, long Count = 1,
		long ChannelSequenceNumber = 0);
	bool ProcessQueue();
	void ParseDevices();
	bool ProcessMessage(class Message *pMessage);

public:
	Command_Impl * m_pCommand_Impl;

	IRBase() : m_IRMutex("ir")
	{
		// Grab the command so we can look at device parameters.
		m_pCommand_Impl = NULL;
	}
	virtual ~IRBase();

	void Init(Command_Impl * pCommand_Impl);
	virtual void SetQuitFlag() = 0;
	void MessageQueueThread();
};

#endif
