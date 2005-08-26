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
	class CMD_Capture_Keyboard_To_Variable_DT : public PreformedCommand {
	public:
		CMD_Capture_Keyboard_To_Variable_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPK_Variable,string sOnOff,string sType,string sReset,int iPK_Text,bool bDataGrid) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,1,7,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),8,sOnOff.c_str(),14,sType.c_str(),24,sReset.c_str(),25,StringUtils::itos(iPK_Text).c_str(),55,(bDataGrid ? "1" : "0")); }
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
	class CMD_Orbiter_Beep_DT : public PreformedCommand {
	public:
		CMD_Orbiter_Beep_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,2,0); }
	};
	class CMD_Orbiter_Beep_Cat : public PreformedCommand {
	public:
		CMD_Orbiter_Beep_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,2,0); }
	};
	class CMD_Display_OnOff : public PreformedCommand {
	public:
		CMD_Display_OnOff(long DeviceIDFrom, long DeviceIDTo,string sOnOff,bool bAlready_processed) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,2,8,sOnOff.c_str(),125,(bAlready_processed ? "1" : "0")); }
	};
	class CMD_Display_OnOff_DL : public PreformedCommand {
	public:
		CMD_Display_OnOff_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff,bool bAlready_processed) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,2,8,sOnOff.c_str(),125,(bAlready_processed ? "1" : "0")); }
	};
	class CMD_Display_OnOff_DT : public PreformedCommand {
	public:
		CMD_Display_OnOff_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff,bool bAlready_processed) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,2,8,sOnOff.c_str(),125,(bAlready_processed ? "1" : "0")); }
	};
	class CMD_Display_OnOff_Cat : public PreformedCommand {
	public:
		CMD_Display_OnOff_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff,bool bAlready_processed) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,3,2,8,sOnOff.c_str(),125,(bAlready_processed ? "1" : "0")); }
	};
	class CMD_Go_back : public PreformedCommand {
	public:
		CMD_Go_back(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Go_back_DL : public PreformedCommand {
	public:
		CMD_Go_back_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Go_back_DT : public PreformedCommand {
	public:
		CMD_Go_back_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Go_back_Cat : public PreformedCommand {
	public:
		CMD_Go_back_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj_CurrentScreen,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,4,2,16,sPK_DesignObj_CurrentScreen.c_str(),21,sForce.c_str()); }
	};
	class CMD_Goto_Screen : public PreformedCommand {
	public:
		CMD_Goto_Screen(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0"),114,(bCant_Go_Back ? "1" : "0")); }
	};
	class CMD_Goto_Screen_DL : public PreformedCommand {
	public:
		CMD_Goto_Screen_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0"),114,(bCant_Go_Back ? "1" : "0")); }
	};
	class CMD_Goto_Screen_DT : public PreformedCommand {
	public:
		CMD_Goto_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0"),114,(bCant_Go_Back ? "1" : "0")); }
	};
	class CMD_Goto_Screen_Cat : public PreformedCommand {
	public:
		CMD_Goto_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sID,string sPK_DesignObj_CurrentScreen,bool bStore_Variables,bool bCant_Go_Back) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,5,6,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),10,sID.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),22,(bStore_Variables ? "1" : "0"),114,(bCant_Go_Back ? "1" : "0")); }
	};
	class CMD_Show_Object : public PreformedCommand {
	public:
		CMD_Show_Object(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,6,5,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),6,sComparisson_Operator.c_str(),7,sComparisson_Value.c_str(),8,sOnOff.c_str()); }
	};
	class CMD_Show_Object_DL : public PreformedCommand {
	public:
		CMD_Show_Object_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,6,5,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),6,sComparisson_Operator.c_str(),7,sComparisson_Value.c_str(),8,sOnOff.c_str()); }
	};
	class CMD_Show_Object_DT : public PreformedCommand {
	public:
		CMD_Show_Object_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPK_Variable,string sComparisson_Operator,string sComparisson_Value,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,6,5,3,sPK_DesignObj.c_str(),4,StringUtils::itos(iPK_Variable).c_str(),6,sComparisson_Operator.c_str(),7,sComparisson_Value.c_str(),8,sOnOff.c_str()); }
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
	class CMD_Terminate_Orbiter_DT : public PreformedCommand {
	public:
		CMD_Terminate_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,7,0); }
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
	class CMD_Remove_Screen_From_History_DT : public PreformedCommand {
	public:
		CMD_Remove_Screen_From_History_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,8,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
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
	class CMD_Scroll_Grid_DT : public PreformedCommand {
	public:
		CMD_Scroll_Grid_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,9,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
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
	class CMD_Move_Highlight_DT : public PreformedCommand {
	public:
		CMD_Move_Highlight_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,10,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Move_Highlight_Cat : public PreformedCommand {
	public:
		CMD_Move_Highlight_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sRelative_Level,string sPK_DesignObj,int iPK_Direction) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,10,3,1,sRelative_Level.c_str(),3,sPK_DesignObj.c_str(),30,StringUtils::itos(iPK_Direction).c_str()); }
	};
	class CMD_Play_Sound : public PreformedCommand {
	public:
		CMD_Play_Sound(long DeviceIDFrom, long DeviceIDTo,char *pData,int iData_Size,string sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,13,2,-19,pData,iData_Size,20,sFormat.c_str()); }
	};
	class CMD_Play_Sound_DL : public PreformedCommand {
	public:
		CMD_Play_Sound_DL(long DeviceIDFrom, string DeviceIDTo,char *pData,int iData_Size,string sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,13,2,-19,pData,iData_Size,20,sFormat.c_str()); }
	};
	class CMD_Play_Sound_DT : public PreformedCommand {
	public:
		CMD_Play_Sound_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,char *pData,int iData_Size,string sFormat) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,13,2,-19,pData,iData_Size,20,sFormat.c_str()); }
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
	class CMD_Refresh_DT : public PreformedCommand {
	public:
		CMD_Refresh_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,14,1,15,sDataGrid_ID.c_str()); }
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
	class CMD_Regen_Screen_DT : public PreformedCommand {
	public:
		CMD_Regen_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,15,0); }
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
	class CMD_Requires_Special_Handling_DT : public PreformedCommand {
	public:
		CMD_Requires_Special_Handling_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,16,0); }
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
	class CMD_Seek_Data_Grid_DT : public PreformedCommand {
	public:
		CMD_Seek_Data_Grid_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,int iPosition_X,int iPosition_Y,string sDataGrid_ID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,17,4,9,sText.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),15,sDataGrid_ID.c_str()); }
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
	class CMD_Set_Graphic_To_Display_DT : public PreformedCommand {
	public:
		CMD_Set_Graphic_To_Display_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,18,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Set_Graphic_To_Display_Cat : public PreformedCommand {
	public:
		CMD_Set_Graphic_To_Display_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,18,2,3,sPK_DesignObj.c_str(),10,sID.c_str()); }
	};
	class CMD_Set_House_Mode : public PreformedCommand {
	public:
		CMD_Set_House_Mode(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,5,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),100,StringUtils::itos(iPK_DeviceGroup).c_str(),101,sHandling_Instructions.c_str()); }
	};
	class CMD_Set_House_Mode_DL : public PreformedCommand {
	public:
		CMD_Set_House_Mode_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,5,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),100,StringUtils::itos(iPK_DeviceGroup).c_str(),101,sHandling_Instructions.c_str()); }
	};
	class CMD_Set_House_Mode_DT : public PreformedCommand {
	public:
		CMD_Set_House_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,5,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),100,StringUtils::itos(iPK_DeviceGroup).c_str(),101,sHandling_Instructions.c_str()); }
	};
	class CMD_Set_House_Mode_Cat : public PreformedCommand {
	public:
		CMD_Set_House_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users,string sPassword,int iPK_DeviceGroup,string sHandling_Instructions) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,19,5,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),100,StringUtils::itos(iPK_DeviceGroup).c_str(),101,sHandling_Instructions.c_str()); }
	};
	class CMD_Set_Object_Parameter : public PreformedCommand {
	public:
		CMD_Set_Object_Parameter(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,20,3,3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),27,StringUtils::itos(iPK_DesignObjParameter).c_str()); }
	};
	class CMD_Set_Object_Parameter_DL : public PreformedCommand {
	public:
		CMD_Set_Object_Parameter_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,20,3,3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),27,StringUtils::itos(iPK_DesignObjParameter).c_str()); }
	};
	class CMD_Set_Object_Parameter_DT : public PreformedCommand {
	public:
		CMD_Set_Object_Parameter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sValue_To_Assign,int iPK_DesignObjParameter) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,20,3,3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),27,StringUtils::itos(iPK_DesignObjParameter).c_str()); }
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
	class CMD_Set_Object_Position_DT : public PreformedCommand {
	public:
		CMD_Set_Object_Position_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,21,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
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
	class CMD_Set_Object_Size_DT : public PreformedCommand {
	public:
		CMD_Set_Object_Size_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,22,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
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
	class CMD_Set_Pos_Rel_To_Parent_DT : public PreformedCommand {
	public:
		CMD_Set_Pos_Rel_To_Parent_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,23,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
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
	class CMD_Set_Size_Rel_To_Parent_DT : public PreformedCommand {
	public:
		CMD_Set_Size_Rel_To_Parent_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,24,3,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
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
	class CMD_Set_Text_DT : public PreformedCommand {
	public:
		CMD_Set_Text_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sText,int iPK_Text) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,25,3,3,sPK_DesignObj.c_str(),9,sText.c_str(),25,StringUtils::itos(iPK_Text).c_str()); }
	};
	class CMD_Set_Text_Cat : public PreformedCommand {
	public:
		CMD_Set_Text_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sText,int iPK_Text) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,25,3,3,sPK_DesignObj.c_str(),9,sText.c_str(),25,StringUtils::itos(iPK_Text).c_str()); }
	};
	class CMD_Set_Bound_Icon : public PreformedCommand {
	public:
		CMD_Set_Bound_Icon(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),14,sType.c_str()); }
	};
	class CMD_Set_Bound_Icon_DL : public PreformedCommand {
	public:
		CMD_Set_Bound_Icon_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),14,sType.c_str()); }
	};
	class CMD_Set_Bound_Icon_DT : public PreformedCommand {
	public:
		CMD_Set_Bound_Icon_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,string sType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),14,sType.c_str()); }
	};
	class CMD_Set_Bound_Icon_Cat : public PreformedCommand {
	public:
		CMD_Set_Bound_Icon_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,26,2,5,sValue_To_Assign.c_str(),14,sType.c_str()); }
	};
	class CMD_Set_Variable : public PreformedCommand {
	public:
		CMD_Set_Variable(long DeviceIDFrom, long DeviceIDTo,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Variable_DL : public PreformedCommand {
	public:
		CMD_Set_Variable_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Variable_DT : public PreformedCommand {
	public:
		CMD_Set_Variable_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Variable_Cat : public PreformedCommand {
	public:
		CMD_Set_Variable_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Variable,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,27,2,4,StringUtils::itos(iPK_Variable).c_str(),5,sValue_To_Assign.c_str()); }
	};
	class CMD_Simulate_Keypress : public PreformedCommand {
	public:
		CMD_Simulate_Keypress(long DeviceIDFrom, long DeviceIDTo,string sPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,sPK_Button.c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Keypress_DL : public PreformedCommand {
	public:
		CMD_Simulate_Keypress_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,sPK_Button.c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Keypress_DT : public PreformedCommand {
	public:
		CMD_Simulate_Keypress_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,sPK_Button.c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Keypress_Cat : public PreformedCommand {
	public:
		CMD_Simulate_Keypress_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_Button,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,28,2,26,sPK_Button.c_str(),50,sName.c_str()); }
	};
	class CMD_Simulate_Mouse_Click : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click(long DeviceIDFrom, long DeviceIDTo,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,29,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Simulate_Mouse_Click_DL : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_DL(long DeviceIDFrom, string DeviceIDTo,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,29,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Simulate_Mouse_Click_DT : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,29,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
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
	class CMD_Stop_Sound_DT : public PreformedCommand {
	public:
		CMD_Stop_Sound_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,30,0); }
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
	class CMD_Store_Variables_DT : public PreformedCommand {
	public:
		CMD_Store_Variables_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,31,0); }
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
	class CMD_Update_Object_Image_DT : public PreformedCommand {
	public:
		CMD_Update_Object_Image_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sType,char *pData,int iData_Size,string sDisable_Aspect_Lock) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,32,4,3,sPK_DesignObj.c_str(),14,sType.c_str(),-19,pData,iData_Size,23,sDisable_Aspect_Lock.c_str()); }
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
	class CMD_Start_Media_DT : public PreformedCommand {
	public:
		CMD_Start_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sFilename,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,33,3,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Start_Media_Cat : public PreformedCommand {
	public:
		CMD_Start_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sFilename,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,33,3,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class RESP_Request_Datagrid_Contents : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;int *m_iRow;
	public:
		RESP_Request_Datagrid_Contents(char **pData,int *iData_Size,int *iRow) { 
		m_pData=pData; m_iData_Size=iData_Size; m_iRow=iRow; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; *m_iRow=atoi(pMessage->m_mapParameters[32].c_str()); };
	};
	class CMD_Request_Datagrid_Contents : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,12,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str(),-19,*pData,*iData_Size,32,StringUtils::itos(*iRow).c_str());		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size,iRow); }
	};
	class CMD_Request_Datagrid_Contents_DL : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,12,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str(),-19,*pData,*iData_Size,32,StringUtils::itos(*iRow).c_str());		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size,iRow); }
	};
	class CMD_Request_Datagrid_Contents_DT : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,12,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str(),-19,*pData,*iData_Size,32,StringUtils::itos(*iRow).c_str());		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size,iRow); }
	};
	class CMD_Request_Datagrid_Contents_Cat : public PreformedCommand {
	public:
		CMD_Request_Datagrid_Contents_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset,char **pData,int *iData_Size,int *iRow) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,12,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str(),-19,*pData,*iData_Size,32,StringUtils::itos(*iRow).c_str());		m_pcResponse = new RESP_Request_Datagrid_Contents(pData,iData_Size,iRow); }
	};
	class CMD_NOREP_Request_Datagrid_Contents : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,10,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str()); }
	};
	class CMD_NOREP_Request_Datagrid_Contents_DL : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,10,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str()); }
	};
	class CMD_NOREP_Request_Datagrid_Contents_DT : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,10,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str()); }
	};
	class CMD_NOREP_Request_Datagrid_Contents_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Request_Datagrid_Contents_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iColumn,int iRow_count,int iColumn_count,bool bKeep_Row_Header,bool bKeep_Column_Header,bool bAdd_UpDown_Arrows,string sSeek,int iOffset) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,34,10,10,sID.c_str(),15,sDataGrid_ID.c_str(),33,StringUtils::itos(iColumn).c_str(),34,StringUtils::itos(iRow_count).c_str(),35,StringUtils::itos(iColumn_count).c_str(),36,(bKeep_Row_Header ? "1" : "0"),37,(bKeep_Column_Header ? "1" : "0"),49,(bAdd_UpDown_Arrows ? "1" : "0"),73,sSeek.c_str(),74,StringUtils::itos(iOffset).c_str()); }
	};
	class RESP_Populate_Datagrid : public PreformedCommandResponse {
		int *m_iPK_Variable;string *m_sValue_To_Assign;bool *m_bIsSuccessful;int *m_iWidth;int *m_iHeight;
	public:
		RESP_Populate_Datagrid(int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight) { 
		m_iPK_Variable=iPK_Variable; m_sValue_To_Assign=sValue_To_Assign; m_bIsSuccessful=bIsSuccessful; m_iWidth=iWidth; m_iHeight=iHeight; }
		void ParseResponse(Message *pMessage) {
			*m_iPK_Variable=atoi(pMessage->m_mapParameters[4].c_str()); *m_sValue_To_Assign=pMessage->m_mapParameters[5]; *m_bIsSuccessful=(pMessage->m_mapParameters[40]=="1" ? true : false); *m_iWidth=atoi(pMessage->m_mapParameters[60].c_str()); *m_iHeight=atoi(pMessage->m_mapParameters[61].c_str()); };
	};
	class CMD_Populate_Datagrid : public PreformedCommand {
	public:
		CMD_Populate_Datagrid(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str(),4,StringUtils::itos(*iPK_Variable).c_str(),5,(*sValue_To_Assign).c_str(),40,(*bIsSuccessful ? "1" : "0"),60,StringUtils::itos(*iWidth).c_str(),61,StringUtils::itos(*iHeight).c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful,iWidth,iHeight); }
	};
	class CMD_Populate_Datagrid_DL : public PreformedCommand {
	public:
		CMD_Populate_Datagrid_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str(),4,StringUtils::itos(*iPK_Variable).c_str(),5,(*sValue_To_Assign).c_str(),40,(*bIsSuccessful ? "1" : "0"),60,StringUtils::itos(*iWidth).c_str(),61,StringUtils::itos(*iHeight).c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful,iWidth,iHeight); }
	};
	class CMD_Populate_Datagrid_DT : public PreformedCommand {
	public:
		CMD_Populate_Datagrid_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str(),4,StringUtils::itos(*iPK_Variable).c_str(),5,(*sValue_To_Assign).c_str(),40,(*bIsSuccessful ? "1" : "0"),60,StringUtils::itos(*iWidth).c_str(),61,StringUtils::itos(*iHeight).c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful,iWidth,iHeight); }
	};
	class CMD_Populate_Datagrid_Cat : public PreformedCommand {
	public:
		CMD_Populate_Datagrid_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions,int *iPK_Variable,string *sValue_To_Assign,bool *bIsSuccessful,int *iWidth,int *iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,9,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str(),4,StringUtils::itos(*iPK_Variable).c_str(),5,(*sValue_To_Assign).c_str(),40,(*bIsSuccessful ? "1" : "0"),60,StringUtils::itos(*iWidth).c_str(),61,StringUtils::itos(*iHeight).c_str());		m_pcResponse = new RESP_Populate_Datagrid(iPK_Variable,sValue_To_Assign,bIsSuccessful,iWidth,iHeight); }
	};
	class CMD_NOREP_Populate_Datagrid : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid(long DeviceIDFrom, long DeviceIDTo,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_NOREP_Populate_Datagrid_DL : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid_DL(long DeviceIDFrom, string DeviceIDTo,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_NOREP_Populate_Datagrid_DT : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_NOREP_Populate_Datagrid_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Populate_Datagrid_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,string sDataGrid_ID,int iPK_DataGrid,string sOptions) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,35,4,10,sID.c_str(),15,sDataGrid_ID.c_str(),38,StringUtils::itos(iPK_DataGrid).c_str(),39,sOptions.c_str()); }
	};
	class CMD_Create_Media : public PreformedCommand {
	public:
		CMD_Create_Media(long DeviceIDFrom, long DeviceIDTo,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Create_Media_DL : public PreformedCommand {
	public:
		CMD_Create_Media_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Create_Media_DT : public PreformedCommand {
	public:
		CMD_Create_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Create_Media_Cat : public PreformedCommand {
	public:
		CMD_Create_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,36,2,13,sFilename.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Play_Media : public PreformedCommand {
	public:
		CMD_Play_Media(long DeviceIDFrom, long DeviceIDTo,string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class CMD_Play_Media_DL : public PreformedCommand {
	public:
		CMD_Play_Media_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class CMD_Play_Media_DT : public PreformedCommand {
	public:
		CMD_Play_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class CMD_Play_Media_Cat : public PreformedCommand {
	public:
		CMD_Play_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,int iPK_MediaType,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,37,4,13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class RESP_Stop_Media : public PreformedCommandResponse {
		string *m_sMediaPosition;
	public:
		RESP_Stop_Media(string *sMediaPosition) { 
		m_sMediaPosition=sMediaPosition; }
		void ParseResponse(Message *pMessage) {
			*m_sMediaPosition=pMessage->m_mapParameters[42]; };
	};
	class CMD_Stop_Media : public PreformedCommand {
	public:
		CMD_Stop_Media(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,2,41,StringUtils::itos(iStreamID).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Stop_Media(sMediaPosition); }
	};
	class CMD_Stop_Media_DL : public PreformedCommand {
	public:
		CMD_Stop_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,2,41,StringUtils::itos(iStreamID).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Stop_Media(sMediaPosition); }
	};
	class CMD_Stop_Media_DT : public PreformedCommand {
	public:
		CMD_Stop_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,2,41,StringUtils::itos(iStreamID).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Stop_Media(sMediaPosition); }
	};
	class CMD_Stop_Media_Cat : public PreformedCommand {
	public:
		CMD_Stop_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,2,41,StringUtils::itos(iStreamID).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Stop_Media(sMediaPosition); }
	};
	class CMD_NOREP_Stop_Media : public PreformedCommand {
	public:
		CMD_NOREP_Stop_Media(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_NOREP_Stop_Media_DL : public PreformedCommand {
	public:
		CMD_NOREP_Stop_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_NOREP_Stop_Media_DT : public PreformedCommand {
	public:
		CMD_NOREP_Stop_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_NOREP_Stop_Media_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Stop_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,38,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Pause_Media : public PreformedCommand {
	public:
		CMD_Pause_Media(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,39,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Pause_Media_DL : public PreformedCommand {
	public:
		CMD_Pause_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,39,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Pause_Media_DT : public PreformedCommand {
	public:
		CMD_Pause_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,39,1,41,StringUtils::itos(iStreamID).c_str()); }
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
	class CMD_Restart_Media_DT : public PreformedCommand {
	public:
		CMD_Restart_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,40,1,41,StringUtils::itos(iStreamID).c_str()); }
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
	class CMD_Change_Playback_Speed_DT : public PreformedCommand {
	public:
		CMD_Change_Playback_Speed_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,int iMediaPlaybackSpeed) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,41,2,41,StringUtils::itos(iStreamID).c_str(),43,StringUtils::itos(iMediaPlaybackSpeed).c_str()); }
	};
	class CMD_Change_Playback_Speed_Cat : public PreformedCommand {
	public:
		CMD_Change_Playback_Speed_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,int iMediaPlaybackSpeed) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,41,2,41,StringUtils::itos(iStreamID).c_str(),43,StringUtils::itos(iMediaPlaybackSpeed).c_str()); }
	};
	class CMD_Jump_to_Position_in_Stream : public PreformedCommand {
	public:
		CMD_Jump_to_Position_in_Stream(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,2,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Jump_to_Position_in_Stream_DL : public PreformedCommand {
	public:
		CMD_Jump_to_Position_in_Stream_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,2,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Jump_to_Position_in_Stream_DT : public PreformedCommand {
	public:
		CMD_Jump_to_Position_in_Stream_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,2,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Jump_to_Position_in_Stream_Cat : public PreformedCommand {
	public:
		CMD_Jump_to_Position_in_Stream_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,42,2,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_MH_Play_Media : public PreformedCommand {
	public:
		CMD_MH_Play_Media(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,7,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str(),116,(bResume ? "1" : "0"),117,StringUtils::itos(iRepeat).c_str()); }
	};
	class CMD_MH_Play_Media_DL : public PreformedCommand {
	public:
		CMD_MH_Play_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,7,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str(),116,(bResume ? "1" : "0"),117,StringUtils::itos(iRepeat).c_str()); }
	};
	class CMD_MH_Play_Media_DT : public PreformedCommand {
	public:
		CMD_MH_Play_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,7,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str(),116,(bResume ? "1" : "0"),117,StringUtils::itos(iRepeat).c_str()); }
	};
	class CMD_MH_Play_Media_Cat : public PreformedCommand {
	public:
		CMD_MH_Play_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sFilename,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea,bool bResume,int iRepeat) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,43,7,2,StringUtils::itos(iPK_Device).c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str(),116,(bResume ? "1" : "0"),117,StringUtils::itos(iRepeat).c_str()); }
	};
	class CMD_MH_Stop_Media : public PreformedCommand {
	public:
		CMD_MH_Stop_Media(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_MH_Stop_Media_DL : public PreformedCommand {
	public:
		CMD_MH_Stop_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_MH_Stop_Media_DT : public PreformedCommand {
	public:
		CMD_MH_Stop_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_MH_Stop_Media_Cat : public PreformedCommand {
	public:
		CMD_MH_Stop_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,int iPK_MediaType,int iPK_DeviceTemplate,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,44,4,2,StringUtils::itos(iPK_Device).c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Disk_Drive_Monitoring_ON : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_ON(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,45,0); }
	};
	class CMD_Disk_Drive_Monitoring_ON_DL : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_ON_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,45,0); }
	};
	class CMD_Disk_Drive_Monitoring_ON_DT : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_ON_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,45,0); }
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
	class CMD_Disk_Drive_Monitoring_OFF_DT : public PreformedCommand {
	public:
		CMD_Disk_Drive_Monitoring_OFF_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,46,0); }
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
	class CMD_Reset_Disk_Drive_DT : public PreformedCommand {
	public:
		CMD_Reset_Disk_Drive_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,47,0); }
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
	class CMD_Eject_Disk_DT : public PreformedCommand {
	public:
		CMD_Eject_Disk_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,48,0); }
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
	class CMD_Start_Burn_Session_DT : public PreformedCommand {
	public:
		CMD_Start_Burn_Session_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,49,0); }
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
	class CMD_Start_Ripping_Session_DT : public PreformedCommand {
	public:
		CMD_Start_Ripping_Session_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,50,0); }
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
	class CMD_Add_File_To_Burning_Session_DT : public PreformedCommand {
	public:
		CMD_Add_File_To_Burning_Session_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,51,0); }
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
	class CMD_Start_Burning_DT : public PreformedCommand {
	public:
		CMD_Start_Burning_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,52,0); }
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
	class CMD_Abort_Burning_DT : public PreformedCommand {
	public:
		CMD_Abort_Burning_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,53,0); }
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
		CMD_Mount_Disk_Image(long DeviceIDFrom, long DeviceIDTo,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,2,13,sFilename.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_Mount_Disk_Image_DL : public PreformedCommand {
	public:
		CMD_Mount_Disk_Image_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,2,13,sFilename.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_Mount_Disk_Image_DT : public PreformedCommand {
	public:
		CMD_Mount_Disk_Image_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,2,13,sFilename.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_Mount_Disk_Image_Cat : public PreformedCommand {
	public:
		CMD_Mount_Disk_Image_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,2,13,sFilename.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Mount_Disk_Image(sMediaURL); }
	};
	class CMD_NOREP_Mount_Disk_Image : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image(long DeviceIDFrom, long DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Mount_Disk_Image_DL : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Mount_Disk_Image_DT : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Mount_Disk_Image_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Mount_Disk_Image_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,54,1,13,sFilename.c_str()); }
	};
	class CMD_Abort_Ripping : public PreformedCommand {
	public:
		CMD_Abort_Ripping(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Abort_Ripping_DL : public PreformedCommand {
	public:
		CMD_Abort_Ripping_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Abort_Ripping_DT : public PreformedCommand {
	public:
		CMD_Abort_Ripping_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Abort_Ripping_Cat : public PreformedCommand {
	public:
		CMD_Abort_Ripping_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,55,0); }
	};
	class CMD_Format_Drive : public PreformedCommand {
	public:
		CMD_Format_Drive(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,56,0); }
	};
	class CMD_Format_Drive_DL : public PreformedCommand {
	public:
		CMD_Format_Drive_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,56,0); }
	};
	class CMD_Format_Drive_DT : public PreformedCommand {
	public:
		CMD_Format_Drive_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,56,0); }
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
	class CMD_Close_Tray_DT : public PreformedCommand {
	public:
		CMD_Close_Tray_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,57,0); }
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
	class CMD_Set_Current_User_DT : public PreformedCommand {
	public:
		CMD_Set_Current_User_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,58,1,17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_Current_User_Cat : public PreformedCommand {
	public:
		CMD_Set_Current_User_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,58,1,17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_Entertainment_Area : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area(long DeviceIDFrom, long DeviceIDTo,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Set_Entertainment_Area_DL : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Set_Entertainment_Area_DT : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Set_Entertainment_Area_Cat : public PreformedCommand {
	public:
		CMD_Set_Entertainment_Area_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,59,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Link_with_mobile_orbiter : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter(long DeviceIDFrom, long DeviceIDTo,string sMac_address,string sVMC_File,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,3,47,sMac_address.c_str(),118,sVMC_File.c_str(),130,sConfig_File.c_str()); }
	};
	class CMD_Link_with_mobile_orbiter_DL : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address,string sVMC_File,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,3,47,sMac_address.c_str(),118,sVMC_File.c_str(),130,sConfig_File.c_str()); }
	};
	class CMD_Link_with_mobile_orbiter_DT : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address,string sVMC_File,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,3,47,sMac_address.c_str(),118,sVMC_File.c_str(),130,sConfig_File.c_str()); }
	};
	class CMD_Link_with_mobile_orbiter_Cat : public PreformedCommand {
	public:
		CMD_Link_with_mobile_orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address,string sVMC_File,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,60,3,47,sMac_address.c_str(),118,sVMC_File.c_str(),130,sConfig_File.c_str()); }
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
		CMD_Get_Signal_Strength(long DeviceIDFrom, long DeviceIDTo,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,2,47,sMac_address.c_str(),48,StringUtils::itos(*iValue).c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_Get_Signal_Strength_DL : public PreformedCommand {
	public:
		CMD_Get_Signal_Strength_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,2,47,sMac_address.c_str(),48,StringUtils::itos(*iValue).c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_Get_Signal_Strength_DT : public PreformedCommand {
	public:
		CMD_Get_Signal_Strength_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,2,47,sMac_address.c_str(),48,StringUtils::itos(*iValue).c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_Get_Signal_Strength_Cat : public PreformedCommand {
	public:
		CMD_Get_Signal_Strength_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address,int *iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,2,47,sMac_address.c_str(),48,StringUtils::itos(*iValue).c_str());		m_pcResponse = new RESP_Get_Signal_Strength(iValue); }
	};
	class CMD_NOREP_Get_Signal_Strength : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength(long DeviceIDFrom, long DeviceIDTo,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_NOREP_Get_Signal_Strength_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_NOREP_Get_Signal_Strength_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_NOREP_Get_Signal_Strength_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Signal_Strength_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,61,1,47,sMac_address.c_str()); }
	};
	class CMD_Create_Mobile_Orbiter : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,4,2,StringUtils::itos(iPK_Device).c_str(),45,sPK_EntertainArea.c_str(),47,sMac_address.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Create_Mobile_Orbiter_DL : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,4,2,StringUtils::itos(iPK_Device).c_str(),45,sPK_EntertainArea.c_str(),47,sMac_address.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Create_Mobile_Orbiter_DT : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,4,2,StringUtils::itos(iPK_Device).c_str(),45,sPK_EntertainArea.c_str(),47,sMac_address.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Create_Mobile_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_Create_Mobile_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPK_EntertainArea,string sMac_address,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,62,4,2,StringUtils::itos(iPK_Device).c_str(),45,sPK_EntertainArea.c_str(),47,sMac_address.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Skip_Fwd_ChannelTrack_Greater : public PreformedCommand {
	public:
		CMD_Skip_Fwd_ChannelTrack_Greater(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Fwd_ChannelTrack_Greater_DL : public PreformedCommand {
	public:
		CMD_Skip_Fwd_ChannelTrack_Greater_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Fwd_ChannelTrack_Greater_DT : public PreformedCommand {
	public:
		CMD_Skip_Fwd_ChannelTrack_Greater_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Fwd_ChannelTrack_Greater_Cat : public PreformedCommand {
	public:
		CMD_Skip_Fwd_ChannelTrack_Greater_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,63,0); }
	};
	class CMD_Skip_Back_ChannelTrack_Lower : public PreformedCommand {
	public:
		CMD_Skip_Back_ChannelTrack_Lower(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Skip_Back_ChannelTrack_Lower_DL : public PreformedCommand {
	public:
		CMD_Skip_Back_ChannelTrack_Lower_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Skip_Back_ChannelTrack_Lower_DT : public PreformedCommand {
	public:
		CMD_Skip_Back_ChannelTrack_Lower_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Skip_Back_ChannelTrack_Lower_Cat : public PreformedCommand {
	public:
		CMD_Skip_Back_ChannelTrack_Lower_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,64,0); }
	};
	class CMD_Jump_Position_In_Playlist : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Jump_Position_In_Playlist_DL : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Jump_Position_In_Playlist_DT : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Jump_Position_In_Playlist_Cat : public PreformedCommand {
	public:
		CMD_Jump_Position_In_Playlist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,65,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Select_Object : public PreformedCommand {
	public:
		CMD_Select_Object(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,3,3,sPK_DesignObj.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),102,sTime.c_str()); }
	};
	class CMD_Select_Object_DL : public PreformedCommand {
	public:
		CMD_Select_Object_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,3,3,sPK_DesignObj.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),102,sTime.c_str()); }
	};
	class CMD_Select_Object_DT : public PreformedCommand {
	public:
		CMD_Select_Object_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,3,3,sPK_DesignObj.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),102,sTime.c_str()); }
	};
	class CMD_Select_Object_Cat : public PreformedCommand {
	public:
		CMD_Select_Object_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sPK_DesignObj_CurrentScreen,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,66,3,3,sPK_DesignObj.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),102,sTime.c_str()); }
	};
	class CMD_Spawn_Application : public PreformedCommand {
	public:
		CMD_Spawn_Application(long DeviceIDFrom, long DeviceIDTo,string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,6,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str(),94,sSendOnFailure.c_str(),95,sSendOnSuccess.c_str(),115,(bShow_logo ? "1" : "0")); }
	};
	class CMD_Spawn_Application_DL : public PreformedCommand {
	public:
		CMD_Spawn_Application_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,6,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str(),94,sSendOnFailure.c_str(),95,sSendOnSuccess.c_str(),115,(bShow_logo ? "1" : "0")); }
	};
	class CMD_Spawn_Application_DT : public PreformedCommand {
	public:
		CMD_Spawn_Application_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,6,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str(),94,sSendOnFailure.c_str(),95,sSendOnSuccess.c_str(),115,(bShow_logo ? "1" : "0")); }
	};
	class CMD_Spawn_Application_Cat : public PreformedCommand {
	public:
		CMD_Spawn_Application_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,string sName,string sArguments,string sSendOnFailure,string sSendOnSuccess,bool bShow_logo) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,67,6,13,sFilename.c_str(),50,sName.c_str(),51,sArguments.c_str(),94,sSendOnFailure.c_str(),95,sSendOnSuccess.c_str(),115,(bShow_logo ? "1" : "0")); }
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
		CMD_Get_Device_Data(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,4,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_Get_Device_Data_DL : public PreformedCommand {
	public:
		CMD_Get_Device_Data_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,4,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_Get_Device_Data_DT : public PreformedCommand {
	public:
		CMD_Get_Device_Data_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,4,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_Get_Device_Data_Cat : public PreformedCommand {
	public:
		CMD_Get_Device_Data_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,int iPK_DeviceData,bool bUseDefault,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,4,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0"),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Device_Data(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Device_Data : public PreformedCommand {
	public:
		CMD_NOREP_Get_Device_Data(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0")); }
	};
	class CMD_NOREP_Get_Device_Data_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Device_Data_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,int iPK_DeviceData,bool bUseDefault) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0")); }
	};
	class CMD_NOREP_Get_Device_Data_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Device_Data_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,int iPK_DeviceData,bool bUseDefault) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,68,3,2,StringUtils::itos(iPK_Device).c_str(),52,StringUtils::itos(iPK_DeviceData).c_str(),53,(bUseDefault ? "1" : "0")); }
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
	class CMD_Kill_Application_DT : public PreformedCommand {
	public:
		CMD_Kill_Application_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,69,1,50,sName.c_str()); }
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
	class CMD_Hide_Application_DT : public PreformedCommand {
	public:
		CMD_Hide_Application_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,70,1,50,sName.c_str()); }
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
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; };
	};
	class CMD_Request_File : public PreformedCommand {
	public:
		CMD_Request_File(long DeviceIDFrom, long DeviceIDTo,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,2,13,sFilename.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_Request_File_DL : public PreformedCommand {
	public:
		CMD_Request_File_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,2,13,sFilename.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_Request_File_DT : public PreformedCommand {
	public:
		CMD_Request_File_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,2,13,sFilename.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_Request_File_Cat : public PreformedCommand {
	public:
		CMD_Request_File_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,2,13,sFilename.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Request_File(pData,iData_Size); }
	};
	class CMD_NOREP_Request_File : public PreformedCommand {
	public:
		CMD_NOREP_Request_File(long DeviceIDFrom, long DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_DL : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_DT : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,71,1,13,sFilename.c_str()); }
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
	class CMD_Surrender_to_OS_DT : public PreformedCommand {
	public:
		CMD_Surrender_to_OS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff,bool bFully_release_keyboard) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,72,2,8,sOnOff.c_str(),54,(bFully_release_keyboard ? "1" : "0")); }
	};
	class CMD_Surrender_to_OS_Cat : public PreformedCommand {
	public:
		CMD_Surrender_to_OS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff,bool bFully_release_keyboard) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,72,2,8,sOnOff.c_str(),54,(bFully_release_keyboard ? "1" : "0")); }
	};
	class CMD_Bind_to_Media_Remote : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,7,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),39,sOptions.c_str(),45,sPK_EntertainArea.c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Bind_to_Media_Remote_DL : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,7,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),39,sOptions.c_str(),45,sPK_EntertainArea.c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Bind_to_Media_Remote_DT : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,7,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),39,sOptions.c_str(),45,sPK_EntertainArea.c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Bind_to_Media_Remote_Cat : public PreformedCommand {
	public:
		CMD_Bind_to_Media_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sOnOff,string sPK_DesignObj_CurrentScreen,string sOptions,string sPK_EntertainArea,int iPK_Text_Synopsis) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,74,7,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),8,sOnOff.c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),39,sOptions.c_str(),45,sPK_EntertainArea.c_str(),63,StringUtils::itos(iPK_Text_Synopsis).c_str()); }
	};
	class CMD_Set_Current_Room : public PreformedCommand {
	public:
		CMD_Set_Current_Room(long DeviceIDFrom, long DeviceIDTo,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Current_Room_DL : public PreformedCommand {
	public:
		CMD_Set_Current_Room_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Current_Room_DT : public PreformedCommand {
	public:
		CMD_Set_Current_Room_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Current_Room_Cat : public PreformedCommand {
	public:
		CMD_Set_Current_Room_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,77,1,57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_New_Mobile_Orbiter : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter(long DeviceIDFrom, long DeviceIDTo,int iPK_Users,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,3,17,StringUtils::itos(iPK_Users).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_New_Mobile_Orbiter_DL : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Users,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,3,17,StringUtils::itos(iPK_Users).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_New_Mobile_Orbiter_DT : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Users,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,3,17,StringUtils::itos(iPK_Users).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_New_Mobile_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_New_Mobile_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Users,int iPK_DeviceTemplate,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,78,3,17,StringUtils::itos(iPK_Users).c_str(),44,StringUtils::itos(iPK_DeviceTemplate).c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Add_Unknown_Device : public PreformedCommand {
	public:
		CMD_Add_Unknown_Device(long DeviceIDFrom, long DeviceIDTo,string sText,string sID,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,79,3,9,sText.c_str(),10,sID.c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Add_Unknown_Device_DL : public PreformedCommand {
	public:
		CMD_Add_Unknown_Device_DL(long DeviceIDFrom, string DeviceIDTo,string sText,string sID,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,79,3,9,sText.c_str(),10,sID.c_str(),47,sMac_address.c_str()); }
	};
	class CMD_Add_Unknown_Device_DT : public PreformedCommand {
	public:
		CMD_Add_Unknown_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,string sID,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,79,3,9,sText.c_str(),10,sID.c_str(),47,sMac_address.c_str()); }
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
	class CMD_Send_File_To_Device_DT : public PreformedCommand {
	public:
		CMD_Send_File_To_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,string sMac_address,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,80,3,13,sFilename.c_str(),47,sMac_address.c_str(),58,sIP_Address.c_str()); }
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
	class CMD_Navigate_Next_DT : public PreformedCommand {
	public:
		CMD_Navigate_Next_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,81,1,41,StringUtils::itos(iStreamID).c_str()); }
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
	class CMD_Navigate_Prev_DT : public PreformedCommand {
	public:
		CMD_Navigate_Prev_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,82,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Navigate_Prev_Cat : public PreformedCommand {
	public:
		CMD_Navigate_Prev_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,82,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class RESP_Get_Video_Frame : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;string *m_sFormat;
	public:
		RESP_Get_Video_Frame(char **pData,int *iData_Size,string *sFormat) { 
		m_pData=pData; m_iData_Size=iData_Size; m_sFormat=sFormat; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; *m_sFormat=pMessage->m_mapParameters[20]; };
	};
	class CMD_Get_Video_Frame : public PreformedCommand {
	public:
		CMD_Get_Video_Frame(long DeviceIDFrom, long DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,6,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str(),-19,*pData,*iData_Size,20,(*sFormat).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_Get_Video_Frame_DL : public PreformedCommand {
	public:
		CMD_Get_Video_Frame_DL(long DeviceIDFrom, string DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,6,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str(),-19,*pData,*iData_Size,20,(*sFormat).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_Get_Video_Frame_DT : public PreformedCommand {
	public:
		CMD_Get_Video_Frame_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,6,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str(),-19,*pData,*iData_Size,20,(*sFormat).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_Get_Video_Frame_Cat : public PreformedCommand {
	public:
		CMD_Get_Video_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight,char **pData,int *iData_Size,string *sFormat) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,6,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str(),-19,*pData,*iData_Size,20,(*sFormat).c_str());		m_pcResponse = new RESP_Get_Video_Frame(pData,iData_Size,sFormat); }
	};
	class CMD_NOREP_Get_Video_Frame : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame(long DeviceIDFrom, long DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_NOREP_Get_Video_Frame_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame_DL(long DeviceIDFrom, string DeviceIDTo,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_NOREP_Get_Video_Frame_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_NOREP_Get_Video_Frame_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Video_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sDisable_Aspect_Lock,int iStreamID,int iWidth,int iHeight) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,84,4,23,sDisable_Aspect_Lock.c_str(),41,StringUtils::itos(iStreamID).c_str(),60,StringUtils::itos(iWidth).c_str(),61,StringUtils::itos(iHeight).c_str()); }
	};
	class CMD_Reset_Highlight : public PreformedCommand {
	public:
		CMD_Reset_Highlight(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Reset_Highlight_DL : public PreformedCommand {
	public:
		CMD_Reset_Highlight_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Reset_Highlight_DT : public PreformedCommand {
	public:
		CMD_Reset_Highlight_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Reset_Highlight_Cat : public PreformedCommand {
	public:
		CMD_Reset_Highlight_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,85,0); }
	};
	class CMD_Menu_Show_Menu : public PreformedCommand {
	public:
		CMD_Menu_Show_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Menu_Show_Menu_DL : public PreformedCommand {
	public:
		CMD_Menu_Show_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Menu_Show_Menu_DT : public PreformedCommand {
	public:
		CMD_Menu_Show_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Menu_Show_Menu_Cat : public PreformedCommand {
	public:
		CMD_Menu_Show_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,86,0); }
	};
	class CMD_Goto_Media_Menu : public PreformedCommand {
	public:
		CMD_Goto_Media_Menu(long DeviceIDFrom, long DeviceIDTo,int iStreamID,int iMenuType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,87,2,41,StringUtils::itos(iStreamID).c_str(),64,StringUtils::itos(iMenuType).c_str()); }
	};
	class CMD_Goto_Media_Menu_DL : public PreformedCommand {
	public:
		CMD_Goto_Media_Menu_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,int iMenuType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,87,2,41,StringUtils::itos(iStreamID).c_str(),64,StringUtils::itos(iMenuType).c_str()); }
	};
	class CMD_Goto_Media_Menu_DT : public PreformedCommand {
	public:
		CMD_Goto_Media_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,int iMenuType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,87,2,41,StringUtils::itos(iStreamID).c_str(),64,StringUtils::itos(iMenuType).c_str()); }
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
	class CMD_Set_Current_Location_DT : public PreformedCommand {
	public:
		CMD_Set_Current_Location_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iLocationID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,88,1,65,StringUtils::itos(iLocationID).c_str()); }
	};
	class CMD_Set_Current_Location_Cat : public PreformedCommand {
	public:
		CMD_Set_Current_Location_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iLocationID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,88,1,65,StringUtils::itos(iLocationID).c_str()); }
	};
	class CMD_Vol_Up : public PreformedCommand {
	public:
		CMD_Vol_Up(long DeviceIDFrom, long DeviceIDTo,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Vol_Up_DL : public PreformedCommand {
	public:
		CMD_Vol_Up_DL(long DeviceIDFrom, string DeviceIDTo,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Vol_Up_DT : public PreformedCommand {
	public:
		CMD_Vol_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Vol_Up_Cat : public PreformedCommand {
	public:
		CMD_Vol_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,89,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Vol_Down : public PreformedCommand {
	public:
		CMD_Vol_Down(long DeviceIDFrom, long DeviceIDTo,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Vol_Down_DL : public PreformedCommand {
	public:
		CMD_Vol_Down_DL(long DeviceIDFrom, string DeviceIDTo,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Vol_Down_DT : public PreformedCommand {
	public:
		CMD_Vol_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Vol_Down_Cat : public PreformedCommand {
	public:
		CMD_Vol_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iRepeat_Command) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,90,1,72,StringUtils::itos(iRepeat_Command).c_str()); }
	};
	class CMD_Input_Select : public PreformedCommand {
	public:
		CMD_Input_Select(long DeviceIDFrom, long DeviceIDTo,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,1,71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Input_Select_DL : public PreformedCommand {
	public:
		CMD_Input_Select_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,1,71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Input_Select_DT : public PreformedCommand {
	public:
		CMD_Input_Select_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,1,71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Input_Select_Cat : public PreformedCommand {
	public:
		CMD_Input_Select_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,91,1,71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Pause : public PreformedCommand {
	public:
		CMD_Pause(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Pause_DL : public PreformedCommand {
	public:
		CMD_Pause_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Pause_DT : public PreformedCommand {
	public:
		CMD_Pause_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Pause_Cat : public PreformedCommand {
	public:
		CMD_Pause_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,92,0); }
	};
	class CMD_Scan_FwdFast_Fwd : public PreformedCommand {
	public:
		CMD_Scan_FwdFast_Fwd(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Scan_FwdFast_Fwd_DL : public PreformedCommand {
	public:
		CMD_Scan_FwdFast_Fwd_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Scan_FwdFast_Fwd_DT : public PreformedCommand {
	public:
		CMD_Scan_FwdFast_Fwd_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Scan_FwdFast_Fwd_Cat : public PreformedCommand {
	public:
		CMD_Scan_FwdFast_Fwd_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,93,0); }
	};
	class CMD_Stop : public PreformedCommand {
	public:
		CMD_Stop(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,95,0); }
	};
	class CMD_Stop_DL : public PreformedCommand {
	public:
		CMD_Stop_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,95,0); }
	};
	class CMD_Stop_DT : public PreformedCommand {
	public:
		CMD_Stop_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,95,0); }
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
	class CMD_Change_Disc_DT : public PreformedCommand {
	public:
		CMD_Change_Disc_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,96,0); }
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
	class CMD_Mute_DT : public PreformedCommand {
	public:
		CMD_Mute_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,97,0); }
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
	class CMD_Slow_Scan_Back_DT : public PreformedCommand {
	public:
		CMD_Slow_Scan_Back_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,98,0); }
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
	class CMD_Slow_Scan_Forward_DT : public PreformedCommand {
	public:
		CMD_Slow_Scan_Forward_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,99,0); }
	};
	class CMD_Slow_Scan_Forward_Cat : public PreformedCommand {
	public:
		CMD_Slow_Scan_Forward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,99,0); }
	};
	class CMD_Record : public PreformedCommand {
	public:
		CMD_Record(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,102,0); }
	};
	class CMD_Record_DL : public PreformedCommand {
	public:
		CMD_Record_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,102,0); }
	};
	class CMD_Record_DT : public PreformedCommand {
	public:
		CMD_Record_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,102,0); }
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
	class CMD_Random_DT : public PreformedCommand {
	public:
		CMD_Random_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,103,0); }
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
	class CMD_Repeat_DT : public PreformedCommand {
	public:
		CMD_Repeat_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,104,0); }
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
	class CMD_Fwd_14x_DT : public PreformedCommand {
	public:
		CMD_Fwd_14x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,105,0); }
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
	class CMD_Fwd_12x_DT : public PreformedCommand {
	public:
		CMD_Fwd_12x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,106,0); }
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
	class CMD_Fwd_2x_DT : public PreformedCommand {
	public:
		CMD_Fwd_2x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,107,0); }
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
	class CMD_Fwd_4x_DT : public PreformedCommand {
	public:
		CMD_Fwd_4x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,108,0); }
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
	class CMD_Fwd_8x_DT : public PreformedCommand {
	public:
		CMD_Fwd_8x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,109,0); }
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
	class CMD_Back_14x_DT : public PreformedCommand {
	public:
		CMD_Back_14x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,110,0); }
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
	class CMD_Back_12x_DT : public PreformedCommand {
	public:
		CMD_Back_12x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,111,0); }
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
	class CMD_Back_1x_DT : public PreformedCommand {
	public:
		CMD_Back_1x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,112,0); }
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
	class CMD_Back_2x_DT : public PreformedCommand {
	public:
		CMD_Back_2x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,113,0); }
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
	class CMD_Back_4x_DT : public PreformedCommand {
	public:
		CMD_Back_4x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,114,0); }
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
	class CMD_Back_8x_DT : public PreformedCommand {
	public:
		CMD_Back_8x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,115,0); }
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
	class CMD_DSP_Mode_DT : public PreformedCommand {
	public:
		CMD_DSP_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,116,0); }
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
	class CMD_Skip_To_End_DT : public PreformedCommand {
	public:
		CMD_Skip_To_End_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,117,0); }
	};
	class CMD_Skip_To_End_Cat : public PreformedCommand {
	public:
		CMD_Skip_To_End_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,117,0); }
	};
	class CMD_Back_16x : public PreformedCommand {
	public:
		CMD_Back_16x(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,119,0); }
	};
	class CMD_Back_16x_DL : public PreformedCommand {
	public:
		CMD_Back_16x_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,119,0); }
	};
	class CMD_Back_16x_DT : public PreformedCommand {
	public:
		CMD_Back_16x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,119,0); }
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
	class CMD_Back_32X_DT : public PreformedCommand {
	public:
		CMD_Back_32X_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,120,0); }
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
	class CMD_Fwd_16x_DT : public PreformedCommand {
	public:
		CMD_Fwd_16x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,121,0); }
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
	class CMD_Fwd_32x_DT : public PreformedCommand {
	public:
		CMD_Fwd_32x_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,122,0); }
	};
	class CMD_Fwd_32x_Cat : public PreformedCommand {
	public:
		CMD_Fwd_32x_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,122,0); }
	};
	class CMD_Info : public PreformedCommand {
	public:
		CMD_Info(long DeviceIDFrom, long DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,1,9,sText.c_str()); }
	};
	class CMD_Info_DL : public PreformedCommand {
	public:
		CMD_Info_DL(long DeviceIDFrom, string DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,1,9,sText.c_str()); }
	};
	class CMD_Info_DT : public PreformedCommand {
	public:
		CMD_Info_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,1,9,sText.c_str()); }
	};
	class CMD_Info_Cat : public PreformedCommand {
	public:
		CMD_Info_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,123,1,9,sText.c_str()); }
	};
	class CMD_Favorites : public PreformedCommand {
	public:
		CMD_Favorites(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Favorites_DL : public PreformedCommand {
	public:
		CMD_Favorites_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Favorites_DT : public PreformedCommand {
	public:
		CMD_Favorites_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Favorites_Cat : public PreformedCommand {
	public:
		CMD_Favorites_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,124,0); }
	};
	class CMD_Scan_BackRewind : public PreformedCommand {
	public:
		CMD_Scan_BackRewind(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Scan_BackRewind_DL : public PreformedCommand {
	public:
		CMD_Scan_BackRewind_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Scan_BackRewind_DT : public PreformedCommand {
	public:
		CMD_Scan_BackRewind_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Scan_BackRewind_Cat : public PreformedCommand {
	public:
		CMD_Scan_BackRewind_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,125,0); }
	};
	class CMD_Guide : public PreformedCommand {
	public:
		CMD_Guide(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,126,0); }
	};
	class CMD_Guide_DL : public PreformedCommand {
	public:
		CMD_Guide_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,126,0); }
	};
	class CMD_Guide_DT : public PreformedCommand {
	public:
		CMD_Guide_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,126,0); }
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
	class CMD_PIP_ON_DT : public PreformedCommand {
	public:
		CMD_PIP_ON_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,127,0); }
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
	class CMD_PIP_OFF_DT : public PreformedCommand {
	public:
		CMD_PIP_OFF_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,128,0); }
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
	class CMD_PIP_Channel_Up_DT : public PreformedCommand {
	public:
		CMD_PIP_Channel_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,129,0); }
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
	class CMD_PIP_Channel_Down_DT : public PreformedCommand {
	public:
		CMD_PIP_Channel_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,130,0); }
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
	class CMD_PIP_Channel_Number_DT : public PreformedCommand {
	public:
		CMD_PIP_Channel_Number_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,131,0); }
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
	class CMD_Taller_DT : public PreformedCommand {
	public:
		CMD_Taller_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,132,0); }
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
	class CMD_Shorter_DT : public PreformedCommand {
	public:
		CMD_Shorter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,133,0); }
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
	class CMD_Wider_DT : public PreformedCommand {
	public:
		CMD_Wider_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,134,0); }
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
	class CMD_Narrower_DT : public PreformedCommand {
	public:
		CMD_Narrower_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,135,0); }
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
	class CMD_Previous_Channel_DT : public PreformedCommand {
	public:
		CMD_Previous_Channel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,136,0); }
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
	class CMD_Music_DT : public PreformedCommand {
	public:
		CMD_Music_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,137,0); }
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
	class CMD_Eject_DT : public PreformedCommand {
	public:
		CMD_Eject_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,138,0); }
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
	class CMD_Play_DT : public PreformedCommand {
	public:
		CMD_Play_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,139,0); }
	};
	class CMD_Play_Cat : public PreformedCommand {
	public:
		CMD_Play_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,139,0); }
	};
	class CMD_Audio_Track : public PreformedCommand {
	public:
		CMD_Audio_Track(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Audio_Track_DL : public PreformedCommand {
	public:
		CMD_Audio_Track_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Audio_Track_DT : public PreformedCommand {
	public:
		CMD_Audio_Track_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Audio_Track_Cat : public PreformedCommand {
	public:
		CMD_Audio_Track_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,140,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Subtitle : public PreformedCommand {
	public:
		CMD_Subtitle(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Subtitle_DL : public PreformedCommand {
	public:
		CMD_Subtitle_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Subtitle_DT : public PreformedCommand {
	public:
		CMD_Subtitle_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Subtitle_Cat : public PreformedCommand {
	public:
		CMD_Subtitle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,141,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Angle : public PreformedCommand {
	public:
		CMD_Angle(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Angle_DL : public PreformedCommand {
	public:
		CMD_Angle_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Angle_DT : public PreformedCommand {
	public:
		CMD_Angle_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Angle_Cat : public PreformedCommand {
	public:
		CMD_Angle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,142,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Progressive : public PreformedCommand {
	public:
		CMD_Progressive(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,144,0); }
	};
	class CMD_Progressive_DL : public PreformedCommand {
	public:
		CMD_Progressive_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,144,0); }
	};
	class CMD_Progressive_DT : public PreformedCommand {
	public:
		CMD_Progressive_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,144,0); }
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
	class CMD_Back_10_seconds_DT : public PreformedCommand {
	public:
		CMD_Back_10_seconds_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,145,0); }
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
	class CMD_Multi_Channel_DT : public PreformedCommand {
	public:
		CMD_Multi_Channel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,146,0); }
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
	class CMD_Hall_DT : public PreformedCommand {
	public:
		CMD_Hall_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,147,0); }
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
	class CMD_Church_DT : public PreformedCommand {
	public:
		CMD_Church_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,148,0); }
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
	class CMD_Jazz_Club_DT : public PreformedCommand {
	public:
		CMD_Jazz_Club_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,149,0); }
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
	class CMD_Rock_Concert_DT : public PreformedCommand {
	public:
		CMD_Rock_Concert_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,150,0); }
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
	class CMD_Entertainment_DT : public PreformedCommand {
	public:
		CMD_Entertainment_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,151,0); }
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
	class CMD_Music_Video_DT : public PreformedCommand {
	public:
		CMD_Music_Video_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,152,0); }
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
	class CMD_Classical_DT : public PreformedCommand {
	public:
		CMD_Classical_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,153,0); }
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
	class CMD_RGB_DT : public PreformedCommand {
	public:
		CMD_RGB_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,154,0); }
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
	class CMD_Video_1_DT : public PreformedCommand {
	public:
		CMD_Video_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,155,0); }
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
	class CMD_Video_2_DT : public PreformedCommand {
	public:
		CMD_Video_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,156,0); }
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
	class CMD_Video_3_DT : public PreformedCommand {
	public:
		CMD_Video_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,157,0); }
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
	class CMD_Video_4_DT : public PreformedCommand {
	public:
		CMD_Video_4_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,158,0); }
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
	class CMD_Video_5_DT : public PreformedCommand {
	public:
		CMD_Video_5_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,159,0); }
	};
	class CMD_Video_5_Cat : public PreformedCommand {
	public:
		CMD_Video_5_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,159,0); }
	};
	class CMD_Tape_Tape_1 : public PreformedCommand {
	public:
		CMD_Tape_Tape_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_Tape_Tape_1_DL : public PreformedCommand {
	public:
		CMD_Tape_Tape_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_Tape_Tape_1_DT : public PreformedCommand {
	public:
		CMD_Tape_Tape_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_Tape_Tape_1_Cat : public PreformedCommand {
	public:
		CMD_Tape_Tape_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,160,0); }
	};
	class CMD_TV : public PreformedCommand {
	public:
		CMD_TV(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,161,0); }
	};
	class CMD_TV_DL : public PreformedCommand {
	public:
		CMD_TV_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,161,0); }
	};
	class CMD_TV_DT : public PreformedCommand {
	public:
		CMD_TV_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,161,0); }
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
	class CMD_CD_DT : public PreformedCommand {
	public:
		CMD_CD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,162,0); }
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
	class CMD_Phono_DT : public PreformedCommand {
	public:
		CMD_Phono_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,163,0); }
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
	class CMD_Aux_DT : public PreformedCommand {
	public:
		CMD_Aux_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,164,0); }
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
	class CMD_DVD_DT : public PreformedCommand {
	public:
		CMD_DVD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,165,0); }
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
	class CMD_Tuner_DT : public PreformedCommand {
	public:
		CMD_Tuner_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,166,0); }
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
	class CMD_Video_6_DT : public PreformedCommand {
	public:
		CMD_Video_6_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,167,0); }
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
	class CMD_Video_7_DT : public PreformedCommand {
	public:
		CMD_Video_7_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,168,0); }
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
	class CMD_AM_DT : public PreformedCommand {
	public:
		CMD_AM_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,169,0); }
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
	class CMD_FM_DT : public PreformedCommand {
	public:
		CMD_FM_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,170,0); }
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
	class CMD_DVD_Multi_DT : public PreformedCommand {
	public:
		CMD_DVD_Multi_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,171,0); }
	};
	class CMD_DVD_Multi_Cat : public PreformedCommand {
	public:
		CMD_DVD_Multi_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,171,0); }
	};
	class CMD_Aux_2 : public PreformedCommand {
	public:
		CMD_Aux_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Aux_2_DL : public PreformedCommand {
	public:
		CMD_Aux_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Aux_2_DT : public PreformedCommand {
	public:
		CMD_Aux_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Aux_2_Cat : public PreformedCommand {
	public:
		CMD_Aux_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,172,0); }
	};
	class CMD_Tape_2 : public PreformedCommand {
	public:
		CMD_Tape_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Tape_2_DL : public PreformedCommand {
	public:
		CMD_Tape_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Tape_2_DT : public PreformedCommand {
	public:
		CMD_Tape_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Tape_2_Cat : public PreformedCommand {
	public:
		CMD_Tape_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,173,0); }
	};
	class CMD_Minidisc : public PreformedCommand {
	public:
		CMD_Minidisc(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Minidisc_DL : public PreformedCommand {
	public:
		CMD_Minidisc_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Minidisc_DT : public PreformedCommand {
	public:
		CMD_Minidisc_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Minidisc_Cat : public PreformedCommand {
	public:
		CMD_Minidisc_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,174,0); }
	};
	class CMD_Tuner_2 : public PreformedCommand {
	public:
		CMD_Tuner_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_Tuner_2_DL : public PreformedCommand {
	public:
		CMD_Tuner_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_Tuner_2_DT : public PreformedCommand {
	public:
		CMD_Tuner_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_Tuner_2_Cat : public PreformedCommand {
	public:
		CMD_Tuner_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,175,0); }
	};
	class CMD_VDP : public PreformedCommand {
	public:
		CMD_VDP(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,176,0); }
	};
	class CMD_VDP_DL : public PreformedCommand {
	public:
		CMD_VDP_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,176,0); }
	};
	class CMD_VDP_DT : public PreformedCommand {
	public:
		CMD_VDP_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,176,0); }
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
	class CMD_Laserdisc_DT : public PreformedCommand {
	public:
		CMD_Laserdisc_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,177,0); }
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
	class CMD_DSS_DT : public PreformedCommand {
	public:
		CMD_DSS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,178,0); }
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
	class CMD_DVI_DT : public PreformedCommand {
	public:
		CMD_DVI_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,179,0); }
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
	class CMD_Svideo_DT : public PreformedCommand {
	public:
		CMD_Svideo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,180,0); }
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
	class CMD_Composite_DT : public PreformedCommand {
	public:
		CMD_Composite_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,181,0); }
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
	class CMD_RF_Input_DT : public PreformedCommand {
	public:
		CMD_RF_Input_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,182,0); }
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
		CMD_Get_Floorplan_Layout(long DeviceIDFrom, long DeviceIDTo,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_Get_Floorplan_Layout_DL : public PreformedCommand {
	public:
		CMD_Get_Floorplan_Layout_DL(long DeviceIDFrom, string DeviceIDTo,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_Get_Floorplan_Layout_DT : public PreformedCommand {
	public:
		CMD_Get_Floorplan_Layout_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_Get_Floorplan_Layout_Cat : public PreformedCommand {
	public:
		CMD_Get_Floorplan_Layout_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Floorplan_Layout(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Floorplan_Layout : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_NOREP_Get_Floorplan_Layout_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_NOREP_Get_Floorplan_Layout_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_NOREP_Get_Floorplan_Layout_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Floorplan_Layout_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,183,0); }
	};
	class CMD_Set_Level : public PreformedCommand {
	public:
		CMD_Set_Level(long DeviceIDFrom, long DeviceIDTo,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,1,76,sLevel.c_str()); }
	};
	class CMD_Set_Level_DL : public PreformedCommand {
	public:
		CMD_Set_Level_DL(long DeviceIDFrom, string DeviceIDTo,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,1,76,sLevel.c_str()); }
	};
	class CMD_Set_Level_DT : public PreformedCommand {
	public:
		CMD_Set_Level_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sLevel) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,1,76,sLevel.c_str()); }
	};
	class CMD_Set_Level_Cat : public PreformedCommand {
	public:
		CMD_Set_Level_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,184,1,76,sLevel.c_str()); }
	};
	class CMD_Schedule_Recording : public PreformedCommand {
	public:
		CMD_Schedule_Recording(long DeviceIDFrom, long DeviceIDTo,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,185,1,68,sProgramID.c_str()); }
	};
	class CMD_Schedule_Recording_DL : public PreformedCommand {
	public:
		CMD_Schedule_Recording_DL(long DeviceIDFrom, string DeviceIDTo,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,185,1,68,sProgramID.c_str()); }
	};
	class CMD_Schedule_Recording_DT : public PreformedCommand {
	public:
		CMD_Schedule_Recording_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,185,1,68,sProgramID.c_str()); }
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
		CMD_Get_Current_Floorplan(long DeviceIDFrom, long DeviceIDTo,string sID,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,3,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str(),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_Get_Current_Floorplan_DL : public PreformedCommand {
	public:
		CMD_Get_Current_Floorplan_DL(long DeviceIDFrom, string DeviceIDTo,string sID,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,3,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str(),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_Get_Current_Floorplan_DT : public PreformedCommand {
	public:
		CMD_Get_Current_Floorplan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,3,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str(),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_Get_Current_Floorplan_Cat : public PreformedCommand {
	public:
		CMD_Get_Current_Floorplan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,int iPK_FloorplanType,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,3,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str(),5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Current_Floorplan(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Current_Floorplan : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan(long DeviceIDFrom, long DeviceIDTo,string sID,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,2,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_NOREP_Get_Current_Floorplan_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan_DL(long DeviceIDFrom, string DeviceIDTo,string sID,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,2,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_NOREP_Get_Current_Floorplan_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sID,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,2,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_NOREP_Get_Current_Floorplan_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Current_Floorplan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sID,int iPK_FloorplanType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,186,2,10,sID.c_str(),46,StringUtils::itos(iPK_FloorplanType).c_str()); }
	};
	class CMD_Tune_to_channel : public PreformedCommand {
	public:
		CMD_Tune_to_channel(long DeviceIDFrom, long DeviceIDTo,string sOptions,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,2,39,sOptions.c_str(),68,sProgramID.c_str()); }
	};
	class CMD_Tune_to_channel_DL : public PreformedCommand {
	public:
		CMD_Tune_to_channel_DL(long DeviceIDFrom, string DeviceIDTo,string sOptions,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,2,39,sOptions.c_str(),68,sProgramID.c_str()); }
	};
	class CMD_Tune_to_channel_DT : public PreformedCommand {
	public:
		CMD_Tune_to_channel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOptions,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,2,39,sOptions.c_str(),68,sProgramID.c_str()); }
	};
	class CMD_Tune_to_channel_Cat : public PreformedCommand {
	public:
		CMD_Tune_to_channel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOptions,string sProgramID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,187,2,39,sOptions.c_str(),68,sProgramID.c_str()); }
	};
	class RESP_Get_Infrared_Codes : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;
	public:
		RESP_Get_Infrared_Codes(char **pData,int *iData_Size) { 
		m_pData=pData; m_iData_Size=iData_Size; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; };
	};
	class CMD_Get_Infrared_Codes : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,2,2,StringUtils::itos(iPK_Device).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Infrared_Codes(pData,iData_Size); }
	};
	class CMD_Get_Infrared_Codes_DL : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,2,2,StringUtils::itos(iPK_Device).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Infrared_Codes(pData,iData_Size); }
	};
	class CMD_Get_Infrared_Codes_DT : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,2,2,StringUtils::itos(iPK_Device).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Infrared_Codes(pData,iData_Size); }
	};
	class CMD_Get_Infrared_Codes_Cat : public PreformedCommand {
	public:
		CMD_Get_Infrared_Codes_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,2,2,StringUtils::itos(iPK_Device).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Infrared_Codes(pData,iData_Size); }
	};
	class CMD_NOREP_Get_Infrared_Codes : public PreformedCommand {
	public:
		CMD_NOREP_Get_Infrared_Codes(long DeviceIDFrom, long DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Infrared_Codes_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Infrared_Codes_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Infrared_Codes_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Infrared_Codes_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,188,1,2,StringUtils::itos(iPK_Device).c_str()); }
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
	class CMD_EnterGo_DT : public PreformedCommand {
	public:
		CMD_EnterGo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,190,0); }
	};
	class CMD_EnterGo_Cat : public PreformedCommand {
	public:
		CMD_EnterGo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,190,0); }
	};
	class CMD_Send_Code : public PreformedCommand {
	public:
		CMD_Send_Code(long DeviceIDFrom, long DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,1,9,sText.c_str()); }
	};
	class CMD_Send_Code_DL : public PreformedCommand {
	public:
		CMD_Send_Code_DL(long DeviceIDFrom, string DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,1,9,sText.c_str()); }
	};
	class CMD_Send_Code_DT : public PreformedCommand {
	public:
		CMD_Send_Code_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,1,9,sText.c_str()); }
	};
	class CMD_Send_Code_Cat : public PreformedCommand {
	public:
		CMD_Send_Code_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,191,1,9,sText.c_str()); }
	};
	class CMD_On : public PreformedCommand {
	public:
		CMD_On(long DeviceIDFrom, long DeviceIDTo,int iPK_Pipe,string sPK_Device_Pipes) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,2,97,StringUtils::itos(iPK_Pipe).c_str(),98,sPK_Device_Pipes.c_str()); }
	};
	class CMD_On_DL : public PreformedCommand {
	public:
		CMD_On_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Pipe,string sPK_Device_Pipes) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,2,97,StringUtils::itos(iPK_Pipe).c_str(),98,sPK_Device_Pipes.c_str()); }
	};
	class CMD_On_DT : public PreformedCommand {
	public:
		CMD_On_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Pipe,string sPK_Device_Pipes) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,2,97,StringUtils::itos(iPK_Pipe).c_str(),98,sPK_Device_Pipes.c_str()); }
	};
	class CMD_On_Cat : public PreformedCommand {
	public:
		CMD_On_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Pipe,string sPK_Device_Pipes) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,192,2,97,StringUtils::itos(iPK_Pipe).c_str(),98,sPK_Device_Pipes.c_str()); }
	};
	class CMD_Off : public PreformedCommand {
	public:
		CMD_Off(long DeviceIDFrom, long DeviceIDTo,int iPK_Pipe) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,1,97,StringUtils::itos(iPK_Pipe).c_str()); }
	};
	class CMD_Off_DL : public PreformedCommand {
	public:
		CMD_Off_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Pipe) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,1,97,StringUtils::itos(iPK_Pipe).c_str()); }
	};
	class CMD_Off_DT : public PreformedCommand {
	public:
		CMD_Off_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Pipe) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,1,97,StringUtils::itos(iPK_Pipe).c_str()); }
	};
	class CMD_Off_Cat : public PreformedCommand {
	public:
		CMD_Off_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Pipe) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,193,1,97,StringUtils::itos(iPK_Pipe).c_str()); }
	};
	class CMD_Toggle_Power : public PreformedCommand {
	public:
		CMD_Toggle_Power(long DeviceIDFrom, long DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,1,8,sOnOff.c_str()); }
	};
	class CMD_Toggle_Power_DL : public PreformedCommand {
	public:
		CMD_Toggle_Power_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,1,8,sOnOff.c_str()); }
	};
	class CMD_Toggle_Power_DT : public PreformedCommand {
	public:
		CMD_Toggle_Power_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,1,8,sOnOff.c_str()); }
	};
	class CMD_Toggle_Power_Cat : public PreformedCommand {
	public:
		CMD_Toggle_Power_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,194,1,8,sOnOff.c_str()); }
	};
	class CMD_Dial : public PreformedCommand {
	public:
		CMD_Dial(long DeviceIDFrom, long DeviceIDTo,string sPhoneNumber) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,195,1,75,sPhoneNumber.c_str()); }
	};
	class CMD_Dial_DL : public PreformedCommand {
	public:
		CMD_Dial_DL(long DeviceIDFrom, string DeviceIDTo,string sPhoneNumber) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,195,1,75,sPhoneNumber.c_str()); }
	};
	class CMD_Dial_DT : public PreformedCommand {
	public:
		CMD_Dial_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPhoneNumber) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,195,1,75,sPhoneNumber.c_str()); }
	};
	class CMD_Dial_Cat : public PreformedCommand {
	public:
		CMD_Dial_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPhoneNumber) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,195,1,75,sPhoneNumber.c_str()); }
	};
	class CMD_Move_Up : public PreformedCommand {
	public:
		CMD_Move_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,200,0); }
	};
	class CMD_Move_Up_DL : public PreformedCommand {
	public:
		CMD_Move_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,200,0); }
	};
	class CMD_Move_Up_DT : public PreformedCommand {
	public:
		CMD_Move_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,200,0); }
	};
	class CMD_Move_Up_Cat : public PreformedCommand {
	public:
		CMD_Move_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,200,0); }
	};
	class CMD_Move_Down : public PreformedCommand {
	public:
		CMD_Move_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,201,0); }
	};
	class CMD_Move_Down_DL : public PreformedCommand {
	public:
		CMD_Move_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,201,0); }
	};
	class CMD_Move_Down_DT : public PreformedCommand {
	public:
		CMD_Move_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,201,0); }
	};
	class CMD_Move_Down_Cat : public PreformedCommand {
	public:
		CMD_Move_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,201,0); }
	};
	class CMD_Move_Left : public PreformedCommand {
	public:
		CMD_Move_Left(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,202,0); }
	};
	class CMD_Move_Left_DL : public PreformedCommand {
	public:
		CMD_Move_Left_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,202,0); }
	};
	class CMD_Move_Left_DT : public PreformedCommand {
	public:
		CMD_Move_Left_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,202,0); }
	};
	class CMD_Move_Left_Cat : public PreformedCommand {
	public:
		CMD_Move_Left_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,202,0); }
	};
	class CMD_Move_Right : public PreformedCommand {
	public:
		CMD_Move_Right(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,203,0); }
	};
	class CMD_Move_Right_DL : public PreformedCommand {
	public:
		CMD_Move_Right_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,203,0); }
	};
	class CMD_Move_Right_DT : public PreformedCommand {
	public:
		CMD_Move_Right_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,203,0); }
	};
	class CMD_Move_Right_Cat : public PreformedCommand {
	public:
		CMD_Move_Right_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,203,0); }
	};
	class CMD_0 : public PreformedCommand {
	public:
		CMD_0(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,204,0); }
	};
	class CMD_0_DL : public PreformedCommand {
	public:
		CMD_0_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,204,0); }
	};
	class CMD_0_DT : public PreformedCommand {
	public:
		CMD_0_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,204,0); }
	};
	class CMD_0_Cat : public PreformedCommand {
	public:
		CMD_0_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,204,0); }
	};
	class CMD_1 : public PreformedCommand {
	public:
		CMD_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,205,0); }
	};
	class CMD_1_DL : public PreformedCommand {
	public:
		CMD_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,205,0); }
	};
	class CMD_1_DT : public PreformedCommand {
	public:
		CMD_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,205,0); }
	};
	class CMD_1_Cat : public PreformedCommand {
	public:
		CMD_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,205,0); }
	};
	class CMD_2 : public PreformedCommand {
	public:
		CMD_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,206,0); }
	};
	class CMD_2_DL : public PreformedCommand {
	public:
		CMD_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,206,0); }
	};
	class CMD_2_DT : public PreformedCommand {
	public:
		CMD_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,206,0); }
	};
	class CMD_2_Cat : public PreformedCommand {
	public:
		CMD_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,206,0); }
	};
	class CMD_3 : public PreformedCommand {
	public:
		CMD_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,207,0); }
	};
	class CMD_3_DL : public PreformedCommand {
	public:
		CMD_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,207,0); }
	};
	class CMD_3_DT : public PreformedCommand {
	public:
		CMD_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,207,0); }
	};
	class CMD_3_Cat : public PreformedCommand {
	public:
		CMD_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,207,0); }
	};
	class CMD_4 : public PreformedCommand {
	public:
		CMD_4(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,208,0); }
	};
	class CMD_4_DL : public PreformedCommand {
	public:
		CMD_4_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,208,0); }
	};
	class CMD_4_DT : public PreformedCommand {
	public:
		CMD_4_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,208,0); }
	};
	class CMD_4_Cat : public PreformedCommand {
	public:
		CMD_4_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,208,0); }
	};
	class CMD_5 : public PreformedCommand {
	public:
		CMD_5(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,209,0); }
	};
	class CMD_5_DL : public PreformedCommand {
	public:
		CMD_5_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,209,0); }
	};
	class CMD_5_DT : public PreformedCommand {
	public:
		CMD_5_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,209,0); }
	};
	class CMD_5_Cat : public PreformedCommand {
	public:
		CMD_5_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,209,0); }
	};
	class CMD_6 : public PreformedCommand {
	public:
		CMD_6(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,210,0); }
	};
	class CMD_6_DL : public PreformedCommand {
	public:
		CMD_6_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,210,0); }
	};
	class CMD_6_DT : public PreformedCommand {
	public:
		CMD_6_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,210,0); }
	};
	class CMD_6_Cat : public PreformedCommand {
	public:
		CMD_6_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,210,0); }
	};
	class CMD_7 : public PreformedCommand {
	public:
		CMD_7(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,211,0); }
	};
	class CMD_7_DL : public PreformedCommand {
	public:
		CMD_7_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,211,0); }
	};
	class CMD_7_DT : public PreformedCommand {
	public:
		CMD_7_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,211,0); }
	};
	class CMD_7_Cat : public PreformedCommand {
	public:
		CMD_7_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,211,0); }
	};
	class CMD_8 : public PreformedCommand {
	public:
		CMD_8(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,212,0); }
	};
	class CMD_8_DL : public PreformedCommand {
	public:
		CMD_8_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,212,0); }
	};
	class CMD_8_DT : public PreformedCommand {
	public:
		CMD_8_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,212,0); }
	};
	class CMD_8_Cat : public PreformedCommand {
	public:
		CMD_8_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,212,0); }
	};
	class CMD_9 : public PreformedCommand {
	public:
		CMD_9(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,213,0); }
	};
	class CMD_9_DL : public PreformedCommand {
	public:
		CMD_9_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,213,0); }
	};
	class CMD_9_DT : public PreformedCommand {
	public:
		CMD_9_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,213,0); }
	};
	class CMD_9_Cat : public PreformedCommand {
	public:
		CMD_9_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,213,0); }
	};
	class CMD_Save_playlist : public PreformedCommand {
	public:
		CMD_Save_playlist(long DeviceIDFrom, long DeviceIDTo,int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,214,4,17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),50,sName.c_str(),77,(bSave_as_new ? "1" : "0")); }
	};
	class CMD_Save_playlist_DL : public PreformedCommand {
	public:
		CMD_Save_playlist_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,214,4,17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),50,sName.c_str(),77,(bSave_as_new ? "1" : "0")); }
	};
	class CMD_Save_playlist_DT : public PreformedCommand {
	public:
		CMD_Save_playlist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,214,4,17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),50,sName.c_str(),77,(bSave_as_new ? "1" : "0")); }
	};
	class CMD_Save_playlist_Cat : public PreformedCommand {
	public:
		CMD_Save_playlist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Users,string sPK_EntertainArea,string sName,bool bSave_as_new) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,214,4,17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),50,sName.c_str(),77,(bSave_as_new ? "1" : "0")); }
	};
	class CMD_Balance_Sel : public PreformedCommand {
	public:
		CMD_Balance_Sel(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,216,0); }
	};
	class CMD_Balance_Sel_DL : public PreformedCommand {
	public:
		CMD_Balance_Sel_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,216,0); }
	};
	class CMD_Balance_Sel_DT : public PreformedCommand {
	public:
		CMD_Balance_Sel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,216,0); }
	};
	class CMD_Balance_Sel_Cat : public PreformedCommand {
	public:
		CMD_Balance_Sel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,216,0); }
	};
	class CMD_Balance_Left : public PreformedCommand {
	public:
		CMD_Balance_Left(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,217,0); }
	};
	class CMD_Balance_Left_DL : public PreformedCommand {
	public:
		CMD_Balance_Left_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,217,0); }
	};
	class CMD_Balance_Left_DT : public PreformedCommand {
	public:
		CMD_Balance_Left_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,217,0); }
	};
	class CMD_Balance_Left_Cat : public PreformedCommand {
	public:
		CMD_Balance_Left_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,217,0); }
	};
	class CMD_Balance_Right : public PreformedCommand {
	public:
		CMD_Balance_Right(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,218,0); }
	};
	class CMD_Balance_Right_DL : public PreformedCommand {
	public:
		CMD_Balance_Right_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,218,0); }
	};
	class CMD_Balance_Right_DT : public PreformedCommand {
	public:
		CMD_Balance_Right_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,218,0); }
	};
	class CMD_Balance_Right_Cat : public PreformedCommand {
	public:
		CMD_Balance_Right_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,218,0); }
	};
	class CMD_Contrast_Sel : public PreformedCommand {
	public:
		CMD_Contrast_Sel(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,219,0); }
	};
	class CMD_Contrast_Sel_DL : public PreformedCommand {
	public:
		CMD_Contrast_Sel_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,219,0); }
	};
	class CMD_Contrast_Sel_DT : public PreformedCommand {
	public:
		CMD_Contrast_Sel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,219,0); }
	};
	class CMD_Contrast_Sel_Cat : public PreformedCommand {
	public:
		CMD_Contrast_Sel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,219,0); }
	};
	class CMD_Contrast_Up : public PreformedCommand {
	public:
		CMD_Contrast_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,220,0); }
	};
	class CMD_Contrast_Up_DL : public PreformedCommand {
	public:
		CMD_Contrast_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,220,0); }
	};
	class CMD_Contrast_Up_DT : public PreformedCommand {
	public:
		CMD_Contrast_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,220,0); }
	};
	class CMD_Contrast_Up_Cat : public PreformedCommand {
	public:
		CMD_Contrast_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,220,0); }
	};
	class CMD_Contrast_Down : public PreformedCommand {
	public:
		CMD_Contrast_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,221,0); }
	};
	class CMD_Contrast_Down_DL : public PreformedCommand {
	public:
		CMD_Contrast_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,221,0); }
	};
	class CMD_Contrast_Down_DT : public PreformedCommand {
	public:
		CMD_Contrast_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,221,0); }
	};
	class CMD_Contrast_Down_Cat : public PreformedCommand {
	public:
		CMD_Contrast_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,221,0); }
	};
	class CMD_Brightness_Sel : public PreformedCommand {
	public:
		CMD_Brightness_Sel(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,222,0); }
	};
	class CMD_Brightness_Sel_DL : public PreformedCommand {
	public:
		CMD_Brightness_Sel_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,222,0); }
	};
	class CMD_Brightness_Sel_DT : public PreformedCommand {
	public:
		CMD_Brightness_Sel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,222,0); }
	};
	class CMD_Brightness_Sel_Cat : public PreformedCommand {
	public:
		CMD_Brightness_Sel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,222,0); }
	};
	class CMD_Brightness_Up : public PreformedCommand {
	public:
		CMD_Brightness_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,223,0); }
	};
	class CMD_Brightness_Up_DL : public PreformedCommand {
	public:
		CMD_Brightness_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,223,0); }
	};
	class CMD_Brightness_Up_DT : public PreformedCommand {
	public:
		CMD_Brightness_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,223,0); }
	};
	class CMD_Brightness_Up_Cat : public PreformedCommand {
	public:
		CMD_Brightness_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,223,0); }
	};
	class CMD_Brightness_Down : public PreformedCommand {
	public:
		CMD_Brightness_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,224,0); }
	};
	class CMD_Brightness_Down_DL : public PreformedCommand {
	public:
		CMD_Brightness_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,224,0); }
	};
	class CMD_Brightness_Down_DT : public PreformedCommand {
	public:
		CMD_Brightness_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,224,0); }
	};
	class CMD_Brightness_Down_Cat : public PreformedCommand {
	public:
		CMD_Brightness_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,224,0); }
	};
	class CMD_Display : public PreformedCommand {
	public:
		CMD_Display(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,225,0); }
	};
	class CMD_Display_DL : public PreformedCommand {
	public:
		CMD_Display_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,225,0); }
	};
	class CMD_Display_DT : public PreformedCommand {
	public:
		CMD_Display_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,225,0); }
	};
	class CMD_Display_Cat : public PreformedCommand {
	public:
		CMD_Display_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,225,0); }
	};
	class CMD_Originate_Renamed_To_Not_Break_Compiles : public PreformedCommand {
	public:
		CMD_Originate_Renamed_To_Not_Break_Compiles(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,226,0); }
	};
	class CMD_Originate_Renamed_To_Not_Break_Compiles_DL : public PreformedCommand {
	public:
		CMD_Originate_Renamed_To_Not_Break_Compiles_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,226,0); }
	};
	class CMD_Originate_Renamed_To_Not_Break_Compiles_DT : public PreformedCommand {
	public:
		CMD_Originate_Renamed_To_Not_Break_Compiles_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,226,0); }
	};
	class CMD_Originate_Renamed_To_Not_Break_Compiles_Cat : public PreformedCommand {
	public:
		CMD_Originate_Renamed_To_Not_Break_Compiles_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,226,0); }
	};
	class CMD_Originate : public PreformedCommand {
	public:
		CMD_Originate(long DeviceIDFrom, long DeviceIDTo,string sPhoneNumber,string sOriginatorNumber,string sOriginatorType,string sCallerID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,227,4,75,sPhoneNumber.c_str(),79,sOriginatorNumber.c_str(),80,sOriginatorType.c_str(),81,sCallerID.c_str()); }
	};
	class CMD_Originate_DL : public PreformedCommand {
	public:
		CMD_Originate_DL(long DeviceIDFrom, string DeviceIDTo,string sPhoneNumber,string sOriginatorNumber,string sOriginatorType,string sCallerID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,227,4,75,sPhoneNumber.c_str(),79,sOriginatorNumber.c_str(),80,sOriginatorType.c_str(),81,sCallerID.c_str()); }
	};
	class CMD_Originate_DT : public PreformedCommand {
	public:
		CMD_Originate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPhoneNumber,string sOriginatorNumber,string sOriginatorType,string sCallerID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,227,4,75,sPhoneNumber.c_str(),79,sOriginatorNumber.c_str(),80,sOriginatorType.c_str(),81,sCallerID.c_str()); }
	};
	class CMD_Originate_Cat : public PreformedCommand {
	public:
		CMD_Originate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPhoneNumber,string sOriginatorNumber,string sOriginatorType,string sCallerID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,227,4,75,sPhoneNumber.c_str(),79,sOriginatorNumber.c_str(),80,sOriginatorType.c_str(),81,sCallerID.c_str()); }
	};
	class CMD_Bass_Sel : public PreformedCommand {
	public:
		CMD_Bass_Sel(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,228,0); }
	};
	class CMD_Bass_Sel_DL : public PreformedCommand {
	public:
		CMD_Bass_Sel_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,228,0); }
	};
	class CMD_Bass_Sel_DT : public PreformedCommand {
	public:
		CMD_Bass_Sel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,228,0); }
	};
	class CMD_Bass_Sel_Cat : public PreformedCommand {
	public:
		CMD_Bass_Sel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,228,0); }
	};
	class CMD_Bass_Up : public PreformedCommand {
	public:
		CMD_Bass_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,229,0); }
	};
	class CMD_Bass_Up_DL : public PreformedCommand {
	public:
		CMD_Bass_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,229,0); }
	};
	class CMD_Bass_Up_DT : public PreformedCommand {
	public:
		CMD_Bass_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,229,0); }
	};
	class CMD_Bass_Up_Cat : public PreformedCommand {
	public:
		CMD_Bass_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,229,0); }
	};
	class CMD_Bass_Down : public PreformedCommand {
	public:
		CMD_Bass_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,230,0); }
	};
	class CMD_Bass_Down_DL : public PreformedCommand {
	public:
		CMD_Bass_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,230,0); }
	};
	class CMD_Bass_Down_DT : public PreformedCommand {
	public:
		CMD_Bass_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,230,0); }
	};
	class CMD_Bass_Down_Cat : public PreformedCommand {
	public:
		CMD_Bass_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,230,0); }
	};
	class CMD_Load_Playlist : public PreformedCommand {
	public:
		CMD_Load_Playlist(long DeviceIDFrom, long DeviceIDTo,string sPK_EntertainArea,int iEK_Playlist) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,231,2,45,sPK_EntertainArea.c_str(),78,StringUtils::itos(iEK_Playlist).c_str()); }
	};
	class CMD_Load_Playlist_DL : public PreformedCommand {
	public:
		CMD_Load_Playlist_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_EntertainArea,int iEK_Playlist) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,231,2,45,sPK_EntertainArea.c_str(),78,StringUtils::itos(iEK_Playlist).c_str()); }
	};
	class CMD_Load_Playlist_DT : public PreformedCommand {
	public:
		CMD_Load_Playlist_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_EntertainArea,int iEK_Playlist) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,231,2,45,sPK_EntertainArea.c_str(),78,StringUtils::itos(iEK_Playlist).c_str()); }
	};
	class CMD_Load_Playlist_Cat : public PreformedCommand {
	public:
		CMD_Load_Playlist_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_EntertainArea,int iEK_Playlist) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,231,2,45,sPK_EntertainArea.c_str(),78,StringUtils::itos(iEK_Playlist).c_str()); }
	};
	class CMD_PL_Originate : public PreformedCommand {
	public:
		CMD_PL_Originate(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPhoneExtension,string sPhoneCallerID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,232,3,2,StringUtils::itos(iPK_Device).c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str()); }
	};
	class CMD_PL_Originate_DL : public PreformedCommand {
	public:
		CMD_PL_Originate_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPhoneExtension,string sPhoneCallerID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,232,3,2,StringUtils::itos(iPK_Device).c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str()); }
	};
	class CMD_PL_Originate_DT : public PreformedCommand {
	public:
		CMD_PL_Originate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPhoneExtension,string sPhoneCallerID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,232,3,2,StringUtils::itos(iPK_Device).c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str()); }
	};
	class CMD_PL_Originate_Cat : public PreformedCommand {
	public:
		CMD_PL_Originate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPhoneExtension,string sPhoneCallerID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,232,3,2,StringUtils::itos(iPK_Device).c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str()); }
	};
	class CMD_PBX_Originate : public PreformedCommand {
	public:
		CMD_PBX_Originate(long DeviceIDFrom, long DeviceIDTo,string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,int iCommandID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,233,5,75,sPhoneNumber.c_str(),82,sPhoneType.c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str(),85,StringUtils::itos(iCommandID).c_str()); }
	};
	class CMD_PBX_Originate_DL : public PreformedCommand {
	public:
		CMD_PBX_Originate_DL(long DeviceIDFrom, string DeviceIDTo,string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,int iCommandID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,233,5,75,sPhoneNumber.c_str(),82,sPhoneType.c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str(),85,StringUtils::itos(iCommandID).c_str()); }
	};
	class CMD_PBX_Originate_DT : public PreformedCommand {
	public:
		CMD_PBX_Originate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,int iCommandID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,233,5,75,sPhoneNumber.c_str(),82,sPhoneType.c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str(),85,StringUtils::itos(iCommandID).c_str()); }
	};
	class CMD_PBX_Originate_Cat : public PreformedCommand {
	public:
		CMD_PBX_Originate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPhoneNumber,string sPhoneType,string sPhoneExtension,string sPhoneCallerID,int iCommandID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,233,5,75,sPhoneNumber.c_str(),82,sPhoneType.c_str(),83,sPhoneExtension.c_str(),84,sPhoneCallerID.c_str(),85,StringUtils::itos(iCommandID).c_str()); }
	};
	class CMD_PL_TransferConferenceDevice : public PreformedCommand {
	public:
		CMD_PL_TransferConferenceDevice(long DeviceIDFrom, long DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,234,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_PL_TransferConferenceDevice_DL : public PreformedCommand {
	public:
		CMD_PL_TransferConferenceDevice_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,234,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_PL_TransferConferenceDevice_DT : public PreformedCommand {
	public:
		CMD_PL_TransferConferenceDevice_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,234,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_PL_TransferConferenceDevice_Cat : public PreformedCommand {
	public:
		CMD_PL_TransferConferenceDevice_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,234,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_PBX_Transfer : public PreformedCommand {
	public:
		CMD_PBX_Transfer(long DeviceIDFrom, long DeviceIDTo,string sPhoneExtension,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,235,3,83,sPhoneExtension.c_str(),85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_PBX_Transfer_DL : public PreformedCommand {
	public:
		CMD_PBX_Transfer_DL(long DeviceIDFrom, string DeviceIDTo,string sPhoneExtension,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,235,3,83,sPhoneExtension.c_str(),85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_PBX_Transfer_DT : public PreformedCommand {
	public:
		CMD_PBX_Transfer_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPhoneExtension,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,235,3,83,sPhoneExtension.c_str(),85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_PBX_Transfer_Cat : public PreformedCommand {
	public:
		CMD_PBX_Transfer_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPhoneExtension,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,235,3,83,sPhoneExtension.c_str(),85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_PL_Hangup : public PreformedCommand {
	public:
		CMD_PL_Hangup(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,236,0); }
	};
	class CMD_PL_Hangup_DL : public PreformedCommand {
	public:
		CMD_PL_Hangup_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,236,0); }
	};
	class CMD_PL_Hangup_DT : public PreformedCommand {
	public:
		CMD_PL_Hangup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,236,0); }
	};
	class CMD_PL_Hangup_Cat : public PreformedCommand {
	public:
		CMD_PL_Hangup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,236,0); }
	};
	class CMD_PBX_Hangup : public PreformedCommand {
	public:
		CMD_PBX_Hangup(long DeviceIDFrom, long DeviceIDTo,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,237,2,85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_PBX_Hangup_DL : public PreformedCommand {
	public:
		CMD_PBX_Hangup_DL(long DeviceIDFrom, string DeviceIDTo,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,237,2,85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_PBX_Hangup_DT : public PreformedCommand {
	public:
		CMD_PBX_Hangup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,237,2,85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_PBX_Hangup_Cat : public PreformedCommand {
	public:
		CMD_PBX_Hangup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iCommandID,string sPhoneCallID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,237,2,85,StringUtils::itos(iCommandID).c_str(),87,sPhoneCallID.c_str()); }
	};
	class CMD_Continuous_Refresh : public PreformedCommand {
	public:
		CMD_Continuous_Refresh(long DeviceIDFrom, long DeviceIDTo,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,238,1,102,sTime.c_str()); }
	};
	class CMD_Continuous_Refresh_DL : public PreformedCommand {
	public:
		CMD_Continuous_Refresh_DL(long DeviceIDFrom, string DeviceIDTo,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,238,1,102,sTime.c_str()); }
	};
	class CMD_Continuous_Refresh_DT : public PreformedCommand {
	public:
		CMD_Continuous_Refresh_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sTime) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,238,1,102,sTime.c_str()); }
	};
	class CMD_Continuous_Refresh_Cat : public PreformedCommand {
	public:
		CMD_Continuous_Refresh_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,238,1,102,sTime.c_str()); }
	};
	class RESP_Request_File_And_Checksum : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;string *m_sChecksum;bool *m_bChecksum_Only;
	public:
		RESP_Request_File_And_Checksum(char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only) { 
		m_pData=pData; m_iData_Size=iData_Size; m_sChecksum=sChecksum; m_bChecksum_Only=bChecksum_Only; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; *m_sChecksum=pMessage->m_mapParameters[91]; *m_bChecksum_Only=(pMessage->m_mapParameters[92]=="1" ? true : false); };
	};
	class CMD_Request_File_And_Checksum : public PreformedCommand {
	public:
		CMD_Request_File_And_Checksum(long DeviceIDFrom, long DeviceIDTo,string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,4,13,sFilename.c_str(),-19,*pData,*iData_Size,91,(*sChecksum).c_str(),92,(*bChecksum_Only ? "1" : "0"));		m_pcResponse = new RESP_Request_File_And_Checksum(pData,iData_Size,sChecksum,bChecksum_Only); }
	};
	class CMD_Request_File_And_Checksum_DL : public PreformedCommand {
	public:
		CMD_Request_File_And_Checksum_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,4,13,sFilename.c_str(),-19,*pData,*iData_Size,91,(*sChecksum).c_str(),92,(*bChecksum_Only ? "1" : "0"));		m_pcResponse = new RESP_Request_File_And_Checksum(pData,iData_Size,sChecksum,bChecksum_Only); }
	};
	class CMD_Request_File_And_Checksum_DT : public PreformedCommand {
	public:
		CMD_Request_File_And_Checksum_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,4,13,sFilename.c_str(),-19,*pData,*iData_Size,91,(*sChecksum).c_str(),92,(*bChecksum_Only ? "1" : "0"));		m_pcResponse = new RESP_Request_File_And_Checksum(pData,iData_Size,sChecksum,bChecksum_Only); }
	};
	class CMD_Request_File_And_Checksum_Cat : public PreformedCommand {
	public:
		CMD_Request_File_And_Checksum_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename,char **pData,int *iData_Size,string *sChecksum,bool *bChecksum_Only) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,4,13,sFilename.c_str(),-19,*pData,*iData_Size,91,(*sChecksum).c_str(),92,(*bChecksum_Only ? "1" : "0"));		m_pcResponse = new RESP_Request_File_And_Checksum(pData,iData_Size,sChecksum,bChecksum_Only); }
	};
	class CMD_NOREP_Request_File_And_Checksum : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_And_Checksum(long DeviceIDFrom, long DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_And_Checksum_DL : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_And_Checksum_DL(long DeviceIDFrom, string DeviceIDTo,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_And_Checksum_DT : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_And_Checksum_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,1,13,sFilename.c_str()); }
	};
	class CMD_NOREP_Request_File_And_Checksum_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Request_File_And_Checksum_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,239,1,13,sFilename.c_str()); }
	};
	class CMD_Back_Prior_Menu : public PreformedCommand {
	public:
		CMD_Back_Prior_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,240,0); }
	};
	class CMD_Back_Prior_Menu_DL : public PreformedCommand {
	public:
		CMD_Back_Prior_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,240,0); }
	};
	class CMD_Back_Prior_Menu_DT : public PreformedCommand {
	public:
		CMD_Back_Prior_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,240,0); }
	};
	class CMD_Back_Prior_Menu_Cat : public PreformedCommand {
	public:
		CMD_Back_Prior_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,240,0); }
	};
	class CMD_MH_Move_Media : public PreformedCommand {
	public:
		CMD_MH_Move_Media(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,241,2,41,StringUtils::itos(iStreamID).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_MH_Move_Media_DL : public PreformedCommand {
	public:
		CMD_MH_Move_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,241,2,41,StringUtils::itos(iStreamID).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_MH_Move_Media_DT : public PreformedCommand {
	public:
		CMD_MH_Move_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,241,2,41,StringUtils::itos(iStreamID).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_MH_Move_Media_Cat : public PreformedCommand {
	public:
		CMD_MH_Move_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,241,2,41,StringUtils::itos(iStreamID).c_str(),45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Set_Now_Playing : public PreformedCommand {
	public:
		CMD_Set_Now_Playing(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,string sText,string sFilename,int iPK_MediaType,int iValue,bool bRetransmit) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,242,8,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),9,sText.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),48,StringUtils::itos(iValue).c_str(),120,(bRetransmit ? "1" : "0")); }
	};
	class CMD_Set_Now_Playing_DL : public PreformedCommand {
	public:
		CMD_Set_Now_Playing_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,string sText,string sFilename,int iPK_MediaType,int iValue,bool bRetransmit) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,242,8,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),9,sText.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),48,StringUtils::itos(iValue).c_str(),120,(bRetransmit ? "1" : "0")); }
	};
	class CMD_Set_Now_Playing_DT : public PreformedCommand {
	public:
		CMD_Set_Now_Playing_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,string sText,string sFilename,int iPK_MediaType,int iValue,bool bRetransmit) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,242,8,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),9,sText.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),48,StringUtils::itos(iValue).c_str(),120,(bRetransmit ? "1" : "0")); }
	};
	class CMD_Set_Now_Playing_Cat : public PreformedCommand {
	public:
		CMD_Set_Now_Playing_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sPK_DesignObj,string sValue_To_Assign,string sText,string sFilename,int iPK_MediaType,int iValue,bool bRetransmit) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,242,8,2,StringUtils::itos(iPK_Device).c_str(),3,sPK_DesignObj.c_str(),5,sValue_To_Assign.c_str(),9,sText.c_str(),13,sFilename.c_str(),29,StringUtils::itos(iPK_MediaType).c_str(),48,StringUtils::itos(iValue).c_str(),120,(bRetransmit ? "1" : "0")); }
	};
	class CMD_Output_Select : public PreformedCommand {
	public:
		CMD_Output_Select(long DeviceIDFrom, long DeviceIDTo,int iPK_Command_Output) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,244,1,96,StringUtils::itos(iPK_Command_Output).c_str()); }
	};
	class CMD_Output_Select_DL : public PreformedCommand {
	public:
		CMD_Output_Select_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Command_Output) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,244,1,96,StringUtils::itos(iPK_Command_Output).c_str()); }
	};
	class CMD_Output_Select_DT : public PreformedCommand {
	public:
		CMD_Output_Select_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Command_Output) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,244,1,96,StringUtils::itos(iPK_Command_Output).c_str()); }
	};
	class CMD_Output_Select_Cat : public PreformedCommand {
	public:
		CMD_Output_Select_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Command_Output) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,244,1,96,StringUtils::itos(iPK_Command_Output).c_str()); }
	};
	class CMD_Learn_IR : public PreformedCommand {
	public:
		CMD_Learn_IR(long DeviceIDFrom, long DeviceIDTo,string sOnOff,int iPK_Text,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,245,3,8,sOnOff.c_str(),25,StringUtils::itos(iPK_Text).c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Learn_IR_DL : public PreformedCommand {
	public:
		CMD_Learn_IR_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff,int iPK_Text,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,245,3,8,sOnOff.c_str(),25,StringUtils::itos(iPK_Text).c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Learn_IR_DT : public PreformedCommand {
	public:
		CMD_Learn_IR_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff,int iPK_Text,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,245,3,8,sOnOff.c_str(),25,StringUtils::itos(iPK_Text).c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Learn_IR_Cat : public PreformedCommand {
	public:
		CMD_Learn_IR_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff,int iPK_Text,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,245,3,8,sOnOff.c_str(),25,StringUtils::itos(iPK_Text).c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Set_Device_Data : public PreformedCommand {
	public:
		CMD_Set_Device_Data(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sValue_To_Assign,int iPK_DeviceData) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,246,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),52,StringUtils::itos(iPK_DeviceData).c_str()); }
	};
	class CMD_Set_Device_Data_DL : public PreformedCommand {
	public:
		CMD_Set_Device_Data_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sValue_To_Assign,int iPK_DeviceData) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,246,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),52,StringUtils::itos(iPK_DeviceData).c_str()); }
	};
	class CMD_Set_Device_Data_DT : public PreformedCommand {
	public:
		CMD_Set_Device_Data_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sValue_To_Assign,int iPK_DeviceData) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,246,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),52,StringUtils::itos(iPK_DeviceData).c_str()); }
	};
	class CMD_Set_Device_Data_Cat : public PreformedCommand {
	public:
		CMD_Set_Device_Data_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sValue_To_Assign,int iPK_DeviceData) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,246,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),52,StringUtils::itos(iPK_DeviceData).c_str()); }
	};
	class CMD_Get_Device_State : public PreformedCommand {
	public:
		CMD_Get_Device_State(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,247,0); }
	};
	class CMD_Get_Device_State_DL : public PreformedCommand {
	public:
		CMD_Get_Device_State_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,247,0); }
	};
	class CMD_Get_Device_State_DT : public PreformedCommand {
	public:
		CMD_Get_Device_State_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,247,0); }
	};
	class CMD_Get_Device_State_Cat : public PreformedCommand {
	public:
		CMD_Get_Device_State_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,247,0); }
	};
	class CMD_Get_Device_Status : public PreformedCommand {
	public:
		CMD_Get_Device_Status(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,248,0); }
	};
	class CMD_Get_Device_Status_DL : public PreformedCommand {
	public:
		CMD_Get_Device_Status_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,248,0); }
	};
	class CMD_Get_Device_Status_DT : public PreformedCommand {
	public:
		CMD_Get_Device_Status_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,248,0); }
	};
	class CMD_Get_Device_Status_Cat : public PreformedCommand {
	public:
		CMD_Get_Device_Status_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,248,0); }
	};
	class RESP_Start_Streaming : public PreformedCommandResponse {
		string *m_sMediaURL;
	public:
		RESP_Start_Streaming(string *sMediaURL) { 
		m_sMediaURL=sMediaURL; }
		void ParseResponse(Message *pMessage) {
			*m_sMediaURL=pMessage->m_mapParameters[59]; };
	};
	class CMD_Start_Streaming : public PreformedCommand {
	public:
		CMD_Start_Streaming(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string sStreamingTargets,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,3,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Start_Streaming(sMediaURL); }
	};
	class CMD_Start_Streaming_DL : public PreformedCommand {
	public:
		CMD_Start_Streaming_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string sStreamingTargets,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,3,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Start_Streaming(sMediaURL); }
	};
	class CMD_Start_Streaming_DT : public PreformedCommand {
	public:
		CMD_Start_Streaming_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string sStreamingTargets,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,3,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Start_Streaming(sMediaURL); }
	};
	class CMD_Start_Streaming_Cat : public PreformedCommand {
	public:
		CMD_Start_Streaming_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string sStreamingTargets,string *sMediaURL) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,3,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str(),59,(*sMediaURL).c_str());		m_pcResponse = new RESP_Start_Streaming(sMediaURL); }
	};
	class CMD_NOREP_Start_Streaming : public PreformedCommand {
	public:
		CMD_NOREP_Start_Streaming(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_NOREP_Start_Streaming_DL : public PreformedCommand {
	public:
		CMD_NOREP_Start_Streaming_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_NOREP_Start_Streaming_DT : public PreformedCommand {
	public:
		CMD_NOREP_Start_Streaming_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_NOREP_Start_Streaming_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Start_Streaming_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,249,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_Store_Infrared_Code : public PreformedCommand {
	public:
		CMD_Store_Infrared_Code(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sValue_To_Assign,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,250,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Store_Infrared_Code_DL : public PreformedCommand {
	public:
		CMD_Store_Infrared_Code_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sValue_To_Assign,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,250,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Store_Infrared_Code_DT : public PreformedCommand {
	public:
		CMD_Store_Infrared_Code_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sValue_To_Assign,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,250,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class CMD_Store_Infrared_Code_Cat : public PreformedCommand {
	public:
		CMD_Store_Infrared_Code_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sValue_To_Assign,int iPK_Command_Input) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,250,3,2,StringUtils::itos(iPK_Device).c_str(),5,sValue_To_Assign.c_str(),71,StringUtils::itos(iPK_Command_Input).c_str()); }
	};
	class RESP_Save_Current_Frame : public PreformedCommandResponse {
		string *m_sFilename;
	public:
		RESP_Save_Current_Frame(string *sFilename) { 
		m_sFilename=sFilename; }
		void ParseResponse(Message *pMessage) {
			*m_sFilename=pMessage->m_mapParameters[13]; };
	};
	class CMD_Save_Current_Frame : public PreformedCommand {
	public:
		CMD_Save_Current_Frame(long DeviceIDFrom, long DeviceIDTo,string *sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,1,13,(*sFilename).c_str());		m_pcResponse = new RESP_Save_Current_Frame(sFilename); }
	};
	class CMD_Save_Current_Frame_DL : public PreformedCommand {
	public:
		CMD_Save_Current_Frame_DL(long DeviceIDFrom, string DeviceIDTo,string *sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,1,13,(*sFilename).c_str());		m_pcResponse = new RESP_Save_Current_Frame(sFilename); }
	};
	class CMD_Save_Current_Frame_DT : public PreformedCommand {
	public:
		CMD_Save_Current_Frame_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string *sFilename) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,1,13,(*sFilename).c_str());		m_pcResponse = new RESP_Save_Current_Frame(sFilename); }
	};
	class CMD_Save_Current_Frame_Cat : public PreformedCommand {
	public:
		CMD_Save_Current_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string *sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,1,13,(*sFilename).c_str());		m_pcResponse = new RESP_Save_Current_Frame(sFilename); }
	};
	class CMD_NOREP_Save_Current_Frame : public PreformedCommand {
	public:
		CMD_NOREP_Save_Current_Frame(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,0); }
	};
	class CMD_NOREP_Save_Current_Frame_DL : public PreformedCommand {
	public:
		CMD_NOREP_Save_Current_Frame_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,0); }
	};
	class CMD_NOREP_Save_Current_Frame_DT : public PreformedCommand {
	public:
		CMD_NOREP_Save_Current_Frame_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,0); }
	};
	class CMD_NOREP_Save_Current_Frame_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Save_Current_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,251,0); }
	};
	class CMD_Send_Audio_To_Device : public PreformedCommand {
	public:
		CMD_Send_Audio_To_Device(long DeviceIDFrom, long DeviceIDTo,string sText,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,253,2,9,sText.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Send_Audio_To_Device_DL : public PreformedCommand {
	public:
		CMD_Send_Audio_To_Device_DL(long DeviceIDFrom, string DeviceIDTo,string sText,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,253,2,9,sText.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Send_Audio_To_Device_DT : public PreformedCommand {
	public:
		CMD_Send_Audio_To_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,253,2,9,sText.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Send_Audio_To_Device_Cat : public PreformedCommand {
	public:
		CMD_Send_Audio_To_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,253,2,9,sText.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Bind_Icon : public PreformedCommand {
	public:
		CMD_Bind_Icon(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sType,bool bChild) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,254,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),104,(bChild ? "1" : "0")); }
	};
	class CMD_Bind_Icon_DL : public PreformedCommand {
	public:
		CMD_Bind_Icon_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sType,bool bChild) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,254,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),104,(bChild ? "1" : "0")); }
	};
	class CMD_Bind_Icon_DT : public PreformedCommand {
	public:
		CMD_Bind_Icon_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sType,bool bChild) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,254,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),104,(bChild ? "1" : "0")); }
	};
	class CMD_Bind_Icon_Cat : public PreformedCommand {
	public:
		CMD_Bind_Icon_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sType,bool bChild) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,254,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),104,(bChild ? "1" : "0")); }
	};
	class RESP_Orbiter_Registered : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;
	public:
		RESP_Orbiter_Registered(char **pData,int *iData_Size) { 
		m_pData=pData; m_iData_Size=iData_Size; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; };
	};
	class CMD_Orbiter_Registered : public PreformedCommand {
	public:
		CMD_Orbiter_Registered(long DeviceIDFrom, long DeviceIDTo,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,5,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Orbiter_Registered(pData,iData_Size); }
	};
	class CMD_Orbiter_Registered_DL : public PreformedCommand {
	public:
		CMD_Orbiter_Registered_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,5,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Orbiter_Registered(pData,iData_Size); }
	};
	class CMD_Orbiter_Registered_DT : public PreformedCommand {
	public:
		CMD_Orbiter_Registered_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,5,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Orbiter_Registered(pData,iData_Size); }
	};
	class CMD_Orbiter_Registered_Cat : public PreformedCommand {
	public:
		CMD_Orbiter_Registered_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,5,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Orbiter_Registered(pData,iData_Size); }
	};
	class CMD_NOREP_Orbiter_Registered : public PreformedCommand {
	public:
		CMD_NOREP_Orbiter_Registered(long DeviceIDFrom, long DeviceIDTo,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,4,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_NOREP_Orbiter_Registered_DL : public PreformedCommand {
	public:
		CMD_NOREP_Orbiter_Registered_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,4,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_NOREP_Orbiter_Registered_DT : public PreformedCommand {
	public:
		CMD_NOREP_Orbiter_Registered_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,4,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_NOREP_Orbiter_Registered_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Orbiter_Registered_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff,int iPK_Users,string sPK_EntertainArea,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,255,4,8,sOnOff.c_str(),17,StringUtils::itos(iPK_Users).c_str(),45,sPK_EntertainArea.c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class RESP_Text_To_Wave : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;
	public:
		RESP_Text_To_Wave(char **pData,int *iData_Size) { 
		m_pData=pData; m_iData_Size=iData_Size; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; };
	};
	class CMD_Text_To_Wave : public PreformedCommand {
	public:
		CMD_Text_To_Wave(long DeviceIDFrom, long DeviceIDTo,string sText,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,2,9,sText.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Text_To_Wave(pData,iData_Size); }
	};
	class CMD_Text_To_Wave_DL : public PreformedCommand {
	public:
		CMD_Text_To_Wave_DL(long DeviceIDFrom, string DeviceIDTo,string sText,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,2,9,sText.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Text_To_Wave(pData,iData_Size); }
	};
	class CMD_Text_To_Wave_DT : public PreformedCommand {
	public:
		CMD_Text_To_Wave_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,2,9,sText.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Text_To_Wave(pData,iData_Size); }
	};
	class CMD_Text_To_Wave_Cat : public PreformedCommand {
	public:
		CMD_Text_To_Wave_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,2,9,sText.c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Text_To_Wave(pData,iData_Size); }
	};
	class CMD_NOREP_Text_To_Wave : public PreformedCommand {
	public:
		CMD_NOREP_Text_To_Wave(long DeviceIDFrom, long DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,1,9,sText.c_str()); }
	};
	class CMD_NOREP_Text_To_Wave_DL : public PreformedCommand {
	public:
		CMD_NOREP_Text_To_Wave_DL(long DeviceIDFrom, string DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,1,9,sText.c_str()); }
	};
	class CMD_NOREP_Text_To_Wave_DT : public PreformedCommand {
	public:
		CMD_NOREP_Text_To_Wave_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,1,9,sText.c_str()); }
	};
	class CMD_NOREP_Text_To_Wave_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Text_To_Wave_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,256,1,9,sText.c_str()); }
	};
	class CMD_Delay : public PreformedCommand {
	public:
		CMD_Delay(long DeviceIDFrom, long DeviceIDTo,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,257,1,102,sTime.c_str()); }
	};
	class CMD_Delay_DL : public PreformedCommand {
	public:
		CMD_Delay_DL(long DeviceIDFrom, string DeviceIDTo,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,257,1,102,sTime.c_str()); }
	};
	class CMD_Delay_DT : public PreformedCommand {
	public:
		CMD_Delay_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sTime) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,257,1,102,sTime.c_str()); }
	};
	class CMD_Delay_Cat : public PreformedCommand {
	public:
		CMD_Delay_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,257,1,102,sTime.c_str()); }
	};
	class CMD_Clear_Selected_Devices : public PreformedCommand {
	public:
		CMD_Clear_Selected_Devices(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,258,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Clear_Selected_Devices_DL : public PreformedCommand {
	public:
		CMD_Clear_Selected_Devices_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,258,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Clear_Selected_Devices_DT : public PreformedCommand {
	public:
		CMD_Clear_Selected_Devices_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,258,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Clear_Selected_Devices_Cat : public PreformedCommand {
	public:
		CMD_Clear_Selected_Devices_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,258,1,3,sPK_DesignObj.c_str()); }
	};
	class RESP_Report_Playback_Position : public PreformedCommandResponse {
		string *m_sText;string *m_sMediaPosition;
	public:
		RESP_Report_Playback_Position(string *sText,string *sMediaPosition) { 
		m_sText=sText; m_sMediaPosition=sMediaPosition; }
		void ParseResponse(Message *pMessage) {
			*m_sText=pMessage->m_mapParameters[9]; *m_sMediaPosition=pMessage->m_mapParameters[42]; };
	};
	class CMD_Report_Playback_Position : public PreformedCommand {
	public:
		CMD_Report_Playback_Position(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string *sText,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,3,41,StringUtils::itos(iStreamID).c_str(),9,(*sText).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Report_Playback_Position(sText,sMediaPosition); }
	};
	class CMD_Report_Playback_Position_DL : public PreformedCommand {
	public:
		CMD_Report_Playback_Position_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string *sText,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,3,41,StringUtils::itos(iStreamID).c_str(),9,(*sText).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Report_Playback_Position(sText,sMediaPosition); }
	};
	class CMD_Report_Playback_Position_DT : public PreformedCommand {
	public:
		CMD_Report_Playback_Position_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string *sText,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,3,41,StringUtils::itos(iStreamID).c_str(),9,(*sText).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Report_Playback_Position(sText,sMediaPosition); }
	};
	class CMD_Report_Playback_Position_Cat : public PreformedCommand {
	public:
		CMD_Report_Playback_Position_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string *sText,string *sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,3,41,StringUtils::itos(iStreamID).c_str(),9,(*sText).c_str(),42,(*sMediaPosition).c_str());		m_pcResponse = new RESP_Report_Playback_Position(sText,sMediaPosition); }
	};
	class CMD_NOREP_Report_Playback_Position : public PreformedCommand {
	public:
		CMD_NOREP_Report_Playback_Position(long DeviceIDFrom, long DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_NOREP_Report_Playback_Position_DL : public PreformedCommand {
	public:
		CMD_NOREP_Report_Playback_Position_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_NOREP_Report_Playback_Position_DT : public PreformedCommand {
	public:
		CMD_NOREP_Report_Playback_Position_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_NOREP_Report_Playback_Position_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Report_Playback_Position_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,259,1,41,StringUtils::itos(iStreamID).c_str()); }
	};
	class CMD_Set_Main_Menu : public PreformedCommand {
	public:
		CMD_Set_Main_Menu(long DeviceIDFrom, long DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,260,1,9,sText.c_str()); }
	};
	class CMD_Set_Main_Menu_DL : public PreformedCommand {
	public:
		CMD_Set_Main_Menu_DL(long DeviceIDFrom, string DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,260,1,9,sText.c_str()); }
	};
	class CMD_Set_Main_Menu_DT : public PreformedCommand {
	public:
		CMD_Set_Main_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,260,1,9,sText.c_str()); }
	};
	class CMD_Set_Main_Menu_Cat : public PreformedCommand {
	public:
		CMD_Set_Main_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,260,1,9,sText.c_str()); }
	};
	class CMD_Set_FollowMe : public PreformedCommand {
	public:
		CMD_Set_FollowMe(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sText,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,261,3,2,StringUtils::itos(iPK_Device).c_str(),9,sText.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_FollowMe_DL : public PreformedCommand {
	public:
		CMD_Set_FollowMe_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sText,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,261,3,2,StringUtils::itos(iPK_Device).c_str(),9,sText.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_FollowMe_DT : public PreformedCommand {
	public:
		CMD_Set_FollowMe_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sText,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,261,3,2,StringUtils::itos(iPK_Device).c_str(),9,sText.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Set_FollowMe_Cat : public PreformedCommand {
	public:
		CMD_Set_FollowMe_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sText,int iPK_Users) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,261,3,2,StringUtils::itos(iPK_Device).c_str(),9,sText.c_str(),17,StringUtils::itos(iPK_Users).c_str()); }
	};
	class CMD_Stop_Streaming : public PreformedCommand {
	public:
		CMD_Stop_Streaming(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,262,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_Stop_Streaming_DL : public PreformedCommand {
	public:
		CMD_Stop_Streaming_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,262,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_Stop_Streaming_DT : public PreformedCommand {
	public:
		CMD_Stop_Streaming_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,262,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_Stop_Streaming_Cat : public PreformedCommand {
	public:
		CMD_Stop_Streaming_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string sStreamingTargets) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,262,2,41,StringUtils::itos(iStreamID).c_str(),105,sStreamingTargets.c_str()); }
	};
	class CMD_Toggle_Event_Handler : public PreformedCommand {
	public:
		CMD_Toggle_Event_Handler(long DeviceIDFrom, long DeviceIDTo,int iPK_EventHandler) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,263,1,107,StringUtils::itos(iPK_EventHandler).c_str()); }
	};
	class CMD_Toggle_Event_Handler_DL : public PreformedCommand {
	public:
		CMD_Toggle_Event_Handler_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_EventHandler) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,263,1,107,StringUtils::itos(iPK_EventHandler).c_str()); }
	};
	class CMD_Toggle_Event_Handler_DT : public PreformedCommand {
	public:
		CMD_Toggle_Event_Handler_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_EventHandler) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,263,1,107,StringUtils::itos(iPK_EventHandler).c_str()); }
	};
	class CMD_Toggle_Event_Handler_Cat : public PreformedCommand {
	public:
		CMD_Toggle_Event_Handler_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_EventHandler) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,263,1,107,StringUtils::itos(iPK_EventHandler).c_str()); }
	};
	class CMD_Track_Frontend_At_IP : public PreformedCommand {
	public:
		CMD_Track_Frontend_At_IP(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,264,2,2,StringUtils::itos(iPK_Device).c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Track_Frontend_At_IP_DL : public PreformedCommand {
	public:
		CMD_Track_Frontend_At_IP_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,264,2,2,StringUtils::itos(iPK_Device).c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Track_Frontend_At_IP_DT : public PreformedCommand {
	public:
		CMD_Track_Frontend_At_IP_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,264,2,2,StringUtils::itos(iPK_Device).c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Track_Frontend_At_IP_Cat : public PreformedCommand {
	public:
		CMD_Track_Frontend_At_IP_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sIP_Address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,264,2,2,StringUtils::itos(iPK_Device).c_str(),58,sIP_Address.c_str()); }
	};
	class CMD_Quit : public PreformedCommand {
	public:
		CMD_Quit(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,265,0); }
	};
	class CMD_Quit_DL : public PreformedCommand {
	public:
		CMD_Quit_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,265,0); }
	};
	class CMD_Quit_DT : public PreformedCommand {
	public:
		CMD_Quit_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,265,0); }
	};
	class CMD_Quit_Cat : public PreformedCommand {
	public:
		CMD_Quit_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,265,0); }
	};
	class CMD_Regen_Orbiter : public PreformedCommand {
	public:
		CMD_Regen_Orbiter(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,266,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Regen_Orbiter_DL : public PreformedCommand {
	public:
		CMD_Regen_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,266,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Regen_Orbiter_DT : public PreformedCommand {
	public:
		CMD_Regen_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,266,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Regen_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_Regen_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,266,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Regen_Orbiter_Finished : public PreformedCommand {
	public:
		CMD_Regen_Orbiter_Finished(long DeviceIDFrom, long DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,267,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Regen_Orbiter_Finished_DL : public PreformedCommand {
	public:
		CMD_Regen_Orbiter_Finished_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,267,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Regen_Orbiter_Finished_DT : public PreformedCommand {
	public:
		CMD_Regen_Orbiter_Finished_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,267,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Regen_Orbiter_Finished_Cat : public PreformedCommand {
	public:
		CMD_Regen_Orbiter_Finished_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,267,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Move_Playlist_entry_Up : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Up(long DeviceIDFrom, long DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,269,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Move_Playlist_entry_Up_DL : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Up_DL(long DeviceIDFrom, string DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,269,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Move_Playlist_entry_Up_DT : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,269,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Move_Playlist_entry_Up_Cat : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,269,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Move_Playlist_entry_Down : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Down(long DeviceIDFrom, long DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,270,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Move_Playlist_entry_Down_DL : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Down_DL(long DeviceIDFrom, string DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,270,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Move_Playlist_entry_Down_DT : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,270,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Move_Playlist_entry_Down_Cat : public PreformedCommand {
	public:
		CMD_Move_Playlist_entry_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,270,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Remove_playlist_entry : public PreformedCommand {
	public:
		CMD_Remove_playlist_entry(long DeviceIDFrom, long DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,271,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Remove_playlist_entry_DL : public PreformedCommand {
	public:
		CMD_Remove_playlist_entry_DL(long DeviceIDFrom, string DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,271,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Remove_playlist_entry_DT : public PreformedCommand {
	public:
		CMD_Remove_playlist_entry_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,271,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Remove_playlist_entry_Cat : public PreformedCommand {
	public:
		CMD_Remove_playlist_entry_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,271,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Restart_DCERouter : public PreformedCommand {
	public:
		CMD_Restart_DCERouter(long DeviceIDFrom, long DeviceIDTo,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,272,1,21,sForce.c_str()); }
	};
	class CMD_Restart_DCERouter_DL : public PreformedCommand {
	public:
		CMD_Restart_DCERouter_DL(long DeviceIDFrom, string DeviceIDTo,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,272,1,21,sForce.c_str()); }
	};
	class CMD_Restart_DCERouter_DT : public PreformedCommand {
	public:
		CMD_Restart_DCERouter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sForce) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,272,1,21,sForce.c_str()); }
	};
	class CMD_Restart_DCERouter_Cat : public PreformedCommand {
	public:
		CMD_Restart_DCERouter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,272,1,21,sForce.c_str()); }
	};
	class CMD_EIB_Write : public PreformedCommand {
	public:
		CMD_EIB_Write(long DeviceIDFrom, long DeviceIDTo,string sAddress,string sData,int iDataType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,273,3,108,sAddress.c_str(),109,sData.c_str(),110,StringUtils::itos(iDataType).c_str()); }
	};
	class CMD_EIB_Write_DL : public PreformedCommand {
	public:
		CMD_EIB_Write_DL(long DeviceIDFrom, string DeviceIDTo,string sAddress,string sData,int iDataType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,273,3,108,sAddress.c_str(),109,sData.c_str(),110,StringUtils::itos(iDataType).c_str()); }
	};
	class CMD_EIB_Write_DT : public PreformedCommand {
	public:
		CMD_EIB_Write_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sAddress,string sData,int iDataType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,273,3,108,sAddress.c_str(),109,sData.c_str(),110,StringUtils::itos(iDataType).c_str()); }
	};
	class CMD_EIB_Write_Cat : public PreformedCommand {
	public:
		CMD_EIB_Write_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sAddress,string sData,int iDataType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,273,3,108,sAddress.c_str(),109,sData.c_str(),110,StringUtils::itos(iDataType).c_str()); }
	};
	class CMD_Set_Room_For_Device : public PreformedCommand {
	public:
		CMD_Set_Room_For_Device(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,274,2,2,StringUtils::itos(iPK_Device).c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Room_For_Device_DL : public PreformedCommand {
	public:
		CMD_Set_Room_For_Device_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,274,2,2,StringUtils::itos(iPK_Device).c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Room_For_Device_DT : public PreformedCommand {
	public:
		CMD_Set_Room_For_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,274,2,2,StringUtils::itos(iPK_Device).c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_Set_Room_For_Device_Cat : public PreformedCommand {
	public:
		CMD_Set_Room_For_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,int iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,274,2,2,StringUtils::itos(iPK_Device).c_str(),57,StringUtils::itos(iPK_Room).c_str()); }
	};
	class CMD_EIB_Read : public PreformedCommand {
	public:
		CMD_EIB_Read(long DeviceIDFrom, long DeviceIDTo,string sAddress) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,275,1,108,sAddress.c_str()); }
	};
	class CMD_EIB_Read_DL : public PreformedCommand {
	public:
		CMD_EIB_Read_DL(long DeviceIDFrom, string DeviceIDTo,string sAddress) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,275,1,108,sAddress.c_str()); }
	};
	class CMD_EIB_Read_DT : public PreformedCommand {
	public:
		CMD_EIB_Read_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sAddress) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,275,1,108,sAddress.c_str()); }
	};
	class CMD_EIB_Read_Cat : public PreformedCommand {
	public:
		CMD_EIB_Read_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sAddress) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,275,1,108,sAddress.c_str()); }
	};
	class CMD_Add_GC100 : public PreformedCommand {
	public:
		CMD_Add_GC100(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,276,0); }
	};
	class CMD_Add_GC100_DL : public PreformedCommand {
	public:
		CMD_Add_GC100_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,276,0); }
	};
	class CMD_Add_GC100_DT : public PreformedCommand {
	public:
		CMD_Add_GC100_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,276,0); }
	};
	class CMD_Add_GC100_Cat : public PreformedCommand {
	public:
		CMD_Add_GC100_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,276,0); }
	};
	class RESP_Get_Capture_Video_Frame : public PreformedCommandResponse {
		char **m_pData;int *m_iData_Size;
	public:
		RESP_Get_Capture_Video_Frame(char **pData,int *iData_Size) { 
		m_pData=pData; m_iData_Size=iData_Size; }
		void ParseResponse(Message *pMessage) {
			*m_pData=pMessage->m_mapData_Parameters[19]; pMessage->m_mapData_Parameters.erase(19); *m_iData_Size=pMessage->m_mapData_Lengths[19]; };
	};
	class CMD_Get_Capture_Video_Frame : public PreformedCommand {
	public:
		CMD_Get_Capture_Video_Frame(long DeviceIDFrom, long DeviceIDTo,int iCameraID,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,2,112,StringUtils::itos(iCameraID).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Capture_Video_Frame(pData,iData_Size); }
	};
	class CMD_Get_Capture_Video_Frame_DL : public PreformedCommand {
	public:
		CMD_Get_Capture_Video_Frame_DL(long DeviceIDFrom, string DeviceIDTo,int iCameraID,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,2,112,StringUtils::itos(iCameraID).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Capture_Video_Frame(pData,iData_Size); }
	};
	class CMD_Get_Capture_Video_Frame_DT : public PreformedCommand {
	public:
		CMD_Get_Capture_Video_Frame_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iCameraID,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,2,112,StringUtils::itos(iCameraID).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Capture_Video_Frame(pData,iData_Size); }
	};
	class CMD_Get_Capture_Video_Frame_Cat : public PreformedCommand {
	public:
		CMD_Get_Capture_Video_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iCameraID,char **pData,int *iData_Size) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,2,112,StringUtils::itos(iCameraID).c_str(),-19,*pData,*iData_Size);		m_pcResponse = new RESP_Get_Capture_Video_Frame(pData,iData_Size); }
	};
	class CMD_NOREP_Get_Capture_Video_Frame : public PreformedCommand {
	public:
		CMD_NOREP_Get_Capture_Video_Frame(long DeviceIDFrom, long DeviceIDTo,int iCameraID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,1,112,StringUtils::itos(iCameraID).c_str()); }
	};
	class CMD_NOREP_Get_Capture_Video_Frame_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Capture_Video_Frame_DL(long DeviceIDFrom, string DeviceIDTo,int iCameraID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,1,112,StringUtils::itos(iCameraID).c_str()); }
	};
	class CMD_NOREP_Get_Capture_Video_Frame_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Capture_Video_Frame_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iCameraID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,1,112,StringUtils::itos(iCameraID).c_str()); }
	};
	class CMD_NOREP_Get_Capture_Video_Frame_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Capture_Video_Frame_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iCameraID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,277,1,112,StringUtils::itos(iCameraID).c_str()); }
	};
	class CMD_Set_Temperature : public PreformedCommand {
	public:
		CMD_Set_Temperature(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,278,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Temperature_DL : public PreformedCommand {
	public:
		CMD_Set_Temperature_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,278,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Temperature_DT : public PreformedCommand {
	public:
		CMD_Set_Temperature_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,278,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Temperature_Cat : public PreformedCommand {
	public:
		CMD_Set_Temperature_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,278,1,5,sValue_To_Assign.c_str()); }
	};
	class CMD_Set_Fan : public PreformedCommand {
	public:
		CMD_Set_Fan(long DeviceIDFrom, long DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,279,1,8,sOnOff.c_str()); }
	};
	class CMD_Set_Fan_DL : public PreformedCommand {
	public:
		CMD_Set_Fan_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,279,1,8,sOnOff.c_str()); }
	};
	class CMD_Set_Fan_DT : public PreformedCommand {
	public:
		CMD_Set_Fan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,279,1,8,sOnOff.c_str()); }
	};
	class CMD_Set_Fan_Cat : public PreformedCommand {
	public:
		CMD_Set_Fan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,279,1,8,sOnOff.c_str()); }
	};
	class CMD_Set_HeatCool : public PreformedCommand {
	public:
		CMD_Set_HeatCool(long DeviceIDFrom, long DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,280,1,8,sOnOff.c_str()); }
	};
	class CMD_Set_HeatCool_DL : public PreformedCommand {
	public:
		CMD_Set_HeatCool_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,280,1,8,sOnOff.c_str()); }
	};
	class CMD_Set_HeatCool_DT : public PreformedCommand {
	public:
		CMD_Set_HeatCool_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,280,1,8,sOnOff.c_str()); }
	};
	class CMD_Set_HeatCool_Cat : public PreformedCommand {
	public:
		CMD_Set_HeatCool_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,280,1,8,sOnOff.c_str()); }
	};
	class CMD_DBSSatellite : public PreformedCommand {
	public:
		CMD_DBSSatellite(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,281,0); }
	};
	class CMD_DBSSatellite_DL : public PreformedCommand {
	public:
		CMD_DBSSatellite_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,281,0); }
	};
	class CMD_DBSSatellite_DT : public PreformedCommand {
	public:
		CMD_DBSSatellite_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,281,0); }
	};
	class CMD_DBSSatellite_Cat : public PreformedCommand {
	public:
		CMD_DBSSatellite_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,281,0); }
	};
	class CMD_VCR1 : public PreformedCommand {
	public:
		CMD_VCR1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,282,0); }
	};
	class CMD_VCR1_DL : public PreformedCommand {
	public:
		CMD_VCR1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,282,0); }
	};
	class CMD_VCR1_DT : public PreformedCommand {
	public:
		CMD_VCR1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,282,0); }
	};
	class CMD_VCR1_Cat : public PreformedCommand {
	public:
		CMD_VCR1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,282,0); }
	};
	class CMD_VCR2 : public PreformedCommand {
	public:
		CMD_VCR2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,283,0); }
	};
	class CMD_VCR2_DL : public PreformedCommand {
	public:
		CMD_VCR2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,283,0); }
	};
	class CMD_VCR2_DT : public PreformedCommand {
	public:
		CMD_VCR2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,283,0); }
	};
	class CMD_VCR2_Cat : public PreformedCommand {
	public:
		CMD_VCR2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,283,0); }
	};
	class CMD_VCR3 : public PreformedCommand {
	public:
		CMD_VCR3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,284,0); }
	};
	class CMD_VCR3_DL : public PreformedCommand {
	public:
		CMD_VCR3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,284,0); }
	};
	class CMD_VCR3_DT : public PreformedCommand {
	public:
		CMD_VCR3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,284,0); }
	};
	class CMD_VCR3_Cat : public PreformedCommand {
	public:
		CMD_VCR3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,284,0); }
	};
	class CMD_VAUX : public PreformedCommand {
	public:
		CMD_VAUX(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,285,0); }
	};
	class CMD_VAUX_DL : public PreformedCommand {
	public:
		CMD_VAUX_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,285,0); }
	};
	class CMD_VAUX_DT : public PreformedCommand {
	public:
		CMD_VAUX_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,285,0); }
	};
	class CMD_VAUX_Cat : public PreformedCommand {
	public:
		CMD_VAUX_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,285,0); }
	};
	class CMD_Pure_Direct : public PreformedCommand {
	public:
		CMD_Pure_Direct(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,286,0); }
	};
	class CMD_Pure_Direct_DL : public PreformedCommand {
	public:
		CMD_Pure_Direct_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,286,0); }
	};
	class CMD_Pure_Direct_DT : public PreformedCommand {
	public:
		CMD_Pure_Direct_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,286,0); }
	};
	class CMD_Pure_Direct_Cat : public PreformedCommand {
	public:
		CMD_Pure_Direct_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,286,0); }
	};
	class CMD_Multi_Channel_In : public PreformedCommand {
	public:
		CMD_Multi_Channel_In(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,287,0); }
	};
	class CMD_Multi_Channel_In_DL : public PreformedCommand {
	public:
		CMD_Multi_Channel_In_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,287,0); }
	};
	class CMD_Multi_Channel_In_DT : public PreformedCommand {
	public:
		CMD_Multi_Channel_In_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,287,0); }
	};
	class CMD_Multi_Channel_In_Cat : public PreformedCommand {
	public:
		CMD_Multi_Channel_In_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,287,0); }
	};
	class CMD_Multi_Channel_Direct : public PreformedCommand {
	public:
		CMD_Multi_Channel_Direct(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,288,0); }
	};
	class CMD_Multi_Channel_Direct_DL : public PreformedCommand {
	public:
		CMD_Multi_Channel_Direct_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,288,0); }
	};
	class CMD_Multi_Channel_Direct_DT : public PreformedCommand {
	public:
		CMD_Multi_Channel_Direct_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,288,0); }
	};
	class CMD_Multi_Channel_Direct_Cat : public PreformedCommand {
	public:
		CMD_Multi_Channel_Direct_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,288,0); }
	};
	class CMD_Multi_Channel_Pure_Direct : public PreformedCommand {
	public:
		CMD_Multi_Channel_Pure_Direct(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,289,0); }
	};
	class CMD_Multi_Channel_Pure_Direct_DL : public PreformedCommand {
	public:
		CMD_Multi_Channel_Pure_Direct_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,289,0); }
	};
	class CMD_Multi_Channel_Pure_Direct_DT : public PreformedCommand {
	public:
		CMD_Multi_Channel_Pure_Direct_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,289,0); }
	};
	class CMD_Multi_Channel_Pure_Direct_Cat : public PreformedCommand {
	public:
		CMD_Multi_Channel_Pure_Direct_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,289,0); }
	};
	class CMD_Dolby_Pro_Logic : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,290,0); }
	};
	class CMD_Dolby_Pro_Logic_DL : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,290,0); }
	};
	class CMD_Dolby_Pro_Logic_DT : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,290,0); }
	};
	class CMD_Dolby_Pro_Logic_Cat : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,290,0); }
	};
	class CMD_Dolby_Pro_Logic_2 : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,291,0); }
	};
	class CMD_Dolby_Pro_Logic_2_DL : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,291,0); }
	};
	class CMD_Dolby_Pro_Logic_2_DT : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,291,0); }
	};
	class CMD_Dolby_Pro_Logic_2_Cat : public PreformedCommand {
	public:
		CMD_Dolby_Pro_Logic_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,291,0); }
	};
	class CMD_Dolby_Digital : public PreformedCommand {
	public:
		CMD_Dolby_Digital(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,292,0); }
	};
	class CMD_Dolby_Digital_DL : public PreformedCommand {
	public:
		CMD_Dolby_Digital_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,292,0); }
	};
	class CMD_Dolby_Digital_DT : public PreformedCommand {
	public:
		CMD_Dolby_Digital_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,292,0); }
	};
	class CMD_Dolby_Digital_Cat : public PreformedCommand {
	public:
		CMD_Dolby_Digital_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,292,0); }
	};
	class CMD_Dolby_Digital_EX : public PreformedCommand {
	public:
		CMD_Dolby_Digital_EX(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,293,0); }
	};
	class CMD_Dolby_Digital_EX_DL : public PreformedCommand {
	public:
		CMD_Dolby_Digital_EX_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,293,0); }
	};
	class CMD_Dolby_Digital_EX_DT : public PreformedCommand {
	public:
		CMD_Dolby_Digital_EX_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,293,0); }
	};
	class CMD_Dolby_Digital_EX_Cat : public PreformedCommand {
	public:
		CMD_Dolby_Digital_EX_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,293,0); }
	};
	class CMD_61_Surround : public PreformedCommand {
	public:
		CMD_61_Surround(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,294,0); }
	};
	class CMD_61_Surround_DL : public PreformedCommand {
	public:
		CMD_61_Surround_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,294,0); }
	};
	class CMD_61_Surround_DT : public PreformedCommand {
	public:
		CMD_61_Surround_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,294,0); }
	};
	class CMD_61_Surround_Cat : public PreformedCommand {
	public:
		CMD_61_Surround_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,294,0); }
	};
	class CMD_DTS_NEO_6 : public PreformedCommand {
	public:
		CMD_DTS_NEO_6(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,295,0); }
	};
	class CMD_DTS_NEO_6_DL : public PreformedCommand {
	public:
		CMD_DTS_NEO_6_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,295,0); }
	};
	class CMD_DTS_NEO_6_DT : public PreformedCommand {
	public:
		CMD_DTS_NEO_6_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,295,0); }
	};
	class CMD_DTS_NEO_6_Cat : public PreformedCommand {
	public:
		CMD_DTS_NEO_6_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,295,0); }
	};
	class CMD_DTS_Surround : public PreformedCommand {
	public:
		CMD_DTS_Surround(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,296,0); }
	};
	class CMD_DTS_Surround_DL : public PreformedCommand {
	public:
		CMD_DTS_Surround_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,296,0); }
	};
	class CMD_DTS_Surround_DT : public PreformedCommand {
	public:
		CMD_DTS_Surround_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,296,0); }
	};
	class CMD_DTS_Surround_Cat : public PreformedCommand {
	public:
		CMD_DTS_Surround_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,296,0); }
	};
	class CMD_DTS_ES_DSCRT61 : public PreformedCommand {
	public:
		CMD_DTS_ES_DSCRT61(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,297,0); }
	};
	class CMD_DTS_ES_DSCRT61_DL : public PreformedCommand {
	public:
		CMD_DTS_ES_DSCRT61_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,297,0); }
	};
	class CMD_DTS_ES_DSCRT61_DT : public PreformedCommand {
	public:
		CMD_DTS_ES_DSCRT61_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,297,0); }
	};
	class CMD_DTS_ES_DSCRT61_Cat : public PreformedCommand {
	public:
		CMD_DTS_ES_DSCRT61_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,297,0); }
	};
	class CMD_DTS_ES_MTRX61 : public PreformedCommand {
	public:
		CMD_DTS_ES_MTRX61(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,298,0); }
	};
	class CMD_DTS_ES_MTRX61_DL : public PreformedCommand {
	public:
		CMD_DTS_ES_MTRX61_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,298,0); }
	};
	class CMD_DTS_ES_MTRX61_DT : public PreformedCommand {
	public:
		CMD_DTS_ES_MTRX61_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,298,0); }
	};
	class CMD_DTS_ES_MTRX61_Cat : public PreformedCommand {
	public:
		CMD_DTS_ES_MTRX61_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,298,0); }
	};
	class CMD_Home_THX_Cinema : public PreformedCommand {
	public:
		CMD_Home_THX_Cinema(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,299,0); }
	};
	class CMD_Home_THX_Cinema_DL : public PreformedCommand {
	public:
		CMD_Home_THX_Cinema_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,299,0); }
	};
	class CMD_Home_THX_Cinema_DT : public PreformedCommand {
	public:
		CMD_Home_THX_Cinema_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,299,0); }
	};
	class CMD_Home_THX_Cinema_Cat : public PreformedCommand {
	public:
		CMD_Home_THX_Cinema_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,299,0); }
	};
	class CMD_THX_51 : public PreformedCommand {
	public:
		CMD_THX_51(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,300,0); }
	};
	class CMD_THX_51_DL : public PreformedCommand {
	public:
		CMD_THX_51_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,300,0); }
	};
	class CMD_THX_51_DT : public PreformedCommand {
	public:
		CMD_THX_51_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,300,0); }
	};
	class CMD_THX_51_Cat : public PreformedCommand {
	public:
		CMD_THX_51_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,300,0); }
	};
	class CMD_THX_U2_Cinema : public PreformedCommand {
	public:
		CMD_THX_U2_Cinema(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,301,0); }
	};
	class CMD_THX_U2_Cinema_DL : public PreformedCommand {
	public:
		CMD_THX_U2_Cinema_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,301,0); }
	};
	class CMD_THX_U2_Cinema_DT : public PreformedCommand {
	public:
		CMD_THX_U2_Cinema_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,301,0); }
	};
	class CMD_THX_U2_Cinema_Cat : public PreformedCommand {
	public:
		CMD_THX_U2_Cinema_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,301,0); }
	};
	class CMD_THX_Music : public PreformedCommand {
	public:
		CMD_THX_Music(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,302,0); }
	};
	class CMD_THX_Music_DL : public PreformedCommand {
	public:
		CMD_THX_Music_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,302,0); }
	};
	class CMD_THX_Music_DT : public PreformedCommand {
	public:
		CMD_THX_Music_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,302,0); }
	};
	class CMD_THX_Music_Cat : public PreformedCommand {
	public:
		CMD_THX_Music_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,302,0); }
	};
	class CMD_THX_61 : public PreformedCommand {
	public:
		CMD_THX_61(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,303,0); }
	};
	class CMD_THX_61_DL : public PreformedCommand {
	public:
		CMD_THX_61_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,303,0); }
	};
	class CMD_THX_61_DT : public PreformedCommand {
	public:
		CMD_THX_61_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,303,0); }
	};
	class CMD_THX_61_Cat : public PreformedCommand {
	public:
		CMD_THX_61_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,303,0); }
	};
	class CMD_THX_Surround_EX : public PreformedCommand {
	public:
		CMD_THX_Surround_EX(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,304,0); }
	};
	class CMD_THX_Surround_EX_DL : public PreformedCommand {
	public:
		CMD_THX_Surround_EX_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,304,0); }
	};
	class CMD_THX_Surround_EX_DT : public PreformedCommand {
	public:
		CMD_THX_Surround_EX_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,304,0); }
	};
	class CMD_THX_Surround_EX_Cat : public PreformedCommand {
	public:
		CMD_THX_Surround_EX_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,304,0); }
	};
	class CMD_Wide_Screen : public PreformedCommand {
	public:
		CMD_Wide_Screen(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,305,0); }
	};
	class CMD_Wide_Screen_DL : public PreformedCommand {
	public:
		CMD_Wide_Screen_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,305,0); }
	};
	class CMD_Wide_Screen_DT : public PreformedCommand {
	public:
		CMD_Wide_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,305,0); }
	};
	class CMD_Wide_Screen_Cat : public PreformedCommand {
	public:
		CMD_Wide_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,305,0); }
	};
	class CMD_5_CH_Stereo : public PreformedCommand {
	public:
		CMD_5_CH_Stereo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,306,0); }
	};
	class CMD_5_CH_Stereo_DL : public PreformedCommand {
	public:
		CMD_5_CH_Stereo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,306,0); }
	};
	class CMD_5_CH_Stereo_DT : public PreformedCommand {
	public:
		CMD_5_CH_Stereo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,306,0); }
	};
	class CMD_5_CH_Stereo_Cat : public PreformedCommand {
	public:
		CMD_5_CH_Stereo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,306,0); }
	};
	class CMD_7_Ch_Stereo : public PreformedCommand {
	public:
		CMD_7_Ch_Stereo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,307,0); }
	};
	class CMD_7_Ch_Stereo_DL : public PreformedCommand {
	public:
		CMD_7_Ch_Stereo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,307,0); }
	};
	class CMD_7_Ch_Stereo_DT : public PreformedCommand {
	public:
		CMD_7_Ch_Stereo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,307,0); }
	};
	class CMD_7_Ch_Stereo_Cat : public PreformedCommand {
	public:
		CMD_7_Ch_Stereo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,307,0); }
	};
	class CMD_Direct : public PreformedCommand {
	public:
		CMD_Direct(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,308,0); }
	};
	class CMD_Direct_DL : public PreformedCommand {
	public:
		CMD_Direct_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,308,0); }
	};
	class CMD_Direct_DT : public PreformedCommand {
	public:
		CMD_Direct_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,308,0); }
	};
	class CMD_Direct_Cat : public PreformedCommand {
	public:
		CMD_Direct_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,308,0); }
	};
	class CMD_Matrix : public PreformedCommand {
	public:
		CMD_Matrix(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,309,0); }
	};
	class CMD_Matrix_DL : public PreformedCommand {
	public:
		CMD_Matrix_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,309,0); }
	};
	class CMD_Matrix_DT : public PreformedCommand {
	public:
		CMD_Matrix_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,309,0); }
	};
	class CMD_Matrix_Cat : public PreformedCommand {
	public:
		CMD_Matrix_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,309,0); }
	};
	class CMD_Mono_Movie : public PreformedCommand {
	public:
		CMD_Mono_Movie(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,310,0); }
	};
	class CMD_Mono_Movie_DL : public PreformedCommand {
	public:
		CMD_Mono_Movie_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,310,0); }
	};
	class CMD_Mono_Movie_DT : public PreformedCommand {
	public:
		CMD_Mono_Movie_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,310,0); }
	};
	class CMD_Mono_Movie_Cat : public PreformedCommand {
	public:
		CMD_Mono_Movie_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,310,0); }
	};
	class CMD_Stereo : public PreformedCommand {
	public:
		CMD_Stereo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,311,0); }
	};
	class CMD_Stereo_DL : public PreformedCommand {
	public:
		CMD_Stereo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,311,0); }
	};
	class CMD_Stereo_DT : public PreformedCommand {
	public:
		CMD_Stereo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,311,0); }
	};
	class CMD_Stereo_Cat : public PreformedCommand {
	public:
		CMD_Stereo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,311,0); }
	};
	class CMD_Stadium : public PreformedCommand {
	public:
		CMD_Stadium(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,312,0); }
	};
	class CMD_Stadium_DL : public PreformedCommand {
	public:
		CMD_Stadium_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,312,0); }
	};
	class CMD_Stadium_DT : public PreformedCommand {
	public:
		CMD_Stadium_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,312,0); }
	};
	class CMD_Stadium_Cat : public PreformedCommand {
	public:
		CMD_Stadium_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,312,0); }
	};
	class CMD_Set_Volume : public PreformedCommand {
	public:
		CMD_Set_Volume(long DeviceIDFrom, long DeviceIDTo,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,313,1,76,sLevel.c_str()); }
	};
	class CMD_Set_Volume_DL : public PreformedCommand {
	public:
		CMD_Set_Volume_DL(long DeviceIDFrom, string DeviceIDTo,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,313,1,76,sLevel.c_str()); }
	};
	class CMD_Set_Volume_DT : public PreformedCommand {
	public:
		CMD_Set_Volume_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sLevel) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,313,1,76,sLevel.c_str()); }
	};
	class CMD_Set_Volume_Cat : public PreformedCommand {
	public:
		CMD_Set_Volume_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,313,1,76,sLevel.c_str()); }
	};
	class CMD_Identify_Media : public PreformedCommand {
	public:
		CMD_Identify_Media(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sID,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,314,3,2,StringUtils::itos(iPK_Device).c_str(),10,sID.c_str(),13,sFilename.c_str()); }
	};
	class CMD_Identify_Media_DL : public PreformedCommand {
	public:
		CMD_Identify_Media_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sID,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,314,3,2,StringUtils::itos(iPK_Device).c_str(),10,sID.c_str(),13,sFilename.c_str()); }
	};
	class CMD_Identify_Media_DT : public PreformedCommand {
	public:
		CMD_Identify_Media_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sID,string sFilename) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,314,3,2,StringUtils::itos(iPK_Device).c_str(),10,sID.c_str(),13,sFilename.c_str()); }
	};
	class CMD_Identify_Media_Cat : public PreformedCommand {
	public:
		CMD_Identify_Media_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sID,string sFilename) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,314,3,2,StringUtils::itos(iPK_Device).c_str(),10,sID.c_str(),13,sFilename.c_str()); }
	};
	class CMD_Key_Pressed : public PreformedCommand {
	public:
		CMD_Key_Pressed(long DeviceIDFrom, long DeviceIDTo,string sKey_On_Remote) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,315,1,113,sKey_On_Remote.c_str()); }
	};
	class CMD_Key_Pressed_DL : public PreformedCommand {
	public:
		CMD_Key_Pressed_DL(long DeviceIDFrom, string DeviceIDTo,string sKey_On_Remote) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,315,1,113,sKey_On_Remote.c_str()); }
	};
	class CMD_Key_Pressed_DT : public PreformedCommand {
	public:
		CMD_Key_Pressed_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sKey_On_Remote) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,315,1,113,sKey_On_Remote.c_str()); }
	};
	class CMD_Key_Pressed_Cat : public PreformedCommand {
	public:
		CMD_Key_Pressed_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sKey_On_Remote) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,315,1,113,sKey_On_Remote.c_str()); }
	};
	class CMD_AV_1 : public PreformedCommand {
	public:
		CMD_AV_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,316,0); }
	};
	class CMD_AV_1_DL : public PreformedCommand {
	public:
		CMD_AV_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,316,0); }
	};
	class CMD_AV_1_DT : public PreformedCommand {
	public:
		CMD_AV_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,316,0); }
	};
	class CMD_AV_1_Cat : public PreformedCommand {
	public:
		CMD_AV_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,316,0); }
	};
	class CMD_AV_2 : public PreformedCommand {
	public:
		CMD_AV_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,317,0); }
	};
	class CMD_AV_2_DL : public PreformedCommand {
	public:
		CMD_AV_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,317,0); }
	};
	class CMD_AV_2_DT : public PreformedCommand {
	public:
		CMD_AV_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,317,0); }
	};
	class CMD_AV_2_Cat : public PreformedCommand {
	public:
		CMD_AV_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,317,0); }
	};
	class CMD_AV_3 : public PreformedCommand {
	public:
		CMD_AV_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,318,0); }
	};
	class CMD_AV_3_DL : public PreformedCommand {
	public:
		CMD_AV_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,318,0); }
	};
	class CMD_AV_3_DT : public PreformedCommand {
	public:
		CMD_AV_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,318,0); }
	};
	class CMD_AV_3_Cat : public PreformedCommand {
	public:
		CMD_AV_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,318,0); }
	};
	class CMD_SVideoYC_1 : public PreformedCommand {
	public:
		CMD_SVideoYC_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,319,0); }
	};
	class CMD_SVideoYC_1_DL : public PreformedCommand {
	public:
		CMD_SVideoYC_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,319,0); }
	};
	class CMD_SVideoYC_1_DT : public PreformedCommand {
	public:
		CMD_SVideoYC_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,319,0); }
	};
	class CMD_SVideoYC_1_Cat : public PreformedCommand {
	public:
		CMD_SVideoYC_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,319,0); }
	};
	class CMD_SVideoYC_2 : public PreformedCommand {
	public:
		CMD_SVideoYC_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,320,0); }
	};
	class CMD_SVideoYC_2_DL : public PreformedCommand {
	public:
		CMD_SVideoYC_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,320,0); }
	};
	class CMD_SVideoYC_2_DT : public PreformedCommand {
	public:
		CMD_SVideoYC_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,320,0); }
	};
	class CMD_SVideoYC_2_Cat : public PreformedCommand {
	public:
		CMD_SVideoYC_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,320,0); }
	};
	class CMD_SVideoYC_3 : public PreformedCommand {
	public:
		CMD_SVideoYC_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,321,0); }
	};
	class CMD_SVideoYC_3_DL : public PreformedCommand {
	public:
		CMD_SVideoYC_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,321,0); }
	};
	class CMD_SVideoYC_3_DT : public PreformedCommand {
	public:
		CMD_SVideoYC_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,321,0); }
	};
	class CMD_SVideoYC_3_Cat : public PreformedCommand {
	public:
		CMD_SVideoYC_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,321,0); }
	};
	class CMD_Wake_Device : public PreformedCommand {
	public:
		CMD_Wake_Device(long DeviceIDFrom, long DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,322,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Wake_Device_DL : public PreformedCommand {
	public:
		CMD_Wake_Device_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,322,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Wake_Device_DT : public PreformedCommand {
	public:
		CMD_Wake_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,322,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Wake_Device_Cat : public PreformedCommand {
	public:
		CMD_Wake_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,322,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Halt_Device : public PreformedCommand {
	public:
		CMD_Halt_Device(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,323,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Halt_Device_DL : public PreformedCommand {
	public:
		CMD_Halt_Device_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,323,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Halt_Device_DT : public PreformedCommand {
	public:
		CMD_Halt_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,323,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Halt_Device_Cat : public PreformedCommand {
	public:
		CMD_Halt_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string sForce) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,323,2,2,StringUtils::itos(iPK_Device).c_str(),21,sForce.c_str()); }
	};
	class CMD_Set_Timeout : public PreformedCommand {
	public:
		CMD_Set_Timeout(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,324,2,3,sPK_DesignObj.c_str(),102,sTime.c_str()); }
	};
	class CMD_Set_Timeout_DL : public PreformedCommand {
	public:
		CMD_Set_Timeout_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,324,2,3,sPK_DesignObj.c_str(),102,sTime.c_str()); }
	};
	class CMD_Set_Timeout_DT : public PreformedCommand {
	public:
		CMD_Set_Timeout_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sTime) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,324,2,3,sPK_DesignObj.c_str(),102,sTime.c_str()); }
	};
	class CMD_Set_Timeout_Cat : public PreformedCommand {
	public:
		CMD_Set_Timeout_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sTime) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,324,2,3,sPK_DesignObj.c_str(),102,sTime.c_str()); }
	};
	class CMD_Keep_Screen_On : public PreformedCommand {
	public:
		CMD_Keep_Screen_On(long DeviceIDFrom, long DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,325,1,8,sOnOff.c_str()); }
	};
	class CMD_Keep_Screen_On_DL : public PreformedCommand {
	public:
		CMD_Keep_Screen_On_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,325,1,8,sOnOff.c_str()); }
	};
	class CMD_Keep_Screen_On_DT : public PreformedCommand {
	public:
		CMD_Keep_Screen_On_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,325,1,8,sOnOff.c_str()); }
	};
	class CMD_Keep_Screen_On_Cat : public PreformedCommand {
	public:
		CMD_Keep_Screen_On_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,325,1,8,sOnOff.c_str()); }
	};
	class CMD_Tuner_3 : public PreformedCommand {
	public:
		CMD_Tuner_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,326,0); }
	};
	class CMD_Tuner_3_DL : public PreformedCommand {
	public:
		CMD_Tuner_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,326,0); }
	};
	class CMD_Tuner_3_DT : public PreformedCommand {
	public:
		CMD_Tuner_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,326,0); }
	};
	class CMD_Tuner_3_Cat : public PreformedCommand {
	public:
		CMD_Tuner_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,326,0); }
	};
	class CMD_WB : public PreformedCommand {
	public:
		CMD_WB(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,327,0); }
	};
	class CMD_WB_DL : public PreformedCommand {
	public:
		CMD_WB_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,327,0); }
	};
	class CMD_WB_DT : public PreformedCommand {
	public:
		CMD_WB_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,327,0); }
	};
	class CMD_WB_Cat : public PreformedCommand {
	public:
		CMD_WB_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,327,0); }
	};
	class CMD_SW : public PreformedCommand {
	public:
		CMD_SW(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,328,0); }
	};
	class CMD_SW_DL : public PreformedCommand {
	public:
		CMD_SW_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,328,0); }
	};
	class CMD_SW_DT : public PreformedCommand {
	public:
		CMD_SW_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,328,0); }
	};
	class CMD_SW_Cat : public PreformedCommand {
	public:
		CMD_SW_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,328,0); }
	};
	class CMD_Set_Mouse_Pointer_Over_Object : public PreformedCommand {
	public:
		CMD_Set_Mouse_Pointer_Over_Object(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,330,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Set_Mouse_Pointer_Over_Object_DL : public PreformedCommand {
	public:
		CMD_Set_Mouse_Pointer_Over_Object_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,330,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Set_Mouse_Pointer_Over_Object_DT : public PreformedCommand {
	public:
		CMD_Set_Mouse_Pointer_Over_Object_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,330,1,3,sPK_DesignObj.c_str()); }
	};
	class CMD_Set_Mouse_Pointer_Over_Object_Cat : public PreformedCommand {
	public:
		CMD_Set_Mouse_Pointer_Over_Object_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,330,1,3,sPK_DesignObj.c_str()); }
	};
	class RESP_Get_EntAreas_For_Device : public PreformedCommandResponse {
		string *m_sText;
	public:
		RESP_Get_EntAreas_For_Device(string *sText) { 
		m_sText=sText; }
		void ParseResponse(Message *pMessage) {
			*m_sText=pMessage->m_mapParameters[9]; };
	};
	class CMD_Get_EntAreas_For_Device : public PreformedCommand {
	public:
		CMD_Get_EntAreas_For_Device(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string *sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,2,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_EntAreas_For_Device(sText); }
	};
	class CMD_Get_EntAreas_For_Device_DL : public PreformedCommand {
	public:
		CMD_Get_EntAreas_For_Device_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string *sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,2,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_EntAreas_For_Device(sText); }
	};
	class CMD_Get_EntAreas_For_Device_DT : public PreformedCommand {
	public:
		CMD_Get_EntAreas_For_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string *sText) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,2,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_EntAreas_For_Device(sText); }
	};
	class CMD_Get_EntAreas_For_Device_Cat : public PreformedCommand {
	public:
		CMD_Get_EntAreas_For_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string *sText) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,2,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_EntAreas_For_Device(sText); }
	};
	class CMD_NOREP_Get_EntAreas_For_Device : public PreformedCommand {
	public:
		CMD_NOREP_Get_EntAreas_For_Device(long DeviceIDFrom, long DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_EntAreas_For_Device_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_EntAreas_For_Device_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_EntAreas_For_Device_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_EntAreas_For_Device_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_EntAreas_For_Device_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_EntAreas_For_Device_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,331,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Ignore_MAC_Address : public PreformedCommand {
	public:
		CMD_Ignore_MAC_Address(long DeviceIDFrom, long DeviceIDTo,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,332,1,47,sMac_address.c_str()); }
	};
	class CMD_Ignore_MAC_Address_DL : public PreformedCommand {
	public:
		CMD_Ignore_MAC_Address_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,332,1,47,sMac_address.c_str()); }
	};
	class CMD_Ignore_MAC_Address_DT : public PreformedCommand {
	public:
		CMD_Ignore_MAC_Address_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,332,1,47,sMac_address.c_str()); }
	};
	class CMD_Ignore_MAC_Address_Cat : public PreformedCommand {
	public:
		CMD_Ignore_MAC_Address_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,332,1,47,sMac_address.c_str()); }
	};
	class CMD_Disconnect_From_Mobile_Orbiter : public PreformedCommand {
	public:
		CMD_Disconnect_From_Mobile_Orbiter(long DeviceIDFrom, long DeviceIDTo,string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,333,4,47,sMac_address.c_str(),118,sVMC_File.c_str(),124,StringUtils::itos(iDeviceToLink).c_str(),130,sConfig_File.c_str()); }
	};
	class CMD_Disconnect_From_Mobile_Orbiter_DL : public PreformedCommand {
	public:
		CMD_Disconnect_From_Mobile_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,333,4,47,sMac_address.c_str(),118,sVMC_File.c_str(),124,StringUtils::itos(iDeviceToLink).c_str(),130,sConfig_File.c_str()); }
	};
	class CMD_Disconnect_From_Mobile_Orbiter_DT : public PreformedCommand {
	public:
		CMD_Disconnect_From_Mobile_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,333,4,47,sMac_address.c_str(),118,sVMC_File.c_str(),124,StringUtils::itos(iDeviceToLink).c_str(),130,sConfig_File.c_str()); }
	};
	class CMD_Disconnect_From_Mobile_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_Disconnect_From_Mobile_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address,string sVMC_File,int iDeviceToLink,string sConfig_File) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,333,4,47,sMac_address.c_str(),118,sVMC_File.c_str(),124,StringUtils::itos(iDeviceToLink).c_str(),130,sConfig_File.c_str()); }
	};
	class CMD_Phone_Initiate : public PreformedCommand {
	public:
		CMD_Phone_Initiate(long DeviceIDFrom, long DeviceIDTo,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,334,1,83,sPhoneExtension.c_str()); }
	};
	class CMD_Phone_Initiate_DL : public PreformedCommand {
	public:
		CMD_Phone_Initiate_DL(long DeviceIDFrom, string DeviceIDTo,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,334,1,83,sPhoneExtension.c_str()); }
	};
	class CMD_Phone_Initiate_DT : public PreformedCommand {
	public:
		CMD_Phone_Initiate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,334,1,83,sPhoneExtension.c_str()); }
	};
	class CMD_Phone_Initiate_Cat : public PreformedCommand {
	public:
		CMD_Phone_Initiate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,334,1,83,sPhoneExtension.c_str()); }
	};
	class CMD_Phone_Answer : public PreformedCommand {
	public:
		CMD_Phone_Answer(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,335,0); }
	};
	class CMD_Phone_Answer_DL : public PreformedCommand {
	public:
		CMD_Phone_Answer_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,335,0); }
	};
	class CMD_Phone_Answer_DT : public PreformedCommand {
	public:
		CMD_Phone_Answer_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,335,0); }
	};
	class CMD_Phone_Answer_Cat : public PreformedCommand {
	public:
		CMD_Phone_Answer_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,335,0); }
	};
	class CMD_Phone_Drop : public PreformedCommand {
	public:
		CMD_Phone_Drop(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,336,0); }
	};
	class CMD_Phone_Drop_DL : public PreformedCommand {
	public:
		CMD_Phone_Drop_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,336,0); }
	};
	class CMD_Phone_Drop_DT : public PreformedCommand {
	public:
		CMD_Phone_Drop_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,336,0); }
	};
	class CMD_Phone_Drop_Cat : public PreformedCommand {
	public:
		CMD_Phone_Drop_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,336,0); }
	};
	class CMD_Rip_Disk : public PreformedCommand {
	public:
		CMD_Rip_Disk(long DeviceIDFrom, long DeviceIDTo,int iPK_Users,string sName,string sTracks,int iEK_Disc) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,337,4,17,StringUtils::itos(iPK_Users).c_str(),50,sName.c_str(),121,sTracks.c_str(),131,StringUtils::itos(iEK_Disc).c_str()); }
	};
	class CMD_Rip_Disk_DL : public PreformedCommand {
	public:
		CMD_Rip_Disk_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Users,string sName,string sTracks,int iEK_Disc) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,337,4,17,StringUtils::itos(iPK_Users).c_str(),50,sName.c_str(),121,sTracks.c_str(),131,StringUtils::itos(iEK_Disc).c_str()); }
	};
	class CMD_Rip_Disk_DT : public PreformedCommand {
	public:
		CMD_Rip_Disk_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Users,string sName,string sTracks,int iEK_Disc) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,337,4,17,StringUtils::itos(iPK_Users).c_str(),50,sName.c_str(),121,sTracks.c_str(),131,StringUtils::itos(iEK_Disc).c_str()); }
	};
	class CMD_Rip_Disk_Cat : public PreformedCommand {
	public:
		CMD_Rip_Disk_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Users,string sName,string sTracks,int iEK_Disc) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,337,4,17,StringUtils::itos(iPK_Users).c_str(),50,sName.c_str(),121,sTracks.c_str(),131,StringUtils::itos(iEK_Disc).c_str()); }
	};
	class CMD_Process_Incoming_Data : public PreformedCommand {
	public:
		CMD_Process_Incoming_Data(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,350,0); }
	};
	class CMD_Process_Incoming_Data_DL : public PreformedCommand {
	public:
		CMD_Process_Incoming_Data_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,350,0); }
	};
	class CMD_Process_Incoming_Data_DT : public PreformedCommand {
	public:
		CMD_Process_Incoming_Data_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,350,0); }
	};
	class CMD_Process_Incoming_Data_Cat : public PreformedCommand {
	public:
		CMD_Process_Incoming_Data_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,350,0); }
	};
	class CMD_Process_IDLE : public PreformedCommand {
	public:
		CMD_Process_IDLE(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,351,0); }
	};
	class CMD_Process_IDLE_DL : public PreformedCommand {
	public:
		CMD_Process_IDLE_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,351,0); }
	};
	class CMD_Process_IDLE_DT : public PreformedCommand {
	public:
		CMD_Process_IDLE_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,351,0); }
	};
	class CMD_Process_IDLE_Cat : public PreformedCommand {
	public:
		CMD_Process_IDLE_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,351,0); }
	};
	class CMD_Show_Mouse_Pointer : public PreformedCommand {
	public:
		CMD_Show_Mouse_Pointer(long DeviceIDFrom, long DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,354,1,8,sOnOff.c_str()); }
	};
	class CMD_Show_Mouse_Pointer_DL : public PreformedCommand {
	public:
		CMD_Show_Mouse_Pointer_DL(long DeviceIDFrom, string DeviceIDTo,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,354,1,8,sOnOff.c_str()); }
	};
	class CMD_Show_Mouse_Pointer_DT : public PreformedCommand {
	public:
		CMD_Show_Mouse_Pointer_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,354,1,8,sOnOff.c_str()); }
	};
	class CMD_Show_Mouse_Pointer_Cat : public PreformedCommand {
	public:
		CMD_Show_Mouse_Pointer_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sOnOff) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,354,1,8,sOnOff.c_str()); }
	};
	class CMD_Process_Initialize : public PreformedCommand {
	public:
		CMD_Process_Initialize(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,355,0); }
	};
	class CMD_Process_Initialize_DL : public PreformedCommand {
	public:
		CMD_Process_Initialize_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,355,0); }
	};
	class CMD_Process_Initialize_DT : public PreformedCommand {
	public:
		CMD_Process_Initialize_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,355,0); }
	};
	class CMD_Process_Initialize_Cat : public PreformedCommand {
	public:
		CMD_Process_Initialize_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,355,0); }
	};
	class CMD_Process_Release : public PreformedCommand {
	public:
		CMD_Process_Release(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,356,0); }
	};
	class CMD_Process_Release_DL : public PreformedCommand {
	public:
		CMD_Process_Release_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,356,0); }
	};
	class CMD_Process_Release_DT : public PreformedCommand {
	public:
		CMD_Process_Release_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,356,0); }
	};
	class CMD_Process_Release_Cat : public PreformedCommand {
	public:
		CMD_Process_Release_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,356,0); }
	};
	class CMD_HDMI : public PreformedCommand {
	public:
		CMD_HDMI(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,361,0); }
	};
	class CMD_HDMI_DL : public PreformedCommand {
	public:
		CMD_HDMI_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,361,0); }
	};
	class CMD_HDMI_DT : public PreformedCommand {
	public:
		CMD_HDMI_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,361,0); }
	};
	class CMD_HDMI_Cat : public PreformedCommand {
	public:
		CMD_HDMI_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,361,0); }
	};
	class CMD_D4 : public PreformedCommand {
	public:
		CMD_D4(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,362,0); }
	};
	class CMD_D4_DL : public PreformedCommand {
	public:
		CMD_D4_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,362,0); }
	};
	class CMD_D4_DT : public PreformedCommand {
	public:
		CMD_D4_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,362,0); }
	};
	class CMD_D4_Cat : public PreformedCommand {
	public:
		CMD_D4_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,362,0); }
	};
	class CMD_Back_Clear_Entry : public PreformedCommand {
	public:
		CMD_Back_Clear_Entry(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,363,0); }
	};
	class CMD_Back_Clear_Entry_DL : public PreformedCommand {
	public:
		CMD_Back_Clear_Entry_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,363,0); }
	};
	class CMD_Back_Clear_Entry_DT : public PreformedCommand {
	public:
		CMD_Back_Clear_Entry_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,363,0); }
	};
	class CMD_Back_Clear_Entry_Cat : public PreformedCommand {
	public:
		CMD_Back_Clear_Entry_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,363,0); }
	};
	class RESP_Get_Room_Description : public PreformedCommandResponse {
		string *m_sText;int *m_iPK_Room;
	public:
		RESP_Get_Room_Description(string *sText,int *iPK_Room) { 
		m_sText=sText; m_iPK_Room=iPK_Room; }
		void ParseResponse(Message *pMessage) {
			*m_sText=pMessage->m_mapParameters[9]; *m_iPK_Room=atoi(pMessage->m_mapParameters[57].c_str()); };
	};
	class CMD_Get_Room_Description : public PreformedCommand {
	public:
		CMD_Get_Room_Description(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,string *sText,int *iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,3,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str(),57,StringUtils::itos(*iPK_Room).c_str());		m_pcResponse = new RESP_Get_Room_Description(sText,iPK_Room); }
	};
	class CMD_Get_Room_Description_DL : public PreformedCommand {
	public:
		CMD_Get_Room_Description_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,string *sText,int *iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,3,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str(),57,StringUtils::itos(*iPK_Room).c_str());		m_pcResponse = new RESP_Get_Room_Description(sText,iPK_Room); }
	};
	class CMD_Get_Room_Description_DT : public PreformedCommand {
	public:
		CMD_Get_Room_Description_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,string *sText,int *iPK_Room) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,3,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str(),57,StringUtils::itos(*iPK_Room).c_str());		m_pcResponse = new RESP_Get_Room_Description(sText,iPK_Room); }
	};
	class CMD_Get_Room_Description_Cat : public PreformedCommand {
	public:
		CMD_Get_Room_Description_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,string *sText,int *iPK_Room) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,3,2,StringUtils::itos(iPK_Device).c_str(),9,(*sText).c_str(),57,StringUtils::itos(*iPK_Room).c_str());		m_pcResponse = new RESP_Get_Room_Description(sText,iPK_Room); }
	};
	class CMD_NOREP_Get_Room_Description : public PreformedCommand {
	public:
		CMD_NOREP_Get_Room_Description(long DeviceIDFrom, long DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Room_Description_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Room_Description_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Room_Description_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Room_Description_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_NOREP_Get_Room_Description_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Room_Description_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,365,1,2,StringUtils::itos(iPK_Device).c_str()); }
	};
	class CMD_Activate_Window : public PreformedCommand {
	public:
		CMD_Activate_Window(long DeviceIDFrom, long DeviceIDTo,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,366,1,50,sName.c_str()); }
	};
	class CMD_Activate_Window_DL : public PreformedCommand {
	public:
		CMD_Activate_Window_DL(long DeviceIDFrom, string DeviceIDTo,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,366,1,50,sName.c_str()); }
	};
	class CMD_Activate_Window_DT : public PreformedCommand {
	public:
		CMD_Activate_Window_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,366,1,50,sName.c_str()); }
	};
	class CMD_Activate_Window_Cat : public PreformedCommand {
	public:
		CMD_Activate_Window_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,366,1,50,sName.c_str()); }
	};
	class CMD_Text : public PreformedCommand {
	public:
		CMD_Text(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,367,0); }
	};
	class CMD_Text_DL : public PreformedCommand {
	public:
		CMD_Text_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,367,0); }
	};
	class CMD_Text_DT : public PreformedCommand {
	public:
		CMD_Text_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,367,0); }
	};
	class CMD_Text_Cat : public PreformedCommand {
	public:
		CMD_Text_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,367,0); }
	};
	class CMD_Help : public PreformedCommand {
	public:
		CMD_Help(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,368,0); }
	};
	class CMD_Help_DL : public PreformedCommand {
	public:
		CMD_Help_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,368,0); }
	};
	class CMD_Help_DT : public PreformedCommand {
	public:
		CMD_Help_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,368,0); }
	};
	class CMD_Help_Cat : public PreformedCommand {
	public:
		CMD_Help_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,368,0); }
	};
	class CMD_Radio_Mode : public PreformedCommand {
	public:
		CMD_Radio_Mode(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,369,0); }
	};
	class CMD_Radio_Mode_DL : public PreformedCommand {
	public:
		CMD_Radio_Mode_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,369,0); }
	};
	class CMD_Radio_Mode_DT : public PreformedCommand {
	public:
		CMD_Radio_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,369,0); }
	};
	class CMD_Radio_Mode_Cat : public PreformedCommand {
	public:
		CMD_Radio_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,369,0); }
	};
	class CMD_Execute_Command_Group : public PreformedCommand {
	public:
		CMD_Execute_Command_Group(long DeviceIDFrom, long DeviceIDTo,int iPK_CommandGroup) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,370,1,28,StringUtils::itos(iPK_CommandGroup).c_str()); }
	};
	class CMD_Execute_Command_Group_DL : public PreformedCommand {
	public:
		CMD_Execute_Command_Group_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_CommandGroup) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,370,1,28,StringUtils::itos(iPK_CommandGroup).c_str()); }
	};
	class CMD_Execute_Command_Group_DT : public PreformedCommand {
	public:
		CMD_Execute_Command_Group_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_CommandGroup) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,370,1,28,StringUtils::itos(iPK_CommandGroup).c_str()); }
	};
	class CMD_Execute_Command_Group_Cat : public PreformedCommand {
	public:
		CMD_Execute_Command_Group_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_CommandGroup) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,370,1,28,StringUtils::itos(iPK_CommandGroup).c_str()); }
	};
	class RESP_Is_Daytime : public PreformedCommandResponse {
		bool *m_bTrueFalse;
	public:
		RESP_Is_Daytime(bool *bTrueFalse) { 
		m_bTrueFalse=bTrueFalse; }
		void ParseResponse(Message *pMessage) {
			*m_bTrueFalse=(pMessage->m_mapParameters[119]=="1" ? true : false); };
	};
	class CMD_Is_Daytime : public PreformedCommand {
	public:
		CMD_Is_Daytime(long DeviceIDFrom, long DeviceIDTo,bool *bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,1,119,(*bTrueFalse ? "1" : "0"));		m_pcResponse = new RESP_Is_Daytime(bTrueFalse); }
	};
	class CMD_Is_Daytime_DL : public PreformedCommand {
	public:
		CMD_Is_Daytime_DL(long DeviceIDFrom, string DeviceIDTo,bool *bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,1,119,(*bTrueFalse ? "1" : "0"));		m_pcResponse = new RESP_Is_Daytime(bTrueFalse); }
	};
	class CMD_Is_Daytime_DT : public PreformedCommand {
	public:
		CMD_Is_Daytime_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,bool *bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,1,119,(*bTrueFalse ? "1" : "0"));		m_pcResponse = new RESP_Is_Daytime(bTrueFalse); }
	};
	class CMD_Is_Daytime_Cat : public PreformedCommand {
	public:
		CMD_Is_Daytime_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,bool *bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,1,119,(*bTrueFalse ? "1" : "0"));		m_pcResponse = new RESP_Is_Daytime(bTrueFalse); }
	};
	class CMD_NOREP_Is_Daytime : public PreformedCommand {
	public:
		CMD_NOREP_Is_Daytime(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,0); }
	};
	class CMD_NOREP_Is_Daytime_DL : public PreformedCommand {
	public:
		CMD_NOREP_Is_Daytime_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,0); }
	};
	class CMD_NOREP_Is_Daytime_DT : public PreformedCommand {
	public:
		CMD_NOREP_Is_Daytime_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,0); }
	};
	class CMD_NOREP_Is_Daytime_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Is_Daytime_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,371,0); }
	};
	class CMD_MH_Set_Volume : public PreformedCommand {
	public:
		CMD_MH_Set_Volume(long DeviceIDFrom, long DeviceIDTo,string sPK_EntertainArea,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,372,2,45,sPK_EntertainArea.c_str(),76,sLevel.c_str()); }
	};
	class CMD_MH_Set_Volume_DL : public PreformedCommand {
	public:
		CMD_MH_Set_Volume_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_EntertainArea,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,372,2,45,sPK_EntertainArea.c_str(),76,sLevel.c_str()); }
	};
	class CMD_MH_Set_Volume_DT : public PreformedCommand {
	public:
		CMD_MH_Set_Volume_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_EntertainArea,string sLevel) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,372,2,45,sPK_EntertainArea.c_str(),76,sLevel.c_str()); }
	};
	class CMD_MH_Set_Volume_Cat : public PreformedCommand {
	public:
		CMD_MH_Set_Volume_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_EntertainArea,string sLevel) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,372,2,45,sPK_EntertainArea.c_str(),76,sLevel.c_str()); }
	};
	class CMD_Private_Method_Listing : public PreformedCommand {
	public:
		CMD_Private_Method_Listing(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,373,0); }
	};
	class CMD_Private_Method_Listing_DL : public PreformedCommand {
	public:
		CMD_Private_Method_Listing_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,373,0); }
	};
	class CMD_Private_Method_Listing_DT : public PreformedCommand {
	public:
		CMD_Private_Method_Listing_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,373,0); }
	};
	class CMD_Private_Method_Listing_Cat : public PreformedCommand {
	public:
		CMD_Private_Method_Listing_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,373,0); }
	};
	class CMD_Input_1 : public PreformedCommand {
	public:
		CMD_Input_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,374,0); }
	};
	class CMD_Input_1_DL : public PreformedCommand {
	public:
		CMD_Input_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,374,0); }
	};
	class CMD_Input_1_DT : public PreformedCommand {
	public:
		CMD_Input_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,374,0); }
	};
	class CMD_Input_1_Cat : public PreformedCommand {
	public:
		CMD_Input_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,374,0); }
	};
	class CMD_Input_2 : public PreformedCommand {
	public:
		CMD_Input_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,375,0); }
	};
	class CMD_Input_2_DL : public PreformedCommand {
	public:
		CMD_Input_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,375,0); }
	};
	class CMD_Input_2_DT : public PreformedCommand {
	public:
		CMD_Input_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,375,0); }
	};
	class CMD_Input_2_Cat : public PreformedCommand {
	public:
		CMD_Input_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,375,0); }
	};
	class CMD_Input_3 : public PreformedCommand {
	public:
		CMD_Input_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,376,0); }
	};
	class CMD_Input_3_DL : public PreformedCommand {
	public:
		CMD_Input_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,376,0); }
	};
	class CMD_Input_3_DT : public PreformedCommand {
	public:
		CMD_Input_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,376,0); }
	};
	class CMD_Input_3_Cat : public PreformedCommand {
	public:
		CMD_Input_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,376,0); }
	};
	class CMD_Input_4 : public PreformedCommand {
	public:
		CMD_Input_4(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,377,0); }
	};
	class CMD_Input_4_DL : public PreformedCommand {
	public:
		CMD_Input_4_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,377,0); }
	};
	class CMD_Input_4_DT : public PreformedCommand {
	public:
		CMD_Input_4_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,377,0); }
	};
	class CMD_Input_4_Cat : public PreformedCommand {
	public:
		CMD_Input_4_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,377,0); }
	};
	class CMD_Input_5 : public PreformedCommand {
	public:
		CMD_Input_5(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,378,0); }
	};
	class CMD_Input_5_DL : public PreformedCommand {
	public:
		CMD_Input_5_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,378,0); }
	};
	class CMD_Input_5_DT : public PreformedCommand {
	public:
		CMD_Input_5_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,378,0); }
	};
	class CMD_Input_5_Cat : public PreformedCommand {
	public:
		CMD_Input_5_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,378,0); }
	};
	class CMD_Input_6 : public PreformedCommand {
	public:
		CMD_Input_6(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,379,0); }
	};
	class CMD_Input_6_DL : public PreformedCommand {
	public:
		CMD_Input_6_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,379,0); }
	};
	class CMD_Input_6_DT : public PreformedCommand {
	public:
		CMD_Input_6_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,379,0); }
	};
	class CMD_Input_6_Cat : public PreformedCommand {
	public:
		CMD_Input_6_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,379,0); }
	};
	class CMD_Input_7 : public PreformedCommand {
	public:
		CMD_Input_7(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,380,0); }
	};
	class CMD_Input_7_DL : public PreformedCommand {
	public:
		CMD_Input_7_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,380,0); }
	};
	class CMD_Input_7_DT : public PreformedCommand {
	public:
		CMD_Input_7_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,380,0); }
	};
	class CMD_Input_7_Cat : public PreformedCommand {
	public:
		CMD_Input_7_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,380,0); }
	};
	class CMD_Input_8 : public PreformedCommand {
	public:
		CMD_Input_8(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,381,0); }
	};
	class CMD_Input_8_DL : public PreformedCommand {
	public:
		CMD_Input_8_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,381,0); }
	};
	class CMD_Input_8_DT : public PreformedCommand {
	public:
		CMD_Input_8_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,381,0); }
	};
	class CMD_Input_8_Cat : public PreformedCommand {
	public:
		CMD_Input_8_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,381,0); }
	};
	class CMD_Input_9 : public PreformedCommand {
	public:
		CMD_Input_9(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,382,0); }
	};
	class CMD_Input_9_DL : public PreformedCommand {
	public:
		CMD_Input_9_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,382,0); }
	};
	class CMD_Input_9_DT : public PreformedCommand {
	public:
		CMD_Input_9_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,382,0); }
	};
	class CMD_Input_9_Cat : public PreformedCommand {
	public:
		CMD_Input_9_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,382,0); }
	};
	class CMD_Input_10 : public PreformedCommand {
	public:
		CMD_Input_10(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,383,0); }
	};
	class CMD_Input_10_DL : public PreformedCommand {
	public:
		CMD_Input_10_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,383,0); }
	};
	class CMD_Input_10_DT : public PreformedCommand {
	public:
		CMD_Input_10_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,383,0); }
	};
	class CMD_Input_10_Cat : public PreformedCommand {
	public:
		CMD_Input_10_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,383,0); }
	};
	class CMD_Process_Receive_Command_For_Child : public PreformedCommand {
	public:
		CMD_Process_Receive_Command_For_Child(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,384,0); }
	};
	class CMD_Process_Receive_Command_For_Child_DL : public PreformedCommand {
	public:
		CMD_Process_Receive_Command_For_Child_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,384,0); }
	};
	class CMD_Process_Receive_Command_For_Child_DT : public PreformedCommand {
	public:
		CMD_Process_Receive_Command_For_Child_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,384,0); }
	};
	class CMD_Process_Receive_Command_For_Child_Cat : public PreformedCommand {
	public:
		CMD_Process_Receive_Command_For_Child_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,384,0); }
	};
	class RESP_Verify_PIN : public PreformedCommandResponse {
		bool *m_bIsSuccessful;
	public:
		RESP_Verify_PIN(bool *bIsSuccessful) { 
		m_bIsSuccessful=bIsSuccessful; }
		void ParseResponse(Message *pMessage) {
			*m_bIsSuccessful=(pMessage->m_mapParameters[40]=="1" ? true : false); };
	};
	class CMD_Verify_PIN : public PreformedCommand {
	public:
		CMD_Verify_PIN(long DeviceIDFrom, long DeviceIDTo,int iPK_Users,string sPassword,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,3,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),40,(*bIsSuccessful ? "1" : "0"));		m_pcResponse = new RESP_Verify_PIN(bIsSuccessful); }
	};
	class CMD_Verify_PIN_DL : public PreformedCommand {
	public:
		CMD_Verify_PIN_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Users,string sPassword,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,3,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),40,(*bIsSuccessful ? "1" : "0"));		m_pcResponse = new RESP_Verify_PIN(bIsSuccessful); }
	};
	class CMD_Verify_PIN_DT : public PreformedCommand {
	public:
		CMD_Verify_PIN_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Users,string sPassword,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,3,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),40,(*bIsSuccessful ? "1" : "0"));		m_pcResponse = new RESP_Verify_PIN(bIsSuccessful); }
	};
	class CMD_Verify_PIN_Cat : public PreformedCommand {
	public:
		CMD_Verify_PIN_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Users,string sPassword,bool *bIsSuccessful) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,3,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str(),40,(*bIsSuccessful ? "1" : "0"));		m_pcResponse = new RESP_Verify_PIN(bIsSuccessful); }
	};
	class CMD_NOREP_Verify_PIN : public PreformedCommand {
	public:
		CMD_NOREP_Verify_PIN(long DeviceIDFrom, long DeviceIDTo,int iPK_Users,string sPassword) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,2,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str()); }
	};
	class CMD_NOREP_Verify_PIN_DL : public PreformedCommand {
	public:
		CMD_NOREP_Verify_PIN_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Users,string sPassword) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,2,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str()); }
	};
	class CMD_NOREP_Verify_PIN_DT : public PreformedCommand {
	public:
		CMD_NOREP_Verify_PIN_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Users,string sPassword) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,2,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str()); }
	};
	class CMD_NOREP_Verify_PIN_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Verify_PIN_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Users,string sPassword) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,387,2,17,StringUtils::itos(iPK_Users).c_str(),99,sPassword.c_str()); }
	};
	class CMD_Set_Media_Private : public PreformedCommand {
	public:
		CMD_Set_Media_Private(long DeviceIDFrom, long DeviceIDTo,string sPK_EntertainArea,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,388,2,45,sPK_EntertainArea.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Media_Private_DL : public PreformedCommand {
	public:
		CMD_Set_Media_Private_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_EntertainArea,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,388,2,45,sPK_EntertainArea.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Media_Private_DT : public PreformedCommand {
	public:
		CMD_Set_Media_Private_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_EntertainArea,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,388,2,45,sPK_EntertainArea.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Media_Private_Cat : public PreformedCommand {
	public:
		CMD_Set_Media_Private_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_EntertainArea,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,388,2,45,sPK_EntertainArea.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Send_Message : public PreformedCommand {
	public:
		CMD_Send_Message(long DeviceIDFrom, long DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,389,1,9,sText.c_str()); }
	};
	class CMD_Send_Message_DL : public PreformedCommand {
	public:
		CMD_Send_Message_DL(long DeviceIDFrom, string DeviceIDTo,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,389,1,9,sText.c_str()); }
	};
	class CMD_Send_Message_DT : public PreformedCommand {
	public:
		CMD_Send_Message_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,389,1,9,sText.c_str()); }
	};
	class CMD_Send_Message_Cat : public PreformedCommand {
	public:
		CMD_Send_Message_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,389,1,9,sText.c_str()); }
	};
	class CMD_Add_Media_Attribute : public PreformedCommand {
	public:
		CMD_Add_Media_Attribute(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,391,4,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str(),121,sTracks.c_str(),122,StringUtils::itos(iEK_AttributeType).c_str()); }
	};
	class CMD_Add_Media_Attribute_DL : public PreformedCommand {
	public:
		CMD_Add_Media_Attribute_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,391,4,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str(),121,sTracks.c_str(),122,StringUtils::itos(iEK_AttributeType).c_str()); }
	};
	class CMD_Add_Media_Attribute_DT : public PreformedCommand {
	public:
		CMD_Add_Media_Attribute_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,391,4,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str(),121,sTracks.c_str(),122,StringUtils::itos(iEK_AttributeType).c_str()); }
	};
	class CMD_Add_Media_Attribute_Cat : public PreformedCommand {
	public:
		CMD_Add_Media_Attribute_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,int iStreamID,string sTracks,int iEK_AttributeType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,391,4,5,sValue_To_Assign.c_str(),41,StringUtils::itos(iStreamID).c_str(),121,sTracks.c_str(),122,StringUtils::itos(iEK_AttributeType).c_str()); }
	};
	class CMD_Set_Media_Attribute_Text : public PreformedCommand {
	public:
		CMD_Set_Media_Attribute_Text(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,392,2,5,sValue_To_Assign.c_str(),123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class CMD_Set_Media_Attribute_Text_DL : public PreformedCommand {
	public:
		CMD_Set_Media_Attribute_Text_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,392,2,5,sValue_To_Assign.c_str(),123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class CMD_Set_Media_Attribute_Text_DT : public PreformedCommand {
	public:
		CMD_Set_Media_Attribute_Text_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,392,2,5,sValue_To_Assign.c_str(),123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class CMD_Set_Media_Attribute_Text_Cat : public PreformedCommand {
	public:
		CMD_Set_Media_Attribute_Text_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,392,2,5,sValue_To_Assign.c_str(),123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class RESP_Get_Attribute : public PreformedCommandResponse {
		string *m_sText;
	public:
		RESP_Get_Attribute(string *sText) { 
		m_sText=sText; }
		void ParseResponse(Message *pMessage) {
			*m_sText=pMessage->m_mapParameters[9]; };
	};
	class CMD_Get_Attribute : public PreformedCommand {
	public:
		CMD_Get_Attribute(long DeviceIDFrom, long DeviceIDTo,int iEK_Attribute,string *sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,2,123,StringUtils::itos(iEK_Attribute).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_Attribute(sText); }
	};
	class CMD_Get_Attribute_DL : public PreformedCommand {
	public:
		CMD_Get_Attribute_DL(long DeviceIDFrom, string DeviceIDTo,int iEK_Attribute,string *sText) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,2,123,StringUtils::itos(iEK_Attribute).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_Attribute(sText); }
	};
	class CMD_Get_Attribute_DT : public PreformedCommand {
	public:
		CMD_Get_Attribute_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iEK_Attribute,string *sText) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,2,123,StringUtils::itos(iEK_Attribute).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_Attribute(sText); }
	};
	class CMD_Get_Attribute_Cat : public PreformedCommand {
	public:
		CMD_Get_Attribute_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iEK_Attribute,string *sText) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,2,123,StringUtils::itos(iEK_Attribute).c_str(),9,(*sText).c_str());		m_pcResponse = new RESP_Get_Attribute(sText); }
	};
	class CMD_NOREP_Get_Attribute : public PreformedCommand {
	public:
		CMD_NOREP_Get_Attribute(long DeviceIDFrom, long DeviceIDTo,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,1,123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class CMD_NOREP_Get_Attribute_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Attribute_DL(long DeviceIDFrom, string DeviceIDTo,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,1,123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class CMD_NOREP_Get_Attribute_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Attribute_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,1,123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class CMD_NOREP_Get_Attribute_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Attribute_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iEK_Attribute) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,393,1,123,StringUtils::itos(iEK_Attribute).c_str()); }
	};
	class CMD_Radio : public PreformedCommand {
	public:
		CMD_Radio(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,394,0); }
	};
	class CMD_Radio_DL : public PreformedCommand {
	public:
		CMD_Radio_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,394,0); }
	};
	class CMD_Radio_DT : public PreformedCommand {
	public:
		CMD_Radio_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,394,0); }
	};
	class CMD_Radio_Cat : public PreformedCommand {
	public:
		CMD_Radio_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,394,0); }
	};
	class CMD_Check_for_updates : public PreformedCommand {
	public:
		CMD_Check_for_updates(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,395,0); }
	};
	class CMD_Check_for_updates_DL : public PreformedCommand {
	public:
		CMD_Check_for_updates_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,395,0); }
	};
	class CMD_Check_for_updates_DT : public PreformedCommand {
	public:
		CMD_Check_for_updates_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,395,0); }
	};
	class CMD_Check_for_updates_Cat : public PreformedCommand {
	public:
		CMD_Check_for_updates_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,395,0); }
	};
	class CMD_Check_for_updates_done : public PreformedCommand {
	public:
		CMD_Check_for_updates_done(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,396,0); }
	};
	class CMD_Check_for_updates_done_DL : public PreformedCommand {
	public:
		CMD_Check_for_updates_done_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,396,0); }
	};
	class CMD_Check_for_updates_done_DT : public PreformedCommand {
	public:
		CMD_Check_for_updates_done_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,396,0); }
	};
	class CMD_Check_for_updates_done_Cat : public PreformedCommand {
	public:
		CMD_Check_for_updates_done_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,396,0); }
	};
	class CMD_Show_Popup : public PreformedCommand {
	public:
		CMD_Show_Popup(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,397,7,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str(),126,(bExclusive ? "1" : "0"),127,(bDont_Auto_Hide ? "1" : "0")); }
	};
	class CMD_Show_Popup_DL : public PreformedCommand {
	public:
		CMD_Show_Popup_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,397,7,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str(),126,(bExclusive ? "1" : "0"),127,(bDont_Auto_Hide ? "1" : "0")); }
	};
	class CMD_Show_Popup_DT : public PreformedCommand {
	public:
		CMD_Show_Popup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,397,7,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str(),126,(bExclusive ? "1" : "0"),127,(bDont_Auto_Hide ? "1" : "0")); }
	};
	class CMD_Show_Popup_Cat : public PreformedCommand {
	public:
		CMD_Show_Popup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen,string sName,bool bExclusive,bool bDont_Auto_Hide) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,397,7,3,sPK_DesignObj.c_str(),11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str(),126,(bExclusive ? "1" : "0"),127,(bDont_Auto_Hide ? "1" : "0")); }
	};
	class CMD_Remove_Popup : public PreformedCommand {
	public:
		CMD_Remove_Popup(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj_CurrentScreen,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,398,2,16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str()); }
	};
	class CMD_Remove_Popup_DL : public PreformedCommand {
	public:
		CMD_Remove_Popup_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj_CurrentScreen,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,398,2,16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str()); }
	};
	class CMD_Remove_Popup_DT : public PreformedCommand {
	public:
		CMD_Remove_Popup_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj_CurrentScreen,string sName) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,398,2,16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str()); }
	};
	class CMD_Remove_Popup_Cat : public PreformedCommand {
	public:
		CMD_Remove_Popup_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj_CurrentScreen,string sName) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,398,2,16,sPK_DesignObj_CurrentScreen.c_str(),50,sName.c_str()); }
	};
	class CMD_Show_Shortcuts : public PreformedCommand {
	public:
		CMD_Show_Shortcuts(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,399,0); }
	};
	class CMD_Show_Shortcuts_DL : public PreformedCommand {
	public:
		CMD_Show_Shortcuts_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,399,0); }
	};
	class CMD_Show_Shortcuts_DT : public PreformedCommand {
	public:
		CMD_Show_Shortcuts_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,399,0); }
	};
	class CMD_Show_Shortcuts_Cat : public PreformedCommand {
	public:
		CMD_Show_Shortcuts_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,399,0); }
	};
	class CMD_Show_File_List : public PreformedCommand {
	public:
		CMD_Show_File_List(long DeviceIDFrom, long DeviceIDTo,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,401,1,29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Show_File_List_DL : public PreformedCommand {
	public:
		CMD_Show_File_List_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,401,1,29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Show_File_List_DT : public PreformedCommand {
	public:
		CMD_Show_File_List_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,401,1,29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Show_File_List_Cat : public PreformedCommand {
	public:
		CMD_Show_File_List_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,401,1,29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Use_Popup_Remote_Controls : public PreformedCommand {
	public:
		CMD_Use_Popup_Remote_Controls(long DeviceIDFrom, long DeviceIDTo,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,402,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Use_Popup_Remote_Controls_DL : public PreformedCommand {
	public:
		CMD_Use_Popup_Remote_Controls_DL(long DeviceIDFrom, string DeviceIDTo,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,402,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Use_Popup_Remote_Controls_DT : public PreformedCommand {
	public:
		CMD_Use_Popup_Remote_Controls_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,402,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Use_Popup_Remote_Controls_Cat : public PreformedCommand {
	public:
		CMD_Use_Popup_Remote_Controls_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,402,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Use_Popup_File_List : public PreformedCommand {
	public:
		CMD_Use_Popup_File_List(long DeviceIDFrom, long DeviceIDTo,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,403,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Use_Popup_File_List_DL : public PreformedCommand {
	public:
		CMD_Use_Popup_File_List_DL(long DeviceIDFrom, string DeviceIDTo,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,403,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Use_Popup_File_List_DT : public PreformedCommand {
	public:
		CMD_Use_Popup_File_List_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,403,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Use_Popup_File_List_Cat : public PreformedCommand {
	public:
		CMD_Use_Popup_File_List_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPosition_X,int iPosition_Y,string sPK_DesignObj_CurrentScreen) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,403,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),16,sPK_DesignObj_CurrentScreen.c_str()); }
	};
	class CMD_Set_Auto_Switch_to_Remote : public PreformedCommand {
	public:
		CMD_Set_Auto_Switch_to_Remote(long DeviceIDFrom, long DeviceIDTo,int iPK_Device,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,404,2,2,StringUtils::itos(iPK_Device).c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Auto_Switch_to_Remote_DL : public PreformedCommand {
	public:
		CMD_Set_Auto_Switch_to_Remote_DL(long DeviceIDFrom, string DeviceIDTo,int iPK_Device,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,404,2,2,StringUtils::itos(iPK_Device).c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Auto_Switch_to_Remote_DT : public PreformedCommand {
	public:
		CMD_Set_Auto_Switch_to_Remote_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPK_Device,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,404,2,2,StringUtils::itos(iPK_Device).c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Auto_Switch_to_Remote_Cat : public PreformedCommand {
	public:
		CMD_Set_Auto_Switch_to_Remote_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPK_Device,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,404,2,2,StringUtils::itos(iPK_Device).c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Scale_this_object : public PreformedCommand {
	public:
		CMD_Scale_this_object(long DeviceIDFrom, long DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,405,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Scale_this_object_DL : public PreformedCommand {
	public:
		CMD_Scale_this_object_DL(long DeviceIDFrom, string DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,405,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Scale_this_object_DT : public PreformedCommand {
	public:
		CMD_Scale_this_object_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,405,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Scale_this_object_Cat : public PreformedCommand {
	public:
		CMD_Scale_this_object_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,405,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Display_Message : public PreformedCommand {
	public:
		CMD_Display_Message(long DeviceIDFrom, long DeviceIDTo,string sText,string sType,string sName,string sTime,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,406,5,9,sText.c_str(),14,sType.c_str(),50,sName.c_str(),102,sTime.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Display_Message_DL : public PreformedCommand {
	public:
		CMD_Display_Message_DL(long DeviceIDFrom, string DeviceIDTo,string sText,string sType,string sName,string sTime,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,406,5,9,sText.c_str(),14,sType.c_str(),50,sName.c_str(),102,sTime.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Display_Message_DT : public PreformedCommand {
	public:
		CMD_Display_Message_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,string sType,string sName,string sTime,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,406,5,9,sText.c_str(),14,sType.c_str(),50,sName.c_str(),102,sTime.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Display_Message_Cat : public PreformedCommand {
	public:
		CMD_Display_Message_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText,string sType,string sName,string sTime,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,406,5,9,sText.c_str(),14,sType.c_str(),50,sName.c_str(),102,sTime.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Set_Floorplan : public PreformedCommand {
	public:
		CMD_Set_Floorplan(long DeviceIDFrom, long DeviceIDTo,string sPK_DesignObj,string sType,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,407,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Floorplan_DL : public PreformedCommand {
	public:
		CMD_Set_Floorplan_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_DesignObj,string sType,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,407,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Floorplan_DT : public PreformedCommand {
	public:
		CMD_Set_Floorplan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_DesignObj,string sType,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,407,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Set_Floorplan_Cat : public PreformedCommand {
	public:
		CMD_Set_Floorplan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_DesignObj,string sType,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,407,3,3,sPK_DesignObj.c_str(),14,sType.c_str(),119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Show_Floorplan : public PreformedCommand {
	public:
		CMD_Show_Floorplan(long DeviceIDFrom, long DeviceIDTo,int iPosition_X,int iPosition_Y,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,408,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),14,sType.c_str()); }
	};
	class CMD_Show_Floorplan_DL : public PreformedCommand {
	public:
		CMD_Show_Floorplan_DL(long DeviceIDFrom, string DeviceIDTo,int iPosition_X,int iPosition_Y,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,408,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),14,sType.c_str()); }
	};
	class CMD_Show_Floorplan_DT : public PreformedCommand {
	public:
		CMD_Show_Floorplan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPosition_X,int iPosition_Y,string sType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,408,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),14,sType.c_str()); }
	};
	class CMD_Show_Floorplan_Cat : public PreformedCommand {
	public:
		CMD_Show_Floorplan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPosition_X,int iPosition_Y,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,408,3,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str(),14,sType.c_str()); }
	};
	class CMD_Save_Bookmark : public PreformedCommand {
	public:
		CMD_Save_Bookmark(long DeviceIDFrom, long DeviceIDTo,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,409,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Save_Bookmark_DL : public PreformedCommand {
	public:
		CMD_Save_Bookmark_DL(long DeviceIDFrom, string DeviceIDTo,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,409,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Save_Bookmark_DT : public PreformedCommand {
	public:
		CMD_Save_Bookmark_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,409,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Save_Bookmark_Cat : public PreformedCommand {
	public:
		CMD_Save_Bookmark_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPK_EntertainArea) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,409,1,45,sPK_EntertainArea.c_str()); }
	};
	class CMD_Delete_Bookmark : public PreformedCommand {
	public:
		CMD_Delete_Bookmark(long DeviceIDFrom, long DeviceIDTo,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,410,1,129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Delete_Bookmark_DL : public PreformedCommand {
	public:
		CMD_Delete_Bookmark_DL(long DeviceIDFrom, string DeviceIDTo,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,410,1,129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Delete_Bookmark_DT : public PreformedCommand {
	public:
		CMD_Delete_Bookmark_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,410,1,129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Delete_Bookmark_Cat : public PreformedCommand {
	public:
		CMD_Delete_Bookmark_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,410,1,129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Rename_Bookmark : public PreformedCommand {
	public:
		CMD_Rename_Bookmark(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,int iPK_Users,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,411,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Rename_Bookmark_DL : public PreformedCommand {
	public:
		CMD_Rename_Bookmark_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,int iPK_Users,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,411,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Rename_Bookmark_DT : public PreformedCommand {
	public:
		CMD_Rename_Bookmark_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,411,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Rename_Bookmark_Cat : public PreformedCommand {
	public:
		CMD_Rename_Bookmark_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users,int iEK_Bookmark) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,411,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),129,StringUtils::itos(iEK_Bookmark).c_str()); }
	};
	class CMD_Set_Media_Position : public PreformedCommand {
	public:
		CMD_Set_Media_Position(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,412,2,41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class CMD_Set_Media_Position_DL : public PreformedCommand {
	public:
		CMD_Set_Media_Position_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,412,2,41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class CMD_Set_Media_Position_DT : public PreformedCommand {
	public:
		CMD_Set_Media_Position_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,412,2,41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class CMD_Set_Media_Position_Cat : public PreformedCommand {
	public:
		CMD_Set_Media_Position_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string sMediaPosition) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,412,2,41,StringUtils::itos(iStreamID).c_str(),42,sMediaPosition.c_str()); }
	};
	class CMD_Forward_local_kb_to_OSD : public PreformedCommand {
	public:
		CMD_Forward_local_kb_to_OSD(long DeviceIDFrom, long DeviceIDTo,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,413,1,119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Forward_local_kb_to_OSD_DL : public PreformedCommand {
	public:
		CMD_Forward_local_kb_to_OSD_DL(long DeviceIDFrom, string DeviceIDTo,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,413,1,119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Forward_local_kb_to_OSD_DT : public PreformedCommand {
	public:
		CMD_Forward_local_kb_to_OSD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,413,1,119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_Forward_local_kb_to_OSD_Cat : public PreformedCommand {
	public:
		CMD_Forward_local_kb_to_OSD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,bool bTrueFalse) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,413,1,119,(bTrueFalse ? "1" : "0")); }
	};
	class CMD_PL_External_Originate : public PreformedCommand {
	public:
		CMD_PL_External_Originate(long DeviceIDFrom, long DeviceIDTo,string sPhoneNumber,string sCallerID,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,414,3,75,sPhoneNumber.c_str(),81,sCallerID.c_str(),83,sPhoneExtension.c_str()); }
	};
	class CMD_PL_External_Originate_DL : public PreformedCommand {
	public:
		CMD_PL_External_Originate_DL(long DeviceIDFrom, string DeviceIDTo,string sPhoneNumber,string sCallerID,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,414,3,75,sPhoneNumber.c_str(),81,sCallerID.c_str(),83,sPhoneExtension.c_str()); }
	};
	class CMD_PL_External_Originate_DT : public PreformedCommand {
	public:
		CMD_PL_External_Originate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sPhoneNumber,string sCallerID,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,414,3,75,sPhoneNumber.c_str(),81,sCallerID.c_str(),83,sPhoneExtension.c_str()); }
	};
	class CMD_PL_External_Originate_Cat : public PreformedCommand {
	public:
		CMD_PL_External_Originate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sPhoneNumber,string sCallerID,string sPhoneExtension) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,414,3,75,sPhoneNumber.c_str(),81,sCallerID.c_str(),83,sPhoneExtension.c_str()); }
	};
	class CMD_Set_Mouse_Position_Relative : public PreformedCommand {
	public:
		CMD_Set_Mouse_Position_Relative(long DeviceIDFrom, long DeviceIDTo,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,415,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Mouse_Position_Relative_DL : public PreformedCommand {
	public:
		CMD_Set_Mouse_Position_Relative_DL(long DeviceIDFrom, string DeviceIDTo,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,415,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Mouse_Position_Relative_DT : public PreformedCommand {
	public:
		CMD_Set_Mouse_Position_Relative_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,415,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Set_Mouse_Position_Relative_Cat : public PreformedCommand {
	public:
		CMD_Set_Mouse_Position_Relative_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iPosition_X,int iPosition_Y) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,415,2,11,StringUtils::itos(iPosition_X).c_str(),12,StringUtils::itos(iPosition_Y).c_str()); }
	};
	class CMD_Simulate_Mouse_Click_At_Present_Pos : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_At_Present_Pos(long DeviceIDFrom, long DeviceIDTo,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,416,1,14,sType.c_str()); }
	};
	class CMD_Simulate_Mouse_Click_At_Present_Pos_DL : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_At_Present_Pos_DL(long DeviceIDFrom, string DeviceIDTo,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,416,1,14,sType.c_str()); }
	};
	class CMD_Simulate_Mouse_Click_At_Present_Pos_DT : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_At_Present_Pos_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,416,1,14,sType.c_str()); }
	};
	class CMD_Simulate_Mouse_Click_At_Present_Pos_Cat : public PreformedCommand {
	public:
		CMD_Simulate_Mouse_Click_At_Present_Pos_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,416,1,14,sType.c_str()); }
	};
	class CMD_Set_Auto_Resume_Options : public PreformedCommand {
	public:
		CMD_Set_Auto_Resume_Options(long DeviceIDFrom, long DeviceIDTo,string sValue_To_Assign,int iPK_Users,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,417,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Set_Auto_Resume_Options_DL : public PreformedCommand {
	public:
		CMD_Set_Auto_Resume_Options_DL(long DeviceIDFrom, string DeviceIDTo,string sValue_To_Assign,int iPK_Users,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,417,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Set_Auto_Resume_Options_DT : public PreformedCommand {
	public:
		CMD_Set_Auto_Resume_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,417,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_Set_Auto_Resume_Options_Cat : public PreformedCommand {
	public:
		CMD_Set_Auto_Resume_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sValue_To_Assign,int iPK_Users,int iPK_MediaType) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,417,3,5,sValue_To_Assign.c_str(),17,StringUtils::itos(iPK_Users).c_str(),29,StringUtils::itos(iPK_MediaType).c_str()); }
	};
	class CMD_AC3 : public PreformedCommand {
	public:
		CMD_AC3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,418,0); }
	};
	class CMD_AC3_DL : public PreformedCommand {
	public:
		CMD_AC3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,418,0); }
	};
	class CMD_AC3_DT : public PreformedCommand {
	public:
		CMD_AC3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,418,0); }
	};
	class CMD_AC3_Cat : public PreformedCommand {
	public:
		CMD_AC3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,418,0); }
	};
	class CMD_Antenna : public PreformedCommand {
	public:
		CMD_Antenna(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,419,0); }
	};
	class CMD_Antenna_DL : public PreformedCommand {
	public:
		CMD_Antenna_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,419,0); }
	};
	class CMD_Antenna_DT : public PreformedCommand {
	public:
		CMD_Antenna_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,419,0); }
	};
	class CMD_Antenna_Cat : public PreformedCommand {
	public:
		CMD_Antenna_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,419,0); }
	};
	class CMD_CDR : public PreformedCommand {
	public:
		CMD_CDR(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,420,0); }
	};
	class CMD_CDR_DL : public PreformedCommand {
	public:
		CMD_CDR_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,420,0); }
	};
	class CMD_CDR_DT : public PreformedCommand {
	public:
		CMD_CDR_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,420,0); }
	};
	class CMD_CDR_Cat : public PreformedCommand {
	public:
		CMD_CDR_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,420,0); }
	};
	class CMD_DAT : public PreformedCommand {
	public:
		CMD_DAT(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,421,0); }
	};
	class CMD_DAT_DL : public PreformedCommand {
	public:
		CMD_DAT_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,421,0); }
	};
	class CMD_DAT_DT : public PreformedCommand {
	public:
		CMD_DAT_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,421,0); }
	};
	class CMD_DAT_Cat : public PreformedCommand {
	public:
		CMD_DAT_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,421,0); }
	};
	class CMD_Dolby_B : public PreformedCommand {
	public:
		CMD_Dolby_B(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,422,0); }
	};
	class CMD_Dolby_B_DL : public PreformedCommand {
	public:
		CMD_Dolby_B_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,422,0); }
	};
	class CMD_Dolby_B_DT : public PreformedCommand {
	public:
		CMD_Dolby_B_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,422,0); }
	};
	class CMD_Dolby_B_Cat : public PreformedCommand {
	public:
		CMD_Dolby_B_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,422,0); }
	};
	class CMD_Dolby_C : public PreformedCommand {
	public:
		CMD_Dolby_C(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,423,0); }
	};
	class CMD_Dolby_C_DL : public PreformedCommand {
	public:
		CMD_Dolby_C_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,423,0); }
	};
	class CMD_Dolby_C_DT : public PreformedCommand {
	public:
		CMD_Dolby_C_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,423,0); }
	};
	class CMD_Dolby_C_Cat : public PreformedCommand {
	public:
		CMD_Dolby_C_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,423,0); }
	};
	class CMD_External_1 : public PreformedCommand {
	public:
		CMD_External_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,424,0); }
	};
	class CMD_External_1_DL : public PreformedCommand {
	public:
		CMD_External_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,424,0); }
	};
	class CMD_External_1_DT : public PreformedCommand {
	public:
		CMD_External_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,424,0); }
	};
	class CMD_External_1_Cat : public PreformedCommand {
	public:
		CMD_External_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,424,0); }
	};
	class CMD_External_2 : public PreformedCommand {
	public:
		CMD_External_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,425,0); }
	};
	class CMD_External_2_DL : public PreformedCommand {
	public:
		CMD_External_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,425,0); }
	};
	class CMD_External_2_DT : public PreformedCommand {
	public:
		CMD_External_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,425,0); }
	};
	class CMD_External_2_Cat : public PreformedCommand {
	public:
		CMD_External_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,425,0); }
	};
	class CMD_External_3 : public PreformedCommand {
	public:
		CMD_External_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,426,0); }
	};
	class CMD_External_3_DL : public PreformedCommand {
	public:
		CMD_External_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,426,0); }
	};
	class CMD_External_3_DT : public PreformedCommand {
	public:
		CMD_External_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,426,0); }
	};
	class CMD_External_3_Cat : public PreformedCommand {
	public:
		CMD_External_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,426,0); }
	};
	class CMD_10 : public PreformedCommand {
	public:
		CMD_10(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,427,0); }
	};
	class CMD_10_DL : public PreformedCommand {
	public:
		CMD_10_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,427,0); }
	};
	class CMD_10_DT : public PreformedCommand {
	public:
		CMD_10_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,427,0); }
	};
	class CMD_10_Cat : public PreformedCommand {
	public:
		CMD_10_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,427,0); }
	};
	class CMD_116 : public PreformedCommand {
	public:
		CMD_116(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,428,0); }
	};
	class CMD_116_DL : public PreformedCommand {
	public:
		CMD_116_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,428,0); }
	};
	class CMD_116_DT : public PreformedCommand {
	public:
		CMD_116_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,428,0); }
	};
	class CMD_116_Cat : public PreformedCommand {
	public:
		CMD_116_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,428,0); }
	};
	class CMD_1732 : public PreformedCommand {
	public:
		CMD_1732(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,429,0); }
	};
	class CMD_1732_DL : public PreformedCommand {
	public:
		CMD_1732_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,429,0); }
	};
	class CMD_1732_DT : public PreformedCommand {
	public:
		CMD_1732_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,429,0); }
	};
	class CMD_1732_Cat : public PreformedCommand {
	public:
		CMD_1732_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,429,0); }
	};
	class CMD_20 : public PreformedCommand {
	public:
		CMD_20(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,430,0); }
	};
	class CMD_20_DL : public PreformedCommand {
	public:
		CMD_20_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,430,0); }
	};
	class CMD_20_DT : public PreformedCommand {
	public:
		CMD_20_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,430,0); }
	};
	class CMD_20_Cat : public PreformedCommand {
	public:
		CMD_20_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,430,0); }
	};
	class CMD_30 : public PreformedCommand {
	public:
		CMD_30(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,431,0); }
	};
	class CMD_30_DL : public PreformedCommand {
	public:
		CMD_30_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,431,0); }
	};
	class CMD_30_DT : public PreformedCommand {
	public:
		CMD_30_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,431,0); }
	};
	class CMD_30_Cat : public PreformedCommand {
	public:
		CMD_30_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,431,0); }
	};
	class CMD_3stereo : public PreformedCommand {
	public:
		CMD_3stereo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,432,0); }
	};
	class CMD_3stereo_DL : public PreformedCommand {
	public:
		CMD_3stereo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,432,0); }
	};
	class CMD_3stereo_DT : public PreformedCommand {
	public:
		CMD_3stereo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,432,0); }
	};
	class CMD_3stereo_Cat : public PreformedCommand {
	public:
		CMD_3stereo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,432,0); }
	};
	class CMD_A : public PreformedCommand {
	public:
		CMD_A(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,433,0); }
	};
	class CMD_A_DL : public PreformedCommand {
	public:
		CMD_A_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,433,0); }
	};
	class CMD_A_DT : public PreformedCommand {
	public:
		CMD_A_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,433,0); }
	};
	class CMD_A_Cat : public PreformedCommand {
	public:
		CMD_A_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,433,0); }
	};
	class CMD_AB : public PreformedCommand {
	public:
		CMD_AB(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,434,0); }
	};
	class CMD_AB_DL : public PreformedCommand {
	public:
		CMD_AB_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,434,0); }
	};
	class CMD_AB_DT : public PreformedCommand {
	public:
		CMD_AB_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,434,0); }
	};
	class CMD_AB_Cat : public PreformedCommand {
	public:
		CMD_AB_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,434,0); }
	};
	class CMD_ACH : public PreformedCommand {
	public:
		CMD_ACH(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,435,0); }
	};
	class CMD_ACH_DL : public PreformedCommand {
	public:
		CMD_ACH_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,435,0); }
	};
	class CMD_ACH_DT : public PreformedCommand {
	public:
		CMD_ACH_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,435,0); }
	};
	class CMD_ACH_Cat : public PreformedCommand {
	public:
		CMD_ACH_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,435,0); }
	};
	class CMD_AV_Status : public PreformedCommand {
	public:
		CMD_AV_Status(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,436,0); }
	};
	class CMD_AV_Status_DL : public PreformedCommand {
	public:
		CMD_AV_Status_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,436,0); }
	};
	class CMD_AV_Status_DT : public PreformedCommand {
	public:
		CMD_AV_Status_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,436,0); }
	};
	class CMD_AV_Status_Cat : public PreformedCommand {
	public:
		CMD_AV_Status_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,436,0); }
	};
	class CMD_Acoustic_Memo : public PreformedCommand {
	public:
		CMD_Acoustic_Memo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,437,0); }
	};
	class CMD_Acoustic_Memo_DL : public PreformedCommand {
	public:
		CMD_Acoustic_Memo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,437,0); }
	};
	class CMD_Acoustic_Memo_DT : public PreformedCommand {
	public:
		CMD_Acoustic_Memo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,437,0); }
	};
	class CMD_Acoustic_Memo_Cat : public PreformedCommand {
	public:
		CMD_Acoustic_Memo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,437,0); }
	};
	class CMD_Acoustic_Select : public PreformedCommand {
	public:
		CMD_Acoustic_Select(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,438,0); }
	};
	class CMD_Acoustic_Select_DL : public PreformedCommand {
	public:
		CMD_Acoustic_Select_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,438,0); }
	};
	class CMD_Acoustic_Select_DT : public PreformedCommand {
	public:
		CMD_Acoustic_Select_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,438,0); }
	};
	class CMD_Acoustic_Select_Cat : public PreformedCommand {
	public:
		CMD_Acoustic_Select_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,438,0); }
	};
	class CMD_All_Off : public PreformedCommand {
	public:
		CMD_All_Off(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,439,0); }
	};
	class CMD_All_Off_DL : public PreformedCommand {
	public:
		CMD_All_Off_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,439,0); }
	};
	class CMD_All_Off_DT : public PreformedCommand {
	public:
		CMD_All_Off_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,439,0); }
	};
	class CMD_All_Off_Cat : public PreformedCommand {
	public:
		CMD_All_Off_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,439,0); }
	};
	class CMD_All_On : public PreformedCommand {
	public:
		CMD_All_On(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,440,0); }
	};
	class CMD_All_On_DL : public PreformedCommand {
	public:
		CMD_All_On_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,440,0); }
	};
	class CMD_All_On_DT : public PreformedCommand {
	public:
		CMD_All_On_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,440,0); }
	};
	class CMD_All_On_Cat : public PreformedCommand {
	public:
		CMD_All_On_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,440,0); }
	};
	class CMD_AMS : public PreformedCommand {
	public:
		CMD_AMS(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,441,0); }
	};
	class CMD_AMS_DL : public PreformedCommand {
	public:
		CMD_AMS_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,441,0); }
	};
	class CMD_AMS_DT : public PreformedCommand {
	public:
		CMD_AMS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,441,0); }
	};
	class CMD_AMS_Cat : public PreformedCommand {
	public:
		CMD_AMS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,441,0); }
	};
	class CMD_Analogue_Down : public PreformedCommand {
	public:
		CMD_Analogue_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,442,0); }
	};
	class CMD_Analogue_Down_DL : public PreformedCommand {
	public:
		CMD_Analogue_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,442,0); }
	};
	class CMD_Analogue_Down_DT : public PreformedCommand {
	public:
		CMD_Analogue_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,442,0); }
	};
	class CMD_Analogue_Down_Cat : public PreformedCommand {
	public:
		CMD_Analogue_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,442,0); }
	};
	class CMD_Analogue_Menu : public PreformedCommand {
	public:
		CMD_Analogue_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,443,0); }
	};
	class CMD_Analogue_Menu_DL : public PreformedCommand {
	public:
		CMD_Analogue_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,443,0); }
	};
	class CMD_Analogue_Menu_DT : public PreformedCommand {
	public:
		CMD_Analogue_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,443,0); }
	};
	class CMD_Analogue_Menu_Cat : public PreformedCommand {
	public:
		CMD_Analogue_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,443,0); }
	};
	class CMD_Analogue_Up : public PreformedCommand {
	public:
		CMD_Analogue_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,444,0); }
	};
	class CMD_Analogue_Up_DL : public PreformedCommand {
	public:
		CMD_Analogue_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,444,0); }
	};
	class CMD_Analogue_Up_DT : public PreformedCommand {
	public:
		CMD_Analogue_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,444,0); }
	};
	class CMD_Analogue_Up_Cat : public PreformedCommand {
	public:
		CMD_Analogue_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,444,0); }
	};
	class CMD_Audio : public PreformedCommand {
	public:
		CMD_Audio(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,445,0); }
	};
	class CMD_Audio_DL : public PreformedCommand {
	public:
		CMD_Audio_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,445,0); }
	};
	class CMD_Audio_DT : public PreformedCommand {
	public:
		CMD_Audio_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,445,0); }
	};
	class CMD_Audio_Cat : public PreformedCommand {
	public:
		CMD_Audio_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,445,0); }
	};
	class CMD_Audio_Menu : public PreformedCommand {
	public:
		CMD_Audio_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,446,0); }
	};
	class CMD_Audio_Menu_DL : public PreformedCommand {
	public:
		CMD_Audio_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,446,0); }
	};
	class CMD_Audio_Menu_DT : public PreformedCommand {
	public:
		CMD_Audio_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,446,0); }
	};
	class CMD_Audio_Menu_Cat : public PreformedCommand {
	public:
		CMD_Audio_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,446,0); }
	};
	class CMD_Auto_Search : public PreformedCommand {
	public:
		CMD_Auto_Search(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,447,0); }
	};
	class CMD_Auto_Search_DL : public PreformedCommand {
	public:
		CMD_Auto_Search_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,447,0); }
	};
	class CMD_Auto_Search_DT : public PreformedCommand {
	public:
		CMD_Auto_Search_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,447,0); }
	};
	class CMD_Auto_Search_Cat : public PreformedCommand {
	public:
		CMD_Auto_Search_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,447,0); }
	};
	class CMD_Auto_Tracking : public PreformedCommand {
	public:
		CMD_Auto_Tracking(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,448,0); }
	};
	class CMD_Auto_Tracking_DL : public PreformedCommand {
	public:
		CMD_Auto_Tracking_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,448,0); }
	};
	class CMD_Auto_Tracking_DT : public PreformedCommand {
	public:
		CMD_Auto_Tracking_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,448,0); }
	};
	class CMD_Auto_Tracking_Cat : public PreformedCommand {
	public:
		CMD_Auto_Tracking_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,448,0); }
	};
	class CMD_AV : public PreformedCommand {
	public:
		CMD_AV(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,449,0); }
	};
	class CMD_AV_DL : public PreformedCommand {
	public:
		CMD_AV_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,449,0); }
	};
	class CMD_AV_DT : public PreformedCommand {
	public:
		CMD_AV_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,449,0); }
	};
	class CMD_AV_Cat : public PreformedCommand {
	public:
		CMD_AV_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,449,0); }
	};
	class CMD_B : public PreformedCommand {
	public:
		CMD_B(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,450,0); }
	};
	class CMD_B_DL : public PreformedCommand {
	public:
		CMD_B_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,450,0); }
	};
	class CMD_B_DT : public PreformedCommand {
	public:
		CMD_B_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,450,0); }
	};
	class CMD_B_Cat : public PreformedCommand {
	public:
		CMD_B_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,450,0); }
	};
	class CMD_Bass_Min : public PreformedCommand {
	public:
		CMD_Bass_Min(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,451,0); }
	};
	class CMD_Bass_Min_DL : public PreformedCommand {
	public:
		CMD_Bass_Min_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,451,0); }
	};
	class CMD_Bass_Min_DT : public PreformedCommand {
	public:
		CMD_Bass_Min_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,451,0); }
	};
	class CMD_Bass_Min_Cat : public PreformedCommand {
	public:
		CMD_Bass_Min_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,451,0); }
	};
	class CMD_Bass_Plus : public PreformedCommand {
	public:
		CMD_Bass_Plus(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,452,0); }
	};
	class CMD_Bass_Plus_DL : public PreformedCommand {
	public:
		CMD_Bass_Plus_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,452,0); }
	};
	class CMD_Bass_Plus_DT : public PreformedCommand {
	public:
		CMD_Bass_Plus_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,452,0); }
	};
	class CMD_Bass_Plus_Cat : public PreformedCommand {
	public:
		CMD_Bass_Plus_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,452,0); }
	};
	class CMD_Blank : public PreformedCommand {
	public:
		CMD_Blank(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,453,0); }
	};
	class CMD_Blank_DL : public PreformedCommand {
	public:
		CMD_Blank_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,453,0); }
	};
	class CMD_Blank_DT : public PreformedCommand {
	public:
		CMD_Blank_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,453,0); }
	};
	class CMD_Blank_Cat : public PreformedCommand {
	public:
		CMD_Blank_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,453,0); }
	};
	class CMD_Blank_Search : public PreformedCommand {
	public:
		CMD_Blank_Search(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,454,0); }
	};
	class CMD_Blank_Search_DL : public PreformedCommand {
	public:
		CMD_Blank_Search_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,454,0); }
	};
	class CMD_Blank_Search_DT : public PreformedCommand {
	public:
		CMD_Blank_Search_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,454,0); }
	};
	class CMD_Blank_Search_Cat : public PreformedCommand {
	public:
		CMD_Blank_Search_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,454,0); }
	};
	class CMD_Blue : public PreformedCommand {
	public:
		CMD_Blue(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,455,0); }
	};
	class CMD_Blue_DL : public PreformedCommand {
	public:
		CMD_Blue_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,455,0); }
	};
	class CMD_Blue_DT : public PreformedCommand {
	public:
		CMD_Blue_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,455,0); }
	};
	class CMD_Blue_Cat : public PreformedCommand {
	public:
		CMD_Blue_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,455,0); }
	};
	class CMD_Boost : public PreformedCommand {
	public:
		CMD_Boost(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,456,0); }
	};
	class CMD_Boost_DL : public PreformedCommand {
	public:
		CMD_Boost_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,456,0); }
	};
	class CMD_Boost_DT : public PreformedCommand {
	public:
		CMD_Boost_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,456,0); }
	};
	class CMD_Boost_Cat : public PreformedCommand {
	public:
		CMD_Boost_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,456,0); }
	};
	class CMD_Bright : public PreformedCommand {
	public:
		CMD_Bright(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,457,0); }
	};
	class CMD_Bright_DL : public PreformedCommand {
	public:
		CMD_Bright_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,457,0); }
	};
	class CMD_Bright_DT : public PreformedCommand {
	public:
		CMD_Bright_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,457,0); }
	};
	class CMD_Bright_Cat : public PreformedCommand {
	public:
		CMD_Bright_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,457,0); }
	};
	class CMD_C : public PreformedCommand {
	public:
		CMD_C(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,458,0); }
	};
	class CMD_C_DL : public PreformedCommand {
	public:
		CMD_C_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,458,0); }
	};
	class CMD_C_DT : public PreformedCommand {
	public:
		CMD_C_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,458,0); }
	};
	class CMD_C_Cat : public PreformedCommand {
	public:
		CMD_C_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,458,0); }
	};
	class CMD_CC : public PreformedCommand {
	public:
		CMD_CC(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,459,0); }
	};
	class CMD_CC_DL : public PreformedCommand {
	public:
		CMD_CC_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,459,0); }
	};
	class CMD_CC_DT : public PreformedCommand {
	public:
		CMD_CC_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,459,0); }
	};
	class CMD_CC_Cat : public PreformedCommand {
	public:
		CMD_CC_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,459,0); }
	};
	class CMD_CD_Synch : public PreformedCommand {
	public:
		CMD_CD_Synch(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,460,0); }
	};
	class CMD_CD_Synch_DL : public PreformedCommand {
	public:
		CMD_CD_Synch_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,460,0); }
	};
	class CMD_CD_Synch_DT : public PreformedCommand {
	public:
		CMD_CD_Synch_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,460,0); }
	};
	class CMD_CD_Synch_Cat : public PreformedCommand {
	public:
		CMD_CD_Synch_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,460,0); }
	};
	class CMD_CDText : public PreformedCommand {
	public:
		CMD_CDText(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,461,0); }
	};
	class CMD_CDText_DL : public PreformedCommand {
	public:
		CMD_CDText_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,461,0); }
	};
	class CMD_CDText_DT : public PreformedCommand {
	public:
		CMD_CDText_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,461,0); }
	};
	class CMD_CDText_Cat : public PreformedCommand {
	public:
		CMD_CDText_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,461,0); }
	};
	class CMD_Center_Down : public PreformedCommand {
	public:
		CMD_Center_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,462,0); }
	};
	class CMD_Center_Down_DL : public PreformedCommand {
	public:
		CMD_Center_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,462,0); }
	};
	class CMD_Center_Down_DT : public PreformedCommand {
	public:
		CMD_Center_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,462,0); }
	};
	class CMD_Center_Down_Cat : public PreformedCommand {
	public:
		CMD_Center_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,462,0); }
	};
	class CMD_Center_Up : public PreformedCommand {
	public:
		CMD_Center_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,463,0); }
	};
	class CMD_Center_Up_DL : public PreformedCommand {
	public:
		CMD_Center_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,463,0); }
	};
	class CMD_Center_Up_DT : public PreformedCommand {
	public:
		CMD_Center_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,463,0); }
	};
	class CMD_Center_Up_Cat : public PreformedCommand {
	public:
		CMD_Center_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,463,0); }
	};
	class CMD_Center_Volume_Down : public PreformedCommand {
	public:
		CMD_Center_Volume_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,464,0); }
	};
	class CMD_Center_Volume_Down_DL : public PreformedCommand {
	public:
		CMD_Center_Volume_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,464,0); }
	};
	class CMD_Center_Volume_Down_DT : public PreformedCommand {
	public:
		CMD_Center_Volume_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,464,0); }
	};
	class CMD_Center_Volume_Down_Cat : public PreformedCommand {
	public:
		CMD_Center_Volume_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,464,0); }
	};
	class CMD_Center_Volume_Up : public PreformedCommand {
	public:
		CMD_Center_Volume_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,465,0); }
	};
	class CMD_Center_Volume_Up_DL : public PreformedCommand {
	public:
		CMD_Center_Volume_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,465,0); }
	};
	class CMD_Center_Volume_Up_DT : public PreformedCommand {
	public:
		CMD_Center_Volume_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,465,0); }
	};
	class CMD_Center_Volume_Up_Cat : public PreformedCommand {
	public:
		CMD_Center_Volume_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,465,0); }
	};
	class CMD_Chapter : public PreformedCommand {
	public:
		CMD_Chapter(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,466,0); }
	};
	class CMD_Chapter_DL : public PreformedCommand {
	public:
		CMD_Chapter_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,466,0); }
	};
	class CMD_Chapter_DT : public PreformedCommand {
	public:
		CMD_Chapter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,466,0); }
	};
	class CMD_Chapter_Cat : public PreformedCommand {
	public:
		CMD_Chapter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,466,0); }
	};
	class CMD_Child_Lock : public PreformedCommand {
	public:
		CMD_Child_Lock(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,467,0); }
	};
	class CMD_Child_Lock_DL : public PreformedCommand {
	public:
		CMD_Child_Lock_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,467,0); }
	};
	class CMD_Child_Lock_DT : public PreformedCommand {
	public:
		CMD_Child_Lock_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,467,0); }
	};
	class CMD_Child_Lock_Cat : public PreformedCommand {
	public:
		CMD_Child_Lock_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,467,0); }
	};
	class CMD_Close : public PreformedCommand {
	public:
		CMD_Close(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,468,0); }
	};
	class CMD_Close_DL : public PreformedCommand {
	public:
		CMD_Close_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,468,0); }
	};
	class CMD_Close_DT : public PreformedCommand {
	public:
		CMD_Close_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,468,0); }
	};
	class CMD_Close_Cat : public PreformedCommand {
	public:
		CMD_Close_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,468,0); }
	};
	class CMD_Closed_Caption : public PreformedCommand {
	public:
		CMD_Closed_Caption(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,469,0); }
	};
	class CMD_Closed_Caption_DL : public PreformedCommand {
	public:
		CMD_Closed_Caption_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,469,0); }
	};
	class CMD_Closed_Caption_DT : public PreformedCommand {
	public:
		CMD_Closed_Caption_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,469,0); }
	};
	class CMD_Closed_Caption_Cat : public PreformedCommand {
	public:
		CMD_Closed_Caption_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,469,0); }
	};
	class CMD_Coax : public PreformedCommand {
	public:
		CMD_Coax(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,470,0); }
	};
	class CMD_Coax_DL : public PreformedCommand {
	public:
		CMD_Coax_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,470,0); }
	};
	class CMD_Coax_DT : public PreformedCommand {
	public:
		CMD_Coax_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,470,0); }
	};
	class CMD_Coax_Cat : public PreformedCommand {
	public:
		CMD_Coax_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,470,0); }
	};
	class CMD_Color_Down : public PreformedCommand {
	public:
		CMD_Color_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,471,0); }
	};
	class CMD_Color_Down_DL : public PreformedCommand {
	public:
		CMD_Color_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,471,0); }
	};
	class CMD_Color_Down_DT : public PreformedCommand {
	public:
		CMD_Color_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,471,0); }
	};
	class CMD_Color_Down_Cat : public PreformedCommand {
	public:
		CMD_Color_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,471,0); }
	};
	class CMD_Color_Up : public PreformedCommand {
	public:
		CMD_Color_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,472,0); }
	};
	class CMD_Color_Up_DL : public PreformedCommand {
	public:
		CMD_Color_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,472,0); }
	};
	class CMD_Color_Up_DT : public PreformedCommand {
	public:
		CMD_Color_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,472,0); }
	};
	class CMD_Color_Up_Cat : public PreformedCommand {
	public:
		CMD_Color_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,472,0); }
	};
	class CMD_Continue : public PreformedCommand {
	public:
		CMD_Continue(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,473,0); }
	};
	class CMD_Continue_DL : public PreformedCommand {
	public:
		CMD_Continue_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,473,0); }
	};
	class CMD_Continue_DT : public PreformedCommand {
	public:
		CMD_Continue_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,473,0); }
	};
	class CMD_Continue_Cat : public PreformedCommand {
	public:
		CMD_Continue_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,473,0); }
	};
	class CMD_Counter_Reset : public PreformedCommand {
	public:
		CMD_Counter_Reset(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,474,0); }
	};
	class CMD_Counter_Reset_DL : public PreformedCommand {
	public:
		CMD_Counter_Reset_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,474,0); }
	};
	class CMD_Counter_Reset_DT : public PreformedCommand {
	public:
		CMD_Counter_Reset_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,474,0); }
	};
	class CMD_Counter_Reset_Cat : public PreformedCommand {
	public:
		CMD_Counter_Reset_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,474,0); }
	};
	class CMD_DailyWeekly : public PreformedCommand {
	public:
		CMD_DailyWeekly(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,475,0); }
	};
	class CMD_DailyWeekly_DL : public PreformedCommand {
	public:
		CMD_DailyWeekly_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,475,0); }
	};
	class CMD_DailyWeekly_DT : public PreformedCommand {
	public:
		CMD_DailyWeekly_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,475,0); }
	};
	class CMD_DailyWeekly_Cat : public PreformedCommand {
	public:
		CMD_DailyWeekly_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,475,0); }
	};
	class CMD_Day_Down : public PreformedCommand {
	public:
		CMD_Day_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,476,0); }
	};
	class CMD_Day_Down_DL : public PreformedCommand {
	public:
		CMD_Day_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,476,0); }
	};
	class CMD_Day_Down_DT : public PreformedCommand {
	public:
		CMD_Day_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,476,0); }
	};
	class CMD_Day_Down_Cat : public PreformedCommand {
	public:
		CMD_Day_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,476,0); }
	};
	class CMD_Day_Up : public PreformedCommand {
	public:
		CMD_Day_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,477,0); }
	};
	class CMD_Day_Up_DL : public PreformedCommand {
	public:
		CMD_Day_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,477,0); }
	};
	class CMD_Day_Up_DT : public PreformedCommand {
	public:
		CMD_Day_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,477,0); }
	};
	class CMD_Day_Up_Cat : public PreformedCommand {
	public:
		CMD_Day_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,477,0); }
	};
	class CMD_Delay_Down : public PreformedCommand {
	public:
		CMD_Delay_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,478,0); }
	};
	class CMD_Delay_Down_DL : public PreformedCommand {
	public:
		CMD_Delay_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,478,0); }
	};
	class CMD_Delay_Down_DT : public PreformedCommand {
	public:
		CMD_Delay_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,478,0); }
	};
	class CMD_Delay_Down_Cat : public PreformedCommand {
	public:
		CMD_Delay_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,478,0); }
	};
	class CMD_Delay_Up : public PreformedCommand {
	public:
		CMD_Delay_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,479,0); }
	};
	class CMD_Delay_Up_DL : public PreformedCommand {
	public:
		CMD_Delay_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,479,0); }
	};
	class CMD_Delay_Up_DT : public PreformedCommand {
	public:
		CMD_Delay_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,479,0); }
	};
	class CMD_Delay_Up_Cat : public PreformedCommand {
	public:
		CMD_Delay_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,479,0); }
	};
	class CMD_Delimiter : public PreformedCommand {
	public:
		CMD_Delimiter(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,480,0); }
	};
	class CMD_Delimiter_DL : public PreformedCommand {
	public:
		CMD_Delimiter_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,480,0); }
	};
	class CMD_Delimiter_DT : public PreformedCommand {
	public:
		CMD_Delimiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,480,0); }
	};
	class CMD_Delimiter_Cat : public PreformedCommand {
	public:
		CMD_Delimiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,480,0); }
	};
	class CMD_Digit_10 : public PreformedCommand {
	public:
		CMD_Digit_10(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,481,0); }
	};
	class CMD_Digit_10_DL : public PreformedCommand {
	public:
		CMD_Digit_10_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,481,0); }
	};
	class CMD_Digit_10_DT : public PreformedCommand {
	public:
		CMD_Digit_10_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,481,0); }
	};
	class CMD_Digit_10_Cat : public PreformedCommand {
	public:
		CMD_Digit_10_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,481,0); }
	};
	class CMD_Digit_11 : public PreformedCommand {
	public:
		CMD_Digit_11(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,482,0); }
	};
	class CMD_Digit_11_DL : public PreformedCommand {
	public:
		CMD_Digit_11_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,482,0); }
	};
	class CMD_Digit_11_DT : public PreformedCommand {
	public:
		CMD_Digit_11_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,482,0); }
	};
	class CMD_Digit_11_Cat : public PreformedCommand {
	public:
		CMD_Digit_11_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,482,0); }
	};
	class CMD_Digit_12 : public PreformedCommand {
	public:
		CMD_Digit_12(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,483,0); }
	};
	class CMD_Digit_12_DL : public PreformedCommand {
	public:
		CMD_Digit_12_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,483,0); }
	};
	class CMD_Digit_12_DT : public PreformedCommand {
	public:
		CMD_Digit_12_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,483,0); }
	};
	class CMD_Digit_12_Cat : public PreformedCommand {
	public:
		CMD_Digit_12_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,483,0); }
	};
	class CMD_Digit_13 : public PreformedCommand {
	public:
		CMD_Digit_13(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,484,0); }
	};
	class CMD_Digit_13_DL : public PreformedCommand {
	public:
		CMD_Digit_13_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,484,0); }
	};
	class CMD_Digit_13_DT : public PreformedCommand {
	public:
		CMD_Digit_13_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,484,0); }
	};
	class CMD_Digit_13_Cat : public PreformedCommand {
	public:
		CMD_Digit_13_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,484,0); }
	};
	class CMD_Digit_14 : public PreformedCommand {
	public:
		CMD_Digit_14(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,485,0); }
	};
	class CMD_Digit_14_DL : public PreformedCommand {
	public:
		CMD_Digit_14_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,485,0); }
	};
	class CMD_Digit_14_DT : public PreformedCommand {
	public:
		CMD_Digit_14_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,485,0); }
	};
	class CMD_Digit_14_Cat : public PreformedCommand {
	public:
		CMD_Digit_14_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,485,0); }
	};
	class CMD_Digit_15 : public PreformedCommand {
	public:
		CMD_Digit_15(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,486,0); }
	};
	class CMD_Digit_15_DL : public PreformedCommand {
	public:
		CMD_Digit_15_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,486,0); }
	};
	class CMD_Digit_15_DT : public PreformedCommand {
	public:
		CMD_Digit_15_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,486,0); }
	};
	class CMD_Digit_15_Cat : public PreformedCommand {
	public:
		CMD_Digit_15_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,486,0); }
	};
	class CMD_Digit_16 : public PreformedCommand {
	public:
		CMD_Digit_16(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,487,0); }
	};
	class CMD_Digit_16_DL : public PreformedCommand {
	public:
		CMD_Digit_16_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,487,0); }
	};
	class CMD_Digit_16_DT : public PreformedCommand {
	public:
		CMD_Digit_16_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,487,0); }
	};
	class CMD_Digit_16_Cat : public PreformedCommand {
	public:
		CMD_Digit_16_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,487,0); }
	};
	class CMD_Dim : public PreformedCommand {
	public:
		CMD_Dim(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,488,0); }
	};
	class CMD_Dim_DL : public PreformedCommand {
	public:
		CMD_Dim_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,488,0); }
	};
	class CMD_Dim_DT : public PreformedCommand {
	public:
		CMD_Dim_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,488,0); }
	};
	class CMD_Dim_Cat : public PreformedCommand {
	public:
		CMD_Dim_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,488,0); }
	};
	class CMD_Direction : public PreformedCommand {
	public:
		CMD_Direction(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,489,0); }
	};
	class CMD_Direction_DL : public PreformedCommand {
	public:
		CMD_Direction_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,489,0); }
	};
	class CMD_Direction_DT : public PreformedCommand {
	public:
		CMD_Direction_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,489,0); }
	};
	class CMD_Direction_Cat : public PreformedCommand {
	public:
		CMD_Direction_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,489,0); }
	};
	class CMD_Disc : public PreformedCommand {
	public:
		CMD_Disc(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,490,0); }
	};
	class CMD_Disc_DL : public PreformedCommand {
	public:
		CMD_Disc_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,490,0); }
	};
	class CMD_Disc_DT : public PreformedCommand {
	public:
		CMD_Disc_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,490,0); }
	};
	class CMD_Disc_Cat : public PreformedCommand {
	public:
		CMD_Disc_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,490,0); }
	};
	class CMD_Disc_1 : public PreformedCommand {
	public:
		CMD_Disc_1(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,491,0); }
	};
	class CMD_Disc_1_DL : public PreformedCommand {
	public:
		CMD_Disc_1_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,491,0); }
	};
	class CMD_Disc_1_DT : public PreformedCommand {
	public:
		CMD_Disc_1_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,491,0); }
	};
	class CMD_Disc_1_Cat : public PreformedCommand {
	public:
		CMD_Disc_1_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,491,0); }
	};
	class CMD_Disc_2 : public PreformedCommand {
	public:
		CMD_Disc_2(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,492,0); }
	};
	class CMD_Disc_2_DL : public PreformedCommand {
	public:
		CMD_Disc_2_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,492,0); }
	};
	class CMD_Disc_2_DT : public PreformedCommand {
	public:
		CMD_Disc_2_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,492,0); }
	};
	class CMD_Disc_2_Cat : public PreformedCommand {
	public:
		CMD_Disc_2_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,492,0); }
	};
	class CMD_Disc_3 : public PreformedCommand {
	public:
		CMD_Disc_3(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,493,0); }
	};
	class CMD_Disc_3_DL : public PreformedCommand {
	public:
		CMD_Disc_3_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,493,0); }
	};
	class CMD_Disc_3_DT : public PreformedCommand {
	public:
		CMD_Disc_3_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,493,0); }
	};
	class CMD_Disc_3_Cat : public PreformedCommand {
	public:
		CMD_Disc_3_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,493,0); }
	};
	class CMD_Disc_4 : public PreformedCommand {
	public:
		CMD_Disc_4(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,494,0); }
	};
	class CMD_Disc_4_DL : public PreformedCommand {
	public:
		CMD_Disc_4_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,494,0); }
	};
	class CMD_Disc_4_DT : public PreformedCommand {
	public:
		CMD_Disc_4_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,494,0); }
	};
	class CMD_Disc_4_Cat : public PreformedCommand {
	public:
		CMD_Disc_4_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,494,0); }
	};
	class CMD_Disc_5 : public PreformedCommand {
	public:
		CMD_Disc_5(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,495,0); }
	};
	class CMD_Disc_5_DL : public PreformedCommand {
	public:
		CMD_Disc_5_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,495,0); }
	};
	class CMD_Disc_5_DT : public PreformedCommand {
	public:
		CMD_Disc_5_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,495,0); }
	};
	class CMD_Disc_5_Cat : public PreformedCommand {
	public:
		CMD_Disc_5_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,495,0); }
	};
	class CMD_Disc_6 : public PreformedCommand {
	public:
		CMD_Disc_6(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,496,0); }
	};
	class CMD_Disc_6_DL : public PreformedCommand {
	public:
		CMD_Disc_6_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,496,0); }
	};
	class CMD_Disc_6_DT : public PreformedCommand {
	public:
		CMD_Disc_6_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,496,0); }
	};
	class CMD_Disc_6_Cat : public PreformedCommand {
	public:
		CMD_Disc_6_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,496,0); }
	};
	class CMD_Disc_Down : public PreformedCommand {
	public:
		CMD_Disc_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,497,0); }
	};
	class CMD_Disc_Down_DL : public PreformedCommand {
	public:
		CMD_Disc_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,497,0); }
	};
	class CMD_Disc_Down_DT : public PreformedCommand {
	public:
		CMD_Disc_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,497,0); }
	};
	class CMD_Disc_Down_Cat : public PreformedCommand {
	public:
		CMD_Disc_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,497,0); }
	};
	class CMD_Disc_Menu : public PreformedCommand {
	public:
		CMD_Disc_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,498,0); }
	};
	class CMD_Disc_Menu_DL : public PreformedCommand {
	public:
		CMD_Disc_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,498,0); }
	};
	class CMD_Disc_Menu_DT : public PreformedCommand {
	public:
		CMD_Disc_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,498,0); }
	};
	class CMD_Disc_Menu_Cat : public PreformedCommand {
	public:
		CMD_Disc_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,498,0); }
	};
	class CMD_Disc_Up : public PreformedCommand {
	public:
		CMD_Disc_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,499,0); }
	};
	class CMD_Disc_Up_DL : public PreformedCommand {
	public:
		CMD_Disc_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,499,0); }
	};
	class CMD_Disc_Up_DT : public PreformedCommand {
	public:
		CMD_Disc_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,499,0); }
	};
	class CMD_Disc_Up_Cat : public PreformedCommand {
	public:
		CMD_Disc_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,499,0); }
	};
	class CMD_DiSeqC : public PreformedCommand {
	public:
		CMD_DiSeqC(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,500,0); }
	};
	class CMD_DiSeqC_DL : public PreformedCommand {
	public:
		CMD_DiSeqC_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,500,0); }
	};
	class CMD_DiSeqC_DT : public PreformedCommand {
	public:
		CMD_DiSeqC_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,500,0); }
	};
	class CMD_DiSeqC_Cat : public PreformedCommand {
	public:
		CMD_DiSeqC_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,500,0); }
	};
	class CMD_DNR : public PreformedCommand {
	public:
		CMD_DNR(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,501,0); }
	};
	class CMD_DNR_DL : public PreformedCommand {
	public:
		CMD_DNR_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,501,0); }
	};
	class CMD_DNR_DT : public PreformedCommand {
	public:
		CMD_DNR_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,501,0); }
	};
	class CMD_DNR_Cat : public PreformedCommand {
	public:
		CMD_DNR_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,501,0); }
	};
	class CMD_Dual : public PreformedCommand {
	public:
		CMD_Dual(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,502,0); }
	};
	class CMD_Dual_DL : public PreformedCommand {
	public:
		CMD_Dual_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,502,0); }
	};
	class CMD_Dual_DT : public PreformedCommand {
	public:
		CMD_Dual_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,502,0); }
	};
	class CMD_Dual_Cat : public PreformedCommand {
	public:
		CMD_Dual_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,502,0); }
	};
	class CMD_Dub : public PreformedCommand {
	public:
		CMD_Dub(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,503,0); }
	};
	class CMD_Dub_DL : public PreformedCommand {
	public:
		CMD_Dub_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,503,0); }
	};
	class CMD_Dub_DT : public PreformedCommand {
	public:
		CMD_Dub_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,503,0); }
	};
	class CMD_Dub_Cat : public PreformedCommand {
	public:
		CMD_Dub_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,503,0); }
	};
	class CMD_Edit : public PreformedCommand {
	public:
		CMD_Edit(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,504,0); }
	};
	class CMD_Edit_DL : public PreformedCommand {
	public:
		CMD_Edit_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,504,0); }
	};
	class CMD_Edit_DT : public PreformedCommand {
	public:
		CMD_Edit_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,504,0); }
	};
	class CMD_Edit_Cat : public PreformedCommand {
	public:
		CMD_Edit_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,504,0); }
	};
	class CMD_Expand : public PreformedCommand {
	public:
		CMD_Expand(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,505,0); }
	};
	class CMD_Expand_DL : public PreformedCommand {
	public:
		CMD_Expand_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,505,0); }
	};
	class CMD_Expand_DT : public PreformedCommand {
	public:
		CMD_Expand_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,505,0); }
	};
	class CMD_Expand_Cat : public PreformedCommand {
	public:
		CMD_Expand_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,505,0); }
	};
	class CMD_Explore : public PreformedCommand {
	public:
		CMD_Explore(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,506,0); }
	};
	class CMD_Explore_DL : public PreformedCommand {
	public:
		CMD_Explore_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,506,0); }
	};
	class CMD_Explore_DT : public PreformedCommand {
	public:
		CMD_Explore_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,506,0); }
	};
	class CMD_Explore_Cat : public PreformedCommand {
	public:
		CMD_Explore_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,506,0); }
	};
	class CMD_Fade_In : public PreformedCommand {
	public:
		CMD_Fade_In(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,507,0); }
	};
	class CMD_Fade_In_DL : public PreformedCommand {
	public:
		CMD_Fade_In_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,507,0); }
	};
	class CMD_Fade_In_DT : public PreformedCommand {
	public:
		CMD_Fade_In_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,507,0); }
	};
	class CMD_Fade_In_Cat : public PreformedCommand {
	public:
		CMD_Fade_In_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,507,0); }
	};
	class CMD_Fade_Out : public PreformedCommand {
	public:
		CMD_Fade_Out(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,508,0); }
	};
	class CMD_Fade_Out_DL : public PreformedCommand {
	public:
		CMD_Fade_Out_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,508,0); }
	};
	class CMD_Fade_Out_DT : public PreformedCommand {
	public:
		CMD_Fade_Out_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,508,0); }
	};
	class CMD_Fade_Out_Cat : public PreformedCommand {
	public:
		CMD_Fade_Out_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,508,0); }
	};
	class CMD_Favorite : public PreformedCommand {
	public:
		CMD_Favorite(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,509,0); }
	};
	class CMD_Favorite_DL : public PreformedCommand {
	public:
		CMD_Favorite_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,509,0); }
	};
	class CMD_Favorite_DT : public PreformedCommand {
	public:
		CMD_Favorite_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,509,0); }
	};
	class CMD_Favorite_Cat : public PreformedCommand {
	public:
		CMD_Favorite_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,509,0); }
	};
	class CMD_Feature : public PreformedCommand {
	public:
		CMD_Feature(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,510,0); }
	};
	class CMD_Feature_DL : public PreformedCommand {
	public:
		CMD_Feature_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,510,0); }
	};
	class CMD_Feature_DT : public PreformedCommand {
	public:
		CMD_Feature_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,510,0); }
	};
	class CMD_Feature_Cat : public PreformedCommand {
	public:
		CMD_Feature_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,510,0); }
	};
	class CMD_Finalize : public PreformedCommand {
	public:
		CMD_Finalize(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,511,0); }
	};
	class CMD_Finalize_DL : public PreformedCommand {
	public:
		CMD_Finalize_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,511,0); }
	};
	class CMD_Finalize_DT : public PreformedCommand {
	public:
		CMD_Finalize_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,511,0); }
	};
	class CMD_Finalize_Cat : public PreformedCommand {
	public:
		CMD_Finalize_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,511,0); }
	};
	class CMD_Format : public PreformedCommand {
	public:
		CMD_Format(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,512,0); }
	};
	class CMD_Format_DL : public PreformedCommand {
	public:
		CMD_Format_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,512,0); }
	};
	class CMD_Format_DT : public PreformedCommand {
	public:
		CMD_Format_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,512,0); }
	};
	class CMD_Format_Cat : public PreformedCommand {
	public:
		CMD_Format_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,512,0); }
	};
	class CMD_Front : public PreformedCommand {
	public:
		CMD_Front(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,513,0); }
	};
	class CMD_Front_DL : public PreformedCommand {
	public:
		CMD_Front_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,513,0); }
	};
	class CMD_Front_DT : public PreformedCommand {
	public:
		CMD_Front_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,513,0); }
	};
	class CMD_Front_Cat : public PreformedCommand {
	public:
		CMD_Front_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,513,0); }
	};
	class CMD_Front_Down : public PreformedCommand {
	public:
		CMD_Front_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,514,0); }
	};
	class CMD_Front_Down_DL : public PreformedCommand {
	public:
		CMD_Front_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,514,0); }
	};
	class CMD_Front_Down_DT : public PreformedCommand {
	public:
		CMD_Front_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,514,0); }
	};
	class CMD_Front_Down_Cat : public PreformedCommand {
	public:
		CMD_Front_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,514,0); }
	};
	class CMD_Front_Up : public PreformedCommand {
	public:
		CMD_Front_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,515,0); }
	};
	class CMD_Front_Up_DL : public PreformedCommand {
	public:
		CMD_Front_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,515,0); }
	};
	class CMD_Front_Up_DT : public PreformedCommand {
	public:
		CMD_Front_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,515,0); }
	};
	class CMD_Front_Up_Cat : public PreformedCommand {
	public:
		CMD_Front_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,515,0); }
	};
	class CMD_Go_To : public PreformedCommand {
	public:
		CMD_Go_To(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,516,0); }
	};
	class CMD_Go_To_DL : public PreformedCommand {
	public:
		CMD_Go_To_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,516,0); }
	};
	class CMD_Go_To_DT : public PreformedCommand {
	public:
		CMD_Go_To_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,516,0); }
	};
	class CMD_Go_To_Cat : public PreformedCommand {
	public:
		CMD_Go_To_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,516,0); }
	};
	class CMD_Goto : public PreformedCommand {
	public:
		CMD_Goto(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,517,0); }
	};
	class CMD_Goto_DL : public PreformedCommand {
	public:
		CMD_Goto_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,517,0); }
	};
	class CMD_Goto_DT : public PreformedCommand {
	public:
		CMD_Goto_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,517,0); }
	};
	class CMD_Goto_Cat : public PreformedCommand {
	public:
		CMD_Goto_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,517,0); }
	};
	class CMD_Green : public PreformedCommand {
	public:
		CMD_Green(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,518,0); }
	};
	class CMD_Green_DL : public PreformedCommand {
	public:
		CMD_Green_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,518,0); }
	};
	class CMD_Green_DT : public PreformedCommand {
	public:
		CMD_Green_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,518,0); }
	};
	class CMD_Green_Cat : public PreformedCommand {
	public:
		CMD_Green_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,518,0); }
	};
	class CMD_Group : public PreformedCommand {
	public:
		CMD_Group(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,519,0); }
	};
	class CMD_Group_DL : public PreformedCommand {
	public:
		CMD_Group_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,519,0); }
	};
	class CMD_Group_DT : public PreformedCommand {
	public:
		CMD_Group_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,519,0); }
	};
	class CMD_Group_Cat : public PreformedCommand {
	public:
		CMD_Group_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,519,0); }
	};
	class CMD_Hold : public PreformedCommand {
	public:
		CMD_Hold(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,520,0); }
	};
	class CMD_Hold_DL : public PreformedCommand {
	public:
		CMD_Hold_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,520,0); }
	};
	class CMD_Hold_DT : public PreformedCommand {
	public:
		CMD_Hold_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,520,0); }
	};
	class CMD_Hold_Cat : public PreformedCommand {
	public:
		CMD_Hold_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,520,0); }
	};
	class CMD_Home : public PreformedCommand {
	public:
		CMD_Home(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,521,0); }
	};
	class CMD_Home_DL : public PreformedCommand {
	public:
		CMD_Home_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,521,0); }
	};
	class CMD_Home_DT : public PreformedCommand {
	public:
		CMD_Home_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,521,0); }
	};
	class CMD_Home_Cat : public PreformedCommand {
	public:
		CMD_Home_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,521,0); }
	};
	class CMD_Index_Down : public PreformedCommand {
	public:
		CMD_Index_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,522,0); }
	};
	class CMD_Index_Down_DL : public PreformedCommand {
	public:
		CMD_Index_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,522,0); }
	};
	class CMD_Index_Down_DT : public PreformedCommand {
	public:
		CMD_Index_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,522,0); }
	};
	class CMD_Index_Down_Cat : public PreformedCommand {
	public:
		CMD_Index_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,522,0); }
	};
	class CMD_Index_Forward : public PreformedCommand {
	public:
		CMD_Index_Forward(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,523,0); }
	};
	class CMD_Index_Forward_DL : public PreformedCommand {
	public:
		CMD_Index_Forward_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,523,0); }
	};
	class CMD_Index_Forward_DT : public PreformedCommand {
	public:
		CMD_Index_Forward_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,523,0); }
	};
	class CMD_Index_Forward_Cat : public PreformedCommand {
	public:
		CMD_Index_Forward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,523,0); }
	};
	class CMD_Index_Mark : public PreformedCommand {
	public:
		CMD_Index_Mark(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,524,0); }
	};
	class CMD_Index_Mark_DL : public PreformedCommand {
	public:
		CMD_Index_Mark_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,524,0); }
	};
	class CMD_Index_Mark_DT : public PreformedCommand {
	public:
		CMD_Index_Mark_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,524,0); }
	};
	class CMD_Index_Mark_Cat : public PreformedCommand {
	public:
		CMD_Index_Mark_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,524,0); }
	};
	class CMD_Index_Reverse : public PreformedCommand {
	public:
		CMD_Index_Reverse(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,525,0); }
	};
	class CMD_Index_Reverse_DL : public PreformedCommand {
	public:
		CMD_Index_Reverse_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,525,0); }
	};
	class CMD_Index_Reverse_DT : public PreformedCommand {
	public:
		CMD_Index_Reverse_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,525,0); }
	};
	class CMD_Index_Reverse_Cat : public PreformedCommand {
	public:
		CMD_Index_Reverse_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,525,0); }
	};
	class CMD_Index_Scan : public PreformedCommand {
	public:
		CMD_Index_Scan(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,526,0); }
	};
	class CMD_Index_Scan_DL : public PreformedCommand {
	public:
		CMD_Index_Scan_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,526,0); }
	};
	class CMD_Index_Scan_DT : public PreformedCommand {
	public:
		CMD_Index_Scan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,526,0); }
	};
	class CMD_Index_Scan_Cat : public PreformedCommand {
	public:
		CMD_Index_Scan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,526,0); }
	};
	class CMD_Index_Up : public PreformedCommand {
	public:
		CMD_Index_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,527,0); }
	};
	class CMD_Index_Up_DL : public PreformedCommand {
	public:
		CMD_Index_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,527,0); }
	};
	class CMD_Index_Up_DT : public PreformedCommand {
	public:
		CMD_Index_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,527,0); }
	};
	class CMD_Index_Up_Cat : public PreformedCommand {
	public:
		CMD_Index_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,527,0); }
	};
	class CMD_Instant_Replay : public PreformedCommand {
	public:
		CMD_Instant_Replay(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,528,0); }
	};
	class CMD_Instant_Replay_DL : public PreformedCommand {
	public:
		CMD_Instant_Replay_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,528,0); }
	};
	class CMD_Instant_Replay_DT : public PreformedCommand {
	public:
		CMD_Instant_Replay_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,528,0); }
	};
	class CMD_Instant_Replay_Cat : public PreformedCommand {
	public:
		CMD_Instant_Replay_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,528,0); }
	};
	class CMD_Karaoke_Melody : public PreformedCommand {
	public:
		CMD_Karaoke_Melody(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,529,0); }
	};
	class CMD_Karaoke_Melody_DL : public PreformedCommand {
	public:
		CMD_Karaoke_Melody_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,529,0); }
	};
	class CMD_Karaoke_Melody_DT : public PreformedCommand {
	public:
		CMD_Karaoke_Melody_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,529,0); }
	};
	class CMD_Karaoke_Melody_Cat : public PreformedCommand {
	public:
		CMD_Karaoke_Melody_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,529,0); }
	};
	class CMD_Karaoke_Mode : public PreformedCommand {
	public:
		CMD_Karaoke_Mode(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,530,0); }
	};
	class CMD_Karaoke_Mode_DL : public PreformedCommand {
	public:
		CMD_Karaoke_Mode_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,530,0); }
	};
	class CMD_Karaoke_Mode_DT : public PreformedCommand {
	public:
		CMD_Karaoke_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,530,0); }
	};
	class CMD_Karaoke_Mode_Cat : public PreformedCommand {
	public:
		CMD_Karaoke_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,530,0); }
	};
	class CMD_Karaoke_OnOff : public PreformedCommand {
	public:
		CMD_Karaoke_OnOff(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,531,0); }
	};
	class CMD_Karaoke_OnOff_DL : public PreformedCommand {
	public:
		CMD_Karaoke_OnOff_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,531,0); }
	};
	class CMD_Karaoke_OnOff_DT : public PreformedCommand {
	public:
		CMD_Karaoke_OnOff_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,531,0); }
	};
	class CMD_Karaoke_OnOff_Cat : public PreformedCommand {
	public:
		CMD_Karaoke_OnOff_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,531,0); }
	};
	class CMD_Karaoke_Once_More : public PreformedCommand {
	public:
		CMD_Karaoke_Once_More(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,532,0); }
	};
	class CMD_Karaoke_Once_More_DL : public PreformedCommand {
	public:
		CMD_Karaoke_Once_More_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,532,0); }
	};
	class CMD_Karaoke_Once_More_DT : public PreformedCommand {
	public:
		CMD_Karaoke_Once_More_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,532,0); }
	};
	class CMD_Karaoke_Once_More_Cat : public PreformedCommand {
	public:
		CMD_Karaoke_Once_More_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,532,0); }
	};
	class CMD_Karaoke_Vocal : public PreformedCommand {
	public:
		CMD_Karaoke_Vocal(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,533,0); }
	};
	class CMD_Karaoke_Vocal_DL : public PreformedCommand {
	public:
		CMD_Karaoke_Vocal_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,533,0); }
	};
	class CMD_Karaoke_Vocal_DT : public PreformedCommand {
	public:
		CMD_Karaoke_Vocal_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,533,0); }
	};
	class CMD_Karaoke_Vocal_Cat : public PreformedCommand {
	public:
		CMD_Karaoke_Vocal_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,533,0); }
	};
	class CMD_Language : public PreformedCommand {
	public:
		CMD_Language(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,534,0); }
	};
	class CMD_Language_DL : public PreformedCommand {
	public:
		CMD_Language_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,534,0); }
	};
	class CMD_Language_DT : public PreformedCommand {
	public:
		CMD_Language_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,534,0); }
	};
	class CMD_Language_Cat : public PreformedCommand {
	public:
		CMD_Language_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,534,0); }
	};
	class CMD_Last_Play : public PreformedCommand {
	public:
		CMD_Last_Play(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,535,0); }
	};
	class CMD_Last_Play_DL : public PreformedCommand {
	public:
		CMD_Last_Play_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,535,0); }
	};
	class CMD_Last_Play_DT : public PreformedCommand {
	public:
		CMD_Last_Play_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,535,0); }
	};
	class CMD_Last_Play_Cat : public PreformedCommand {
	public:
		CMD_Last_Play_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,535,0); }
	};
	class CMD_Last_Record_Cancel : public PreformedCommand {
	public:
		CMD_Last_Record_Cancel(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,536,0); }
	};
	class CMD_Last_Record_Cancel_DL : public PreformedCommand {
	public:
		CMD_Last_Record_Cancel_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,536,0); }
	};
	class CMD_Last_Record_Cancel_DT : public PreformedCommand {
	public:
		CMD_Last_Record_Cancel_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,536,0); }
	};
	class CMD_Last_Record_Cancel_Cat : public PreformedCommand {
	public:
		CMD_Last_Record_Cancel_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,536,0); }
	};
	class CMD_Level_Down : public PreformedCommand {
	public:
		CMD_Level_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,537,0); }
	};
	class CMD_Level_Down_DL : public PreformedCommand {
	public:
		CMD_Level_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,537,0); }
	};
	class CMD_Level_Down_DT : public PreformedCommand {
	public:
		CMD_Level_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,537,0); }
	};
	class CMD_Level_Down_Cat : public PreformedCommand {
	public:
		CMD_Level_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,537,0); }
	};
	class CMD_Level_Up : public PreformedCommand {
	public:
		CMD_Level_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,538,0); }
	};
	class CMD_Level_Up_DL : public PreformedCommand {
	public:
		CMD_Level_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,538,0); }
	};
	class CMD_Level_Up_DT : public PreformedCommand {
	public:
		CMD_Level_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,538,0); }
	};
	class CMD_Level_Up_Cat : public PreformedCommand {
	public:
		CMD_Level_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,538,0); }
	};
	class CMD_Light : public PreformedCommand {
	public:
		CMD_Light(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,539,0); }
	};
	class CMD_Light_DL : public PreformedCommand {
	public:
		CMD_Light_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,539,0); }
	};
	class CMD_Light_DT : public PreformedCommand {
	public:
		CMD_Light_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,539,0); }
	};
	class CMD_Light_Cat : public PreformedCommand {
	public:
		CMD_Light_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,539,0); }
	};
	class CMD_LNB : public PreformedCommand {
	public:
		CMD_LNB(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,540,0); }
	};
	class CMD_LNB_DL : public PreformedCommand {
	public:
		CMD_LNB_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,540,0); }
	};
	class CMD_LNB_DT : public PreformedCommand {
	public:
		CMD_LNB_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,540,0); }
	};
	class CMD_LNB_Cat : public PreformedCommand {
	public:
		CMD_LNB_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,540,0); }
	};
	class CMD_LocalDistant : public PreformedCommand {
	public:
		CMD_LocalDistant(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,541,0); }
	};
	class CMD_LocalDistant_DL : public PreformedCommand {
	public:
		CMD_LocalDistant_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,541,0); }
	};
	class CMD_LocalDistant_DT : public PreformedCommand {
	public:
		CMD_LocalDistant_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,541,0); }
	};
	class CMD_LocalDistant_Cat : public PreformedCommand {
	public:
		CMD_LocalDistant_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,541,0); }
	};
	class CMD_Mail : public PreformedCommand {
	public:
		CMD_Mail(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,542,0); }
	};
	class CMD_Mail_DL : public PreformedCommand {
	public:
		CMD_Mail_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,542,0); }
	};
	class CMD_Mail_DT : public PreformedCommand {
	public:
		CMD_Mail_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,542,0); }
	};
	class CMD_Mail_Cat : public PreformedCommand {
	public:
		CMD_Mail_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,542,0); }
	};
	class CMD_Main_Freeze : public PreformedCommand {
	public:
		CMD_Main_Freeze(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,543,0); }
	};
	class CMD_Main_Freeze_DL : public PreformedCommand {
	public:
		CMD_Main_Freeze_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,543,0); }
	};
	class CMD_Main_Freeze_DT : public PreformedCommand {
	public:
		CMD_Main_Freeze_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,543,0); }
	};
	class CMD_Main_Freeze_Cat : public PreformedCommand {
	public:
		CMD_Main_Freeze_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,543,0); }
	};
	class CMD_Main_Stored : public PreformedCommand {
	public:
		CMD_Main_Stored(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,544,0); }
	};
	class CMD_Main_Stored_DL : public PreformedCommand {
	public:
		CMD_Main_Stored_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,544,0); }
	};
	class CMD_Main_Stored_DT : public PreformedCommand {
	public:
		CMD_Main_Stored_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,544,0); }
	};
	class CMD_Main_Stored_Cat : public PreformedCommand {
	public:
		CMD_Main_Stored_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,544,0); }
	};
	class CMD_Memclr : public PreformedCommand {
	public:
		CMD_Memclr(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,545,0); }
	};
	class CMD_Memclr_DL : public PreformedCommand {
	public:
		CMD_Memclr_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,545,0); }
	};
	class CMD_Memclr_DT : public PreformedCommand {
	public:
		CMD_Memclr_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,545,0); }
	};
	class CMD_Memclr_Cat : public PreformedCommand {
	public:
		CMD_Memclr_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,545,0); }
	};
	class CMD_Memo_Clear : public PreformedCommand {
	public:
		CMD_Memo_Clear(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,546,0); }
	};
	class CMD_Memo_Clear_DL : public PreformedCommand {
	public:
		CMD_Memo_Clear_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,546,0); }
	};
	class CMD_Memo_Clear_DT : public PreformedCommand {
	public:
		CMD_Memo_Clear_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,546,0); }
	};
	class CMD_Memo_Clear_Cat : public PreformedCommand {
	public:
		CMD_Memo_Clear_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,546,0); }
	};
	class CMD_Memory : public PreformedCommand {
	public:
		CMD_Memory(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,547,0); }
	};
	class CMD_Memory_DL : public PreformedCommand {
	public:
		CMD_Memory_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,547,0); }
	};
	class CMD_Memory_DT : public PreformedCommand {
	public:
		CMD_Memory_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,547,0); }
	};
	class CMD_Memory_Cat : public PreformedCommand {
	public:
		CMD_Memory_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,547,0); }
	};
	class CMD_Menu : public PreformedCommand {
	public:
		CMD_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,548,0); }
	};
	class CMD_Menu_DL : public PreformedCommand {
	public:
		CMD_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,548,0); }
	};
	class CMD_Menu_DT : public PreformedCommand {
	public:
		CMD_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,548,0); }
	};
	class CMD_Menu_Cat : public PreformedCommand {
	public:
		CMD_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,548,0); }
	};
	class CMD_Mix : public PreformedCommand {
	public:
		CMD_Mix(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,549,0); }
	};
	class CMD_Mix_DL : public PreformedCommand {
	public:
		CMD_Mix_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,549,0); }
	};
	class CMD_Mix_DT : public PreformedCommand {
	public:
		CMD_Mix_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,549,0); }
	};
	class CMD_Mix_Cat : public PreformedCommand {
	public:
		CMD_Mix_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,549,0); }
	};
	class CMD_Mode : public PreformedCommand {
	public:
		CMD_Mode(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,550,0); }
	};
	class CMD_Mode_DL : public PreformedCommand {
	public:
		CMD_Mode_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,550,0); }
	};
	class CMD_Mode_DT : public PreformedCommand {
	public:
		CMD_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,550,0); }
	};
	class CMD_Mode_Cat : public PreformedCommand {
	public:
		CMD_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,550,0); }
	};
	class CMD_Monitor : public PreformedCommand {
	public:
		CMD_Monitor(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,551,0); }
	};
	class CMD_Monitor_DL : public PreformedCommand {
	public:
		CMD_Monitor_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,551,0); }
	};
	class CMD_Monitor_DT : public PreformedCommand {
	public:
		CMD_Monitor_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,551,0); }
	};
	class CMD_Monitor_Cat : public PreformedCommand {
	public:
		CMD_Monitor_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,551,0); }
	};
	class CMD_Mosaic : public PreformedCommand {
	public:
		CMD_Mosaic(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,552,0); }
	};
	class CMD_Mosaic_DL : public PreformedCommand {
	public:
		CMD_Mosaic_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,552,0); }
	};
	class CMD_Mosaic_DT : public PreformedCommand {
	public:
		CMD_Mosaic_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,552,0); }
	};
	class CMD_Mosaic_Cat : public PreformedCommand {
	public:
		CMD_Mosaic_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,552,0); }
	};
	class CMD_Movie : public PreformedCommand {
	public:
		CMD_Movie(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,553,0); }
	};
	class CMD_Movie_DL : public PreformedCommand {
	public:
		CMD_Movie_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,553,0); }
	};
	class CMD_Movie_DT : public PreformedCommand {
	public:
		CMD_Movie_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,553,0); }
	};
	class CMD_Movie_Cat : public PreformedCommand {
	public:
		CMD_Movie_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,553,0); }
	};
	class CMD_Multi_Screen : public PreformedCommand {
	public:
		CMD_Multi_Screen(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,554,0); }
	};
	class CMD_Multi_Screen_DL : public PreformedCommand {
	public:
		CMD_Multi_Screen_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,554,0); }
	};
	class CMD_Multi_Screen_DT : public PreformedCommand {
	public:
		CMD_Multi_Screen_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,554,0); }
	};
	class CMD_Multi_Screen_Cat : public PreformedCommand {
	public:
		CMD_Multi_Screen_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,554,0); }
	};
	class CMD_Multiroom : public PreformedCommand {
	public:
		CMD_Multiroom(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,555,0); }
	};
	class CMD_Multiroom_DL : public PreformedCommand {
	public:
		CMD_Multiroom_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,555,0); }
	};
	class CMD_Multiroom_DT : public PreformedCommand {
	public:
		CMD_Multiroom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,555,0); }
	};
	class CMD_Multiroom_Cat : public PreformedCommand {
	public:
		CMD_Multiroom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,555,0); }
	};
	class CMD_Natural_Colour : public PreformedCommand {
	public:
		CMD_Natural_Colour(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,556,0); }
	};
	class CMD_Natural_Colour_DL : public PreformedCommand {
	public:
		CMD_Natural_Colour_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,556,0); }
	};
	class CMD_Natural_Colour_DT : public PreformedCommand {
	public:
		CMD_Natural_Colour_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,556,0); }
	};
	class CMD_Natural_Colour_Cat : public PreformedCommand {
	public:
		CMD_Natural_Colour_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,556,0); }
	};
	class CMD_New_Track : public PreformedCommand {
	public:
		CMD_New_Track(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,557,0); }
	};
	class CMD_New_Track_DL : public PreformedCommand {
	public:
		CMD_New_Track_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,557,0); }
	};
	class CMD_New_Track_DT : public PreformedCommand {
	public:
		CMD_New_Track_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,557,0); }
	};
	class CMD_New_Track_Cat : public PreformedCommand {
	public:
		CMD_New_Track_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,557,0); }
	};
	class CMD_NewsTA : public PreformedCommand {
	public:
		CMD_NewsTA(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,558,0); }
	};
	class CMD_NewsTA_DL : public PreformedCommand {
	public:
		CMD_NewsTA_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,558,0); }
	};
	class CMD_NewsTA_DT : public PreformedCommand {
	public:
		CMD_NewsTA_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,558,0); }
	};
	class CMD_NewsTA_Cat : public PreformedCommand {
	public:
		CMD_NewsTA_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,558,0); }
	};
	class CMD_Next : public PreformedCommand {
	public:
		CMD_Next(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,559,0); }
	};
	class CMD_Next_DL : public PreformedCommand {
	public:
		CMD_Next_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,559,0); }
	};
	class CMD_Next_DT : public PreformedCommand {
	public:
		CMD_Next_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,559,0); }
	};
	class CMD_Next_Cat : public PreformedCommand {
	public:
		CMD_Next_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,559,0); }
	};
	class CMD_Next_Speaker : public PreformedCommand {
	public:
		CMD_Next_Speaker(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,560,0); }
	};
	class CMD_Next_Speaker_DL : public PreformedCommand {
	public:
		CMD_Next_Speaker_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,560,0); }
	};
	class CMD_Next_Speaker_DT : public PreformedCommand {
	public:
		CMD_Next_Speaker_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,560,0); }
	};
	class CMD_Next_Speaker_Cat : public PreformedCommand {
	public:
		CMD_Next_Speaker_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,560,0); }
	};
	class CMD_Night : public PreformedCommand {
	public:
		CMD_Night(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,561,0); }
	};
	class CMD_Night_DL : public PreformedCommand {
	public:
		CMD_Night_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,561,0); }
	};
	class CMD_Night_DT : public PreformedCommand {
	public:
		CMD_Night_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,561,0); }
	};
	class CMD_Night_Cat : public PreformedCommand {
	public:
		CMD_Night_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,561,0); }
	};
	class CMD_Normal_Backward : public PreformedCommand {
	public:
		CMD_Normal_Backward(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,562,0); }
	};
	class CMD_Normal_Backward_DL : public PreformedCommand {
	public:
		CMD_Normal_Backward_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,562,0); }
	};
	class CMD_Normal_Backward_DT : public PreformedCommand {
	public:
		CMD_Normal_Backward_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,562,0); }
	};
	class CMD_Normal_Backward_Cat : public PreformedCommand {
	public:
		CMD_Normal_Backward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,562,0); }
	};
	class CMD_Normal_Forward : public PreformedCommand {
	public:
		CMD_Normal_Forward(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,563,0); }
	};
	class CMD_Normal_Forward_DL : public PreformedCommand {
	public:
		CMD_Normal_Forward_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,563,0); }
	};
	class CMD_Normal_Forward_DT : public PreformedCommand {
	public:
		CMD_Normal_Forward_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,563,0); }
	};
	class CMD_Normal_Forward_Cat : public PreformedCommand {
	public:
		CMD_Normal_Forward_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,563,0); }
	};
	class CMD_Normal_Picture : public PreformedCommand {
	public:
		CMD_Normal_Picture(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,564,0); }
	};
	class CMD_Normal_Picture_DL : public PreformedCommand {
	public:
		CMD_Normal_Picture_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,564,0); }
	};
	class CMD_Normal_Picture_DT : public PreformedCommand {
	public:
		CMD_Normal_Picture_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,564,0); }
	};
	class CMD_Normal_Picture_Cat : public PreformedCommand {
	public:
		CMD_Normal_Picture_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,564,0); }
	};
	class CMD_Normal_Rewind : public PreformedCommand {
	public:
		CMD_Normal_Rewind(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,565,0); }
	};
	class CMD_Normal_Rewind_DL : public PreformedCommand {
	public:
		CMD_Normal_Rewind_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,565,0); }
	};
	class CMD_Normal_Rewind_DT : public PreformedCommand {
	public:
		CMD_Normal_Rewind_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,565,0); }
	};
	class CMD_Normal_Rewind_Cat : public PreformedCommand {
	public:
		CMD_Normal_Rewind_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,565,0); }
	};
	class CMD_Open : public PreformedCommand {
	public:
		CMD_Open(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,566,0); }
	};
	class CMD_Open_DL : public PreformedCommand {
	public:
		CMD_Open_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,566,0); }
	};
	class CMD_Open_DT : public PreformedCommand {
	public:
		CMD_Open_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,566,0); }
	};
	class CMD_Open_Cat : public PreformedCommand {
	public:
		CMD_Open_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,566,0); }
	};
	class CMD_Opt : public PreformedCommand {
	public:
		CMD_Opt(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,567,0); }
	};
	class CMD_Opt_DL : public PreformedCommand {
	public:
		CMD_Opt_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,567,0); }
	};
	class CMD_Opt_DT : public PreformedCommand {
	public:
		CMD_Opt_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,567,0); }
	};
	class CMD_Opt_Cat : public PreformedCommand {
	public:
		CMD_Opt_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,567,0); }
	};
	class CMD_Options : public PreformedCommand {
	public:
		CMD_Options(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,568,0); }
	};
	class CMD_Options_DL : public PreformedCommand {
	public:
		CMD_Options_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,568,0); }
	};
	class CMD_Options_DT : public PreformedCommand {
	public:
		CMD_Options_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,568,0); }
	};
	class CMD_Options_Cat : public PreformedCommand {
	public:
		CMD_Options_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,568,0); }
	};
	class CMD_Oscillate : public PreformedCommand {
	public:
		CMD_Oscillate(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,569,0); }
	};
	class CMD_Oscillate_DL : public PreformedCommand {
	public:
		CMD_Oscillate_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,569,0); }
	};
	class CMD_Oscillate_DT : public PreformedCommand {
	public:
		CMD_Oscillate_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,569,0); }
	};
	class CMD_Oscillate_Cat : public PreformedCommand {
	public:
		CMD_Oscillate_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,569,0); }
	};
	class CMD_OTR : public PreformedCommand {
	public:
		CMD_OTR(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,570,0); }
	};
	class CMD_OTR_DL : public PreformedCommand {
	public:
		CMD_OTR_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,570,0); }
	};
	class CMD_OTR_DT : public PreformedCommand {
	public:
		CMD_OTR_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,570,0); }
	};
	class CMD_OTR_Cat : public PreformedCommand {
	public:
		CMD_OTR_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,570,0); }
	};
	class CMD_Page_Down : public PreformedCommand {
	public:
		CMD_Page_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,571,0); }
	};
	class CMD_Page_Down_DL : public PreformedCommand {
	public:
		CMD_Page_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,571,0); }
	};
	class CMD_Page_Down_DT : public PreformedCommand {
	public:
		CMD_Page_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,571,0); }
	};
	class CMD_Page_Down_Cat : public PreformedCommand {
	public:
		CMD_Page_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,571,0); }
	};
	class CMD_Page_Up : public PreformedCommand {
	public:
		CMD_Page_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,572,0); }
	};
	class CMD_Page_Up_DL : public PreformedCommand {
	public:
		CMD_Page_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,572,0); }
	};
	class CMD_Page_Up_DT : public PreformedCommand {
	public:
		CMD_Page_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,572,0); }
	};
	class CMD_Page_Up_Cat : public PreformedCommand {
	public:
		CMD_Page_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,572,0); }
	};
	class CMD_PAP : public PreformedCommand {
	public:
		CMD_PAP(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,573,0); }
	};
	class CMD_PAP_DL : public PreformedCommand {
	public:
		CMD_PAP_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,573,0); }
	};
	class CMD_PAP_DT : public PreformedCommand {
	public:
		CMD_PAP_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,573,0); }
	};
	class CMD_PAP_Cat : public PreformedCommand {
	public:
		CMD_PAP_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,573,0); }
	};
	class CMD_Parental_Lock : public PreformedCommand {
	public:
		CMD_Parental_Lock(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,574,0); }
	};
	class CMD_Parental_Lock_DL : public PreformedCommand {
	public:
		CMD_Parental_Lock_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,574,0); }
	};
	class CMD_Parental_Lock_DT : public PreformedCommand {
	public:
		CMD_Parental_Lock_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,574,0); }
	};
	class CMD_Parental_Lock_Cat : public PreformedCommand {
	public:
		CMD_Parental_Lock_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,574,0); }
	};
	class CMD_Pay_Per_View : public PreformedCommand {
	public:
		CMD_Pay_Per_View(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,575,0); }
	};
	class CMD_Pay_Per_View_DL : public PreformedCommand {
	public:
		CMD_Pay_Per_View_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,575,0); }
	};
	class CMD_Pay_Per_View_DT : public PreformedCommand {
	public:
		CMD_Pay_Per_View_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,575,0); }
	};
	class CMD_Pay_Per_View_Cat : public PreformedCommand {
	public:
		CMD_Pay_Per_View_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,575,0); }
	};
	class CMD_PBC : public PreformedCommand {
	public:
		CMD_PBC(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,576,0); }
	};
	class CMD_PBC_DL : public PreformedCommand {
	public:
		CMD_PBC_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,576,0); }
	};
	class CMD_PBC_DT : public PreformedCommand {
	public:
		CMD_PBC_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,576,0); }
	};
	class CMD_PBC_Cat : public PreformedCommand {
	public:
		CMD_PBC_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,576,0); }
	};
	class CMD_Peak_Hold : public PreformedCommand {
	public:
		CMD_Peak_Hold(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,577,0); }
	};
	class CMD_Peak_Hold_DL : public PreformedCommand {
	public:
		CMD_Peak_Hold_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,577,0); }
	};
	class CMD_Peak_Hold_DT : public PreformedCommand {
	public:
		CMD_Peak_Hold_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,577,0); }
	};
	class CMD_Peak_Hold_Cat : public PreformedCommand {
	public:
		CMD_Peak_Hold_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,577,0); }
	};
	class CMD_Picture_Menu : public PreformedCommand {
	public:
		CMD_Picture_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,578,0); }
	};
	class CMD_Picture_Menu_DL : public PreformedCommand {
	public:
		CMD_Picture_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,578,0); }
	};
	class CMD_Picture_Menu_DT : public PreformedCommand {
	public:
		CMD_Picture_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,578,0); }
	};
	class CMD_Picture_Menu_Cat : public PreformedCommand {
	public:
		CMD_Picture_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,578,0); }
	};
	class CMD_PIP_Freeze : public PreformedCommand {
	public:
		CMD_PIP_Freeze(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,579,0); }
	};
	class CMD_PIP_Freeze_DL : public PreformedCommand {
	public:
		CMD_PIP_Freeze_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,579,0); }
	};
	class CMD_PIP_Freeze_DT : public PreformedCommand {
	public:
		CMD_PIP_Freeze_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,579,0); }
	};
	class CMD_PIP_Freeze_Cat : public PreformedCommand {
	public:
		CMD_PIP_Freeze_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,579,0); }
	};
	class CMD_PIP_Main_Swap : public PreformedCommand {
	public:
		CMD_PIP_Main_Swap(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,580,0); }
	};
	class CMD_PIP_Main_Swap_DL : public PreformedCommand {
	public:
		CMD_PIP_Main_Swap_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,580,0); }
	};
	class CMD_PIP_Main_Swap_DT : public PreformedCommand {
	public:
		CMD_PIP_Main_Swap_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,580,0); }
	};
	class CMD_PIP_Main_Swap_Cat : public PreformedCommand {
	public:
		CMD_PIP_Main_Swap_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,580,0); }
	};
	class CMD_PIP_Select : public PreformedCommand {
	public:
		CMD_PIP_Select(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,581,0); }
	};
	class CMD_PIP_Select_DL : public PreformedCommand {
	public:
		CMD_PIP_Select_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,581,0); }
	};
	class CMD_PIP_Select_DT : public PreformedCommand {
	public:
		CMD_PIP_Select_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,581,0); }
	};
	class CMD_PIP_Select_Cat : public PreformedCommand {
	public:
		CMD_PIP_Select_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,581,0); }
	};
	class CMD_PIP_Shift : public PreformedCommand {
	public:
		CMD_PIP_Shift(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,582,0); }
	};
	class CMD_PIP_Shift_DL : public PreformedCommand {
	public:
		CMD_PIP_Shift_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,582,0); }
	};
	class CMD_PIP_Shift_DT : public PreformedCommand {
	public:
		CMD_PIP_Shift_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,582,0); }
	};
	class CMD_PIP_Shift_Cat : public PreformedCommand {
	public:
		CMD_PIP_Shift_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,582,0); }
	};
	class CMD_PIP_Strobe : public PreformedCommand {
	public:
		CMD_PIP_Strobe(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,583,0); }
	};
	class CMD_PIP_Strobe_DL : public PreformedCommand {
	public:
		CMD_PIP_Strobe_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,583,0); }
	};
	class CMD_PIP_Strobe_DT : public PreformedCommand {
	public:
		CMD_PIP_Strobe_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,583,0); }
	};
	class CMD_PIP_Strobe_Cat : public PreformedCommand {
	public:
		CMD_PIP_Strobe_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,583,0); }
	};
	class CMD_Prefs : public PreformedCommand {
	public:
		CMD_Prefs(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,584,0); }
	};
	class CMD_Prefs_DL : public PreformedCommand {
	public:
		CMD_Prefs_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,584,0); }
	};
	class CMD_Prefs_DT : public PreformedCommand {
	public:
		CMD_Prefs_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,584,0); }
	};
	class CMD_Prefs_Cat : public PreformedCommand {
	public:
		CMD_Prefs_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,584,0); }
	};
	class CMD_Preset_Down : public PreformedCommand {
	public:
		CMD_Preset_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,585,0); }
	};
	class CMD_Preset_Down_DL : public PreformedCommand {
	public:
		CMD_Preset_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,585,0); }
	};
	class CMD_Preset_Down_DT : public PreformedCommand {
	public:
		CMD_Preset_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,585,0); }
	};
	class CMD_Preset_Down_Cat : public PreformedCommand {
	public:
		CMD_Preset_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,585,0); }
	};
	class CMD_Preset_Up : public PreformedCommand {
	public:
		CMD_Preset_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,586,0); }
	};
	class CMD_Preset_Up_DL : public PreformedCommand {
	public:
		CMD_Preset_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,586,0); }
	};
	class CMD_Preset_Up_DT : public PreformedCommand {
	public:
		CMD_Preset_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,586,0); }
	};
	class CMD_Preset_Up_Cat : public PreformedCommand {
	public:
		CMD_Preset_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,586,0); }
	};
	class CMD_PresetProgFreq : public PreformedCommand {
	public:
		CMD_PresetProgFreq(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,587,0); }
	};
	class CMD_PresetProgFreq_DL : public PreformedCommand {
	public:
		CMD_PresetProgFreq_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,587,0); }
	};
	class CMD_PresetProgFreq_DT : public PreformedCommand {
	public:
		CMD_PresetProgFreq_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,587,0); }
	};
	class CMD_PresetProgFreq_Cat : public PreformedCommand {
	public:
		CMD_PresetProgFreq_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,587,0); }
	};
	class CMD_Previous : public PreformedCommand {
	public:
		CMD_Previous(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,588,0); }
	};
	class CMD_Previous_DL : public PreformedCommand {
	public:
		CMD_Previous_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,588,0); }
	};
	class CMD_Previous_DT : public PreformedCommand {
	public:
		CMD_Previous_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,588,0); }
	};
	class CMD_Previous_Cat : public PreformedCommand {
	public:
		CMD_Previous_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,588,0); }
	};
	class CMD_Previous_Program : public PreformedCommand {
	public:
		CMD_Previous_Program(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,589,0); }
	};
	class CMD_Previous_Program_DL : public PreformedCommand {
	public:
		CMD_Previous_Program_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,589,0); }
	};
	class CMD_Previous_Program_DT : public PreformedCommand {
	public:
		CMD_Previous_Program_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,589,0); }
	};
	class CMD_Previous_Program_Cat : public PreformedCommand {
	public:
		CMD_Previous_Program_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,589,0); }
	};
	class CMD_Program : public PreformedCommand {
	public:
		CMD_Program(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,590,0); }
	};
	class CMD_Program_DL : public PreformedCommand {
	public:
		CMD_Program_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,590,0); }
	};
	class CMD_Program_DT : public PreformedCommand {
	public:
		CMD_Program_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,590,0); }
	};
	class CMD_Program_Cat : public PreformedCommand {
	public:
		CMD_Program_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,590,0); }
	};
	class CMD_PTY : public PreformedCommand {
	public:
		CMD_PTY(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,591,0); }
	};
	class CMD_PTY_DL : public PreformedCommand {
	public:
		CMD_PTY_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,591,0); }
	};
	class CMD_PTY_DT : public PreformedCommand {
	public:
		CMD_PTY_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,591,0); }
	};
	class CMD_PTY_Cat : public PreformedCommand {
	public:
		CMD_PTY_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,591,0); }
	};
	class CMD_PVR_Menu : public PreformedCommand {
	public:
		CMD_PVR_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,592,0); }
	};
	class CMD_PVR_Menu_DL : public PreformedCommand {
	public:
		CMD_PVR_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,592,0); }
	};
	class CMD_PVR_Menu_DT : public PreformedCommand {
	public:
		CMD_PVR_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,592,0); }
	};
	class CMD_PVR_Menu_Cat : public PreformedCommand {
	public:
		CMD_PVR_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,592,0); }
	};
	class CMD_Quick_Skip : public PreformedCommand {
	public:
		CMD_Quick_Skip(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,593,0); }
	};
	class CMD_Quick_Skip_DL : public PreformedCommand {
	public:
		CMD_Quick_Skip_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,593,0); }
	};
	class CMD_Quick_Skip_DT : public PreformedCommand {
	public:
		CMD_Quick_Skip_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,593,0); }
	};
	class CMD_Quick_Skip_Cat : public PreformedCommand {
	public:
		CMD_Quick_Skip_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,593,0); }
	};
	class CMD_RDS_info : public PreformedCommand {
	public:
		CMD_RDS_info(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,594,0); }
	};
	class CMD_RDS_info_DL : public PreformedCommand {
	public:
		CMD_RDS_info_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,594,0); }
	};
	class CMD_RDS_info_DT : public PreformedCommand {
	public:
		CMD_RDS_info_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,594,0); }
	};
	class CMD_RDS_info_Cat : public PreformedCommand {
	public:
		CMD_RDS_info_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,594,0); }
	};
	class CMD_Rear : public PreformedCommand {
	public:
		CMD_Rear(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,595,0); }
	};
	class CMD_Rear_DL : public PreformedCommand {
	public:
		CMD_Rear_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,595,0); }
	};
	class CMD_Rear_DT : public PreformedCommand {
	public:
		CMD_Rear_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,595,0); }
	};
	class CMD_Rear_Cat : public PreformedCommand {
	public:
		CMD_Rear_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,595,0); }
	};
	class CMD_Rear_Down : public PreformedCommand {
	public:
		CMD_Rear_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,596,0); }
	};
	class CMD_Rear_Down_DL : public PreformedCommand {
	public:
		CMD_Rear_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,596,0); }
	};
	class CMD_Rear_Down_DT : public PreformedCommand {
	public:
		CMD_Rear_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,596,0); }
	};
	class CMD_Rear_Down_Cat : public PreformedCommand {
	public:
		CMD_Rear_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,596,0); }
	};
	class CMD_Rear_Up : public PreformedCommand {
	public:
		CMD_Rear_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,597,0); }
	};
	class CMD_Rear_Up_DL : public PreformedCommand {
	public:
		CMD_Rear_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,597,0); }
	};
	class CMD_Rear_Up_DT : public PreformedCommand {
	public:
		CMD_Rear_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,597,0); }
	};
	class CMD_Rear_Up_Cat : public PreformedCommand {
	public:
		CMD_Rear_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,597,0); }
	};
	class CMD_Recall : public PreformedCommand {
	public:
		CMD_Recall(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,598,0); }
	};
	class CMD_Recall_DL : public PreformedCommand {
	public:
		CMD_Recall_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,598,0); }
	};
	class CMD_Recall_DT : public PreformedCommand {
	public:
		CMD_Recall_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,598,0); }
	};
	class CMD_Recall_Cat : public PreformedCommand {
	public:
		CMD_Recall_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,598,0); }
	};
	class CMD_Recall_Main : public PreformedCommand {
	public:
		CMD_Recall_Main(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,599,0); }
	};
	class CMD_Recall_Main_DL : public PreformedCommand {
	public:
		CMD_Recall_Main_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,599,0); }
	};
	class CMD_Recall_Main_DT : public PreformedCommand {
	public:
		CMD_Recall_Main_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,599,0); }
	};
	class CMD_Recall_Main_Cat : public PreformedCommand {
	public:
		CMD_Recall_Main_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,599,0); }
	};
	class CMD_Recent : public PreformedCommand {
	public:
		CMD_Recent(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,600,0); }
	};
	class CMD_Recent_DL : public PreformedCommand {
	public:
		CMD_Recent_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,600,0); }
	};
	class CMD_Recent_DT : public PreformedCommand {
	public:
		CMD_Recent_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,600,0); }
	};
	class CMD_Recent_Cat : public PreformedCommand {
	public:
		CMD_Recent_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,600,0); }
	};
	class CMD_Record_Blank : public PreformedCommand {
	public:
		CMD_Record_Blank(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,601,0); }
	};
	class CMD_Record_Blank_DL : public PreformedCommand {
	public:
		CMD_Record_Blank_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,601,0); }
	};
	class CMD_Record_Blank_DT : public PreformedCommand {
	public:
		CMD_Record_Blank_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,601,0); }
	};
	class CMD_Record_Blank_Cat : public PreformedCommand {
	public:
		CMD_Record_Blank_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,601,0); }
	};
	class CMD_Record_Level_Down : public PreformedCommand {
	public:
		CMD_Record_Level_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,602,0); }
	};
	class CMD_Record_Level_Down_DL : public PreformedCommand {
	public:
		CMD_Record_Level_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,602,0); }
	};
	class CMD_Record_Level_Down_DT : public PreformedCommand {
	public:
		CMD_Record_Level_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,602,0); }
	};
	class CMD_Record_Level_Down_Cat : public PreformedCommand {
	public:
		CMD_Record_Level_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,602,0); }
	};
	class CMD_Record_Level_Up : public PreformedCommand {
	public:
		CMD_Record_Level_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,603,0); }
	};
	class CMD_Record_Level_Up_DL : public PreformedCommand {
	public:
		CMD_Record_Level_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,603,0); }
	};
	class CMD_Record_Level_Up_DT : public PreformedCommand {
	public:
		CMD_Record_Level_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,603,0); }
	};
	class CMD_Record_Level_Up_Cat : public PreformedCommand {
	public:
		CMD_Record_Level_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,603,0); }
	};
	class CMD_Record_Mode : public PreformedCommand {
	public:
		CMD_Record_Mode(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,604,0); }
	};
	class CMD_Record_Mode_DL : public PreformedCommand {
	public:
		CMD_Record_Mode_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,604,0); }
	};
	class CMD_Record_Mode_DT : public PreformedCommand {
	public:
		CMD_Record_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,604,0); }
	};
	class CMD_Record_Mode_Cat : public PreformedCommand {
	public:
		CMD_Record_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,604,0); }
	};
	class CMD_Red : public PreformedCommand {
	public:
		CMD_Red(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,605,0); }
	};
	class CMD_Red_DL : public PreformedCommand {
	public:
		CMD_Red_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,605,0); }
	};
	class CMD_Red_DT : public PreformedCommand {
	public:
		CMD_Red_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,605,0); }
	};
	class CMD_Red_Cat : public PreformedCommand {
	public:
		CMD_Red_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,605,0); }
	};
	class CMD_Reload : public PreformedCommand {
	public:
		CMD_Reload(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,606,0); }
	};
	class CMD_Reload_DL : public PreformedCommand {
	public:
		CMD_Reload_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,606,0); }
	};
	class CMD_Reload_DT : public PreformedCommand {
	public:
		CMD_Reload_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,606,0); }
	};
	class CMD_Reload_Cat : public PreformedCommand {
	public:
		CMD_Reload_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,606,0); }
	};
	class CMD_Repeat_AB : public PreformedCommand {
	public:
		CMD_Repeat_AB(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,607,0); }
	};
	class CMD_Repeat_AB_DL : public PreformedCommand {
	public:
		CMD_Repeat_AB_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,607,0); }
	};
	class CMD_Repeat_AB_DT : public PreformedCommand {
	public:
		CMD_Repeat_AB_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,607,0); }
	};
	class CMD_Repeat_AB_Cat : public PreformedCommand {
	public:
		CMD_Repeat_AB_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,607,0); }
	};
	class CMD_Repeat_All : public PreformedCommand {
	public:
		CMD_Repeat_All(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,609,0); }
	};
	class CMD_Repeat_All_DL : public PreformedCommand {
	public:
		CMD_Repeat_All_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,609,0); }
	};
	class CMD_Repeat_All_DT : public PreformedCommand {
	public:
		CMD_Repeat_All_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,609,0); }
	};
	class CMD_Repeat_All_Cat : public PreformedCommand {
	public:
		CMD_Repeat_All_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,609,0); }
	};
	class CMD_Repeat_One : public PreformedCommand {
	public:
		CMD_Repeat_One(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,610,0); }
	};
	class CMD_Repeat_One_DL : public PreformedCommand {
	public:
		CMD_Repeat_One_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,610,0); }
	};
	class CMD_Repeat_One_DT : public PreformedCommand {
	public:
		CMD_Repeat_One_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,610,0); }
	};
	class CMD_Repeat_One_Cat : public PreformedCommand {
	public:
		CMD_Repeat_One_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,610,0); }
	};
	class CMD_Return_to_Live : public PreformedCommand {
	public:
		CMD_Return_to_Live(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,611,0); }
	};
	class CMD_Return_to_Live_DL : public PreformedCommand {
	public:
		CMD_Return_to_Live_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,611,0); }
	};
	class CMD_Return_to_Live_DT : public PreformedCommand {
	public:
		CMD_Return_to_Live_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,611,0); }
	};
	class CMD_Return_to_Live_Cat : public PreformedCommand {
	public:
		CMD_Return_to_Live_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,611,0); }
	};
	class CMD_Reveal : public PreformedCommand {
	public:
		CMD_Reveal(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,612,0); }
	};
	class CMD_Reveal_DL : public PreformedCommand {
	public:
		CMD_Reveal_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,612,0); }
	};
	class CMD_Reveal_DT : public PreformedCommand {
	public:
		CMD_Reveal_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,612,0); }
	};
	class CMD_Reveal_Cat : public PreformedCommand {
	public:
		CMD_Reveal_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,612,0); }
	};
	class CMD_RF : public PreformedCommand {
	public:
		CMD_RF(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,613,0); }
	};
	class CMD_RF_DL : public PreformedCommand {
	public:
		CMD_RF_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,613,0); }
	};
	class CMD_RF_DT : public PreformedCommand {
	public:
		CMD_RF_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,613,0); }
	};
	class CMD_RF_Cat : public PreformedCommand {
	public:
		CMD_RF_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,613,0); }
	};
	class CMD_Save : public PreformedCommand {
	public:
		CMD_Save(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,614,0); }
	};
	class CMD_Save_DL : public PreformedCommand {
	public:
		CMD_Save_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,614,0); }
	};
	class CMD_Save_DT : public PreformedCommand {
	public:
		CMD_Save_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,614,0); }
	};
	class CMD_Save_Cat : public PreformedCommand {
	public:
		CMD_Save_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,614,0); }
	};
	class CMD_Scan : public PreformedCommand {
	public:
		CMD_Scan(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,615,0); }
	};
	class CMD_Scan_DL : public PreformedCommand {
	public:
		CMD_Scan_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,615,0); }
	};
	class CMD_Scan_DT : public PreformedCommand {
	public:
		CMD_Scan_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,615,0); }
	};
	class CMD_Scan_Cat : public PreformedCommand {
	public:
		CMD_Scan_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,615,0); }
	};
	class CMD_Select_A : public PreformedCommand {
	public:
		CMD_Select_A(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,616,0); }
	};
	class CMD_Select_A_DL : public PreformedCommand {
	public:
		CMD_Select_A_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,616,0); }
	};
	class CMD_Select_A_DT : public PreformedCommand {
	public:
		CMD_Select_A_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,616,0); }
	};
	class CMD_Select_A_Cat : public PreformedCommand {
	public:
		CMD_Select_A_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,616,0); }
	};
	class CMD_Select_B : public PreformedCommand {
	public:
		CMD_Select_B(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,617,0); }
	};
	class CMD_Select_B_DL : public PreformedCommand {
	public:
		CMD_Select_B_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,617,0); }
	};
	class CMD_Select_B_DT : public PreformedCommand {
	public:
		CMD_Select_B_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,617,0); }
	};
	class CMD_Select_B_Cat : public PreformedCommand {
	public:
		CMD_Select_B_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,617,0); }
	};
	class CMD_Select_Down : public PreformedCommand {
	public:
		CMD_Select_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,618,0); }
	};
	class CMD_Select_Down_DL : public PreformedCommand {
	public:
		CMD_Select_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,618,0); }
	};
	class CMD_Select_Down_DT : public PreformedCommand {
	public:
		CMD_Select_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,618,0); }
	};
	class CMD_Select_Down_Cat : public PreformedCommand {
	public:
		CMD_Select_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,618,0); }
	};
	class CMD_Select_Up : public PreformedCommand {
	public:
		CMD_Select_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,619,0); }
	};
	class CMD_Select_Up_DL : public PreformedCommand {
	public:
		CMD_Select_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,619,0); }
	};
	class CMD_Select_Up_DT : public PreformedCommand {
	public:
		CMD_Select_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,619,0); }
	};
	class CMD_Select_Up_Cat : public PreformedCommand {
	public:
		CMD_Select_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,619,0); }
	};
	class CMD_Select_VPS : public PreformedCommand {
	public:
		CMD_Select_VPS(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,620,0); }
	};
	class CMD_Select_VPS_DL : public PreformedCommand {
	public:
		CMD_Select_VPS_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,620,0); }
	};
	class CMD_Select_VPS_DT : public PreformedCommand {
	public:
		CMD_Select_VPS_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,620,0); }
	};
	class CMD_Select_VPS_Cat : public PreformedCommand {
	public:
		CMD_Select_VPS_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,620,0); }
	};
	class CMD_Sequential : public PreformedCommand {
	public:
		CMD_Sequential(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,621,0); }
	};
	class CMD_Sequential_DL : public PreformedCommand {
	public:
		CMD_Sequential_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,621,0); }
	};
	class CMD_Sequential_DT : public PreformedCommand {
	public:
		CMD_Sequential_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,621,0); }
	};
	class CMD_Sequential_Cat : public PreformedCommand {
	public:
		CMD_Sequential_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,621,0); }
	};
	class CMD_Setup_Menu : public PreformedCommand {
	public:
		CMD_Setup_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,622,0); }
	};
	class CMD_Setup_Menu_DL : public PreformedCommand {
	public:
		CMD_Setup_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,622,0); }
	};
	class CMD_Setup_Menu_DT : public PreformedCommand {
	public:
		CMD_Setup_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,622,0); }
	};
	class CMD_Setup_Menu_Cat : public PreformedCommand {
	public:
		CMD_Setup_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,622,0); }
	};
	class CMD_Shuffle : public PreformedCommand {
	public:
		CMD_Shuffle(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,623,0); }
	};
	class CMD_Shuffle_DL : public PreformedCommand {
	public:
		CMD_Shuffle_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,623,0); }
	};
	class CMD_Shuffle_DT : public PreformedCommand {
	public:
		CMD_Shuffle_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,623,0); }
	};
	class CMD_Shuffle_Cat : public PreformedCommand {
	public:
		CMD_Shuffle_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,623,0); }
	};
	class CMD_Shuffle_All : public PreformedCommand {
	public:
		CMD_Shuffle_All(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,624,0); }
	};
	class CMD_Shuffle_All_DL : public PreformedCommand {
	public:
		CMD_Shuffle_All_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,624,0); }
	};
	class CMD_Shuffle_All_DT : public PreformedCommand {
	public:
		CMD_Shuffle_All_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,624,0); }
	};
	class CMD_Shuffle_All_Cat : public PreformedCommand {
	public:
		CMD_Shuffle_All_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,624,0); }
	};
	class CMD_Shuffle_One : public PreformedCommand {
	public:
		CMD_Shuffle_One(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,625,0); }
	};
	class CMD_Shuffle_One_DL : public PreformedCommand {
	public:
		CMD_Shuffle_One_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,625,0); }
	};
	class CMD_Shuffle_One_DT : public PreformedCommand {
	public:
		CMD_Shuffle_One_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,625,0); }
	};
	class CMD_Shuffle_One_Cat : public PreformedCommand {
	public:
		CMD_Shuffle_One_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,625,0); }
	};
	class CMD_Size : public PreformedCommand {
	public:
		CMD_Size(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,626,0); }
	};
	class CMD_Size_DL : public PreformedCommand {
	public:
		CMD_Size_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,626,0); }
	};
	class CMD_Size_DT : public PreformedCommand {
	public:
		CMD_Size_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,626,0); }
	};
	class CMD_Size_Cat : public PreformedCommand {
	public:
		CMD_Size_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,626,0); }
	};
	class CMD_Skip_30 : public PreformedCommand {
	public:
		CMD_Skip_30(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,627,0); }
	};
	class CMD_Skip_30_DL : public PreformedCommand {
	public:
		CMD_Skip_30_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,627,0); }
	};
	class CMD_Skip_30_DT : public PreformedCommand {
	public:
		CMD_Skip_30_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,627,0); }
	};
	class CMD_Skip_30_Cat : public PreformedCommand {
	public:
		CMD_Skip_30_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,627,0); }
	};
	class CMD_Sleep : public PreformedCommand {
	public:
		CMD_Sleep(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,628,0); }
	};
	class CMD_Sleep_DL : public PreformedCommand {
	public:
		CMD_Sleep_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,628,0); }
	};
	class CMD_Sleep_DT : public PreformedCommand {
	public:
		CMD_Sleep_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,628,0); }
	};
	class CMD_Sleep_Cat : public PreformedCommand {
	public:
		CMD_Sleep_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,628,0); }
	};
	class CMD_Smart_Picture : public PreformedCommand {
	public:
		CMD_Smart_Picture(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,629,0); }
	};
	class CMD_Smart_Picture_DL : public PreformedCommand {
	public:
		CMD_Smart_Picture_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,629,0); }
	};
	class CMD_Smart_Picture_DT : public PreformedCommand {
	public:
		CMD_Smart_Picture_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,629,0); }
	};
	class CMD_Smart_Picture_Cat : public PreformedCommand {
	public:
		CMD_Smart_Picture_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,629,0); }
	};
	class CMD_Sound_Menu : public PreformedCommand {
	public:
		CMD_Sound_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,630,0); }
	};
	class CMD_Sound_Menu_DL : public PreformedCommand {
	public:
		CMD_Sound_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,630,0); }
	};
	class CMD_Sound_Menu_DT : public PreformedCommand {
	public:
		CMD_Sound_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,630,0); }
	};
	class CMD_Sound_Menu_Cat : public PreformedCommand {
	public:
		CMD_Sound_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,630,0); }
	};
	class CMD_Sound_Scroll : public PreformedCommand {
	public:
		CMD_Sound_Scroll(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,631,0); }
	};
	class CMD_Sound_Scroll_DL : public PreformedCommand {
	public:
		CMD_Sound_Scroll_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,631,0); }
	};
	class CMD_Sound_Scroll_DT : public PreformedCommand {
	public:
		CMD_Sound_Scroll_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,631,0); }
	};
	class CMD_Sound_Scroll_Cat : public PreformedCommand {
	public:
		CMD_Sound_Scroll_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,631,0); }
	};
	class CMD_SPLP : public PreformedCommand {
	public:
		CMD_SPLP(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,632,0); }
	};
	class CMD_SPLP_DL : public PreformedCommand {
	public:
		CMD_SPLP_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,632,0); }
	};
	class CMD_SPLP_DT : public PreformedCommand {
	public:
		CMD_SPLP_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,632,0); }
	};
	class CMD_SPLP_Cat : public PreformedCommand {
	public:
		CMD_SPLP_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,632,0); }
	};
	class CMD_Spatial_Sound : public PreformedCommand {
	public:
		CMD_Spatial_Sound(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,633,0); }
	};
	class CMD_Spatial_Sound_DL : public PreformedCommand {
	public:
		CMD_Spatial_Sound_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,633,0); }
	};
	class CMD_Spatial_Sound_DT : public PreformedCommand {
	public:
		CMD_Spatial_Sound_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,633,0); }
	};
	class CMD_Spatial_Sound_Cat : public PreformedCommand {
	public:
		CMD_Spatial_Sound_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,633,0); }
	};
	class CMD_Speaker : public PreformedCommand {
	public:
		CMD_Speaker(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,634,0); }
	};
	class CMD_Speaker_DL : public PreformedCommand {
	public:
		CMD_Speaker_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,634,0); }
	};
	class CMD_Speaker_DT : public PreformedCommand {
	public:
		CMD_Speaker_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,634,0); }
	};
	class CMD_Speaker_Cat : public PreformedCommand {
	public:
		CMD_Speaker_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,634,0); }
	};
	class CMD_Speed : public PreformedCommand {
	public:
		CMD_Speed(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,635,0); }
	};
	class CMD_Speed_DL : public PreformedCommand {
	public:
		CMD_Speed_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,635,0); }
	};
	class CMD_Speed_DT : public PreformedCommand {
	public:
		CMD_Speed_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,635,0); }
	};
	class CMD_Speed_Cat : public PreformedCommand {
	public:
		CMD_Speed_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,635,0); }
	};
	class CMD_Step_Down : public PreformedCommand {
	public:
		CMD_Step_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,636,0); }
	};
	class CMD_Step_Down_DL : public PreformedCommand {
	public:
		CMD_Step_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,636,0); }
	};
	class CMD_Step_Down_DT : public PreformedCommand {
	public:
		CMD_Step_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,636,0); }
	};
	class CMD_Step_Down_Cat : public PreformedCommand {
	public:
		CMD_Step_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,636,0); }
	};
	class CMD_Step_Up : public PreformedCommand {
	public:
		CMD_Step_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,637,0); }
	};
	class CMD_Step_Up_DL : public PreformedCommand {
	public:
		CMD_Step_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,637,0); }
	};
	class CMD_Step_Up_DT : public PreformedCommand {
	public:
		CMD_Step_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,637,0); }
	};
	class CMD_Step_Up_Cat : public PreformedCommand {
	public:
		CMD_Step_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,637,0); }
	};
	class CMD_StereoMono : public PreformedCommand {
	public:
		CMD_StereoMono(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,638,0); }
	};
	class CMD_StereoMono_DL : public PreformedCommand {
	public:
		CMD_StereoMono_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,638,0); }
	};
	class CMD_StereoMono_DT : public PreformedCommand {
	public:
		CMD_StereoMono_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,638,0); }
	};
	class CMD_StereoMono_Cat : public PreformedCommand {
	public:
		CMD_StereoMono_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,638,0); }
	};
	class CMD_Still : public PreformedCommand {
	public:
		CMD_Still(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,639,0); }
	};
	class CMD_Still_DL : public PreformedCommand {
	public:
		CMD_Still_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,639,0); }
	};
	class CMD_Still_DT : public PreformedCommand {
	public:
		CMD_Still_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,639,0); }
	};
	class CMD_Still_Cat : public PreformedCommand {
	public:
		CMD_Still_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,639,0); }
	};
	class CMD_Store : public PreformedCommand {
	public:
		CMD_Store(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,640,0); }
	};
	class CMD_Store_DL : public PreformedCommand {
	public:
		CMD_Store_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,640,0); }
	};
	class CMD_Store_DT : public PreformedCommand {
	public:
		CMD_Store_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,640,0); }
	};
	class CMD_Store_Cat : public PreformedCommand {
	public:
		CMD_Store_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,640,0); }
	};
	class CMD_Strobe : public PreformedCommand {
	public:
		CMD_Strobe(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,641,0); }
	};
	class CMD_Strobe_DL : public PreformedCommand {
	public:
		CMD_Strobe_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,641,0); }
	};
	class CMD_Strobe_DT : public PreformedCommand {
	public:
		CMD_Strobe_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,641,0); }
	};
	class CMD_Strobe_Cat : public PreformedCommand {
	public:
		CMD_Strobe_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,641,0); }
	};
	class CMD_Subpage : public PreformedCommand {
	public:
		CMD_Subpage(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,642,0); }
	};
	class CMD_Subpage_DL : public PreformedCommand {
	public:
		CMD_Subpage_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,642,0); }
	};
	class CMD_Subpage_DT : public PreformedCommand {
	public:
		CMD_Subpage_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,642,0); }
	};
	class CMD_Subpage_Cat : public PreformedCommand {
	public:
		CMD_Subpage_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,642,0); }
	};
	class CMD_Surround : public PreformedCommand {
	public:
		CMD_Surround(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,643,0); }
	};
	class CMD_Surround_DL : public PreformedCommand {
	public:
		CMD_Surround_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,643,0); }
	};
	class CMD_Surround_DT : public PreformedCommand {
	public:
		CMD_Surround_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,643,0); }
	};
	class CMD_Surround_Cat : public PreformedCommand {
	public:
		CMD_Surround_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,643,0); }
	};
	class CMD_Surround_Down : public PreformedCommand {
	public:
		CMD_Surround_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,644,0); }
	};
	class CMD_Surround_Down_DL : public PreformedCommand {
	public:
		CMD_Surround_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,644,0); }
	};
	class CMD_Surround_Down_DT : public PreformedCommand {
	public:
		CMD_Surround_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,644,0); }
	};
	class CMD_Surround_Down_Cat : public PreformedCommand {
	public:
		CMD_Surround_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,644,0); }
	};
	class CMD_Surround_Mode : public PreformedCommand {
	public:
		CMD_Surround_Mode(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,645,0); }
	};
	class CMD_Surround_Mode_DL : public PreformedCommand {
	public:
		CMD_Surround_Mode_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,645,0); }
	};
	class CMD_Surround_Mode_DT : public PreformedCommand {
	public:
		CMD_Surround_Mode_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,645,0); }
	};
	class CMD_Surround_Mode_Cat : public PreformedCommand {
	public:
		CMD_Surround_Mode_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,645,0); }
	};
	class CMD_Surround_Up : public PreformedCommand {
	public:
		CMD_Surround_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,646,0); }
	};
	class CMD_Surround_Up_DL : public PreformedCommand {
	public:
		CMD_Surround_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,646,0); }
	};
	class CMD_Surround_Up_DT : public PreformedCommand {
	public:
		CMD_Surround_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,646,0); }
	};
	class CMD_Surround_Up_Cat : public PreformedCommand {
	public:
		CMD_Surround_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,646,0); }
	};
	class CMD_System_Select : public PreformedCommand {
	public:
		CMD_System_Select(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,647,0); }
	};
	class CMD_System_Select_DL : public PreformedCommand {
	public:
		CMD_System_Select_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,647,0); }
	};
	class CMD_System_Select_DT : public PreformedCommand {
	public:
		CMD_System_Select_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,647,0); }
	};
	class CMD_System_Select_Cat : public PreformedCommand {
	public:
		CMD_System_Select_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,647,0); }
	};
	class CMD_Tape_Select : public PreformedCommand {
	public:
		CMD_Tape_Select(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,648,0); }
	};
	class CMD_Tape_Select_DL : public PreformedCommand {
	public:
		CMD_Tape_Select_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,648,0); }
	};
	class CMD_Tape_Select_DT : public PreformedCommand {
	public:
		CMD_Tape_Select_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,648,0); }
	};
	class CMD_Tape_Select_Cat : public PreformedCommand {
	public:
		CMD_Tape_Select_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,648,0); }
	};
	class CMD_Test_Tone_On : public PreformedCommand {
	public:
		CMD_Test_Tone_On(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,649,0); }
	};
	class CMD_Test_Tone_On_DL : public PreformedCommand {
	public:
		CMD_Test_Tone_On_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,649,0); }
	};
	class CMD_Test_Tone_On_DT : public PreformedCommand {
	public:
		CMD_Test_Tone_On_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,649,0); }
	};
	class CMD_Test_Tone_On_Cat : public PreformedCommand {
	public:
		CMD_Test_Tone_On_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,649,0); }
	};
	class CMD_Text_Off : public PreformedCommand {
	public:
		CMD_Text_Off(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,650,0); }
	};
	class CMD_Text_Off_DL : public PreformedCommand {
	public:
		CMD_Text_Off_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,650,0); }
	};
	class CMD_Text_Off_DT : public PreformedCommand {
	public:
		CMD_Text_Off_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,650,0); }
	};
	class CMD_Text_Off_Cat : public PreformedCommand {
	public:
		CMD_Text_Off_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,650,0); }
	};
	class CMD_Thumbs_Down : public PreformedCommand {
	public:
		CMD_Thumbs_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,651,0); }
	};
	class CMD_Thumbs_Down_DL : public PreformedCommand {
	public:
		CMD_Thumbs_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,651,0); }
	};
	class CMD_Thumbs_Down_DT : public PreformedCommand {
	public:
		CMD_Thumbs_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,651,0); }
	};
	class CMD_Thumbs_Down_Cat : public PreformedCommand {
	public:
		CMD_Thumbs_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,651,0); }
	};
	class CMD_Thumbs_Up : public PreformedCommand {
	public:
		CMD_Thumbs_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,652,0); }
	};
	class CMD_Thumbs_Up_DL : public PreformedCommand {
	public:
		CMD_Thumbs_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,652,0); }
	};
	class CMD_Thumbs_Up_DT : public PreformedCommand {
	public:
		CMD_Thumbs_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,652,0); }
	};
	class CMD_Thumbs_Up_Cat : public PreformedCommand {
	public:
		CMD_Thumbs_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,652,0); }
	};
	class CMD_Time_Page : public PreformedCommand {
	public:
		CMD_Time_Page(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,653,0); }
	};
	class CMD_Time_Page_DL : public PreformedCommand {
	public:
		CMD_Time_Page_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,653,0); }
	};
	class CMD_Time_Page_DT : public PreformedCommand {
	public:
		CMD_Time_Page_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,653,0); }
	};
	class CMD_Time_Page_Cat : public PreformedCommand {
	public:
		CMD_Time_Page_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,653,0); }
	};
	class CMD_Timer : public PreformedCommand {
	public:
		CMD_Timer(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,654,0); }
	};
	class CMD_Timer_DL : public PreformedCommand {
	public:
		CMD_Timer_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,654,0); }
	};
	class CMD_Timer_DT : public PreformedCommand {
	public:
		CMD_Timer_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,654,0); }
	};
	class CMD_Timer_Cat : public PreformedCommand {
	public:
		CMD_Timer_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,654,0); }
	};
	class CMD_Timer_Prog_View : public PreformedCommand {
	public:
		CMD_Timer_Prog_View(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,655,0); }
	};
	class CMD_Timer_Prog_View_DL : public PreformedCommand {
	public:
		CMD_Timer_Prog_View_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,655,0); }
	};
	class CMD_Timer_Prog_View_DT : public PreformedCommand {
	public:
		CMD_Timer_Prog_View_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,655,0); }
	};
	class CMD_Timer_Prog_View_Cat : public PreformedCommand {
	public:
		CMD_Timer_Prog_View_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,655,0); }
	};
	class CMD_Timer_Program : public PreformedCommand {
	public:
		CMD_Timer_Program(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,656,0); }
	};
	class CMD_Timer_Program_DL : public PreformedCommand {
	public:
		CMD_Timer_Program_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,656,0); }
	};
	class CMD_Timer_Program_DT : public PreformedCommand {
	public:
		CMD_Timer_Program_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,656,0); }
	};
	class CMD_Timer_Program_Cat : public PreformedCommand {
	public:
		CMD_Timer_Program_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,656,0); }
	};
	class CMD_Timer_Standby : public PreformedCommand {
	public:
		CMD_Timer_Standby(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,657,0); }
	};
	class CMD_Timer_Standby_DL : public PreformedCommand {
	public:
		CMD_Timer_Standby_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,657,0); }
	};
	class CMD_Timer_Standby_DT : public PreformedCommand {
	public:
		CMD_Timer_Standby_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,657,0); }
	};
	class CMD_Timer_Standby_Cat : public PreformedCommand {
	public:
		CMD_Timer_Standby_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,657,0); }
	};
	class CMD_Title : public PreformedCommand {
	public:
		CMD_Title(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,658,0); }
	};
	class CMD_Title_DL : public PreformedCommand {
	public:
		CMD_Title_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,658,0); }
	};
	class CMD_Title_DT : public PreformedCommand {
	public:
		CMD_Title_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,658,0); }
	};
	class CMD_Title_Cat : public PreformedCommand {
	public:
		CMD_Title_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,658,0); }
	};
	class CMD_Today : public PreformedCommand {
	public:
		CMD_Today(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,659,0); }
	};
	class CMD_Today_DL : public PreformedCommand {
	public:
		CMD_Today_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,659,0); }
	};
	class CMD_Today_DT : public PreformedCommand {
	public:
		CMD_Today_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,659,0); }
	};
	class CMD_Today_Cat : public PreformedCommand {
	public:
		CMD_Today_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,659,0); }
	};
	class CMD_Track : public PreformedCommand {
	public:
		CMD_Track(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,660,0); }
	};
	class CMD_Track_DL : public PreformedCommand {
	public:
		CMD_Track_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,660,0); }
	};
	class CMD_Track_DT : public PreformedCommand {
	public:
		CMD_Track_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,660,0); }
	};
	class CMD_Track_Cat : public PreformedCommand {
	public:
		CMD_Track_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,660,0); }
	};
	class CMD_Tracking_Down : public PreformedCommand {
	public:
		CMD_Tracking_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,661,0); }
	};
	class CMD_Tracking_Down_DL : public PreformedCommand {
	public:
		CMD_Tracking_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,661,0); }
	};
	class CMD_Tracking_Down_DT : public PreformedCommand {
	public:
		CMD_Tracking_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,661,0); }
	};
	class CMD_Tracking_Down_Cat : public PreformedCommand {
	public:
		CMD_Tracking_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,661,0); }
	};
	class CMD_Tracking_Up : public PreformedCommand {
	public:
		CMD_Tracking_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,662,0); }
	};
	class CMD_Tracking_Up_DL : public PreformedCommand {
	public:
		CMD_Tracking_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,662,0); }
	};
	class CMD_Tracking_Up_DT : public PreformedCommand {
	public:
		CMD_Tracking_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,662,0); }
	};
	class CMD_Tracking_Up_Cat : public PreformedCommand {
	public:
		CMD_Tracking_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,662,0); }
	};
	class CMD_Treble_Down : public PreformedCommand {
	public:
		CMD_Treble_Down(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,663,0); }
	};
	class CMD_Treble_Down_DL : public PreformedCommand {
	public:
		CMD_Treble_Down_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,663,0); }
	};
	class CMD_Treble_Down_DT : public PreformedCommand {
	public:
		CMD_Treble_Down_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,663,0); }
	};
	class CMD_Treble_Down_Cat : public PreformedCommand {
	public:
		CMD_Treble_Down_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,663,0); }
	};
	class CMD_Treble_Min : public PreformedCommand {
	public:
		CMD_Treble_Min(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,664,0); }
	};
	class CMD_Treble_Min_DL : public PreformedCommand {
	public:
		CMD_Treble_Min_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,664,0); }
	};
	class CMD_Treble_Min_DT : public PreformedCommand {
	public:
		CMD_Treble_Min_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,664,0); }
	};
	class CMD_Treble_Min_Cat : public PreformedCommand {
	public:
		CMD_Treble_Min_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,664,0); }
	};
	class CMD_Treble_Plus : public PreformedCommand {
	public:
		CMD_Treble_Plus(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,665,0); }
	};
	class CMD_Treble_Plus_DL : public PreformedCommand {
	public:
		CMD_Treble_Plus_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,665,0); }
	};
	class CMD_Treble_Plus_DT : public PreformedCommand {
	public:
		CMD_Treble_Plus_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,665,0); }
	};
	class CMD_Treble_Plus_Cat : public PreformedCommand {
	public:
		CMD_Treble_Plus_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,665,0); }
	};
	class CMD_Treble_Up : public PreformedCommand {
	public:
		CMD_Treble_Up(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,666,0); }
	};
	class CMD_Treble_Up_DL : public PreformedCommand {
	public:
		CMD_Treble_Up_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,666,0); }
	};
	class CMD_Treble_Up_DT : public PreformedCommand {
	public:
		CMD_Treble_Up_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,666,0); }
	};
	class CMD_Treble_Up_Cat : public PreformedCommand {
	public:
		CMD_Treble_Up_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,666,0); }
	};
	class CMD_TVCable : public PreformedCommand {
	public:
		CMD_TVCable(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,667,0); }
	};
	class CMD_TVCable_DL : public PreformedCommand {
	public:
		CMD_TVCable_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,667,0); }
	};
	class CMD_TVCable_DT : public PreformedCommand {
	public:
		CMD_TVCable_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,667,0); }
	};
	class CMD_TVCable_Cat : public PreformedCommand {
	public:
		CMD_TVCable_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,667,0); }
	};
	class CMD_TVDVD : public PreformedCommand {
	public:
		CMD_TVDVD(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,668,0); }
	};
	class CMD_TVDVD_DL : public PreformedCommand {
	public:
		CMD_TVDVD_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,668,0); }
	};
	class CMD_TVDVD_DT : public PreformedCommand {
	public:
		CMD_TVDVD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,668,0); }
	};
	class CMD_TVDVD_Cat : public PreformedCommand {
	public:
		CMD_TVDVD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,668,0); }
	};
	class CMD_TVLD : public PreformedCommand {
	public:
		CMD_TVLD(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,669,0); }
	};
	class CMD_TVLD_DL : public PreformedCommand {
	public:
		CMD_TVLD_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,669,0); }
	};
	class CMD_TVLD_DT : public PreformedCommand {
	public:
		CMD_TVLD_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,669,0); }
	};
	class CMD_TVLD_Cat : public PreformedCommand {
	public:
		CMD_TVLD_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,669,0); }
	};
	class CMD_TVRadio : public PreformedCommand {
	public:
		CMD_TVRadio(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,670,0); }
	};
	class CMD_TVRadio_DL : public PreformedCommand {
	public:
		CMD_TVRadio_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,670,0); }
	};
	class CMD_TVRadio_DT : public PreformedCommand {
	public:
		CMD_TVRadio_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,670,0); }
	};
	class CMD_TVRadio_Cat : public PreformedCommand {
	public:
		CMD_TVRadio_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,670,0); }
	};
	class CMD_TVSAT : public PreformedCommand {
	public:
		CMD_TVSAT(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,671,0); }
	};
	class CMD_TVSAT_DL : public PreformedCommand {
	public:
		CMD_TVSAT_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,671,0); }
	};
	class CMD_TVSAT_DT : public PreformedCommand {
	public:
		CMD_TVSAT_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,671,0); }
	};
	class CMD_TVSAT_Cat : public PreformedCommand {
	public:
		CMD_TVSAT_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,671,0); }
	};
	class CMD_TVVCR : public PreformedCommand {
	public:
		CMD_TVVCR(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,672,0); }
	};
	class CMD_TVVCR_DL : public PreformedCommand {
	public:
		CMD_TVVCR_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,672,0); }
	};
	class CMD_TVVCR_DT : public PreformedCommand {
	public:
		CMD_TVVCR_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,672,0); }
	};
	class CMD_TVVCR_Cat : public PreformedCommand {
	public:
		CMD_TVVCR_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,672,0); }
	};
	class CMD_TVVideo : public PreformedCommand {
	public:
		CMD_TVVideo(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,673,0); }
	};
	class CMD_TVVideo_DL : public PreformedCommand {
	public:
		CMD_TVVideo_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,673,0); }
	};
	class CMD_TVVideo_DT : public PreformedCommand {
	public:
		CMD_TVVideo_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,673,0); }
	};
	class CMD_TVVideo_Cat : public PreformedCommand {
	public:
		CMD_TVVideo_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,673,0); }
	};
	class CMD_Update : public PreformedCommand {
	public:
		CMD_Update(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,674,0); }
	};
	class CMD_Update_DL : public PreformedCommand {
	public:
		CMD_Update_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,674,0); }
	};
	class CMD_Update_DT : public PreformedCommand {
	public:
		CMD_Update_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,674,0); }
	};
	class CMD_Update_Cat : public PreformedCommand {
	public:
		CMD_Update_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,674,0); }
	};
	class CMD_Video_Menu : public PreformedCommand {
	public:
		CMD_Video_Menu(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,675,0); }
	};
	class CMD_Video_Menu_DL : public PreformedCommand {
	public:
		CMD_Video_Menu_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,675,0); }
	};
	class CMD_Video_Menu_DT : public PreformedCommand {
	public:
		CMD_Video_Menu_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,675,0); }
	};
	class CMD_Video_Menu_Cat : public PreformedCommand {
	public:
		CMD_Video_Menu_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,675,0); }
	};
	class CMD_Video_Mute : public PreformedCommand {
	public:
		CMD_Video_Mute(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,676,0); }
	};
	class CMD_Video_Mute_DL : public PreformedCommand {
	public:
		CMD_Video_Mute_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,676,0); }
	};
	class CMD_Video_Mute_DT : public PreformedCommand {
	public:
		CMD_Video_Mute_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,676,0); }
	};
	class CMD_Video_Mute_Cat : public PreformedCommand {
	public:
		CMD_Video_Mute_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,676,0); }
	};
	class CMD_VideoShowview : public PreformedCommand {
	public:
		CMD_VideoShowview(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,677,0); }
	};
	class CMD_VideoShowview_DL : public PreformedCommand {
	public:
		CMD_VideoShowview_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,677,0); }
	};
	class CMD_VideoShowview_DT : public PreformedCommand {
	public:
		CMD_VideoShowview_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,677,0); }
	};
	class CMD_VideoShowview_Cat : public PreformedCommand {
	public:
		CMD_VideoShowview_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,677,0); }
	};
	class CMD_View : public PreformedCommand {
	public:
		CMD_View(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,678,0); }
	};
	class CMD_View_DL : public PreformedCommand {
	public:
		CMD_View_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,678,0); }
	};
	class CMD_View_DT : public PreformedCommand {
	public:
		CMD_View_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,678,0); }
	};
	class CMD_View_Cat : public PreformedCommand {
	public:
		CMD_View_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,678,0); }
	};
	class CMD_VISS_Index_Next : public PreformedCommand {
	public:
		CMD_VISS_Index_Next(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,679,0); }
	};
	class CMD_VISS_Index_Next_DL : public PreformedCommand {
	public:
		CMD_VISS_Index_Next_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,679,0); }
	};
	class CMD_VISS_Index_Next_DT : public PreformedCommand {
	public:
		CMD_VISS_Index_Next_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,679,0); }
	};
	class CMD_VISS_Index_Next_Cat : public PreformedCommand {
	public:
		CMD_VISS_Index_Next_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,679,0); }
	};
	class CMD_VISS_Index_Previous : public PreformedCommand {
	public:
		CMD_VISS_Index_Previous(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,680,0); }
	};
	class CMD_VISS_Index_Previous_DL : public PreformedCommand {
	public:
		CMD_VISS_Index_Previous_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,680,0); }
	};
	class CMD_VISS_Index_Previous_DT : public PreformedCommand {
	public:
		CMD_VISS_Index_Previous_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,680,0); }
	};
	class CMD_VISS_Index_Previous_Cat : public PreformedCommand {
	public:
		CMD_VISS_Index_Previous_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,680,0); }
	};
	class CMD_White : public PreformedCommand {
	public:
		CMD_White(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,681,0); }
	};
	class CMD_White_DL : public PreformedCommand {
	public:
		CMD_White_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,681,0); }
	};
	class CMD_White_DT : public PreformedCommand {
	public:
		CMD_White_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,681,0); }
	};
	class CMD_White_Cat : public PreformedCommand {
	public:
		CMD_White_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,681,0); }
	};
	class CMD_Yellow : public PreformedCommand {
	public:
		CMD_Yellow(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,682,0); }
	};
	class CMD_Yellow_DL : public PreformedCommand {
	public:
		CMD_Yellow_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,682,0); }
	};
	class CMD_Yellow_DT : public PreformedCommand {
	public:
		CMD_Yellow_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,682,0); }
	};
	class CMD_Yellow_Cat : public PreformedCommand {
	public:
		CMD_Yellow_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,682,0); }
	};
	class CMD_Zoom : public PreformedCommand {
	public:
		CMD_Zoom(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,683,0); }
	};
	class CMD_Zoom_DL : public PreformedCommand {
	public:
		CMD_Zoom_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,683,0); }
	};
	class CMD_Zoom_DT : public PreformedCommand {
	public:
		CMD_Zoom_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,683,0); }
	};
	class CMD_Zoom_Cat : public PreformedCommand {
	public:
		CMD_Zoom_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,683,0); }
	};
	class CMD_Zoom_In : public PreformedCommand {
	public:
		CMD_Zoom_In(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,684,0); }
	};
	class CMD_Zoom_In_DL : public PreformedCommand {
	public:
		CMD_Zoom_In_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,684,0); }
	};
	class CMD_Zoom_In_DT : public PreformedCommand {
	public:
		CMD_Zoom_In_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,684,0); }
	};
	class CMD_Zoom_In_Cat : public PreformedCommand {
	public:
		CMD_Zoom_In_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,684,0); }
	};
	class CMD_Zoom_Out : public PreformedCommand {
	public:
		CMD_Zoom_Out(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,685,0); }
	};
	class CMD_Zoom_Out_DL : public PreformedCommand {
	public:
		CMD_Zoom_Out_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,685,0); }
	};
	class CMD_Zoom_Out_DT : public PreformedCommand {
	public:
		CMD_Zoom_Out_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,685,0); }
	};
	class CMD_Zoom_Out_Cat : public PreformedCommand {
	public:
		CMD_Zoom_Out_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,685,0); }
	};
	class CMD_Display_Dialog_Box_On_Orbiter : public PreformedCommand {
	public:
		CMD_Display_Dialog_Box_On_Orbiter(long DeviceIDFrom, long DeviceIDTo,string sText,string sOptions,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,686,3,9,sText.c_str(),39,sOptions.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Display_Dialog_Box_On_Orbiter_DL : public PreformedCommand {
	public:
		CMD_Display_Dialog_Box_On_Orbiter_DL(long DeviceIDFrom, string DeviceIDTo,string sText,string sOptions,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,686,3,9,sText.c_str(),39,sOptions.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Display_Dialog_Box_On_Orbiter_DT : public PreformedCommand {
	public:
		CMD_Display_Dialog_Box_On_Orbiter_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sText,string sOptions,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,686,3,9,sText.c_str(),39,sOptions.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Display_Dialog_Box_On_Orbiter_Cat : public PreformedCommand {
	public:
		CMD_Display_Dialog_Box_On_Orbiter_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sText,string sOptions,string sPK_Device_List) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,686,3,9,sText.c_str(),39,sOptions.c_str(),103,sPK_Device_List.c_str()); }
	};
	class CMD_Set_Screen_Type : public PreformedCommand {
	public:
		CMD_Set_Screen_Type(long DeviceIDFrom, long DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,687,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Set_Screen_Type_DL : public PreformedCommand {
	public:
		CMD_Set_Screen_Type_DL(long DeviceIDFrom, string DeviceIDTo,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,687,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Set_Screen_Type_DT : public PreformedCommand {
	public:
		CMD_Set_Screen_Type_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,687,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class CMD_Set_Screen_Type_Cat : public PreformedCommand {
	public:
		CMD_Set_Screen_Type_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iValue) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,687,1,48,StringUtils::itos(iValue).c_str()); }
	};
	class RESP_Get_Remote_Control_Mapping : public PreformedCommandResponse {
		string *m_sValue_To_Assign;
	public:
		RESP_Get_Remote_Control_Mapping(string *sValue_To_Assign) { 
		m_sValue_To_Assign=sValue_To_Assign; }
		void ParseResponse(Message *pMessage) {
			*m_sValue_To_Assign=pMessage->m_mapParameters[5]; };
	};
	class CMD_Get_Remote_Control_Mapping : public PreformedCommand {
	public:
		CMD_Get_Remote_Control_Mapping(long DeviceIDFrom, long DeviceIDTo,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Remote_Control_Mapping(sValue_To_Assign); }
	};
	class CMD_Get_Remote_Control_Mapping_DL : public PreformedCommand {
	public:
		CMD_Get_Remote_Control_Mapping_DL(long DeviceIDFrom, string DeviceIDTo,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Remote_Control_Mapping(sValue_To_Assign); }
	};
	class CMD_Get_Remote_Control_Mapping_DT : public PreformedCommand {
	public:
		CMD_Get_Remote_Control_Mapping_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Remote_Control_Mapping(sValue_To_Assign); }
	};
	class CMD_Get_Remote_Control_Mapping_Cat : public PreformedCommand {
	public:
		CMD_Get_Remote_Control_Mapping_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string *sValue_To_Assign) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,1,5,(*sValue_To_Assign).c_str());		m_pcResponse = new RESP_Get_Remote_Control_Mapping(sValue_To_Assign); }
	};
	class CMD_NOREP_Get_Remote_Control_Mapping : public PreformedCommand {
	public:
		CMD_NOREP_Get_Remote_Control_Mapping(long DeviceIDFrom, long DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,0); }
	};
	class CMD_NOREP_Get_Remote_Control_Mapping_DL : public PreformedCommand {
	public:
		CMD_NOREP_Get_Remote_Control_Mapping_DL(long DeviceIDFrom, string DeviceIDTo) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,0); }
	};
	class CMD_NOREP_Get_Remote_Control_Mapping_DT : public PreformedCommand {
	public:
		CMD_NOREP_Get_Remote_Control_Mapping_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,0); }
	};
	class CMD_NOREP_Get_Remote_Control_Mapping_Cat : public PreformedCommand {
	public:
		CMD_NOREP_Get_Remote_Control_Mapping_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,688,0); }
	};
	class CMD_Update_Time_Code : public PreformedCommand {
	public:
		CMD_Update_Time_Code(long DeviceIDFrom, long DeviceIDTo,int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,689,6,41,StringUtils::itos(iStreamID).c_str(),102,sTime.c_str(),132,sTotal.c_str(),133,sSpeed.c_str(),134,sTitle.c_str(),135,sSection.c_str()); }
	};
	class CMD_Update_Time_Code_DL : public PreformedCommand {
	public:
		CMD_Update_Time_Code_DL(long DeviceIDFrom, string DeviceIDTo,int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,689,6,41,StringUtils::itos(iStreamID).c_str(),102,sTime.c_str(),132,sTotal.c_str(),133,sSpeed.c_str(),134,sTitle.c_str(),135,sSection.c_str()); }
	};
	class CMD_Update_Time_Code_DT : public PreformedCommand {
	public:
		CMD_Update_Time_Code_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,689,6,41,StringUtils::itos(iStreamID).c_str(),102,sTime.c_str(),132,sTotal.c_str(),133,sSpeed.c_str(),134,sTitle.c_str(),135,sSection.c_str()); }
	};
	class CMD_Update_Time_Code_Cat : public PreformedCommand {
	public:
		CMD_Update_Time_Code_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,int iStreamID,string sTime,string sTotal,string sSpeed,string sTitle,string sSection) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,689,6,41,StringUtils::itos(iStreamID).c_str(),102,sTime.c_str(),132,sTotal.c_str(),133,sSpeed.c_str(),134,sTitle.c_str(),135,sSection.c_str()); }
	};
	class CMD_Send_File_To_Phone : public PreformedCommand {
	public:
		CMD_Send_File_To_Phone(long DeviceIDFrom, long DeviceIDTo,string sMac_address,string sCommand_Line,int iApp_Server_Device_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,693,3,47,sMac_address.c_str(),137,sCommand_Line.c_str(),138,StringUtils::itos(iApp_Server_Device_ID).c_str()); }
	};
	class CMD_Send_File_To_Phone_DL : public PreformedCommand {
	public:
		CMD_Send_File_To_Phone_DL(long DeviceIDFrom, string DeviceIDTo,string sMac_address,string sCommand_Line,int iApp_Server_Device_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceIDTo, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,693,3,47,sMac_address.c_str(),137,sCommand_Line.c_str(),138,StringUtils::itos(iApp_Server_Device_ID).c_str()); }
	};
	class CMD_Send_File_To_Phone_DT : public PreformedCommand {
	public:
		CMD_Send_File_To_Phone_DT(long DeviceIDFrom, long MasterDevice, eBroadcastLevel eB,string sMac_address,string sCommand_Line,int iApp_Server_Device_ID) { m_pMessage = new Message(DeviceIDFrom, MasterDevice, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,693,3,47,sMac_address.c_str(),137,sCommand_Line.c_str(),138,StringUtils::itos(iApp_Server_Device_ID).c_str()); }
	};
	class CMD_Send_File_To_Phone_Cat : public PreformedCommand {
	public:
		CMD_Send_File_To_Phone_Cat(long DeviceIDFrom, long DeviceCategory, bool bIncludeChildren, eBroadcastLevel eB,string sMac_address,string sCommand_Line,int iApp_Server_Device_ID) { m_pMessage = new Message(DeviceIDFrom, DeviceCategory, bIncludeChildren, eB, MESSAGETYPE_COMMAND, PRIORITY_NORMAL,693,3,47,sMac_address.c_str(),137,sCommand_Line.c_str(),138,StringUtils::itos(iApp_Server_Device_ID).c_str()); }
	};
}
#endif
