#ifndef IRBASE
#define IRBASE

#include "DCE/Command_Impl.h"
#include "PlutoUtils/MultiThreadIncludes.h"

using namespace DCE;

#define DEFAULT_PREDELAY 100
#define DEFAULT_POSTDELAY 0

// IR Queue element
class IRQueue
{
public:
	IRQueue(string Port, string IRCode, long Delay = 0, long DeviceNum = -1, long CommandNum = -1, long StepNumber = 0,
		long PreDelay = 0, long ChannelSequenceNumber = 0)
	: m_Port(Port), m_IRCode(IRCode), m_PreDelay(PreDelay), m_Delay(Delay),
		m_DeviceNum(DeviceNum), m_CommandNum(CommandNum), m_StepNumber(StepNumber),
		m_ChannelSequenceNumber(ChannelSequenceNumber)
	{}
	string m_Port, m_IRCode;
	long m_PreDelay, m_Delay, m_DeviceNum, m_CommandNum, m_StepNumber, m_ChannelSequenceNumber;
};

class IRBase
{
protected:
	pluto_pthread_mutex_t m_IRMutex;

	virtual void SendIR(string Port, string IRCode) = 0;
	
	bool AddChannelChangeToQueue(long ChannelNumber, long Device);
	void AddIRToQueue(string Port, string IRCode, long Delay = 0, long DeviceNum = -1, long CommandNum = -1, long Count = 1,
		long ChannelSequenceNumber = 0);
	bool ProcessQueue();
	void ParseDevices();
	bool ProcessMessage(class Message *pMessage);

	typedef pair<long, long> longPair;
	map<longPair, string> m_CodeMap;		//maps from pin # to event object
	map<long, string> m_mapsDevicePort;		// map deviceIDs to their full port names
	map<long, clock_t> m_DeviceNextAvailableTime;
	map<string, clock_t> m_PortLastIRSent;
	map<long, clock_t> m_DevicePreDelays;
	map<long, clock_t> m_DeviceDefaultDelays;
	map<long, long> m_ExecutingChannelSequenceNumber;
	list<IRQueue *> m_IRQueue;
	long m_CurrentChannelSequenceNumber;

public:
	Command_Impl * m_pCommand_Impl;

	IRBase() : m_IRMutex("ir")
	{
		// Grab the command so we can look at device parameters.
		m_pCommand_Impl = NULL;
	}
	virtual ~IRBase();

	void Init(Command_Impl * pCommand_Impl);
};

#endif
