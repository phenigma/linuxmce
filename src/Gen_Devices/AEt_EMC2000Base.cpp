#include "AEt_EMC2000Base.h"
#include "DeviceData_Impl.h"
#include "Logger.h"

using namespace DCE;
#include "AEt_EMC2000Base.h"
extern AEt_EMC2000_Command *Create_AEt_EMC2000(Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent, Router *pRouter);
DeviceData_Impl *AEt_EMC2000_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
{
	// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position
	SerializeClass b;
	b.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);
	int iPK_Device = b.Read_unsigned_long();
	int iPK_Installation = b.Read_unsigned_long();
	int iPK_DeviceTemplate = b.Read_unsigned_long();
	switch(iPK_DeviceTemplate) {
		case 2031:
			return new AEt_EMC2000_Data();
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateData for unknown type %d.", iPK_DeviceTemplate);
	return NULL;
}

Event_Impl *AEt_EMC2000_Event::CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice )
{
	switch(dwPK_DeviceTemplate) {
		case 2031:
			return (Event_Impl *) new AEt_EMC2000_Event(pOCClientSocket, dwDevice);
	};
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateEvent for unknown type %d.", dwPK_DeviceTemplate);
	return NULL;
}
Command_Impl  *AEt_EMC2000_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
	LoggerWrapper::GetInstance()->Write(LV_STATUS, "Got CreateCommand for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
