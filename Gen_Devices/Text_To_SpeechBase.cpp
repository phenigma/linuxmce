#include "Text_To_SpeechBase.h"
#include "DeviceData_Impl.h"
#include "Logger.h"

using namespace DCE;
#include "Text_To_SpeechBase.h"
DeviceData_Impl *Text_To_Speech_Data::CreateData(DeviceData_Impl *Parent,char *pDataBlock,unsigned long AllocatedSize,char *CurrentPosition)
{
	// Peek ahead in the stream.  We're going to pass in the above pointers anyway so it won't affect the position
	SerializeClass b;
	b.ResumeReadWrite(pDataBlock,AllocatedSize,CurrentPosition);
	int iPK_Device = b.Read_unsigned_long();
	int iPK_Installation = b.Read_unsigned_long();
	int iPK_DeviceTemplate = b.Read_unsigned_long();
	switch(iPK_DeviceTemplate) {
		case 57:
			return new Text_To_Speech_Data();
	};
	g_pPlutoLogger->Write(LV_STATUS, "Got CreateData for unknown type %d.", iPK_DeviceTemplate);
	return NULL;
}

Event_Impl *Text_To_Speech_Event::CreateEvent(int PK_DeviceTemplate, ClientSocket *pOCClientSocket, int DeviceID)
{
	switch(PK_DeviceTemplate) {
		case 57:
			return (Event_Impl *) new Text_To_Speech_Event(pOCClientSocket, DeviceID);
	};
	g_pPlutoLogger->Write(LV_STATUS, "Got CreateEvent for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
Command_Impl  *Text_To_Speech_Command::CreateCommand(int PK_DeviceTemplate, Command_Impl *pPrimaryDeviceCommand, DeviceData_Impl *pData, Event_Impl *pEvent)
{
	switch(PK_DeviceTemplate)
	{
		case 57:
			return (Command_Impl *) new Text_To_Speech_Command(pPrimaryDeviceCommand, pData, pEvent, m_pRouter);
	};
	g_pPlutoLogger->Write(LV_STATUS, "Got CreateCommand for unknown type %d.", PK_DeviceTemplate);
	return NULL;
}
