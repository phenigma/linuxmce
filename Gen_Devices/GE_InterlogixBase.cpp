#include "GE_InterlogixBase.h"
#include "DeviceData_Impl.h"
#include "Logger.h"

using namespace DCE;
#include "../GE_Interlogix/GE_Interlogix.h"
DeviceData_Impl *GE_Interlogix_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
{
	// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position
	SerializeClass b;
	b.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);
	int iPK_Device = b.Read_unsigned_long();
	int iPK_Installation = b.Read_unsigned_long();
	int iPK_DeviceTemplate = b.Read_unsigned_long();
	switch(iPK_DeviceTemplate) {
		case 44:
			return new GE_Interlogix_Data();
	};
	g_pPlutoLogger->Write(LV_STATUS, "Got CreateData for unknown type %d.", iPK_DeviceTemplate);
	return NULL;
}

Event_Impl *GE_Interlogix_Event::CreateEvent( unsigned long dwPK_DeviceTemplate, ClientSocket *pOCClientSocket, unsigned long dwDevice )
{
	switch(dwPK_DeviceTemplate) {
		case 44:
			return (Event_Impl *) new GE_Interlogix_Event(pOCClientSocket, dwDevice);
	};
	g_pPlutoLogger->Write(LV_STATUS, "Got CreateEvent for unknown type %d.", dwPK_DeviceTemplate);
	return NULL;
}
Command_Impl  *GE_Interlogix_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
	g_pPlutoLogger->Write(LV_STATUS, "Got CreateCommand for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
