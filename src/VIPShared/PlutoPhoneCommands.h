#ifndef PlutoPhoneCommands_h
#define PlutoPhoneCommands_h

#define BD_PC_DISCONNECT				2
#define BD_PC_KEY_WAS_PRESSED			3
#define BD_PC_VMC_TERMINATED			4
#define BD_PC_VMC_HIDDEN				5
#define BD_PC_REPORTING_VARIABLES		6
#define BD_PC_ORDER_PLACED				7
#define BD_PC_SELECTED_FROM_LIST		8
#define BD_PC_SET_VARIABLE				9
#define BD_PC_REPORT_MY_VERSION			10

#define BD_CP_SEND_ME_KEYSTROKES		101
#define BD_CP_SHOW_IMAGE				102
#define BD_CP_SHOW_VMC					103
#define BD_CP_SHOW_LIST					104
#define BD_CP_DISCONNECT				105
#define BD_CP_UPDATE_VARIABLES			106
#define BD_CP_CAPTURE_KEYBOARD			107
#define BD_CP_SEND_FILE					108
#define BD_CP_SIMULATE_EVENT			109

class BDCommand *BuildCommandFromData( unsigned long dwType );

#endif
