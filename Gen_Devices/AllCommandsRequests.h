#ifndef AllCommandsRequest_h
#define AllCommandsRequest_h
#include "Message.h"
#include "PreformedCommand.h"
namespace DCE
{
	 //  Commands
	class CMD_Capture_Keyboard_To_Variable : public PreformedCommand {
	public:
		CMD_Capture_Keyboard_To_Variable(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,1,7,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),8,sOnOff.c_str(),14,sType.c_str(),24,sReset.c_str(),25,StringUtils::itos(iPK_Text).c_str(),55,(bDataGrid ? "1" : "0")); }
	};
	class CMD_Capture_Keyboard_To_Variable_DL : public PreformedCommand {
	public:
		CMD_Capture_Keyboard_To_Variable_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,1,7,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),8,sOnOff.c_str(),14,sType.c_str(),24,sReset.c_str(),25,StringUtils::itos(iPK_Text).c_str(),55,(bDataGrid ? "1" : "0")); }
	};
	class CMD_Capture_Keyboard_To_Variable_MD : public PreformedCommand {
	public:
		CMD_Capture_Keyboard_To_Variable_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,1,7,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),8,sOnOff.c_str(),14,sType.c_str(),24,sReset.c_str(),25,StringUtils::itos(iPK_Text).c_str(),55,(bDataGrid ? "1" : "0")); }
	};
	class CMD_Capture_Keyboard_To_Variable_Cat : public PreformedCommand {
	public:
		CMD_Capture_Keyboard_To_Variable_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,1,7,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),8,sOnOff.c_str(),14,sType.c_str(),24,sReset.c_str(),25,StringUtils::itos(iPK_Text).c_str(),55,(bDataGrid ? "1" : "0")); }
	};
	class CMD_Orbiter_Beep : public PreformedCommand {
	public:
		CMD_Orbiter_Beep(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,2,0); }
	};
	class CMD_Orbiter_Beep_DL : public PreformedCommand {
	public:
		CMD_Orbiter_Beep_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,2,0); }
	};
	class CMD_Orbiter_Beep_MD : public PreformedCommand {
	public:
		CMD_Orbiter_Beep_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,2,0); }
	};
	class CMD_Orbiter_Beep_Cat : public PreformedCommand {
	public:
		CMD_Orbiter_Beep_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,2,0); }
	};
	class CMD_Display_OnOff : public PreformedCommand {
	public:
		CMD_Display_OnOff(long DeviceIDFrom, long DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,1,8,sOnOff.c_str()); }
	};
	class CMD_Display_OnOff_DL : public PreformedCommand {
	public:
		CMD_Display_OnOff_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,1,8,sOnOff.c_str()); }
	};
	class CMD_Display_OnOff_MD : public PreformedCommand {
	public:
		CMD_Display_OnOff_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,1,8,sOnOff.c_str()); }
	};
	class CMD_Display_OnOff_Cat : public PreformedCommand {
	public:
		CMD_Display_OnOff_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,1,8,sOnOff.c_str()); }
	};
	class CMD_Go_back : public PreformedCommand {
	public:
		CMD_Go_back(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Go_back_DL : public PreformedCommand {
	public:
		CMD_Go_back_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Go_back_MD : public PreformedCommand {
	public:
		CMD_Go_back_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Go_back_Cat : public PreformedCommand {
	public:
		CMD_Go_back_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Goto_Screen : public PreformedCommand {
	public:
		CMD_Goto_Screen(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,5,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0")); }
	};
	class CMD_Goto_Screen_DL : public PreformedCommand {
	public:
		CMD_Goto_Screen_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,5,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0")); }
	};
	class CMD_Goto_Screen_MD : public PreformedCommand {
	public:
		CMD_Goto_Screen_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,5,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0")); }
	};
	class CMD_Goto_Screen_Cat : public PreformedCommand {
	public:
		CMD_Goto_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,5,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0")); }
	};
	class CMD_Show_Object : public PreformedCommand {
	public:
		CMD_Show_Object(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,6,5,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),6,sComparisson_Operator.c_str(),7,sComparisson_Value.c_str(),8,sOnOff.c_str()); }
	};
	class CMD_Show_Object_DL : public PreformedCommand {
	public:
		CMD_Show_Object_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,6,5,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),6,sComparisson_Operator.c_str(),7,sComparisson_Value.c_str(),8,sOnOff.c_str()); }
	};
	class CMD_Show_Object_MD : public PreformedCommand {
	public:
		CMD_Show_Object_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,6,5,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),6,sComparisson_Operator.c_str(),7,sComparisson_Value.c_str(),8,sOnOff.c_str()); }
	};
	class CMD_Show_Object_Cat : public PreformedCommand {
	public:
		CMD_Show_Object_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,6,5,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),6,sComparisson_Operator.c_str(),7,sComparisson_Value.c_str(),8,sOnOff.c_str()); }
	};
	class CMD_Terminate_Orbiter : public PreformedCommand {
	public:
		CMD_Terminate_Orbiter(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,7,0); }
	};
	class CMD_Terminate_Orbiter_DL : public PreformedCommand {
	public:
		CMD_Terminate_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,7,0); }
	};
	class CMD_Terminate_Orbiter_MD : public PreformedCommand {
	public:
		CMD_Terminate_Orbiter_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,7,0); }
	};
	class CMD_Terminate_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_Terminate_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,7,0); }
	};
	class CMD_Remove_Screen_From_History : public PreformedCommand {
	public:
		CMD_Remove_Screen_From_History(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,8,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Remove_Screen_From_History_DL : public PreformedCommand {
	public:
		CMD_Remove_Screen_From_History_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,8,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Remove_Screen_From_History_MD : public PreformedCommand {
	public:
		CMD_Remove_Screen_From_History_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,8,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Remove_Screen_From_History_Cat : public PreformedCommand {
	public:
		CMD_Remove_Screen_From_History_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,8,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Scroll_Grid : public PreformedCommand {
	public:
		CMD_Scroll_Grid(long DeviceIDFrom, long DeviceIDTo,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,9,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Scroll_Grid_DL : public PreformedCommand {
	public:
		CMD_Scroll_Grid_DL(long DeviceIDFrom, string DeviceIDTo,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,9,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Scroll_Grid_MD : public PreformedCommand {
	public:
		CMD_Scroll_Grid_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,9,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Scroll_Grid_Cat : public PreformedCommand {
	public:
		CMD_Scroll_Grid_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,9,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Move_Highlight : public PreformedCommand {
	public:
		CMD_Move_Highlight(long DeviceIDFrom, long DeviceIDTo,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,10,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Move_Highlight_DL : public PreformedCommand {
	public:
		CMD_Move_Highlight_DL(long DeviceIDFrom, string DeviceIDTo,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,10,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Move_Highlight_MD : public PreformedCommand {
	public:
		CMD_Move_Highlight_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,10,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Move_Highlight_Cat : public PreformedCommand {
	public:
		CMD_Move_Highlight_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,10,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class RESP_Request_Datagrid_Contents : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;
	public:
		RESP_Request_Datagrid_Contents(char **pData,int *iData_Size) { 
		m_pData=pData; m_iData_Size=iData_Size; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; *m_iData_Size=pMessage->m_mapData_Lengths[19]; };
	};
	class CMD_Request_Datagrid_Contents : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"));		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size); }
	};
	class CMD_Request_Datagrid_Contents_DL : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"));		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size); }
	};
	class CMD_Request_Datagrid_Contents_MD : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"));		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size); }
	};
	class CMD_Request_Datagrid_Contents_Cat : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"));		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size); }
	};
	class CMD_NOREP_Request_Datagrid_Contents : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0")); }
	};
	class CMD_NOREP_Request_Datagrid_Contents_DL : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0")); }
	};
	class CMD_NOREP_Request_Datagrid_Contents_MD : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0")); }
	};
	class CMD_NOREP_Request_Datagrid_Contents_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iRow,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),32,StringUtils::itos(iRow).c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0")); }
	};
	class CMD_Play_Sound : public PreformedCommand {
	public:
		CMD_Play_Sound(long DeviceIDFrom, long DeviceIDTo,char *pData,int iData_Size,string sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,13,2,-19,pData,iData_Size,20,sFormat.c_str()); }
	};
	class CMD_Play_Sound_DL : public PreformedCommand {
	public:
		CMD_Play_Sound_DL(long DeviceIDFrom, string DeviceIDTo,char *pData,int iData_Size,string sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,13,2,-19,pData,iData_Size,20,sFormat.c_str()); }
	};
	class CMD_Play_Sound_MD : public PreformedCommand {
	public:
		CMD_Play_Sound_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,char *pData,int iData_Size,string sFormat) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,13,2,-19,pData,iData_Size,20,sFormat.c_str()); }
	};
	class CMD_Play_Sound_Cat : public PreformedCommand {
	public:
		CMD_Play_Sound_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,char *pData,int iData_Size,string sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,13,2,-19,pData,iData_Size,20,sFormat.c_str()); }
	};
	class CMD_Refresh : public PreformedCommand {
	public:
		CMD_Refresh(long DeviceIDFrom, long DeviceIDTo,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,14,1,15,sDataGrid_ID.c_str()); }
	};
	class CMD_Refresh_DL : public PreformedCommand {
	public:
		CMD_Refresh_DL(long DeviceIDFrom, string DeviceIDTo,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,14,1,15,sDataGrid_ID.c_str()); }
	};
	class CMD_Refresh_MD : public PreformedCommand {
	public:
		CMD_Refresh_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,14,1,15,sDataGrid_ID.c_str()); }
	};
	class CMD_Refresh_Cat : public PreformedCommand {
	public:
		CMD_Refresh_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,14,1,15,sDataGrid_ID.c_str()); }
	};
	class CMD_Regen_Screen : public PreformedCommand {
	public:
		CMD_Regen_Screen(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,15,0); }
	};
	class CMD_Regen_Screen_DL : public PreformedCommand {
	public:
		CMD_Regen_Screen_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,15,0); }
	};
	class CMD_Regen_Screen_MD : public PreformedCommand {
	public:
		CMD_Regen_Screen_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,15,0); }
	};
	class CMD_Regen_Screen_Cat : public PreformedCommand {
	public:
		CMD_Regen_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,15,0); }
	};
	class CMD_Requires_Special_Handling : public PreformedCommand {
	public:
		CMD_Requires_Special_Handling(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,16,0); }
	};
	class CMD_Requires_Special_Handling_DL : public PreformedCommand {
	public:
		CMD_Requires_Special_Handling_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,16,0); }
	};
	class CMD_Requires_Special_Handling_MD : public PreformedCommand {
	public:
		CMD_Requires_Special_Handling_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,16,0); }
	};
	class CMD_Requires_Special_Handling_Cat : public PreformedCommand {
	public:
		CMD_Requires_Special_Handling_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,16,0); }
	};
	class CMD_Seek_Data_Grid : public PreformedCommand {
	public:
		CMD_Seek_Data_Grid(long DeviceIDFrom, long DeviceIDTo,string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,17,4,9,sText.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),15,sDataGrid_ID.c_str()); }
	};
	class CMD_Seek_Data_Grid_DL : public PreformedCommand {
	public:
		CMD_Seek_Data_Grid_DL(long DeviceIDFrom, string DeviceIDTo,string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,17,4,9,sText.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),15,sDataGrid_ID.c_str()); }
	};
	class CMD_Seek_Data_Grid_MD : public PreformedCommand {
	public:
		CMD_Seek_Data_Grid_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,17,4,9,sText.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),15,sDataGrid_ID.c_str()); }
	};
	class CMD_Seek_Data_Grid_Cat : public PreformedCommand {
	public:
		CMD_Seek_Data_Grid_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,17,4,9,sText.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),15,sDataGrid_ID.c_str()); }
	};
	class CMD_Set_Graphic_To_Display : public PreformedCommand {
	public:
		CMD_Set_Graphic_To_Display(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,18,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Set_Graphic_To_Display_DL : public PreformedCommand {
	public:
		CMD_Set_Graphic_To_Display_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,18,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Set_Graphic_To_Display_MD : public PreformedCommand {
	public:
		CMD_Set_Graphic_To_Display_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,18,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Set_Graphic_To_Display_Cat : public PreformedCommand {
	public:
		CMD_Set_Graphic_To_Display_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,18,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Set_House_Mode : public PreformedCommand {
	public:
		CMD_Set_House_Mode(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,string sErrors) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,2,5,sValue_To_Assign.c_str(),18,sErrors.c_str()); }
	};
	class CMD_Set_House_Mode_DL : public PreformedCommand {
	public:
		CMD_Set_House_Mode_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,string sErrors) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,2,5,sValue_To_Assign.c_str(),18,sErrors.c_str()); }
	};
	class CMD_Set_House_Mode_MD : public PreformedCommand {
	public:
		CMD_Set_House_Mode_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,string sErrors) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,2,5,sValue_To_Assign.c_str(),18,sErrors.c_str()); }
	};
	class CMD_Set_House_Mode_Cat : public PreformedCommand {
	public:
		CMD_Set_House_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,string sErrors) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,2,5,sValue_To_Assign.c_str(),18,sErrors.c_str()); }
	};
	class CMD_Set_Object_Parameter : public PreformedCommand {
	public:
		CMD_Set_Object_Parameter(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,20,3,3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),27,StringUtils::itos(iPK_DesignObjParameter).c_str()); }
	};
	class CMD_Set_Object_Parameter_DL : public PreformedCommand {
	public:
		CMD_Set_Object_Parameter_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,20,3,3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),27,StringUtils::itos(iPK_DesignObjParameter).c_str()); }
	};
	class CMD_Set_Object_Parameter_MD : public PreformedCommand {
	public:
		CMD_Set_Object_Parameter_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,20,3,3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),27,StringUtils::itos(iPK_DesignObjParameter).c_str()); }
	};
	class CMD_Set_Object_Parameter_Cat : public PreformedCommand {
	public:
		CMD_Set_Object_Parameter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,20,3,3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),27,StringUtils::itos(iPK_DesignObjParameter).c_str()); }
	};
	class CMD_Set_Object_Position : public PreformedCommand {
	public:
		CMD_Set_Object_Position(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,21,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Object_Position_DL : public PreformedCommand {
	public:
		CMD_Set_Object_Position_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,21,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Object_Position_MD : public PreformedCommand {
	public:
		CMD_Set_Object_Position_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,21,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Object_Position_Cat : public PreformedCommand {
	public:
		CMD_Set_Object_Position_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,21,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Object_Size : public PreformedCommand {
	public:
		CMD_Set_Object_Size(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,22,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Object_Size_DL : public PreformedCommand {
	public:
		CMD_Set_Object_Size_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,22,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Object_Size_MD : public PreformedCommand {
	public:
		CMD_Set_Object_Size_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,22,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Object_Size_Cat : public PreformedCommand {
	public:
		CMD_Set_Object_Size_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,22,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Pos_Rel_To_Parent : public PreformedCommand {
	public:
		CMD_Set_Pos_Rel_To_Parent(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,23,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Pos_Rel_To_Parent_DL : public PreformedCommand {
	public:
		CMD_Set_Pos_Rel_To_Parent_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,23,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Pos_Rel_To_Parent_MD : public PreformedCommand {
	public:
		CMD_Set_Pos_Rel_To_Parent_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,23,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Pos_Rel_To_Parent_Cat : public PreformedCommand {
	public:
		CMD_Set_Pos_Rel_To_Parent_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,23,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Size_Rel_To_Parent : public PreformedCommand {
	public:
		CMD_Set_Size_Rel_To_Parent(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,24,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Size_Rel_To_Parent_DL : public PreformedCommand {
	public:
		CMD_Set_Size_Rel_To_Parent_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,24,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Size_Rel_To_Parent_MD : public PreformedCommand {
	public:
		CMD_Set_Size_Rel_To_Parent_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,24,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Size_Rel_To_Parent_Cat : public PreformedCommand {
	public:
		CMD_Set_Size_Rel_To_Parent_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,24,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Text : public PreformedCommand {
	public:
		CMD_Set_Text(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sText,int iPK_Text) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,25,3,3,sPK_DesignObj.c_str(),9,sText.c_str(),25,StringUtils::itos(iPK_Text).c_str()); }
	};
	class CMD_Set_Text_DL : public PreformedCommand {
	public:
		CMD_Set_Text_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sText,int iPK_Text) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,25,3,3,sPK_DesignObj.c_str(),9,sText.c_str(),25,StringUtils::itos(iPK_Text).c_str()); }
	};
	class CMD_Set_Text_MD : public PreformedCommand {
	public:
		CMD_Set_Text_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sText,int iPK_Text) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,25,3,3,sPK_DesignObj.c_str(),9,sText.c_str(),25,StringUtils::itos(iPK_Text).c_str()); }
	};
	class CMD_Set_Text_Cat : public PreformedCommand {
	public:
		CMD_Set_Text_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sText,int iPK_Text) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,25,3,3,sPK_DesignObj.c_str(),9,sText.c_str(),25,StringUtils::itos(iPK_Text).c_str()); }
	};
	class CMD_Set_User_Mode : public PreformedCommand {
	public:
		CMD_Set_User_Mode(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_User_Mode_DL : public PreformedCommand {
	public:
		CMD_Set_User_Mode_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_User_Mode_MD : public PreformedCommand {
	public:
		CMD_Set_User_Mode_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_User_Mode_Cat : public PreformedCommand {
	public:
		CMD_Set_User_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_Variable : public PreformedCommand {
	public:
		CMD_Set_Variable(long DeviceIDFrom, long DeviceIDTo,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Variable_DL : public PreformedCommand {
	public:
		CMD_Set_Variable_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Variable_MD : public PreformedCommand {
	public:
		CMD_Set_Variable_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Variable_Cat : public PreformedCommand {
	public:
		CMD_Set_Variable_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Simulate_Keypress : public PreformedCommand {
	public:
		CMD_Simulate_Keypress(long DeviceIDFrom, long DeviceIDTo,int iPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,StringUtils::itos(iPK_Button).c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Keypress_DL : public PreformedCommand {
	public:
		CMD_Simulate_Keypress_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,StringUtils::itos(iPK_Button).c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Keypress_MD : public PreformedCommand {
	public:
		CMD_Simulate_Keypress_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,StringUtils::itos(iPK_Button).c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Keypress_Cat : public PreformedCommand {
	public:
		CMD_Simulate_Keypress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,StringUtils::itos(iPK_Button).c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Mouse_Click : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click(long DeviceIDFrom, long DeviceIDTo,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,29,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Simulate_Mouse_Click_DL : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_DL(long DeviceIDFrom, string DeviceIDTo,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,29,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Simulate_Mouse_Click_MD : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,29,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Simulate_Mouse_Click_Cat : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,29,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Stop_Sound : public PreformedCommand {
	public:
		CMD_Stop_Sound(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,30,0); }
	};
	class CMD_Stop_Sound_DL : public PreformedCommand {
	public:
		CMD_Stop_Sound_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,30,0); }
	};
	class CMD_Stop_Sound_MD : public PreformedCommand {
	public:
		CMD_Stop_Sound_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,30,0); }
	};
	class CMD_Stop_Sound_Cat : public PreformedCommand {
	public:
		CMD_Stop_Sound_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,30,0); }
	};
	class CMD_Store_Variables : public PreformedCommand {
	public:
		CMD_Store_Variables(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,31,0); }
	};
	class CMD_Store_Variables_DL : public PreformedCommand {
	public:
		CMD_Store_Variables_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,31,0); }
	};
	class CMD_Store_Variables_MD : public PreformedCommand {
	public:
		CMD_Store_Variables_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,31,0); }
	};
	class CMD_Store_Variables_Cat : public PreformedCommand {
	public:
		CMD_Store_Variables_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,31,0); }
	};
	class CMD_Update_Object_Image : public PreformedCommand {
	public:
		CMD_Update_Object_Image(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,32,4,3,sPK_DesignObj.c_str(),14,sType.c_str(),-19,pData,iData_Size,23,sDisable_Aspect_Lock.c_str()); }
	};
	class CMD_Update_Object_Image_DL : public PreformedCommand {
	public:
		CMD_Update_Object_Image_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,32,4,3,sPK_DesignObj.c_str(),14,sType.c_str(),-19,pData,iData_Size,23,sDisable_Aspect_Lock.c_str()); }
	};
	class CMD_Update_Object_Image_MD : public PreformedCommand {
	public:
		CMD_Update_Object_Image_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,32,4,3,sPK_DesignObj.c_str(),14,sType.c_str(),-19,pData,iData_Size,23,sDisable_Aspect_Lock.c_str()); }
	};
	class CMD_Update_Object_Image_Cat : public PreformedCommand {
	public:
		CMD_Update_Object_Image_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,32,4,3,sPK_DesignObj.c_str(),14,sType.c_str(),-19,pData,iData_Size,23,sDisable_Aspect_Lock.c_str()); }
	};
	class CMD_Start_Media : public PreformedCommand {
	public:
		CMD_Start_Media(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sFilename,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,33,3,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Start_Media_DL : public PreformedCommand {
	public:
		CMD_Start_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sFilename,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,33,3,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Start_Media_MD : public PreformedCommand {
	public:
		CMD_Start_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sFilename,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,33,3,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Start_Media_Cat : public PreformedCommand {
	public:
		CMD_Start_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sFilename,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,33,3,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class RESP_Populate_Datagrid : public PreformedCommandResponse {
		int *m_iPK_Variable;string *m_sValue_To_Assign;bool *m_bIsSuccessful;
	public:
		RESP_Populate_Datagrid(int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful) { 
		m_iPK_Variable=iPK_Variable; m_sValue_To_Assign=sValue_To_Assign; m_bIsSuccessful=bIsSuccessful; }
		void ParseResponse(Message *pMessage) {
			*m_iPK_Variable=atoi(pMessage->m_mapParameters[4].c_str()); *m_sValue_To_Assign=pMessage->m_mapParameters[5]; *m_bIsSuccessful=(pMessage->m_mapParameters[40]=="1" ? true : false); };
	};
	class CMD_Populate_Datagrid : public PreformedCommand {
	public:
		CMD_Populate_Datagrid(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful); }
	};
	class CMD_Populate_Datagrid_DL : public PreformedCommand {
	public:
		CMD_Populate_Datagrid_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful); }
	};
	class CMD_Populate_Datagrid_MD : public PreformedCommand {
	public:
		CMD_Populate_Datagrid_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful); }
	};
	class CMD_Populate_Datagrid_Cat : public PreformedCommand {
	public:
		CMD_Populate_Datagrid_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful); }
	};
	class CMD_NOREP_Populate_Datagrid : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_NOREP_Populate_Datagrid_DL : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_NOREP_Populate_Datagrid_MD : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_NOREP_Populate_Datagrid_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_Datagrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_Datagrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_Create_Media : public PreformedCommand {
	public:
		CMD_Create_Media(long DeviceIDFrom, long DeviceIDTo,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Create_Media_DL : public PreformedCommand {
	public:
		CMD_Create_Media_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Create_Media_MD : public PreformedCommand {
	public:
		CMD_Create_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Create_Media_Cat : public PreformedCommand {
	public:
		CMD_Create_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Play_Media : public PreformedCommand {
	public:
		CMD_Play_Media(long DeviceIDFrom, long DeviceIDTo,string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,StringUtils::itos(iMediaPosition).c_str()); }
	};
	class CMD_Play_Media_DL : public PreformedCommand {
	public:
		CMD_Play_Media_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,StringUtils::itos(iMediaPosition).c_str()); }
	};
	class CMD_Play_Media_MD : public PreformedCommand {
	public:
		CMD_Play_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,StringUtils::itos(iMediaPosition).c_str()); }
	};
	class CMD_Play_Media_Cat : public PreformedCommand {
	public:
		CMD_Play_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,int iPK_MediaType,int iStreamID,int iMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,StringUtils::itos(iMediaPosition).c_str()); }
	};
	class CMD_Stop_Media : public PreformedCommand {
	public:
		CMD_Stop_Media(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Stop_Media_DL : public PreformedCommand {
	public:
		CMD_Stop_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Stop_Media_MD : public PreformedCommand {
	public:
		CMD_Stop_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Stop_Media_Cat : public PreformedCommand {
	public:
		CMD_Stop_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Pause_Media : public PreformedCommand {
	public:
		CMD_Pause_Media(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,39,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Pause_Media_DL : public PreformedCommand {
	public:
		CMD_Pause_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,39,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Pause_Media_MD : public PreformedCommand {
	public:
		CMD_Pause_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,39,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Pause_Media_Cat : public PreformedCommand {
	public:
		CMD_Pause_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,39,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Restart_Media : public PreformedCommand {
	public:
		CMD_Restart_Media(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,40,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Restart_Media_DL : public PreformedCommand {
	public:
		CMD_Restart_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,40,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Restart_Media_MD : public PreformedCommand {
	public:
		CMD_Restart_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,40,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Restart_Media_Cat : public PreformedCommand {
	public:
		CMD_Restart_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,40,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Change_Playback_Speed : public PreformedCommand {
	public:
		CMD_Change_Playback_Speed(long DeviceIDFrom, long DeviceIDTo,int iStreamID,int iMediaPlaybackSpeed) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,41,2,41,StringUtils::itos(iStreamID).c_str(),43,StringUtils::itos(iMediaPlaybackSpeed).c_str()); }
	};
	class CMD_Change_Playback_Speed_DL : public PreformedCommand {
	public:
		CMD_Change_Playback_Speed_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,int iMediaPlaybackSpeed) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,41,2,41,StringUtils::itos(iStreamID).c_str(),43,StringUtils::itos(iMediaPlaybackSpeed).c_str()); }
	};
	class CMD_Change_Playback_Speed_MD : public PreformedCommand {
	public:
		CMD_Change_Playback_Speed_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,int iMediaPlaybackSpeed) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,41,2,41,StringUtils::itos(iStreamID).c_str(),43,StringUtils::itos(iMediaPlaybackSpeed).c_str()); }
	};
	class CMD_Change_Playback_Speed_Cat : public PreformedCommand {
	public:
		CMD_Change_Playback_Speed_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,int iMediaPlaybackSpeed) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,41,2,41,StringUtils::itos(iStreamID).c_str(),43,StringUtils::itos(iMediaPlaybackSpeed).c_str()); }
	};
	class CMD_Rewind_Playback : public PreformedCommand {
	public:
		CMD_Rewind_Playback(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,0); }
	};
	class CMD_Rewind_Playback_DL : public PreformedCommand {
	public:
		CMD_Rewind_Playback_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,0); }
	};
	class CMD_Rewind_Playback_MD : public PreformedCommand {
	public:
		CMD_Rewind_Playback_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,0); }
	};
	class CMD_Rewind_Playback_Cat : public PreformedCommand {
	public:
		CMD_Rewind_Playback_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,0); }
	};
	class CMD_MH_Play_Media : public PreformedCommand {
	public:
		CMD_MH_Play_Media(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_MH_Play_Media_DL : public PreformedCommand {
	public:
		CMD_MH_Play_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_MH_Play_Media_MD : public PreformedCommand {
	public:
		CMD_MH_Play_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_MH_Play_Media_Cat : public PreformedCommand {
	public:
		CMD_MH_Play_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_MH_Stop_Media : public PreformedCommand {
	public:
		CMD_MH_Stop_Media(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_MH_Stop_Media_DL : public PreformedCommand {
	public:
		CMD_MH_Stop_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_MH_Stop_Media_MD : public PreformedCommand {
	public:
		CMD_MH_Stop_Media_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_MH_Stop_Media_Cat : public PreformedCommand {
	public:
		CMD_MH_Stop_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_Disk_Drive_Monitoring_ON : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_ON(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,45,0); }
	};
	class CMD_Disk_Drive_Monitoring_ON_DL : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_ON_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,45,0); }
	};
	class CMD_Disk_Drive_Monitoring_ON_MD : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_ON_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,45,0); }
	};
	class CMD_Disk_Drive_Monitoring_ON_Cat : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_ON_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,45,0); }
	};
	class CMD_Disk_Drive_Monitoring_OFF : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_OFF(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,46,0); }
	};
	class CMD_Disk_Drive_Monitoring_OFF_DL : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_OFF_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,46,0); }
	};
	class CMD_Disk_Drive_Monitoring_OFF_MD : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_OFF_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,46,0); }
	};
	class CMD_Disk_Drive_Monitoring_OFF_Cat : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_OFF_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,46,0); }
	};
	class CMD_Reset_Disk_Drive : public PreformedCommand {
	public:
		CMD_Reset_Disk_Drive(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,47,0); }
	};
	class CMD_Reset_Disk_Drive_DL : public PreformedCommand {
	public:
		CMD_Reset_Disk_Drive_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,47,0); }
	};
	class CMD_Reset_Disk_Drive_MD : public PreformedCommand {
	public:
		CMD_Reset_Disk_Drive_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,47,0); }
	};
	class CMD_Reset_Disk_Drive_Cat : public PreformedCommand {
	public:
		CMD_Reset_Disk_Drive_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,47,0); }
	};
	class CMD_Eject_Disk : public PreformedCommand {
	public:
		CMD_Eject_Disk(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,48,0); }
	};
	class CMD_Eject_Disk_DL : public PreformedCommand {
	public:
		CMD_Eject_Disk_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,48,0); }
	};
	class CMD_Eject_Disk_MD : public PreformedCommand {
	public:
		CMD_Eject_Disk_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,48,0); }
	};
	class CMD_Eject_Disk_Cat : public PreformedCommand {
	public:
		CMD_Eject_Disk_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,48,0); }
	};
	class CMD_Start_Burn_Session : public PreformedCommand {
	public:
		CMD_Start_Burn_Session(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,49,0); }
	};
	class CMD_Start_Burn_Session_DL : public PreformedCommand {
	public:
		CMD_Start_Burn_Session_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,49,0); }
	};
	class CMD_Start_Burn_Session_MD : public PreformedCommand {
	public:
		CMD_Start_Burn_Session_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,49,0); }
	};
	class CMD_Start_Burn_Session_Cat : public PreformedCommand {
	public:
		CMD_Start_Burn_Session_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,49,0); }
	};
	class CMD_Start_Ripping_Session : public PreformedCommand {
	public:
		CMD_Start_Ripping_Session(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,50,0); }
	};
	class CMD_Start_Ripping_Session_DL : public PreformedCommand {
	public:
		CMD_Start_Ripping_Session_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,50,0); }
	};
	class CMD_Start_Ripping_Session_MD : public PreformedCommand {
	public:
		CMD_Start_Ripping_Session_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,50,0); }
	};
	class CMD_Start_Ripping_Session_Cat : public PreformedCommand {
	public:
		CMD_Start_Ripping_Session_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,50,0); }
	};
	class CMD_Add_File_To_Burning_Session : public PreformedCommand {
	public:
		CMD_Add_File_To_Burning_Session(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,51,0); }
	};
	class CMD_Add_File_To_Burning_Session_DL : public PreformedCommand {
	public:
		CMD_Add_File_To_Burning_Session_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,51,0); }
	};
	class CMD_Add_File_To_Burning_Session_MD : public PreformedCommand {
	public:
		CMD_Add_File_To_Burning_Session_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,51,0); }
	};
	class CMD_Add_File_To_Burning_Session_Cat : public PreformedCommand {
	public:
		CMD_Add_File_To_Burning_Session_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,51,0); }
	};
	class CMD_Start_Burning : public PreformedCommand {
	public:
		CMD_Start_Burning(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,52,0); }
	};
	class CMD_Start_Burning_DL : public PreformedCommand {
	public:
		CMD_Start_Burning_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,52,0); }
	};
	class CMD_Start_Burning_MD : public PreformedCommand {
	public:
		CMD_Start_Burning_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,52,0); }
	};
	class CMD_Start_Burning_Cat : public PreformedCommand {
	public:
		CMD_Start_Burning_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,52,0); }
	};
	class CMD_Abort_Burning : public PreformedCommand {
	public:
		CMD_Abort_Burning(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,53,0); }
	};
	class CMD_Abort_Burning_DL : public PreformedCommand {
	public:
		CMD_Abort_Burning_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,53,0); }
	};
	class CMD_Abort_Burning_MD : public PreformedCommand {
	public:
		CMD_Abort_Burning_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,53,0); }
	};
	class CMD_Abort_Burning_Cat : public PreformedCommand {
	public:
		CMD_Abort_Burning_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,53,0); }
	};
	class RESP_Mount_Disk_Image : public PreformedCommandResponse {
		string *m_sMediaURL;
	public:
		RESP_Mount_Disk_Image(string *sMediaURL) { 
		m_sMediaURL=sMediaURL; }
		void ParseResponse(Message *pMessage) {
			*m_sMediaURL=pMessage->m_mapParameters[59]; };
	};
	class CMD_Mount_Disk_Image : public PreformedCommand {
	public:
		CMD_Mount_Disk_Image(long DeviceIDFrom, long DeviceIDTo,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_Mount_Disk_Image_DL : public PreformedCommand {
	public:
		CMD_Mount_Disk_Image_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_Mount_Disk_Image_MD : public PreformedCommand {
	public:
		CMD_Mount_Disk_Image_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_Mount_Disk_Image_Cat : public PreformedCommand {
	public:
		CMD_Mount_Disk_Image_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_NOREP_Mount_Disk_Image : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image(long DeviceIDFrom, long DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Mount_Disk_Image_DL : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Mount_Disk_Image_MD : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Mount_Disk_Image_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_Start_Ripping_DVD : public PreformedCommand {
	public:
		CMD_Start_Ripping_DVD(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Start_Ripping_DVD_DL : public PreformedCommand {
	public:
		CMD_Start_Ripping_DVD_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Start_Ripping_DVD_MD : public PreformedCommand {
	public:
		CMD_Start_Ripping_DVD_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Start_Ripping_DVD_Cat : public PreformedCommand {
	public:
		CMD_Start_Ripping_DVD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Format_Drive : public PreformedCommand {
	public:
		CMD_Format_Drive(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,56,0); }
	};
	class CMD_Format_Drive_DL : public PreformedCommand {
	public:
		CMD_Format_Drive_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,56,0); }
	};
	class CMD_Format_Drive_MD : public PreformedCommand {
	public:
		CMD_Format_Drive_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,56,0); }
	};
	class CMD_Format_Drive_Cat : public PreformedCommand {
	public:
		CMD_Format_Drive_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,56,0); }
	};
	class CMD_Close_Tray : public PreformedCommand {
	public:
		CMD_Close_Tray(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,57,0); }
	};
	class CMD_Close_Tray_DL : public PreformedCommand {
	public:
		CMD_Close_Tray_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,57,0); }
	};
	class CMD_Close_Tray_MD : public PreformedCommand {
	public:
		CMD_Close_Tray_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,57,0); }
	};
	class CMD_Close_Tray_Cat : public PreformedCommand {
	public:
		CMD_Close_Tray_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,57,0); }
	};
	class CMD_Set_Current_User : public PreformedCommand {
	public:
		CMD_Set_Current_User(long DeviceIDFrom, long DeviceIDTo,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,58,1,17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_Current_User_DL : public PreformedCommand {
	public:
		CMD_Set_Current_User_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,58,1,17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_Current_User_MD : public PreformedCommand {
	public:
		CMD_Set_Current_User_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,58,1,17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_Current_User_Cat : public PreformedCommand {
	public:
		CMD_Set_Current_User_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,58,1,17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_Entertainment_Area : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area(long DeviceIDFrom, long DeviceIDTo,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_Set_Entertainment_Area_DL : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_Set_Entertainment_Area_MD : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_Set_Entertainment_Area_Cat : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,StringUtils::itos(iPK_EntertainArea).c_str()); }
	};
	class CMD_Link_with_mobile_orbiter : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter(long DeviceIDFrom, long DeviceIDTo,int iMediaPosition,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,2,42,StringUtils::itos(iMediaPosition).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Link_with_mobile_orbiter_DL : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter_DL(long DeviceIDFrom, string DeviceIDTo,int iMediaPosition,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,2,42,StringUtils::itos(iMediaPosition).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Link_with_mobile_orbiter_MD : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iMediaPosition,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,2,42,StringUtils::itos(iMediaPosition).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Link_with_mobile_orbiter_Cat : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iMediaPosition,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,2,42,StringUtils::itos(iMediaPosition).c_str(),47,sMac_address.c_str()); }
	};
	class RESP_Get_Signal_Strength : public PreformedCommandResponse {
		int *m_iValue;
	public:
		RESP_Get_Signal_Strength(int *iValue) { 
		m_iValue=iValue; }
		void ParseResponse(Message *pMessage) {
			*m_iValue=atoi(pMessage->m_mapParameters[48].c_str()); };
	};
	class CMD_Get_Signal_Strength : public PreformedCommand {
	public:
		CMD_Get_Signal_Strength(long DeviceIDFrom, long DeviceIDTo,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_Get_Signal_Strength_DL : public PreformedCommand {
	public:
		CMD_Get_Signal_Strength_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_Get_Signal_Strength_MD : public PreformedCommand {
	public:
		CMD_Get_Signal_Strength_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_Get_Signal_Strength_Cat : public PreformedCommand {
	public:
		CMD_Get_Signal_Strength_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_NOREP_Get_Signal_Strength : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength(long DeviceIDFrom, long DeviceIDTo,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_NOREP_Get_Signal_Strength_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_NOREP_Get_Signal_Strength_MD : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_NOREP_Get_Signal_Strength_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_Create_Mobile_Orbiter : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,2,2,StringUtils::itos(iPK_Device).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Create_Mobile_Orbiter_DL : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,2,2,StringUtils::itos(iPK_Device).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Create_Mobile_Orbiter_MD : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,2,2,StringUtils::itos(iPK_Device).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Create_Mobile_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,2,2,StringUtils::itos(iPK_Device).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Skip_Forward : public PreformedCommand {
	public:
		CMD_Skip_Forward(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Forward_DL : public PreformedCommand {
	public:
		CMD_Skip_Forward_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Forward_MD : public PreformedCommand {
	public:
		CMD_Skip_Forward_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Forward_Cat : public PreformedCommand {
	public:
		CMD_Skip_Forward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Back : public PreformedCommand {
	public:
		CMD_Skip_Back(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Skip_Back_DL : public PreformedCommand {
	public:
		CMD_Skip_Back_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Skip_Back_MD : public PreformedCommand {
	public:
		CMD_Skip_Back_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Skip_Back_Cat : public PreformedCommand {
	public:
		CMD_Skip_Back_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Jump_Position_In_Playlist : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Jump_Position_In_Playlist_DL : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Jump_Position_In_Playlist_MD : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Jump_Position_In_Playlist_Cat : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Select_Object : public PreformedCommand {
	public:
		CMD_Select_Object(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Select_Object_DL : public PreformedCommand {
	public:
		CMD_Select_Object_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Select_Object_MD : public PreformedCommand {
	public:
		CMD_Select_Object_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Select_Object_Cat : public PreformedCommand {
	public:
		CMD_Select_Object_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Spawn_Application : public PreformedCommand {
	public:
		CMD_Spawn_Application(long DeviceIDFrom, long DeviceIDTo,string sFilename,string sName,string sArguments) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,3,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str()); }
	};
	class CMD_Spawn_Application_DL : public PreformedCommand {
	public:
		CMD_Spawn_Application_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,string sName,string sArguments) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,3,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str()); }
	};
	class CMD_Spawn_Application_MD : public PreformedCommand {
	public:
		CMD_Spawn_Application_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,string sName,string sArguments) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,3,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str()); }
	};
	class CMD_Spawn_Application_Cat : public PreformedCommand {
	public:
		CMD_Spawn_Application_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,string sName,string sArguments) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,3,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str()); }
	};
	class RESP_Get_Device_Data : public PreformedCommandResponse {
		string *m_sValue_To_Assign;
	public:
		RESP_Get_Device_Data(string *sValue_To_Assign) { 
		m_sValue_To_Assign=sValue_To_Assign; }
		void ParseResponse(Message *pMessage) {
			*m_sValue_To_Assign=pMessage->m_mapParameters[5]; };
	};
	class CMD_Get_Device_Data : public PreformedCommand {
	public:
		CMD_Get_Device_Data(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"));		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_Get_Device_Data_DL : public PreformedCommand {
	public:
		CMD_Get_Device_Data_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"));		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_Get_Device_Data_MD : public PreformedCommand {
	public:
		CMD_Get_Device_Data_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"));		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_Get_Device_Data_Cat : public PreformedCommand {
	public:
		CMD_Get_Device_Data_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"));		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Device_Data : public PreformedCommand {
	public:
		CMD_NOREP_Get_Device_Data(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0")); }
	};
	class CMD_NOREP_Get_Device_Data_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Device_Data_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0")); }
	};
	class CMD_NOREP_Get_Device_Data_MD : public PreformedCommand {
	public:
		CMD_NOREP_Get_Device_Data_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,int iPK_DeviceData,bool bUseDefault) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0")); }
	};
	class CMD_NOREP_Get_Device_Data_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Device_Data_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,int iPK_DeviceData,bool bUseDefault) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0")); }
	};
	class CMD_Kill_Application : public PreformedCommand {
	public:
		CMD_Kill_Application(long DeviceIDFrom, long DeviceIDTo,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,69,1,50,sName.c_str()); }
	};
	class CMD_Kill_Application_DL : public PreformedCommand {
	public:
		CMD_Kill_Application_DL(long DeviceIDFrom, string DeviceIDTo,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,69,1,50,sName.c_str()); }
	};
	class CMD_Kill_Application_MD : public PreformedCommand {
	public:
		CMD_Kill_Application_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,69,1,50,sName.c_str()); }
	};
	class CMD_Kill_Application_Cat : public PreformedCommand {
	public:
		CMD_Kill_Application_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,69,1,50,sName.c_str()); }
	};
	class CMD_Hide_Application : public PreformedCommand {
	public:
		CMD_Hide_Application(long DeviceIDFrom, long DeviceIDTo,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,70,1,50,sName.c_str()); }
	};
	class CMD_Hide_Application_DL : public PreformedCommand {
	public:
		CMD_Hide_Application_DL(long DeviceIDFrom, string DeviceIDTo,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,70,1,50,sName.c_str()); }
	};
	class CMD_Hide_Application_MD : public PreformedCommand {
	public:
		CMD_Hide_Application_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,70,1,50,sName.c_str()); }
	};
	class CMD_Hide_Application_Cat : public PreformedCommand {
	public:
		CMD_Hide_Application_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,70,1,50,sName.c_str()); }
	};
	class RESP_Request_File : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;
	public:
		RESP_Request_File(char **pData,int *iData_Size) { 
		m_pData=pData; m_iData_Size=iData_Size; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; *m_iData_Size=pMessage->m_mapData_Lengths[19]; };
	};
	class CMD_Request_File : public PreformedCommand {
	public:
		CMD_Request_File(long DeviceIDFrom, long DeviceIDTo,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str());		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_Request_File_DL : public PreformedCommand {
	public:
		CMD_Request_File_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str());		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_Request_File_MD : public PreformedCommand {
	public:
		CMD_Request_File_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str());		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_Request_File_Cat : public PreformedCommand {
	public:
		CMD_Request_File_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str());		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_NOREP_Request_File : public PreformedCommand {
	public:
		CMD_NOREP_Request_File(long DeviceIDFrom, long DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_DL : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_MD : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str()); }
	};
	class CMD_Surrender_to_OS : public PreformedCommand {
	public:
		CMD_Surrender_to_OS(long DeviceIDFrom, long DeviceIDTo,string sOnOff,bool bFully_release_keyboard) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,72,2,8,sOnOff.c_str(),54,(bFully_release_keyboard ? "1" : "0")); }
	};
	class CMD_Surrender_to_OS_DL : public PreformedCommand {
	public:
		CMD_Surrender_to_OS_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff,bool bFully_release_keyboard) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,72,2,8,sOnOff.c_str(),54,(bFully_release_keyboard ? "1" : "0")); }
	};
	class CMD_Surrender_to_OS_MD : public PreformedCommand {
	public:
		CMD_Surrender_to_OS_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff,bool bFully_release_keyboard) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,72,2,8,sOnOff.c_str(),54,(bFully_release_keyboard ? "1" : "0")); }
	};
	class CMD_Surrender_to_OS_Cat : public PreformedCommand {
	public:
		CMD_Surrender_to_OS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff,bool bFully_release_keyboard) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,72,2,8,sOnOff.c_str(),54,(bFully_release_keyboard ? "1" : "0")); }
	};
	class CMD_MH_Send_Me_To_Remote : public PreformedCommand {
	public:
		CMD_MH_Send_Me_To_Remote(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,73,0); }
	};
	class CMD_MH_Send_Me_To_Remote_DL : public PreformedCommand {
	public:
		CMD_MH_Send_Me_To_Remote_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,73,0); }
	};
	class CMD_MH_Send_Me_To_Remote_MD : public PreformedCommand {
	public:
		CMD_MH_Send_Me_To_Remote_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,73,0); }
	};
	class CMD_MH_Send_Me_To_Remote_Cat : public PreformedCommand {
	public:
		CMD_MH_Send_Me_To_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,73,0); }
	};
	class CMD_Bind_to_Media_Remote : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,int iPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,10,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),25,StringUtils::itos(iPK_Text).c_str(),39,sOptions.c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str(),56,StringUtils::itos(iPK_Text_Timecode).c_str(),62,StringUtils::itos(iPK_Text_SectionDesc).c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Bind_to_Media_Remote_DL : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,int iPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,10,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),25,StringUtils::itos(iPK_Text).c_str(),39,sOptions.c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str(),56,StringUtils::itos(iPK_Text_Timecode).c_str(),62,StringUtils::itos(iPK_Text_SectionDesc).c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Bind_to_Media_Remote_MD : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,int iPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,10,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),25,StringUtils::itos(iPK_Text).c_str(),39,sOptions.c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str(),56,StringUtils::itos(iPK_Text_Timecode).c_str(),62,StringUtils::itos(iPK_Text_SectionDesc).c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Bind_to_Media_Remote_Cat : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,int iPK_Text,string sOptions,int iPK_EntertainArea,int iPK_Text_Timecode,int iPK_Text_SectionDesc,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,10,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),25,StringUtils::itos(iPK_Text).c_str(),39,sOptions.c_str(),45,StringUtils::itos(iPK_EntertainArea).c_str(),56,StringUtils::itos(iPK_Text_Timecode).c_str(),62,StringUtils::itos(iPK_Text_SectionDesc).c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Start_TV : public PreformedCommand {
	public:
		CMD_Start_TV(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,75,0); }
	};
	class CMD_Start_TV_DL : public PreformedCommand {
	public:
		CMD_Start_TV_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,75,0); }
	};
	class CMD_Start_TV_MD : public PreformedCommand {
	public:
		CMD_Start_TV_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,75,0); }
	};
	class CMD_Start_TV_Cat : public PreformedCommand {
	public:
		CMD_Start_TV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,75,0); }
	};
	class CMD_Stop_TV : public PreformedCommand {
	public:
		CMD_Stop_TV(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,76,0); }
	};
	class CMD_Stop_TV_DL : public PreformedCommand {
	public:
		CMD_Stop_TV_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,76,0); }
	};
	class CMD_Stop_TV_MD : public PreformedCommand {
	public:
		CMD_Stop_TV_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,76,0); }
	};
	class CMD_Stop_TV_Cat : public PreformedCommand {
	public:
		CMD_Stop_TV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,76,0); }
	};
	class CMD_Set_Current_Room : public PreformedCommand {
	public:
		CMD_Set_Current_Room(long DeviceIDFrom, long DeviceIDTo,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Current_Room_DL : public PreformedCommand {
	public:
		CMD_Set_Current_Room_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Current_Room_MD : public PreformedCommand {
	public:
		CMD_Set_Current_Room_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Current_Room_Cat : public PreformedCommand {
	public:
		CMD_Set_Current_Room_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_New_Mobile_Orbiter : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter(long DeviceIDFrom, long DeviceIDTo,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,2,44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_New_Mobile_Orbiter_DL : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,2,44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_New_Mobile_Orbiter_MD : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,2,44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_New_Mobile_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,2,44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Add_Unknown_Device : public PreformedCommand {
	public:
		CMD_Add_Unknown_Device(long DeviceIDFrom, long DeviceIDTo,string sText,string sID,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,79,3,9,sText.c_str(),10,sID.c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Add_Unknown_Device_DL : public PreformedCommand {
	public:
		CMD_Add_Unknown_Device_DL(long DeviceIDFrom, string DeviceIDTo,string sText,string sID,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,79,3,9,sText.c_str(),10,sID.c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Add_Unknown_Device_MD : public PreformedCommand {
	public:
		CMD_Add_Unknown_Device_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,string sID,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,79,3,9,sText.c_str(),10,sID.c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Add_Unknown_Device_Cat : public PreformedCommand {
	public:
		CMD_Add_Unknown_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText,string sID,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,79,3,9,sText.c_str(),10,sID.c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Send_File_To_Device : public PreformedCommand {
	public:
		CMD_Send_File_To_Device(long DeviceIDFrom, long DeviceIDTo,string sFilename,string sMac_address,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,80,3,13,sFilename.c_str(),47,sMac_address.c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Send_File_To_Device_DL : public PreformedCommand {
	public:
		CMD_Send_File_To_Device_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,string sMac_address,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,80,3,13,sFilename.c_str(),47,sMac_address.c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Send_File_To_Device_MD : public PreformedCommand {
	public:
		CMD_Send_File_To_Device_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,string sMac_address,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,80,3,13,sFilename.c_str(),47,sMac_address.c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Send_File_To_Device_Cat : public PreformedCommand {
	public:
		CMD_Send_File_To_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,string sMac_address,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,80,3,13,sFilename.c_str(),47,sMac_address.c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Navigate_Next : public PreformedCommand {
	public:
		CMD_Navigate_Next(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,81,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Next_DL : public PreformedCommand {
	public:
		CMD_Navigate_Next_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,81,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Next_MD : public PreformedCommand {
	public:
		CMD_Navigate_Next_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,81,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Next_Cat : public PreformedCommand {
	public:
		CMD_Navigate_Next_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,81,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Prev : public PreformedCommand {
	public:
		CMD_Navigate_Prev(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,82,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Prev_DL : public PreformedCommand {
	public:
		CMD_Navigate_Prev_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,82,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Prev_MD : public PreformedCommand {
	public:
		CMD_Navigate_Prev_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,82,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Prev_Cat : public PreformedCommand {
	public:
		CMD_Navigate_Prev_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,82,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Select_Current_Navigable_Area : public PreformedCommand {
	public:
		CMD_Select_Current_Navigable_Area(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,83,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Select_Current_Navigable_Area_DL : public PreformedCommand {
	public:
		CMD_Select_Current_Navigable_Area_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,83,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Select_Current_Navigable_Area_MD : public PreformedCommand {
	public:
		CMD_Select_Current_Navigable_Area_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,83,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Select_Current_Navigable_Area_Cat : public PreformedCommand {
	public:
		CMD_Select_Current_Navigable_Area_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,83,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class RESP_Get_Video_Frame : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;string *m_sFormat;
	public:
		RESP_Get_Video_Frame(char **pData,int *iData_Size,string *sFormat) { 
		m_pData=pData; m_iData_Size=iData_Size; m_sFormat=sFormat; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; *m_iData_Size=pMessage->m_mapData_Lengths[19]; *m_sFormat=pMessage->m_mapParameters[20]; };
	};
	class CMD_Get_Video_Frame : public PreformedCommand {
	public:
		CMD_Get_Video_Frame(long DeviceIDFrom, long DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_Get_Video_Frame_DL : public PreformedCommand {
	public:
		CMD_Get_Video_Frame_DL(long DeviceIDFrom, string DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_Get_Video_Frame_MD : public PreformedCommand {
	public:
		CMD_Get_Video_Frame_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_Get_Video_Frame_Cat : public PreformedCommand {
	public:
		CMD_Get_Video_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_NOREP_Get_Video_Frame : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame(long DeviceIDFrom, long DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_NOREP_Get_Video_Frame_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame_DL(long DeviceIDFrom, string DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_NOREP_Get_Video_Frame_MD : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_NOREP_Get_Video_Frame_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_Rest_Highlight : public PreformedCommand {
	public:
		CMD_Rest_Highlight(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Rest_Highlight_DL : public PreformedCommand {
	public:
		CMD_Rest_Highlight_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Rest_Highlight_MD : public PreformedCommand {
	public:
		CMD_Rest_Highlight_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Rest_Highlight_Cat : public PreformedCommand {
	public:
		CMD_Rest_Highlight_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Goto_Menu_not_used : public PreformedCommand {
	public:
		CMD_Goto_Menu_not_used(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Goto_Menu_not_used_DL : public PreformedCommand {
	public:
		CMD_Goto_Menu_not_used_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Goto_Menu_not_used_MD : public PreformedCommand {
	public:
		CMD_Goto_Menu_not_used_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Goto_Menu_not_used_Cat : public PreformedCommand {
	public:
		CMD_Goto_Menu_not_used_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Goto_Media_Menu : public PreformedCommand {
	public:
		CMD_Goto_Media_Menu(long DeviceIDFrom, long DeviceIDTo,int iStreamID,int iMenuType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,87,2,41,StringUtils::itos(iStreamID).c_str(),64,StringUtils::itos(iMenuType).c_str()); }
	};
	class CMD_Goto_Media_Menu_DL : public PreformedCommand {
	public:
		CMD_Goto_Media_Menu_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,int iMenuType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,87,2,41,StringUtils::itos(iStreamID).c_str(),64,StringUtils::itos(iMenuType).c_str()); }
	};
	class CMD_Goto_Media_Menu_MD : public PreformedCommand {
	public:
		CMD_Goto_Media_Menu_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,int iMenuType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,87,2,41,StringUtils::itos(iStreamID).c_str(),64,StringUtils::itos(iMenuType).c_str()); }
	};
	class CMD_Goto_Media_Menu_Cat : public PreformedCommand {
	public:
		CMD_Goto_Media_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,int iMenuType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,87,2,41,StringUtils::itos(iStreamID).c_str(),64,StringUtils::itos(iMenuType).c_str()); }
	};
	class CMD_Set_Current_Location : public PreformedCommand {
	public:
		CMD_Set_Current_Location(long DeviceIDFrom, long DeviceIDTo,int iLocationID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,88,1,65,StringUtils::itos(iLocationID).c_str()); }
	};
	class CMD_Set_Current_Location_DL : public PreformedCommand {
	public:
		CMD_Set_Current_Location_DL(long DeviceIDFrom, string DeviceIDTo,int iLocationID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,88,1,65,StringUtils::itos(iLocationID).c_str()); }
	};
	class CMD_Set_Current_Location_MD : public PreformedCommand {
	public:
		CMD_Set_Current_Location_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iLocationID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,88,1,65,StringUtils::itos(iLocationID).c_str()); }
	};
	class CMD_Set_Current_Location_Cat : public PreformedCommand {
	public:
		CMD_Set_Current_Location_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iLocationID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,88,1,65,StringUtils::itos(iLocationID).c_str()); }
	};
	class CMD_Vol_Up : public PreformedCommand {
	public:
		CMD_Vol_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,0); }
	};
	class CMD_Vol_Up_DL : public PreformedCommand {
	public:
		CMD_Vol_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,0); }
	};
	class CMD_Vol_Up_MD : public PreformedCommand {
	public:
		CMD_Vol_Up_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,0); }
	};
	class CMD_Vol_Up_Cat : public PreformedCommand {
	public:
		CMD_Vol_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,0); }
	};
	class CMD_Vol_Down : public PreformedCommand {
	public:
		CMD_Vol_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,0); }
	};
	class CMD_Vol_Down_DL : public PreformedCommand {
	public:
		CMD_Vol_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,0); }
	};
	class CMD_Vol_Down_MD : public PreformedCommand {
	public:
		CMD_Vol_Down_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,0); }
	};
	class CMD_Vol_Down_Cat : public PreformedCommand {
	public:
		CMD_Vol_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,0); }
	};
	class CMD_Input_Select : public PreformedCommand {
	public:
		CMD_Input_Select(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,0); }
	};
	class CMD_Input_Select_DL : public PreformedCommand {
	public:
		CMD_Input_Select_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,0); }
	};
	class CMD_Input_Select_MD : public PreformedCommand {
	public:
		CMD_Input_Select_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,0); }
	};
	class CMD_Input_Select_Cat : public PreformedCommand {
	public:
		CMD_Input_Select_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,0); }
	};
	class CMD_Pause : public PreformedCommand {
	public:
		CMD_Pause(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Pause_DL : public PreformedCommand {
	public:
		CMD_Pause_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Pause_MD : public PreformedCommand {
	public:
		CMD_Pause_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Pause_Cat : public PreformedCommand {
	public:
		CMD_Pause_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Fast_Scan_Forward : public PreformedCommand {
	public:
		CMD_Fast_Scan_Forward(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Fast_Scan_Forward_DL : public PreformedCommand {
	public:
		CMD_Fast_Scan_Forward_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Fast_Scan_Forward_MD : public PreformedCommand {
	public:
		CMD_Fast_Scan_Forward_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Fast_Scan_Forward_Cat : public PreformedCommand {
	public:
		CMD_Fast_Scan_Forward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Fast_Scan_Back : public PreformedCommand {
	public:
		CMD_Fast_Scan_Back(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,94,0); }
	};
	class CMD_Fast_Scan_Back_DL : public PreformedCommand {
	public:
		CMD_Fast_Scan_Back_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,94,0); }
	};
	class CMD_Fast_Scan_Back_MD : public PreformedCommand {
	public:
		CMD_Fast_Scan_Back_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,94,0); }
	};
	class CMD_Fast_Scan_Back_Cat : public PreformedCommand {
	public:
		CMD_Fast_Scan_Back_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,94,0); }
	};
	class CMD_Stop : public PreformedCommand {
	public:
		CMD_Stop(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,95,0); }
	};
	class CMD_Stop_DL : public PreformedCommand {
	public:
		CMD_Stop_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,95,0); }
	};
	class CMD_Stop_MD : public PreformedCommand {
	public:
		CMD_Stop_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,95,0); }
	};
	class CMD_Stop_Cat : public PreformedCommand {
	public:
		CMD_Stop_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,95,0); }
	};
	class CMD_Change_Disc : public PreformedCommand {
	public:
		CMD_Change_Disc(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,96,0); }
	};
	class CMD_Change_Disc_DL : public PreformedCommand {
	public:
		CMD_Change_Disc_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,96,0); }
	};
	class CMD_Change_Disc_MD : public PreformedCommand {
	public:
		CMD_Change_Disc_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,96,0); }
	};
	class CMD_Change_Disc_Cat : public PreformedCommand {
	public:
		CMD_Change_Disc_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,96,0); }
	};
	class CMD_Mute : public PreformedCommand {
	public:
		CMD_Mute(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,97,0); }
	};
	class CMD_Mute_DL : public PreformedCommand {
	public:
		CMD_Mute_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,97,0); }
	};
	class CMD_Mute_MD : public PreformedCommand {
	public:
		CMD_Mute_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,97,0); }
	};
	class CMD_Mute_Cat : public PreformedCommand {
	public:
		CMD_Mute_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,97,0); }
	};
	class CMD_Slow_Scan_Back : public PreformedCommand {
	public:
		CMD_Slow_Scan_Back(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,98,0); }
	};
	class CMD_Slow_Scan_Back_DL : public PreformedCommand {
	public:
		CMD_Slow_Scan_Back_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,98,0); }
	};
	class CMD_Slow_Scan_Back_MD : public PreformedCommand {
	public:
		CMD_Slow_Scan_Back_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,98,0); }
	};
	class CMD_Slow_Scan_Back_Cat : public PreformedCommand {
	public:
		CMD_Slow_Scan_Back_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,98,0); }
	};
	class CMD_Slow_Scan_Forward : public PreformedCommand {
	public:
		CMD_Slow_Scan_Forward(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,99,0); }
	};
	class CMD_Slow_Scan_Forward_DL : public PreformedCommand {
	public:
		CMD_Slow_Scan_Forward_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,99,0); }
	};
	class CMD_Slow_Scan_Forward_MD : public PreformedCommand {
	public:
		CMD_Slow_Scan_Forward_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,99,0); }
	};
	class CMD_Slow_Scan_Forward_Cat : public PreformedCommand {
	public:
		CMD_Slow_Scan_Forward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,99,0); }
	};
	class CMD_Step_Forward : public PreformedCommand {
	public:
		CMD_Step_Forward(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,100,0); }
	};
	class CMD_Step_Forward_DL : public PreformedCommand {
	public:
		CMD_Step_Forward_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,100,0); }
	};
	class CMD_Step_Forward_MD : public PreformedCommand {
	public:
		CMD_Step_Forward_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,100,0); }
	};
	class CMD_Step_Forward_Cat : public PreformedCommand {
	public:
		CMD_Step_Forward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,100,0); }
	};
	class CMD_Step_Back : public PreformedCommand {
	public:
		CMD_Step_Back(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,101,0); }
	};
	class CMD_Step_Back_DL : public PreformedCommand {
	public:
		CMD_Step_Back_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,101,0); }
	};
	class CMD_Step_Back_MD : public PreformedCommand {
	public:
		CMD_Step_Back_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,101,0); }
	};
	class CMD_Step_Back_Cat : public PreformedCommand {
	public:
		CMD_Step_Back_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,101,0); }
	};
	class CMD_Record : public PreformedCommand {
	public:
		CMD_Record(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,102,0); }
	};
	class CMD_Record_DL : public PreformedCommand {
	public:
		CMD_Record_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,102,0); }
	};
	class CMD_Record_MD : public PreformedCommand {
	public:
		CMD_Record_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,102,0); }
	};
	class CMD_Record_Cat : public PreformedCommand {
	public:
		CMD_Record_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,102,0); }
	};
	class CMD_Random : public PreformedCommand {
	public:
		CMD_Random(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,103,0); }
	};
	class CMD_Random_DL : public PreformedCommand {
	public:
		CMD_Random_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,103,0); }
	};
	class CMD_Random_MD : public PreformedCommand {
	public:
		CMD_Random_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,103,0); }
	};
	class CMD_Random_Cat : public PreformedCommand {
	public:
		CMD_Random_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,103,0); }
	};
	class CMD_Repeat : public PreformedCommand {
	public:
		CMD_Repeat(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,104,0); }
	};
	class CMD_Repeat_DL : public PreformedCommand {
	public:
		CMD_Repeat_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,104,0); }
	};
	class CMD_Repeat_MD : public PreformedCommand {
	public:
		CMD_Repeat_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,104,0); }
	};
	class CMD_Repeat_Cat : public PreformedCommand {
	public:
		CMD_Repeat_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,104,0); }
	};
	class CMD_Fwd_14x : public PreformedCommand {
	public:
		CMD_Fwd_14x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,105,0); }
	};
	class CMD_Fwd_14x_DL : public PreformedCommand {
	public:
		CMD_Fwd_14x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,105,0); }
	};
	class CMD_Fwd_14x_MD : public PreformedCommand {
	public:
		CMD_Fwd_14x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,105,0); }
	};
	class CMD_Fwd_14x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_14x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,105,0); }
	};
	class CMD_Fwd_12x : public PreformedCommand {
	public:
		CMD_Fwd_12x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,106,0); }
	};
	class CMD_Fwd_12x_DL : public PreformedCommand {
	public:
		CMD_Fwd_12x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,106,0); }
	};
	class CMD_Fwd_12x_MD : public PreformedCommand {
	public:
		CMD_Fwd_12x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,106,0); }
	};
	class CMD_Fwd_12x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_12x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,106,0); }
	};
	class CMD_Fwd_2x : public PreformedCommand {
	public:
		CMD_Fwd_2x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,107,0); }
	};
	class CMD_Fwd_2x_DL : public PreformedCommand {
	public:
		CMD_Fwd_2x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,107,0); }
	};
	class CMD_Fwd_2x_MD : public PreformedCommand {
	public:
		CMD_Fwd_2x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,107,0); }
	};
	class CMD_Fwd_2x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_2x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,107,0); }
	};
	class CMD_Fwd_4x : public PreformedCommand {
	public:
		CMD_Fwd_4x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,108,0); }
	};
	class CMD_Fwd_4x_DL : public PreformedCommand {
	public:
		CMD_Fwd_4x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,108,0); }
	};
	class CMD_Fwd_4x_MD : public PreformedCommand {
	public:
		CMD_Fwd_4x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,108,0); }
	};
	class CMD_Fwd_4x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_4x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,108,0); }
	};
	class CMD_Fwd_8x : public PreformedCommand {
	public:
		CMD_Fwd_8x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,109,0); }
	};
	class CMD_Fwd_8x_DL : public PreformedCommand {
	public:
		CMD_Fwd_8x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,109,0); }
	};
	class CMD_Fwd_8x_MD : public PreformedCommand {
	public:
		CMD_Fwd_8x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,109,0); }
	};
	class CMD_Fwd_8x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_8x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,109,0); }
	};
	class CMD_Back_14x : public PreformedCommand {
	public:
		CMD_Back_14x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,110,0); }
	};
	class CMD_Back_14x_DL : public PreformedCommand {
	public:
		CMD_Back_14x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,110,0); }
	};
	class CMD_Back_14x_MD : public PreformedCommand {
	public:
		CMD_Back_14x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,110,0); }
	};
	class CMD_Back_14x_Cat : public PreformedCommand {
	public:
		CMD_Back_14x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,110,0); }
	};
	class CMD_Back_12x : public PreformedCommand {
	public:
		CMD_Back_12x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,111,0); }
	};
	class CMD_Back_12x_DL : public PreformedCommand {
	public:
		CMD_Back_12x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,111,0); }
	};
	class CMD_Back_12x_MD : public PreformedCommand {
	public:
		CMD_Back_12x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,111,0); }
	};
	class CMD_Back_12x_Cat : public PreformedCommand {
	public:
		CMD_Back_12x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,111,0); }
	};
	class CMD_Back_1x : public PreformedCommand {
	public:
		CMD_Back_1x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,112,0); }
	};
	class CMD_Back_1x_DL : public PreformedCommand {
	public:
		CMD_Back_1x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,112,0); }
	};
	class CMD_Back_1x_MD : public PreformedCommand {
	public:
		CMD_Back_1x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,112,0); }
	};
	class CMD_Back_1x_Cat : public PreformedCommand {
	public:
		CMD_Back_1x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,112,0); }
	};
	class CMD_Back_2x : public PreformedCommand {
	public:
		CMD_Back_2x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,113,0); }
	};
	class CMD_Back_2x_DL : public PreformedCommand {
	public:
		CMD_Back_2x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,113,0); }
	};
	class CMD_Back_2x_MD : public PreformedCommand {
	public:
		CMD_Back_2x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,113,0); }
	};
	class CMD_Back_2x_Cat : public PreformedCommand {
	public:
		CMD_Back_2x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,113,0); }
	};
	class CMD_Back_4x : public PreformedCommand {
	public:
		CMD_Back_4x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,114,0); }
	};
	class CMD_Back_4x_DL : public PreformedCommand {
	public:
		CMD_Back_4x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,114,0); }
	};
	class CMD_Back_4x_MD : public PreformedCommand {
	public:
		CMD_Back_4x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,114,0); }
	};
	class CMD_Back_4x_Cat : public PreformedCommand {
	public:
		CMD_Back_4x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,114,0); }
	};
	class CMD_Back_8x : public PreformedCommand {
	public:
		CMD_Back_8x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,115,0); }
	};
	class CMD_Back_8x_DL : public PreformedCommand {
	public:
		CMD_Back_8x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,115,0); }
	};
	class CMD_Back_8x_MD : public PreformedCommand {
	public:
		CMD_Back_8x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,115,0); }
	};
	class CMD_Back_8x_Cat : public PreformedCommand {
	public:
		CMD_Back_8x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,115,0); }
	};
	class CMD_DSP_Mode : public PreformedCommand {
	public:
		CMD_DSP_Mode(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,116,0); }
	};
	class CMD_DSP_Mode_DL : public PreformedCommand {
	public:
		CMD_DSP_Mode_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,116,0); }
	};
	class CMD_DSP_Mode_MD : public PreformedCommand {
	public:
		CMD_DSP_Mode_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,116,0); }
	};
	class CMD_DSP_Mode_Cat : public PreformedCommand {
	public:
		CMD_DSP_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,116,0); }
	};
	class CMD_Skip_To_End : public PreformedCommand {
	public:
		CMD_Skip_To_End(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,117,0); }
	};
	class CMD_Skip_To_End_DL : public PreformedCommand {
	public:
		CMD_Skip_To_End_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,117,0); }
	};
	class CMD_Skip_To_End_MD : public PreformedCommand {
	public:
		CMD_Skip_To_End_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,117,0); }
	};
	class CMD_Skip_To_End_Cat : public PreformedCommand {
	public:
		CMD_Skip_To_End_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,117,0); }
	};
	class CMD_Source : public PreformedCommand {
	public:
		CMD_Source(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,118,0); }
	};
	class CMD_Source_DL : public PreformedCommand {
	public:
		CMD_Source_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,118,0); }
	};
	class CMD_Source_MD : public PreformedCommand {
	public:
		CMD_Source_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,118,0); }
	};
	class CMD_Source_Cat : public PreformedCommand {
	public:
		CMD_Source_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,118,0); }
	};
	class CMD_Back_16x : public PreformedCommand {
	public:
		CMD_Back_16x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,119,0); }
	};
	class CMD_Back_16x_DL : public PreformedCommand {
	public:
		CMD_Back_16x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,119,0); }
	};
	class CMD_Back_16x_MD : public PreformedCommand {
	public:
		CMD_Back_16x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,119,0); }
	};
	class CMD_Back_16x_Cat : public PreformedCommand {
	public:
		CMD_Back_16x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,119,0); }
	};
	class CMD_Back_32X : public PreformedCommand {
	public:
		CMD_Back_32X(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,120,0); }
	};
	class CMD_Back_32X_DL : public PreformedCommand {
	public:
		CMD_Back_32X_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,120,0); }
	};
	class CMD_Back_32X_MD : public PreformedCommand {
	public:
		CMD_Back_32X_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,120,0); }
	};
	class CMD_Back_32X_Cat : public PreformedCommand {
	public:
		CMD_Back_32X_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,120,0); }
	};
	class CMD_Fwd_16x : public PreformedCommand {
	public:
		CMD_Fwd_16x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,121,0); }
	};
	class CMD_Fwd_16x_DL : public PreformedCommand {
	public:
		CMD_Fwd_16x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,121,0); }
	};
	class CMD_Fwd_16x_MD : public PreformedCommand {
	public:
		CMD_Fwd_16x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,121,0); }
	};
	class CMD_Fwd_16x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_16x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,121,0); }
	};
	class CMD_Fwd_32x : public PreformedCommand {
	public:
		CMD_Fwd_32x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,122,0); }
	};
	class CMD_Fwd_32x_DL : public PreformedCommand {
	public:
		CMD_Fwd_32x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,122,0); }
	};
	class CMD_Fwd_32x_MD : public PreformedCommand {
	public:
		CMD_Fwd_32x_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,122,0); }
	};
	class CMD_Fwd_32x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_32x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,122,0); }
	};
	class CMD_Info : public PreformedCommand {
	public:
		CMD_Info(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,0); }
	};
	class CMD_Info_DL : public PreformedCommand {
	public:
		CMD_Info_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,0); }
	};
	class CMD_Info_MD : public PreformedCommand {
	public:
		CMD_Info_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,0); }
	};
	class CMD_Info_Cat : public PreformedCommand {
	public:
		CMD_Info_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,0); }
	};
	class CMD_Favorites : public PreformedCommand {
	public:
		CMD_Favorites(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Favorites_DL : public PreformedCommand {
	public:
		CMD_Favorites_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Favorites_MD : public PreformedCommand {
	public:
		CMD_Favorites_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Favorites_Cat : public PreformedCommand {
	public:
		CMD_Favorites_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Back_AV : public PreformedCommand {
	public:
		CMD_Back_AV(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Back_AV_DL : public PreformedCommand {
	public:
		CMD_Back_AV_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Back_AV_MD : public PreformedCommand {
	public:
		CMD_Back_AV_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Back_AV_Cat : public PreformedCommand {
	public:
		CMD_Back_AV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Guide : public PreformedCommand {
	public:
		CMD_Guide(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,126,0); }
	};
	class CMD_Guide_DL : public PreformedCommand {
	public:
		CMD_Guide_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,126,0); }
	};
	class CMD_Guide_MD : public PreformedCommand {
	public:
		CMD_Guide_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,126,0); }
	};
	class CMD_Guide_Cat : public PreformedCommand {
	public:
		CMD_Guide_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,126,0); }
	};
	class CMD_PIP_ON : public PreformedCommand {
	public:
		CMD_PIP_ON(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,127,0); }
	};
	class CMD_PIP_ON_DL : public PreformedCommand {
	public:
		CMD_PIP_ON_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,127,0); }
	};
	class CMD_PIP_ON_MD : public PreformedCommand {
	public:
		CMD_PIP_ON_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,127,0); }
	};
	class CMD_PIP_ON_Cat : public PreformedCommand {
	public:
		CMD_PIP_ON_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,127,0); }
	};
	class CMD_PIP_OFF : public PreformedCommand {
	public:
		CMD_PIP_OFF(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,128,0); }
	};
	class CMD_PIP_OFF_DL : public PreformedCommand {
	public:
		CMD_PIP_OFF_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,128,0); }
	};
	class CMD_PIP_OFF_MD : public PreformedCommand {
	public:
		CMD_PIP_OFF_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,128,0); }
	};
	class CMD_PIP_OFF_Cat : public PreformedCommand {
	public:
		CMD_PIP_OFF_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,128,0); }
	};
	class CMD_PIP_Channel_Up : public PreformedCommand {
	public:
		CMD_PIP_Channel_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,129,0); }
	};
	class CMD_PIP_Channel_Up_DL : public PreformedCommand {
	public:
		CMD_PIP_Channel_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,129,0); }
	};
	class CMD_PIP_Channel_Up_MD : public PreformedCommand {
	public:
		CMD_PIP_Channel_Up_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,129,0); }
	};
	class CMD_PIP_Channel_Up_Cat : public PreformedCommand {
	public:
		CMD_PIP_Channel_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,129,0); }
	};
	class CMD_PIP_Channel_Down : public PreformedCommand {
	public:
		CMD_PIP_Channel_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,130,0); }
	};
	class CMD_PIP_Channel_Down_DL : public PreformedCommand {
	public:
		CMD_PIP_Channel_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,130,0); }
	};
	class CMD_PIP_Channel_Down_MD : public PreformedCommand {
	public:
		CMD_PIP_Channel_Down_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,130,0); }
	};
	class CMD_PIP_Channel_Down_Cat : public PreformedCommand {
	public:
		CMD_PIP_Channel_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,130,0); }
	};
	class CMD_PIP_Channel_Number : public PreformedCommand {
	public:
		CMD_PIP_Channel_Number(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,131,0); }
	};
	class CMD_PIP_Channel_Number_DL : public PreformedCommand {
	public:
		CMD_PIP_Channel_Number_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,131,0); }
	};
	class CMD_PIP_Channel_Number_MD : public PreformedCommand {
	public:
		CMD_PIP_Channel_Number_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,131,0); }
	};
	class CMD_PIP_Channel_Number_Cat : public PreformedCommand {
	public:
		CMD_PIP_Channel_Number_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,131,0); }
	};
	class CMD_Taller : public PreformedCommand {
	public:
		CMD_Taller(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,132,0); }
	};
	class CMD_Taller_DL : public PreformedCommand {
	public:
		CMD_Taller_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,132,0); }
	};
	class CMD_Taller_MD : public PreformedCommand {
	public:
		CMD_Taller_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,132,0); }
	};
	class CMD_Taller_Cat : public PreformedCommand {
	public:
		CMD_Taller_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,132,0); }
	};
	class CMD_Shorter : public PreformedCommand {
	public:
		CMD_Shorter(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,133,0); }
	};
	class CMD_Shorter_DL : public PreformedCommand {
	public:
		CMD_Shorter_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,133,0); }
	};
	class CMD_Shorter_MD : public PreformedCommand {
	public:
		CMD_Shorter_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,133,0); }
	};
	class CMD_Shorter_Cat : public PreformedCommand {
	public:
		CMD_Shorter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,133,0); }
	};
	class CMD_Wider : public PreformedCommand {
	public:
		CMD_Wider(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,134,0); }
	};
	class CMD_Wider_DL : public PreformedCommand {
	public:
		CMD_Wider_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,134,0); }
	};
	class CMD_Wider_MD : public PreformedCommand {
	public:
		CMD_Wider_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,134,0); }
	};
	class CMD_Wider_Cat : public PreformedCommand {
	public:
		CMD_Wider_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,134,0); }
	};
	class CMD_Narrower : public PreformedCommand {
	public:
		CMD_Narrower(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,135,0); }
	};
	class CMD_Narrower_DL : public PreformedCommand {
	public:
		CMD_Narrower_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,135,0); }
	};
	class CMD_Narrower_MD : public PreformedCommand {
	public:
		CMD_Narrower_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,135,0); }
	};
	class CMD_Narrower_Cat : public PreformedCommand {
	public:
		CMD_Narrower_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,135,0); }
	};
	class CMD_Previous_Channel : public PreformedCommand {
	public:
		CMD_Previous_Channel(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,136,0); }
	};
	class CMD_Previous_Channel_DL : public PreformedCommand {
	public:
		CMD_Previous_Channel_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,136,0); }
	};
	class CMD_Previous_Channel_MD : public PreformedCommand {
	public:
		CMD_Previous_Channel_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,136,0); }
	};
	class CMD_Previous_Channel_Cat : public PreformedCommand {
	public:
		CMD_Previous_Channel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,136,0); }
	};
	class CMD_Music : public PreformedCommand {
	public:
		CMD_Music(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,137,0); }
	};
	class CMD_Music_DL : public PreformedCommand {
	public:
		CMD_Music_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,137,0); }
	};
	class CMD_Music_MD : public PreformedCommand {
	public:
		CMD_Music_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,137,0); }
	};
	class CMD_Music_Cat : public PreformedCommand {
	public:
		CMD_Music_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,137,0); }
	};
	class CMD_Eject : public PreformedCommand {
	public:
		CMD_Eject(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,138,0); }
	};
	class CMD_Eject_DL : public PreformedCommand {
	public:
		CMD_Eject_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,138,0); }
	};
	class CMD_Eject_MD : public PreformedCommand {
	public:
		CMD_Eject_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,138,0); }
	};
	class CMD_Eject_Cat : public PreformedCommand {
	public:
		CMD_Eject_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,138,0); }
	};
	class CMD_Play : public PreformedCommand {
	public:
		CMD_Play(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,139,0); }
	};
	class CMD_Play_DL : public PreformedCommand {
	public:
		CMD_Play_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,139,0); }
	};
	class CMD_Play_MD : public PreformedCommand {
	public:
		CMD_Play_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,139,0); }
	};
	class CMD_Play_Cat : public PreformedCommand {
	public:
		CMD_Play_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,139,0); }
	};
	class CMD_Audio_Track : public PreformedCommand {
	public:
		CMD_Audio_Track(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,0); }
	};
	class CMD_Audio_Track_DL : public PreformedCommand {
	public:
		CMD_Audio_Track_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,0); }
	};
	class CMD_Audio_Track_MD : public PreformedCommand {
	public:
		CMD_Audio_Track_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,0); }
	};
	class CMD_Audio_Track_Cat : public PreformedCommand {
	public:
		CMD_Audio_Track_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,0); }
	};
	class CMD_Subtitle : public PreformedCommand {
	public:
		CMD_Subtitle(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,0); }
	};
	class CMD_Subtitle_DL : public PreformedCommand {
	public:
		CMD_Subtitle_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,0); }
	};
	class CMD_Subtitle_MD : public PreformedCommand {
	public:
		CMD_Subtitle_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,0); }
	};
	class CMD_Subtitle_Cat : public PreformedCommand {
	public:
		CMD_Subtitle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,0); }
	};
	class CMD_Angle : public PreformedCommand {
	public:
		CMD_Angle(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,0); }
	};
	class CMD_Angle_DL : public PreformedCommand {
	public:
		CMD_Angle_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,0); }
	};
	class CMD_Angle_MD : public PreformedCommand {
	public:
		CMD_Angle_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,0); }
	};
	class CMD_Angle_Cat : public PreformedCommand {
	public:
		CMD_Angle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,0); }
	};
	class CMD_Root_Menu : public PreformedCommand {
	public:
		CMD_Root_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,143,0); }
	};
	class CMD_Root_Menu_DL : public PreformedCommand {
	public:
		CMD_Root_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,143,0); }
	};
	class CMD_Root_Menu_MD : public PreformedCommand {
	public:
		CMD_Root_Menu_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,143,0); }
	};
	class CMD_Root_Menu_Cat : public PreformedCommand {
	public:
		CMD_Root_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,143,0); }
	};
	class CMD_Progressive : public PreformedCommand {
	public:
		CMD_Progressive(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,144,0); }
	};
	class CMD_Progressive_DL : public PreformedCommand {
	public:
		CMD_Progressive_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,144,0); }
	};
	class CMD_Progressive_MD : public PreformedCommand {
	public:
		CMD_Progressive_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,144,0); }
	};
	class CMD_Progressive_Cat : public PreformedCommand {
	public:
		CMD_Progressive_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,144,0); }
	};
	class CMD_Back_10_seconds : public PreformedCommand {
	public:
		CMD_Back_10_seconds(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,145,0); }
	};
	class CMD_Back_10_seconds_DL : public PreformedCommand {
	public:
		CMD_Back_10_seconds_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,145,0); }
	};
	class CMD_Back_10_seconds_MD : public PreformedCommand {
	public:
		CMD_Back_10_seconds_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,145,0); }
	};
	class CMD_Back_10_seconds_Cat : public PreformedCommand {
	public:
		CMD_Back_10_seconds_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,145,0); }
	};
	class CMD_Multi_Channel : public PreformedCommand {
	public:
		CMD_Multi_Channel(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,146,0); }
	};
	class CMD_Multi_Channel_DL : public PreformedCommand {
	public:
		CMD_Multi_Channel_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,146,0); }
	};
	class CMD_Multi_Channel_MD : public PreformedCommand {
	public:
		CMD_Multi_Channel_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,146,0); }
	};
	class CMD_Multi_Channel_Cat : public PreformedCommand {
	public:
		CMD_Multi_Channel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,146,0); }
	};
	class CMD_Hall : public PreformedCommand {
	public:
		CMD_Hall(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,147,0); }
	};
	class CMD_Hall_DL : public PreformedCommand {
	public:
		CMD_Hall_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,147,0); }
	};
	class CMD_Hall_MD : public PreformedCommand {
	public:
		CMD_Hall_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,147,0); }
	};
	class CMD_Hall_Cat : public PreformedCommand {
	public:
		CMD_Hall_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,147,0); }
	};
	class CMD_Church : public PreformedCommand {
	public:
		CMD_Church(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,148,0); }
	};
	class CMD_Church_DL : public PreformedCommand {
	public:
		CMD_Church_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,148,0); }
	};
	class CMD_Church_MD : public PreformedCommand {
	public:
		CMD_Church_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,148,0); }
	};
	class CMD_Church_Cat : public PreformedCommand {
	public:
		CMD_Church_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,148,0); }
	};
	class CMD_Jazz_Club : public PreformedCommand {
	public:
		CMD_Jazz_Club(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,149,0); }
	};
	class CMD_Jazz_Club_DL : public PreformedCommand {
	public:
		CMD_Jazz_Club_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,149,0); }
	};
	class CMD_Jazz_Club_MD : public PreformedCommand {
	public:
		CMD_Jazz_Club_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,149,0); }
	};
	class CMD_Jazz_Club_Cat : public PreformedCommand {
	public:
		CMD_Jazz_Club_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,149,0); }
	};
	class CMD_Rock_Concert : public PreformedCommand {
	public:
		CMD_Rock_Concert(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,150,0); }
	};
	class CMD_Rock_Concert_DL : public PreformedCommand {
	public:
		CMD_Rock_Concert_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,150,0); }
	};
	class CMD_Rock_Concert_MD : public PreformedCommand {
	public:
		CMD_Rock_Concert_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,150,0); }
	};
	class CMD_Rock_Concert_Cat : public PreformedCommand {
	public:
		CMD_Rock_Concert_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,150,0); }
	};
	class CMD_Entertainment : public PreformedCommand {
	public:
		CMD_Entertainment(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,151,0); }
	};
	class CMD_Entertainment_DL : public PreformedCommand {
	public:
		CMD_Entertainment_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,151,0); }
	};
	class CMD_Entertainment_MD : public PreformedCommand {
	public:
		CMD_Entertainment_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,151,0); }
	};
	class CMD_Entertainment_Cat : public PreformedCommand {
	public:
		CMD_Entertainment_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,151,0); }
	};
	class CMD_Music_Video : public PreformedCommand {
	public:
		CMD_Music_Video(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,152,0); }
	};
	class CMD_Music_Video_DL : public PreformedCommand {
	public:
		CMD_Music_Video_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,152,0); }
	};
	class CMD_Music_Video_MD : public PreformedCommand {
	public:
		CMD_Music_Video_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,152,0); }
	};
	class CMD_Music_Video_Cat : public PreformedCommand {
	public:
		CMD_Music_Video_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,152,0); }
	};
	class CMD_Classical : public PreformedCommand {
	public:
		CMD_Classical(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,153,0); }
	};
	class CMD_Classical_DL : public PreformedCommand {
	public:
		CMD_Classical_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,153,0); }
	};
	class CMD_Classical_MD : public PreformedCommand {
	public:
		CMD_Classical_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,153,0); }
	};
	class CMD_Classical_Cat : public PreformedCommand {
	public:
		CMD_Classical_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,153,0); }
	};
	class CMD_RGB : public PreformedCommand {
	public:
		CMD_RGB(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,154,0); }
	};
	class CMD_RGB_DL : public PreformedCommand {
	public:
		CMD_RGB_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,154,0); }
	};
	class CMD_RGB_MD : public PreformedCommand {
	public:
		CMD_RGB_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,154,0); }
	};
	class CMD_RGB_Cat : public PreformedCommand {
	public:
		CMD_RGB_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,154,0); }
	};
	class CMD_Video_1 : public PreformedCommand {
	public:
		CMD_Video_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,155,0); }
	};
	class CMD_Video_1_DL : public PreformedCommand {
	public:
		CMD_Video_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,155,0); }
	};
	class CMD_Video_1_MD : public PreformedCommand {
	public:
		CMD_Video_1_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,155,0); }
	};
	class CMD_Video_1_Cat : public PreformedCommand {
	public:
		CMD_Video_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,155,0); }
	};
	class CMD_Video_2 : public PreformedCommand {
	public:
		CMD_Video_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,156,0); }
	};
	class CMD_Video_2_DL : public PreformedCommand {
	public:
		CMD_Video_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,156,0); }
	};
	class CMD_Video_2_MD : public PreformedCommand {
	public:
		CMD_Video_2_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,156,0); }
	};
	class CMD_Video_2_Cat : public PreformedCommand {
	public:
		CMD_Video_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,156,0); }
	};
	class CMD_Video_3 : public PreformedCommand {
	public:
		CMD_Video_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,157,0); }
	};
	class CMD_Video_3_DL : public PreformedCommand {
	public:
		CMD_Video_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,157,0); }
	};
	class CMD_Video_3_MD : public PreformedCommand {
	public:
		CMD_Video_3_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,157,0); }
	};
	class CMD_Video_3_Cat : public PreformedCommand {
	public:
		CMD_Video_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,157,0); }
	};
	class CMD_Video_4 : public PreformedCommand {
	public:
		CMD_Video_4(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,158,0); }
	};
	class CMD_Video_4_DL : public PreformedCommand {
	public:
		CMD_Video_4_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,158,0); }
	};
	class CMD_Video_4_MD : public PreformedCommand {
	public:
		CMD_Video_4_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,158,0); }
	};
	class CMD_Video_4_Cat : public PreformedCommand {
	public:
		CMD_Video_4_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,158,0); }
	};
	class CMD_Video_5 : public PreformedCommand {
	public:
		CMD_Video_5(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,159,0); }
	};
	class CMD_Video_5_DL : public PreformedCommand {
	public:
		CMD_Video_5_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,159,0); }
	};
	class CMD_Video_5_MD : public PreformedCommand {
	public:
		CMD_Video_5_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,159,0); }
	};
	class CMD_Video_5_Cat : public PreformedCommand {
	public:
		CMD_Video_5_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,159,0); }
	};
	class CMD_Tape : public PreformedCommand {
	public:
		CMD_Tape(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_Tape_DL : public PreformedCommand {
	public:
		CMD_Tape_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_Tape_MD : public PreformedCommand {
	public:
		CMD_Tape_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_Tape_Cat : public PreformedCommand {
	public:
		CMD_Tape_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_TV : public PreformedCommand {
	public:
		CMD_TV(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,161,0); }
	};
	class CMD_TV_DL : public PreformedCommand {
	public:
		CMD_TV_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,161,0); }
	};
	class CMD_TV_MD : public PreformedCommand {
	public:
		CMD_TV_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,161,0); }
	};
	class CMD_TV_Cat : public PreformedCommand {
	public:
		CMD_TV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,161,0); }
	};
	class CMD_CD : public PreformedCommand {
	public:
		CMD_CD(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,162,0); }
	};
	class CMD_CD_DL : public PreformedCommand {
	public:
		CMD_CD_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,162,0); }
	};
	class CMD_CD_MD : public PreformedCommand {
	public:
		CMD_CD_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,162,0); }
	};
	class CMD_CD_Cat : public PreformedCommand {
	public:
		CMD_CD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,162,0); }
	};
	class CMD_Phono : public PreformedCommand {
	public:
		CMD_Phono(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,163,0); }
	};
	class CMD_Phono_DL : public PreformedCommand {
	public:
		CMD_Phono_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,163,0); }
	};
	class CMD_Phono_MD : public PreformedCommand {
	public:
		CMD_Phono_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,163,0); }
	};
	class CMD_Phono_Cat : public PreformedCommand {
	public:
		CMD_Phono_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,163,0); }
	};
	class CMD_Aux : public PreformedCommand {
	public:
		CMD_Aux(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,164,0); }
	};
	class CMD_Aux_DL : public PreformedCommand {
	public:
		CMD_Aux_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,164,0); }
	};
	class CMD_Aux_MD : public PreformedCommand {
	public:
		CMD_Aux_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,164,0); }
	};
	class CMD_Aux_Cat : public PreformedCommand {
	public:
		CMD_Aux_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,164,0); }
	};
	class CMD_DVD : public PreformedCommand {
	public:
		CMD_DVD(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,165,0); }
	};
	class CMD_DVD_DL : public PreformedCommand {
	public:
		CMD_DVD_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,165,0); }
	};
	class CMD_DVD_MD : public PreformedCommand {
	public:
		CMD_DVD_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,165,0); }
	};
	class CMD_DVD_Cat : public PreformedCommand {
	public:
		CMD_DVD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,165,0); }
	};
	class CMD_Tuner : public PreformedCommand {
	public:
		CMD_Tuner(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,166,0); }
	};
	class CMD_Tuner_DL : public PreformedCommand {
	public:
		CMD_Tuner_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,166,0); }
	};
	class CMD_Tuner_MD : public PreformedCommand {
	public:
		CMD_Tuner_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,166,0); }
	};
	class CMD_Tuner_Cat : public PreformedCommand {
	public:
		CMD_Tuner_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,166,0); }
	};
	class CMD_Video_6 : public PreformedCommand {
	public:
		CMD_Video_6(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,167,0); }
	};
	class CMD_Video_6_DL : public PreformedCommand {
	public:
		CMD_Video_6_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,167,0); }
	};
	class CMD_Video_6_MD : public PreformedCommand {
	public:
		CMD_Video_6_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,167,0); }
	};
	class CMD_Video_6_Cat : public PreformedCommand {
	public:
		CMD_Video_6_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,167,0); }
	};
	class CMD_Video_7 : public PreformedCommand {
	public:
		CMD_Video_7(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,168,0); }
	};
	class CMD_Video_7_DL : public PreformedCommand {
	public:
		CMD_Video_7_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,168,0); }
	};
	class CMD_Video_7_MD : public PreformedCommand {
	public:
		CMD_Video_7_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,168,0); }
	};
	class CMD_Video_7_Cat : public PreformedCommand {
	public:
		CMD_Video_7_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,168,0); }
	};
	class CMD_AM : public PreformedCommand {
	public:
		CMD_AM(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,169,0); }
	};
	class CMD_AM_DL : public PreformedCommand {
	public:
		CMD_AM_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,169,0); }
	};
	class CMD_AM_MD : public PreformedCommand {
	public:
		CMD_AM_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,169,0); }
	};
	class CMD_AM_Cat : public PreformedCommand {
	public:
		CMD_AM_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,169,0); }
	};
	class CMD_FM : public PreformedCommand {
	public:
		CMD_FM(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,170,0); }
	};
	class CMD_FM_DL : public PreformedCommand {
	public:
		CMD_FM_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,170,0); }
	};
	class CMD_FM_MD : public PreformedCommand {
	public:
		CMD_FM_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,170,0); }
	};
	class CMD_FM_Cat : public PreformedCommand {
	public:
		CMD_FM_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,170,0); }
	};
	class CMD_DVD_Multi : public PreformedCommand {
	public:
		CMD_DVD_Multi(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,171,0); }
	};
	class CMD_DVD_Multi_DL : public PreformedCommand {
	public:
		CMD_DVD_Multi_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,171,0); }
	};
	class CMD_DVD_Multi_MD : public PreformedCommand {
	public:
		CMD_DVD_Multi_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,171,0); }
	};
	class CMD_DVD_Multi_Cat : public PreformedCommand {
	public:
		CMD_DVD_Multi_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,171,0); }
	};
	class CMD_Aux2 : public PreformedCommand {
	public:
		CMD_Aux2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Aux2_DL : public PreformedCommand {
	public:
		CMD_Aux2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Aux2_MD : public PreformedCommand {
	public:
		CMD_Aux2_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Aux2_Cat : public PreformedCommand {
	public:
		CMD_Aux2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Tape2 : public PreformedCommand {
	public:
		CMD_Tape2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Tape2_DL : public PreformedCommand {
	public:
		CMD_Tape2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Tape2_MD : public PreformedCommand {
	public:
		CMD_Tape2_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Tape2_Cat : public PreformedCommand {
	public:
		CMD_Tape2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Minidisc : public PreformedCommand {
	public:
		CMD_Minidisc(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Minidisc_DL : public PreformedCommand {
	public:
		CMD_Minidisc_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Minidisc_MD : public PreformedCommand {
	public:
		CMD_Minidisc_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Minidisc_Cat : public PreformedCommand {
	public:
		CMD_Minidisc_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Tuner2 : public PreformedCommand {
	public:
		CMD_Tuner2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_Tuner2_DL : public PreformedCommand {
	public:
		CMD_Tuner2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_Tuner2_MD : public PreformedCommand {
	public:
		CMD_Tuner2_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_Tuner2_Cat : public PreformedCommand {
	public:
		CMD_Tuner2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_VDP : public PreformedCommand {
	public:
		CMD_VDP(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,176,0); }
	};
	class CMD_VDP_DL : public PreformedCommand {
	public:
		CMD_VDP_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,176,0); }
	};
	class CMD_VDP_MD : public PreformedCommand {
	public:
		CMD_VDP_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,176,0); }
	};
	class CMD_VDP_Cat : public PreformedCommand {
	public:
		CMD_VDP_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,176,0); }
	};
	class CMD_Laserdisc : public PreformedCommand {
	public:
		CMD_Laserdisc(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,177,0); }
	};
	class CMD_Laserdisc_DL : public PreformedCommand {
	public:
		CMD_Laserdisc_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,177,0); }
	};
	class CMD_Laserdisc_MD : public PreformedCommand {
	public:
		CMD_Laserdisc_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,177,0); }
	};
	class CMD_Laserdisc_Cat : public PreformedCommand {
	public:
		CMD_Laserdisc_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,177,0); }
	};
	class CMD_DSS : public PreformedCommand {
	public:
		CMD_DSS(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,178,0); }
	};
	class CMD_DSS_DL : public PreformedCommand {
	public:
		CMD_DSS_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,178,0); }
	};
	class CMD_DSS_MD : public PreformedCommand {
	public:
		CMD_DSS_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,178,0); }
	};
	class CMD_DSS_Cat : public PreformedCommand {
	public:
		CMD_DSS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,178,0); }
	};
	class CMD_DVI : public PreformedCommand {
	public:
		CMD_DVI(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,179,0); }
	};
	class CMD_DVI_DL : public PreformedCommand {
	public:
		CMD_DVI_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,179,0); }
	};
	class CMD_DVI_MD : public PreformedCommand {
	public:
		CMD_DVI_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,179,0); }
	};
	class CMD_DVI_Cat : public PreformedCommand {
	public:
		CMD_DVI_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,179,0); }
	};
	class CMD_Svideo : public PreformedCommand {
	public:
		CMD_Svideo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,180,0); }
	};
	class CMD_Svideo_DL : public PreformedCommand {
	public:
		CMD_Svideo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,180,0); }
	};
	class CMD_Svideo_MD : public PreformedCommand {
	public:
		CMD_Svideo_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,180,0); }
	};
	class CMD_Svideo_Cat : public PreformedCommand {
	public:
		CMD_Svideo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,180,0); }
	};
	class CMD_Composite : public PreformedCommand {
	public:
		CMD_Composite(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,181,0); }
	};
	class CMD_Composite_DL : public PreformedCommand {
	public:
		CMD_Composite_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,181,0); }
	};
	class CMD_Composite_MD : public PreformedCommand {
	public:
		CMD_Composite_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,181,0); }
	};
	class CMD_Composite_Cat : public PreformedCommand {
	public:
		CMD_Composite_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,181,0); }
	};
	class CMD_RF_Input : public PreformedCommand {
	public:
		CMD_RF_Input(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,182,0); }
	};
	class CMD_RF_Input_DL : public PreformedCommand {
	public:
		CMD_RF_Input_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,182,0); }
	};
	class CMD_RF_Input_MD : public PreformedCommand {
	public:
		CMD_RF_Input_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,182,0); }
	};
	class CMD_RF_Input_Cat : public PreformedCommand {
	public:
		CMD_RF_Input_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,182,0); }
	};
	class RESP_Get_Floorplan_Layout : public PreformedCommandResponse {
		string *m_sValue_To_Assign;
	public:
		RESP_Get_Floorplan_Layout(string *sValue_To_Assign) { 
		m_sValue_To_Assign=sValue_To_Assign; }
		void ParseResponse(Message *pMessage) {
			*m_sValue_To_Assign=pMessage->m_mapParameters[5]; };
	};
	class CMD_Get_Floorplan_Layout : public PreformedCommand {
	public:
		CMD_Get_Floorplan_Layout(long DeviceIDFrom, long DeviceIDTo,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0);		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_Get_Floorplan_Layout_DL : public PreformedCommand {
	public:
		CMD_Get_Floorplan_Layout_DL(long DeviceIDFrom, string DeviceIDTo,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0);		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_Get_Floorplan_Layout_MD : public PreformedCommand {
	public:
		CMD_Get_Floorplan_Layout_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0);		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_Get_Floorplan_Layout_Cat : public PreformedCommand {
	public:
		CMD_Get_Floorplan_Layout_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0);		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Floorplan_Layout : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_NOREP_Get_Floorplan_Layout_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_NOREP_Get_Floorplan_Layout_MD : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_NOREP_Get_Floorplan_Layout_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_UNUSED : public PreformedCommand {
	public:
		CMD_UNUSED(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,0); }
	};
	class CMD_UNUSED_DL : public PreformedCommand {
	public:
		CMD_UNUSED_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,0); }
	};
	class CMD_UNUSED_MD : public PreformedCommand {
	public:
		CMD_UNUSED_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,0); }
	};
	class CMD_UNUSED_Cat : public PreformedCommand {
	public:
		CMD_UNUSED_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,0); }
	};
	class CMD_Schedule_Recording : public PreformedCommand {
	public:
		CMD_Schedule_Recording(long DeviceIDFrom, long DeviceIDTo,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,185,1,68,sProgramID.c_str()); }
	};
	class CMD_Schedule_Recording_DL : public PreformedCommand {
	public:
		CMD_Schedule_Recording_DL(long DeviceIDFrom, string DeviceIDTo,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,185,1,68,sProgramID.c_str()); }
	};
	class CMD_Schedule_Recording_MD : public PreformedCommand {
	public:
		CMD_Schedule_Recording_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,185,1,68,sProgramID.c_str()); }
	};
	class CMD_Schedule_Recording_Cat : public PreformedCommand {
	public:
		CMD_Schedule_Recording_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,185,1,68,sProgramID.c_str()); }
	};
	class RESP_Get_Current_Floorplan : public PreformedCommandResponse {
		string *m_sValue_To_Assign;
	public:
		RESP_Get_Current_Floorplan(string *sValue_To_Assign) { 
		m_sValue_To_Assign=sValue_To_Assign; }
		void ParseResponse(Message *pMessage) {
			*m_sValue_To_Assign=pMessage->m_mapParameters[5]; };
	};
	class CMD_Get_Current_Floorplan : public PreformedCommand {
	public:
		CMD_Get_Current_Floorplan(long DeviceIDFrom, long DeviceIDTo,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_Get_Current_Floorplan_DL : public PreformedCommand {
	public:
		CMD_Get_Current_Floorplan_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_Get_Current_Floorplan_MD : public PreformedCommand {
	public:
		CMD_Get_Current_Floorplan_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_Get_Current_Floorplan_Cat : public PreformedCommand {
	public:
		CMD_Get_Current_Floorplan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Current_Floorplan : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan(long DeviceIDFrom, long DeviceIDTo,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_NOREP_Get_Current_Floorplan_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_NOREP_Get_Current_Floorplan_MD : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_NOREP_Get_Current_Floorplan_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,1,46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_Tune_to_channel : public PreformedCommand {
	public:
		CMD_Tune_to_channel(long DeviceIDFrom, long DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Tune_to_channel_DL : public PreformedCommand {
	public:
		CMD_Tune_to_channel_DL(long DeviceIDFrom, string DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Tune_to_channel_MD : public PreformedCommand {
	public:
		CMD_Tune_to_channel_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Tune_to_channel_Cat : public PreformedCommand {
	public:
		CMD_Tune_to_channel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class RESP_Get_Infrared_Codes : public PreformedCommandResponse {
		string *m_sValue_To_Assign;
	public:
		RESP_Get_Infrared_Codes(string *sValue_To_Assign) { 
		m_sValue_To_Assign=sValue_To_Assign; }
		void ParseResponse(Message *pMessage) {
			*m_sValue_To_Assign=pMessage->m_mapParameters[5]; };
	};
	class CMD_Get_Infrared_Codes : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str());		m_pcResponse = new RESP_Get_Infrared_Codes(sValue_To_Assign); }
	};
	class CMD_Get_Infrared_Codes_DL : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str());		m_pcResponse = new RESP_Get_Infrared_Codes(sValue_To_Assign); }
	};
	class CMD_Get_Infrared_Codes_MD : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str());		m_pcResponse = new RESP_Get_Infrared_Codes(sValue_To_Assign); }
	};
	class CMD_Get_Infrared_Codes_Cat : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str());		m_pcResponse = new RESP_Get_Infrared_Codes(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Infrared_Codes : public PreformedCommand {
	public:
		CMD_NOREP_Get_Infrared_Codes(long DeviceIDFrom, long DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Infrared_Codes_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Infrared_Codes_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Infrared_Codes_MD : public PreformedCommand {
	public:
		CMD_NOREP_Get_Infrared_Codes_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Infrared_Codes_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Infrared_Codes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_EnterGo : public PreformedCommand {
	public:
		CMD_EnterGo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,190,0); }
	};
	class CMD_EnterGo_DL : public PreformedCommand {
	public:
		CMD_EnterGo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,190,0); }
	};
	class CMD_EnterGo_MD : public PreformedCommand {
	public:
		CMD_EnterGo_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,190,0); }
	};
	class CMD_EnterGo_Cat : public PreformedCommand {
	public:
		CMD_EnterGo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,190,0); }
	};
	class CMD_Send_Code : public PreformedCommand {
	public:
		CMD_Send_Code(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,0); }
	};
	class CMD_Send_Code_DL : public PreformedCommand {
	public:
		CMD_Send_Code_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,0); }
	};
	class CMD_Send_Code_MD : public PreformedCommand {
	public:
		CMD_Send_Code_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,0); }
	};
	class CMD_Send_Code_Cat : public PreformedCommand {
	public:
		CMD_Send_Code_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,0); }
	};
	class CMD_On : public PreformedCommand {
	public:
		CMD_On(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,0); }
	};
	class CMD_On_DL : public PreformedCommand {
	public:
		CMD_On_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,0); }
	};
	class CMD_On_MD : public PreformedCommand {
	public:
		CMD_On_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,0); }
	};
	class CMD_On_Cat : public PreformedCommand {
	public:
		CMD_On_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,0); }
	};
	class CMD_Off : public PreformedCommand {
	public:
		CMD_Off(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,0); }
	};
	class CMD_Off_DL : public PreformedCommand {
	public:
		CMD_Off_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,0); }
	};
	class CMD_Off_MD : public PreformedCommand {
	public:
		CMD_Off_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,0); }
	};
	class CMD_Off_Cat : public PreformedCommand {
	public:
		CMD_Off_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,0); }
	};
	class CMD_Power : public PreformedCommand {
	public:
		CMD_Power(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,0); }
	};
	class CMD_Power_DL : public PreformedCommand {
	public:
		CMD_Power_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,0); }
	};
	class CMD_Power_MD : public PreformedCommand {
	public:
		CMD_Power_MD(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,0); }
	};
	class CMD_Power_Cat : public PreformedCommand {
	public:
		CMD_Power_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,0); }
	};
}
#endif
