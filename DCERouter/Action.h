#ifndef COMMAND_H
#define COMMAND_H

class Command
{
public:
	int m_PK_Command,m_PK_Device,m_PK_DeviceGroup,m_PK_C_Array;
	string m_sDeviceList; // AB 1-30-2005 this was an afterthought
	map<int,class CommandParameter *> m_mapCommandParameter;
	
	Command(int PK_Command,int PK_Device,int PK_DeviceGroup,int PK_C_Array)
	{
		m_PK_Command = PK_Command;
		m_PK_Device = PK_Device;
		m_PK_DeviceGroup = PK_DeviceGroup;
		m_PK_C_Array = PK_C_Array;
	}

	~Command();
};

#endif

